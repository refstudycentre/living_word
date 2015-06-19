<?php

/**
 * Implements hook_element_info().
 * https://api.drupal.org/api/drupal/modules%21system%21system.api.php/function/hook_element_info/7
 * Declare a FORM API element for the verse picker
 */
function scripture_element_info() {
  return array(
    'verse_picker' => array(
      '#default_value'  => '',
      '#input'          => TRUE,
      '#process'        => array('scripture_verse_picker_element_process'),
      '#theme'          => array('scripture_verse_picker_element'),
      '#theme_wrappers' => array('form_element'),
      '#tree'           => TRUE,
      '#value_callback' => 'scripture_verse_picker_value_callback',
    ),
  );
}

/**
 * gets called when verse picker element is used in a form
 */
function scripture_verse_picker_element_process($element, $form_state, $complete_form) {
  
  // TODO: settings for single/multi verse, subverses enabled, translations enabled
  $title = 'todo title'; //empty($settings['title']) ? "Verse picker" : $settings['title'];
  
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
  
  // AJAX call to replace the preview
  $ajax = array(
    'wrapper' => 'scripture-preview-text',
    'callback' => 'scripture_ajax_preview_text',
    'method' => 'replace',
    'effect' => 'fade'
  );
  
  // The compound form element
  $element['verse_picker'] = array(
    '#type' => 'fieldset',
    '#title' => $title,
    'preview' => array(
  
      '#prefix' => '<div id="scripture-preview-text"><label for="scripture-preview-text">'.t('Text preview:').'</label>',
      '#suffix' => '</div>',
      '#markup' => scripture_preview($init),
    ),
    'translation' => array(
      '#type' => 'select',
      '#title' => t('Translation'),
      '#options' => array(
        0 => 'N/A'
      ) + $translations,
      '#description' => t('Choose N/A if this field does not reference a specific translation.'),
      '#required' => false,
      '#default_value' => empty($init['translation']) ? 'N/A' : $init['translation'],
      '#ajax' => &$ajax
    ),
    'book' => array(
      '#type' => 'select',
      '#title' => t('Book'),
      '#options' => $books,
      '#required' => true,
      '#default_value' => $init['book'],
      '#ajax' => &$ajax
    ),
    'from_chapter' => array(
      '#type' => 'textfield',
      '#title' => t('From chapter'),
      '#description' => t('First chapter in the range.'),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => true,
      '#default_value' => $init['from_chapter'],
      '#ajax' => &$ajax
    ),
    'from_verse' => array(
      '#type' => 'textfield',
      '#title' => t('From verse'),
      '#description' => t('First verse in the range.'),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => true,
      '#default_value' => $init['from_verse'],
      '#ajax' => &$ajax
    ),
    'to_chapter' => array(
      '#type' => 'textfield',
      '#title' => t('To chapter'),
      '#description' => t('Last chapter in the range. Leave blank to use the "from chapter" value.'),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => false,
      '#default_value' => $init['to_chapter'],
      '#ajax' => &$ajax
    ),
    'to_verse' => array(
      '#type' => 'textfield',
      '#title' => t('To verse'),
      '#description' => t('Last verse in the range. Leave this and "To chapter" blank to use the "from verse" value.'),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => false,
      '#default_value' => $init['to_verse'],
      '#ajax' => &$ajax
    ),
    'sid' => array(
      '#type' => 'textfield',
      '#title' => t('Subverse'),
      '#description' => t('This will be ignored if the first and last verses and chapters are not the same.'),
      '#size' => 1,
      '#maxlength' => 3,
      '#required' => false,
      '#default_value' => $init['sid'],
      '#ajax' => &$ajax
    ),
  );
  
  return $element;
  
}

/**
 * Implements hook_theme($existing, $type, $theme, $path).
 * https://api.drupal.org/api/drupal/modules%21system%21system.api.php/function/hook_theme/7
 * Registers the module's theme implementations
 */
function scripture_theme($existing, $type, $theme, $path) {
  return array(
    'scripture_verse_picker_element' => array(
      'render element' => 'element',
    ),
  );
}

/**
 * Renders the inside of the element
 */
function theme_scripture_verse_picker_element($variables) {
  return drupal_render($variables['element']);
}

/**
 * Checks the value of the element?
 * Can alternatively use validation hook?
 * Three conditions:
 * - input provided directly (form submitted)
 * - no input; get default value from field definition
 * - no input & no default value
 * Optionally return modified input
 */
function scripture_verse_picker_element_value_callback($element, $input = FALSE, &$form_state) {
  
}

/**
 * Update preview for current verse(s) via AJAX
 */
function scripture_ajax_preview_text($form, $form_state) {
  return $form['verse_picker']['preview'];
  // FIXME: this does not work on node edit page!!
}

/**
 * Default validation for verse picker
 */
function scripture_picker_form_validate($form, &$form_state) {
  
}

/**
 * Default form submission for verse picker
 */
function scripture_picker_form_submit($form, &$form_state) {
  
}

