<?php

/**
 * Renders the inside of the element
 * DO NOT call drupal_render($variables['element']) (i.e. on the entire element), as this will cause recursion.
 */
function theme_scripture_verse_picker($variables) {
  $output = "";
  $e = &$variables['element'];
  
  $e['book']['#title_display'] = 'invisible';
  $e['from_chapter']['#title_display'] = 'invisible';
  $e['from_verse']['#title_display'] = 'invisible';
  $e['to_chapter']['#title_display'] = 'invisible';
  $e['to_verse']['#title_display'] = 'invisible';
  $e['sid']['#title_display'] = 'invisible';
  
  $e['book']['#attributes']['title'] = $e['book']['#title'];
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

//   $output .= drupal_render($e['preview']);
  $t = drupal_render($e['translation']);
  $b = drupal_render($e['book']);
  $fc = drupal_render($e['from_chapter']);
  $fv = drupal_render($e['from_verse']);
  $tc = drupal_render($e['to_chapter']);
  $tv = drupal_render($e['to_verse']);
  $s = drupal_render($e['sid']);
  
  $output .= "<div class='container-inline'>{$t}</div><div class='container-inline'><label>".t('Book and verse(s):')."</label>{$b} {$fc} : ${fv}";
  
  if (strlen($tv)) {
    if (strlen($tc)) {
      $output .= " - {$tc} : {$tv}";
    } else {
      $output .= " - {$tv}";
    }
  }
  
  if (strlen($s)) {
    $output .= " ({$s})";
  }
  
  $output .= "</div>";
  
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
//   dpm($element, "element in value");
//   dpm($input, "input in value");
//   dpm($form_state, "form_state in value");
  // check verse ranges logically
  // check for existence of verse ranges
  // update preview?
  // express input as vid range with optional translation (ready for saving in field)
}

/**
 * gets called when verse picker element is used in a form
 */
function scripture_verse_picker_process($element, &$form_state, $complete_form) {
//   dpm($element, "element in process");
//   dpm($form_state, "form_state in process");
//   dpm($complete_form, "complete_form in process");
  
  // Element settings
  $settings = empty($element['#scripture_settings']) ? array() : $element['#scripture_settings'];
  $settings += array(
    'require_translation' => FALSE, // translation is optional if FALSE
    'allow_translation'   => TRUE,  // translation cannot be edited if FALSE
    'allow_book'          => TRUE,  // book cannot be edited if FALSE
    'allow_range'         => TRUE,  // only a single verse may be selected if FALSE
  );

  // Default values to populate element
  $init = empty($element['#value']) ? array() : $element['#value'];
  $init += empty($element['#default_value']) ? array() : $element['#default_value'];
  $init += array(
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
  if (!isset($translations[$init['translation']])) {
    $init['translation'] = NULL;
  }
  $books = scripture_get_books($init['translation']);
  foreach ($books as $booknum => &$b) {
    $b = "{$booknum}. {$b->bookname}";
  }
  if (!isset($books[$init['book']])) {
    $init['book'] = NULL;
  }

//   $preview = scripture_preview($init);

  // AJAX call to replace the preview
  //   $ajax = array(
  //     'wrapper' => 'scripture-preview-text',
  //     'callback' => 'scripture_ajax_preview_text',
  //     'method' => 'replace',
  //     'effect' => 'fade'
  //   );
  $ajax = NULL;
  // TODO: http://drupal.stackexchange.com/questions/162582/making-a-custom-form-element-ajax-aware

  // The compound form element:
  
  $element['#tree'] = TRUE;
  
//   $element['preview'] = array(
//     '#prefix' => '<div id="scripture-preview-text"><label for="scripture-preview-text">'.t('Text preview:').'</label>',
//     '#suffix' => '</div>',
//     '#markup' => $preview,
//   );

  $book_wrapper = 'scripture_verse_picker_book'; // TODO: if there is more than one verse picker on the page, this needs to be a unique ID

  $element['translation'] = array(
    '#type' => 'select',
    '#title' => t('Translation:'),
    '#title_display' => 'before',
    '#ajax' => array(
      'wrapper'  => $book_wrapper,
      'callback' => 'scripture_ajax_translation_changed',
      'method'   => 'replace',
    ),
    '#disabled' => !$settings['allow_translation'],
  );
  
  if ($settings['require_translation']) {
    $element['translation']['#required'] = TRUE;
    $element['translation']['#options'] = $translations;
    $element['translation']['#default_value'] = $init['translation'];
  } else {
    $element['translation']['#options'] = array(0 => 'N/A') + $translations;
    $element['translation']['#default_value'] = empty($init['translation']) ? 'N/A' : $init['translation'];
  }
  
  $element['book'] = array(
    '#prefix'        => "<div id='{$book_wrapper}'>",
    '#suffix'        => "</div>",
    '#type'          => 'select',
    '#title'         => t('Book'),
    '#options'       => $books,
    '#required'      => true,
    '#default_value' => $init['book'],
    '#ajax'          => &$ajax,
    '#disabled'      => !$settings['allow_book'],
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
    '#ajax' => &$ajax,
    '#access' => $settings['allow_range'],
  );
  
  $element['to_verse'] = array(
    '#type' => 'textfield',
    '#title' => t('To verse'),
    '#description' => t('Last verse in the range. Leave this and "To chapter" blank to use the "from verse" value.'),
    '#size' => 3,
    '#maxlength' => 3,
    '#required' => false,
    '#default_value' => $init['to_verse'],
    '#ajax' => &$ajax,
    '#access' => $settings['allow_range'],
  );
    
  // TODO: hide this field (using AJAX?) if a range of verses is specified
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
 * Validate user input
 * Checks the verse ranges logically, but does not check if verses exist in database
 */
function scripture_verse_picker_validate($element, &$form_state) {
//   dpm($element, "element in validate");
//   dpm($form_state, "form_state in validate");
  $v = &$element['#value'];
  
  // check that verse accompanies chapter
  if (!empty($v['to_chapter']) && empty($v['to_verse']))
    form_error($element, t('You filled in a chapter, but not a verse.'));
  
  if (empty($v['to_chapter'])) {
  
    // use the from_chapter value if to_chapter was empty
    $v['to_chapter'] = $v['from_chapter'];
  
    if (empty($v['to_verse'])) {
      // use the from_verse value if both to_chapter and to_verse were empty
      $v['to_verse'] = $v['from_verse'];
    }
  
  }
  
  // check that the verse range is positive
  if ($v['from_chapter'] > $v['to_chapter'] || ($v['from_chapter'] == $v['to_chapter'] && $v['from_verse'] > $v['to_verse']))
    form_error($element, t('First verse and chapter must be before the last verse and chapter.'));
  
  return $element;
}

/**
 * React to user changing the translation
 * Update list of books
 *
 * Check this out: https://www.drupal.org/node/279246#comment-2661792 ...
 */
function scripture_ajax_translation_changed($form, $form_state) {
  $array_parents = $form_state['triggering_element']['#array_parents'];
  array_pop($array_parents); // go one level up from the triggering element
  $scripture_verse_picker_element = drupal_array_get_nested_value($form, $array_parents);
  return $scripture_verse_picker_element['book'];
}

/**
 * Update preview for current verse(s) via AJAX
 */
// function scripture_ajax_preview_text($form, $form_state) {
//   return $form['preview'];
// }
