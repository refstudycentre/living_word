<?php

/**
 * Build form elements for verse picker
 */
function scripture_picker_form($form, &$form_state) {
  
  $title='todo title';
  $preview='todo preview';
  $translation=NULL; // TODO
  
  $translations = scripture_get_translations();
  foreach ($translations as $id => &$t) {
    $t = "{$t->abbr} / {$t->lang} / {$t->name}";
  }
  
  $books = scripture_get_books($translation);
  foreach ($books as $booknum => &$b) {
    $b = "{$booknum}. {$b->bookname}";
  }
  
  // TODO: settings for single/multi verse, subverses enabled, translations enabled
  $translation='todo default translation';
  $book='todo default book';
  $from_chapter='todo from chapter';
  $from_verse='todo from verse';
  $to_chapter='todo to chapter';
  $to_verse='todo to verse';
  $sid='todo sid';
  
  $form['picker'] = array(
    '#type' => 'fieldset',
    '#title' => $title,
    'preview' => array(
      '#title' => t('Text preview'),
      '#prefix' => '<div id="scripture-preview-text">',
      '#suffix' => '</div>',
      '#markup' => $preview,
    ),
    'translation' => array(
      '#type' => 'select',
      '#title' => t('Translation'),
      '#options' => array(
        0 => 'N/A'
      ) + $translations,
      '#description' => t('Choose N/A if this field does not reference a specific translation.'),
      '#required' => false,
      '#default_value' => empty($translation) ? 'N/A' : $translation,
      '#ajax' => &$ajax
    ),
    'book' => array(
      '#type' => 'select',
      '#title' => t('Book'),
      '#options' => $books,
      '#required' => true,
      '#default_value' => $book,
      '#ajax' => &$ajax
    ),
    'from_chapter' => array(
      '#type' => 'textfield',
      '#title' => t('From chapter'),
      '#description' => t('First chapter in the range.'),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => true,
      '#default_value' => $from_chapter,
      '#ajax' => &$ajax
    ),
    'from_verse' => array(
      '#type' => 'textfield',
      '#title' => t('From verse'),
      '#description' => t('First verse in the range.'),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => true,
      '#default_value' => $from_verse,
      '#ajax' => &$ajax
    ),
    'to_chapter' => array(
      '#type' => 'textfield',
      '#title' => t('To chapter'),
      '#description' => t('Last chapter in the range. Leave blank to use the "from chapter" value.'),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => false,
      '#default_value' => $to_chapter,
      '#ajax' => &$ajax
    ),
    'to_verse' => array(
      '#type' => 'textfield',
      '#title' => t('To verse'),
      '#description' => t('Last verse in the range. Leave this and "To chapter" blank to use the "from verse" value.'),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => false,
      '#default_value' => $to_verse,
      '#ajax' => &$ajax
    ),
    'sid' => array(
      '#type' => 'textfield',
      '#title' => t('Subverse'),
      '#description' => t('This will be ignored if the first and last verses and chapters are not the same.'),
      '#size' => 1,
      '#maxlength' => 3,
      '#required' => false,
      '#default_value' => $sid,
      '#ajax' => &$ajax
    ),
  );
  
  return $form;
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

