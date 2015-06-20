<?php

/*
 * ADMIN PAGE CALLBACKS
 * Three different strategies for page callbacks: http://www.drupalcoder.com/blog/how-to-define-page-callbacks-that-you-dont-want-to-render-using-page-template-in-drupal.html
 * Build the callbacks using renderable arrays. Good reading on renderable arrays:
 * - https://www.drupal.org/node/930760
 * - http://cocoate.com/ddbook/scary-render-array
 * - http://drupal.stackexchange.com/questions/11438/how-to-nest-elements-in-a-render-array
 */

/**
 * Page callback for admin/structure/scripture[/status]
 * summarize what is going on in the Scripture db tables
 */
function scripture_callback_status() {
  $content = array();
  
  $translations = scripture_get_translations();
  
  $translation_rows = array();
  foreach ($translations as $id => $translation) {
    
    $books = scripture_get_books($id);
    
    $translation_rows[] = array(
      $translation->name,
      count($books),
    );
    
  }
  
  $content['summary'] = array(
    '#prefix' => '<h2>' . t('Summary') . '</h2>',
    '#theme' => 'table',
    '#rows' => array(
      array(
        t('Number of translations'),
        count($translations)
      ),
    )
  );
  
  $content['translations'] = array(
    '#prefix' => '<h2>' . t('Translations') . '</h2>',
    '#theme' => 'table',
    '#header' => array(
      t('Translation'),
      t('Number of books'),
    ),
    '#rows' => $translation_rows
  );
  
  $content['TODO'] = array(
    '#prefix' => '<h2>TODO</h2>',
    '#markup' => 'TODO: display any other required statistics here',
  );
  
  return $content;
}

/**
 * Page callback for admin/structure/scripture/translations
 */
function scripture_callback_translations() {
  $form = array();
  
  $translations = scripture_get_translations();
  foreach ($translations as $id => &$t) {
    $t = "{$t->abbr} / {$t->lang} / {$t->name}";
  }
  
  $form['scripture_default_translation'] = array(
    '#type' => 'select',
    '#title' => t('Translation'),
    '#options' => $translations,
    '#description' => t('Choose the default translation to use when none is specified.'),
    '#required' => true,
    '#default_value' => array(
      variable_get('scripture_default_translation', NULL)
    )
  );
  
  return system_settings_form($form);
}

/**
 * Form for admin/structure/scripture/subverses
 * Build a form to manage (add, modify, delete) the subverses, making use of single verse picker
 *
 * Multi-step form tutorial:
 * http://www.jaypan.com/tutorial/drupal-7-ajax-driven-multi-step-forms-non-javascript-fallback
 */
function scripture_callback_subverses($form, &$form_state) {
  
  $s = $form_state['step'] = isset($form_state['step']) ? $form_state['step'] : 1;
  $n = 10; // number of steps
   
  // a wrapper in which to place the form step
  $wrapper = "subverses_form_step";
  $form['#prefix'] = "<div id='{$wrapper}'>";
  $form['#suffix'] = "</div>";
  
  // load the default values for the current step
  if (isset($form_state['values']["step_{$s}"])) {
    $default = $form_state['values']["step_{$s}"];
  } elseif (isset($form_state['storage']["step_{$s}"])) {
    $default = $form_state['storage']["step_{$s}"];
  } else {
    $default = NULL;
  }
  
  // Generate the appropriate form step
  switch($s) {
    case 1:
      $form['step_1'] = array(
        '#type' => 'scripture_verse_picker',
        '#title' => 'Pick a verse to work with',
        '#required' => TRUE,
        '#scripture_settings' => array(
          'require_translation' => TRUE,
          'allow_range' => FALSE,
        ),
        '#default_value' => $default,
      );
      break;
    case 2:
//       dpm($form_state['storage']['step_1']);
      $form['step_2'] = array(
        '#type'   => 'markup',
        '#markup' => scripture_preview($form_state['storage']['step_1']),
      );
      break;
  }
  
  // Create buttons
  
  // using the same callback for all buttons
  $ajax = array(
    'wrapper'  => $wrapper,
    'callback' => 'scripture_callback_subverses_ajax'
  );
  
  $form['buttons'] = array(
    '#type' => 'container',
  );
  
  if ($s != 1) {
    $form['buttons']['prev'] = array(
      '#type' => 'submit',
      '#value' => t('Back'),
      '#limit_validation_errors' => array(),
      '#submit' => array('scripture_callback_subverses_back'),
      '#ajax' => &$ajax,
    );
  }
  
  if ($s !== $n) {
    $form['buttons']['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
      '#ajax' => &$ajax,
    );
  } else {
    $form['buttons']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#ajax' => &$ajax,
    );
  }
  
  return $form;
}

/**
 * Callback for clicking the "back" button
 */
function scripture_callback_subverses_back($form, &$form_state) {
  $form_state['step']--;
  $form_state['rebuild'] = TRUE;
}

/**
 * Callback for clicking any AJAX button
 */
function scripture_callback_subverses_ajax($form, &$form_state) {
  return $form;
}

/**
 * Valdiate each step of the form
 */
function scripture_callback_subverses_validate($form, &$form_state) {
  switch($form_state['step'])
  {
    case "1":
      break;
  
    case "2":
      break;
  
    case "3":
//    form_set_error('step_3', t('asdf'));
      break;
  }
}

/**
 * Submit the form when clicking "next" or "submit" buttons
 */
function scripture_callback_subverses_submit($form, &$form_state) {
  $step = $form_state['step'];
  $form_state['storage']['step_' . $step] = $form_state['values']['step_' . $step];
  
  if (isset($form_state['values']['next']) && $form_state['values']['op'] == $form_state['values']['next']) {
    $form_state['step']++; // next step
  } elseif (isset($form_state['values']['submit']) && $form_state['values']['op'] == $form_state['values']['submit']) {
    // TODO: use the submitted values to complete the purpose of the form
    // $form_state['storage']

    // reset the form
    $form_state['step'] = 1;
    $form_state['storage'] = array();
  }
  
  $form_state['rebuild'] = TRUE;
}
