<?php

/**
 * Renders the inside of the element
 * DO NOT call drupal_render($variables['element']) (i.e. on the entire element), as this will cause recursion.
 */
function theme_scripture_verse_picker($variables) {
  $output = "";
  $e = &$variables['element'];
  
  dpm($variables);
  
  $e['from_chapter']['#title_display'] = 'invisible';
  $e['from_verse']['#title_display'] = 'invisible';
  $e['to_chapter']['#title_display'] = 'invisible';
  $e['to_verse']['#title_display'] = 'invisible';
  $e['sid']['#title_display'] = 'invisible';
  
  $e['from_chapter']['#attributes']['title'] = $e['from_chapter']['#title'] . ": " . $e['from_chapter']['#description'];
  $e['from_verse']['#attributes']['title'] = $e['from_verse']['#title'] . ": " . $e['from_verse']['#description'];
  $e['to_chapter']['#attributes']['title'] = $e['to_chapter']['#title'] . ": " . $e['to_chapter']['#description'];
  $e['to_verse']['#attributes']['title'] = $e['to_verse']['#title'] . ": " . $e['to_verse']['#description'];
  $e['sid']['#attributes']['title'] = $e['sid']['#title'] . ": " . $e['sid']['#description'];
  
  $e['from_chapter']['#description'] = NULL;
  $e['from_verse']['#description'] = NULL;
  $e['to_chapter']['#description'] = NULL;
  $e['to_verse']['#description'] = NULL;
  $e['sid']['#description'] = NULL;

  $output .= drupal_render($e['preview']);
  $output .= drupal_render($e['translation']);
  $output .= drupal_render($e['book']);
  $output .= '<div class="container-inline">';
  $output .= drupal_render($e['from_chapter']);
  $output .= drupal_render($e['from_verse']);
  $output .= drupal_render($e['to_chapter']);
  $output .= drupal_render($e['to_verse']);
  $output .= drupal_render($e['sid']);
  $output .= '</div>';
  
  return $output;
}

/**
 * Value callback for the verse picker
 *
 * Three conditions:
 * - input provided directly (form submitted)
 * - no input; get default value from field definition
 * - no input & no default value
 *
 * Optionally return modified input
 * If you just want #value set to #default_value, you don't need this hook at all, since that will happen anyway
 */
function form_type_scripture_verse_picker_value($element, $input = FALSE, &$form_state) {
  // check verse ranges logically
  // check for existence of verse ranges
  // update preview?
  // express input as vid range with optional translation (ready for saving in field)
}

/**
 * gets called when verse picker element is used in a form
 */
function scripture_verse_picker_process($element, &$form_state, $complete_form) {

  // TODO: settings for single/multi verse, subverses enabled, translations enabled

  // Initial values to populate form
  $default = empty($element['#default_value']['verse_picker']) ? array() : $element['#default_value']['verse_picker'];
  $init = $default + array(
    'translation' => NULL,
    'book' => NULL,
    'from_chapter' => '',
    'from_verse' => '',
    'to_chapter' => '',
    'to_verse' => '',
    'sid' => '',
  );

  // Options for select lists
  $translations = scripture_get_translations();
  foreach ($translations as $id => &$t) {
    $t = "{$t->abbr} / {$t->lang} / {$t->name}";
  }
  $books = scripture_get_books($init['translation']); // TODO: reload list of books on AJAX call
  foreach ($books as $booknum => &$b) {
    $b = "{$booknum}. {$b->bookname}";
  }

  $preview = scripture_preview($init);

  // AJAX call to replace the preview
  $ajax = array(
    'wrapper' => 'scripture-preview-text',
    'callback' => 'scripture_ajax_preview_text',
    'method' => 'replace',
    'effect' => 'fade'
  );

  // The compound form element:
  
  $element['#tree'] = TRUE;
  
  $element['preview'] = array(
    '#prefix' => '<div id="scripture-preview-text"><label for="scripture-preview-text">'.t('Text preview:').'</label>',
    '#suffix' => '</div>',
    '#markup' => $preview,
  );
  
  $element['translation'] = array(
    '#type' => 'select',
    '#title' => t('Translation'),
    '#title_display' => 'before',
    '#options' => array(
      0 => 'N/A'
    ) + $translations,
    '#description' => t('Choose N/A if this field does not reference a specific translation.'),
    '#required' => false,
    '#default_value' => empty($init['translation']) ? 'N/A' : $init['translation'],
    '#ajax' => &$ajax
  );
  
  $element['book'] = array(
    '#type' => 'select',
    '#title' => t('Book'),
    '#options' => $books,
    '#required' => true,
    '#default_value' => $init['book'],
    '#ajax' => &$ajax
  );
  
  $element['from_chapter'] = array(
    '#type' => 'textfield',
    '#title' => t('From chapter'),
    '#description' => t('First chapter in the range.'),
    '#size' => 3,
    '#maxlength' => 3,
    '#required' => true,
    '#default_value' => $init['from_chapter'],
    '#ajax' => &$ajax
  );
  
  $element['from_verse'] = array(
    '#type' => 'textfield',
    '#title' => t('From verse'),
    '#description' => t('First verse in the range.'),
    '#size' => 3,
    '#maxlength' => 3,
    '#required' => true,
    '#default_value' => $init['from_verse'],
    '#ajax' => &$ajax
  );
  
  $element['to_chapter'] = array(
    '#type' => 'textfield',
    '#title' => t('To chapter'),
    '#description' => t('Last chapter in the range. Leave blank to use the "from chapter" value.'),
    '#size' => 3,
    '#maxlength' => 3,
    '#required' => false,
    '#default_value' => $init['to_chapter'],
    '#ajax' => &$ajax
  );
  
  $element['to_verse'] = array(
    '#type' => 'textfield',
    '#title' => t('To verse'),
    '#description' => t('Last verse in the range. Leave this and "To chapter" blank to use the "from verse" value.'),
    '#size' => 3,
    '#maxlength' => 3,
    '#required' => false,
    '#default_value' => $init['to_verse'],
    '#ajax' => &$ajax
  );
  
  $element['sid'] = array(
    '#type' => 'textfield',
    '#title' => t('Subverse'),
    '#description' => t('This will be ignored if the first and last verses and chapters are not the same.'),
    '#size' => 1,
    '#maxlength' => 3,
    '#required' => false,
    '#default_value' => $init['sid'],
    '#ajax' => &$ajax
  );

  return $element;

}

/**
 * Update preview for current verse(s) via AJAX
 */
function scripture_ajax_preview_text($form, $form_state) {
  return $form['verse_picker']['preview'];
}
