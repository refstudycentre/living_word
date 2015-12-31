<?php

/*
 * USER PAGE CALLBACKS
 */

///**
// * Page callback for
// */
//function living_word_admin_callback_overview($param) {
//  $content = array();
//
//  $content['TODO'] = array(
//    '#markup' => 'TODO: content overview',
//  );
//
//  return $content;
//}


/**
 * Page callback for
 */
function living_word_admin_add_form($form, &$form_state) {

  $bundle = 'living_word_comment';
  global $user;
  $wrapper = 'living-word-admin-add-form-nodes';

  // Create a blank entity for re-using widgets, etc.
  $node = entity_create('node', array(
    'type' => $bundle,
    'uid' => $user->uid,
    'status' => 0,  // unpublished
    'comment' => 1, // comments closed
    'promote' => 0, // not promoted
  ));

  // If this is the first time the form is loaded, show only one new node
  if (!isset($form_state['number_of_nodes'])) {
    $form_state['number_of_nodes'] = 1;
  }

  $form['#tree'] = TRUE;

  $form['common'] = array(
    '#type'         => 'fieldset',
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
    '#title'        => t('Common values'),
    '#description'  => t('These fields are the same for all LWCs added below.'),
    '#parents'      => array('common'),
  );

  // add field widgets just like on the node edit form
  living_word_admin_field_attach_form('lw_heading', 'node', $bundle, $node, $form['common'], $form_state);
  living_word_admin_field_attach_form('lw_scripture', 'node', $bundle, $node, $form['common'], $form_state);

  $form['unique'] = array(
    '#type'         => 'fieldset',
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
    '#title'        => t('Unique values'),
    '#description'  => t('These fields are unique for every LWC.'),
  );

  $form['unique']['nodes'] = array(
    '#type' => 'container',
    '#id'   => $wrapper,
  );

  for ($i=0; $i < $form_state['number_of_nodes']; $i++) {

    $form['unique']['nodes'][$i] = array(
      '#type'         => 'fieldset',
      '#title'        => "LWC #${i}",
      '#collapsible'  => TRUE,
      '#collapsed'    => FALSE,
      /*
       * From https://www.drupal.org/node/48643 :
       * You can set #parents manually, but the need for this is rare. More
       * common is to read #parents to determine where in the form tree the
       * current element is. Setting #parents does not affect the rendering of
       * the form; that's decided by the indexes. However, setting #parents does
       *  affect placement of $form_values, as can be seen from filter_form().
       */
      '#parents'      => array('unique', 'nodes', $i),
    );

    // add field widgets just like on the node edit form
    living_word_admin_field_attach_form('lw_body', 'node', $bundle, $node, $form['unique']['nodes'][$i], $form_state);
    living_word_admin_field_attach_form('lw_type', 'node', $bundle, $node, $form['unique']['nodes'][$i], $form_state);

    // add a remove button if there is more than one in the list
    if ($form_state['number_of_nodes'] > 1) {
      $form['unique']['nodes'][$i]['remove'] = array(
        '#type'                     => 'submit',
        '#value'                    => t('Remove LWC @num', array('@num' => "#${i}")),
        '#limit_validation_errors'  => TRUE, // Do not validate the form when clicking "remove"
        '#validate'                 => FALSE,
        '#submit'                   => array('living_word_admin_add_form_remove_button_submit'),
        '#ajax'                     => array(
          'wrapper'  => $wrapper,
          'callback' => 'living_word_admin_add_form_remove_button_ajax',
        ),
        '#weight'                   => 100,
      );
    }

  }

  $form['unique']['controls'] = array(
    '#type' => 'container',
  );

  // add a button to add more LWCs via AJAX
  $form['unique']['controls']['add'] = array(
    '#type'                     => 'submit',
    '#value'                    => t('Add another LWC'),
    '#limit_validation_errors'  => TRUE, // Do not validate the form when clicking "add"
    '#validate'                 => FALSE,
    '#submit'                   => array('living_word_admin_add_form_add_button_submit'),
    '#ajax'                     => array(
      'wrapper'  => $wrapper,
      'callback' => 'living_word_admin_add_form_add_button_ajax',
    ),
  );

  // add a button to submit the form and create the nodes
  $form['unique']['controls']['save'] = array(
    '#type'                     => 'submit',
    '#value'                    => t('Create nodes'),
    // This will use the default submit and validate handlers
  );

  return $form;

}


/**
 * Submit handler for add button.
 * This function is required for #limit_validation_errors = TRUE and AJAX to work
 */
function living_word_admin_add_form_add_button_submit($form, &$form_state) {
  $form_state['number_of_nodes']++;
  $form_state['rebuild'] = TRUE;
}


/**
 * AJAX callback for the add button
 */
function living_word_admin_add_form_add_button_ajax($form, &$form_state) {
  return $form['unique']['nodes']; // The part of the form being replaced
}


/**
 * Submit handler for remove button.
 */
function living_word_admin_add_form_remove_button_submit($form, &$form_state) {

  // decrement the number of node forms
  $form_state['number_of_nodes']--;

  // get the id of the form to remove
  $id = $form_state['triggering_element']['#parents'][2];

  // remove the applicable node's data from the array and reindex
  unset($form_state['input']['unique']['nodes'][$id]); // remove form input
  $form_state['input']['unique']['nodes'] = array_values($form_state['input']['unique']['nodes']); // reindex

  // rebuild the form for #limit_validation_errors = TRUE and AJAX to work
  $form_state['rebuild'] = TRUE;
}


/**
 * AJAX callback for the remove button
 */
function living_word_admin_add_form_remove_button_ajax($form, &$form_state) {
  return $form['unique']['nodes']; // The part of the form being replaced
}


/**
 * Default validation handler
 */
function living_word_admin_add_form_validate($form, &$form_state) {
  // The individual widgets will be validated after this function, so there is nothing left to do here.
}


/**
 * Default submit handler
 */
function living_word_admin_add_form_submit($form, &$form_state) {

  $bundle = 'living_word_comment';
  global $user;
  $v = &$form_state['values'];

  // Collect information and create new nodes
  foreach ($v['unique']['nodes'] as $delta => $node) {

    $u = &$v['unique']['nodes'][$delta];
    $c = &$v['common'];

    // Collect fields
    $fields = array(
      'lw_heading'    => $c['lw_heading'],
      'lw_scripture'  => $c['lw_scripture'],
      'lw_body'       => $u['lw_body'],
      'lw_type'       => $u['lw_type'],
    );

    // Remove anything that is not an actual field value (e.g. "add another" buttons)
    foreach($fields as $field_name => &$field_value) {
      foreach ($field_value as $lang => &$field_items) {
        foreach ($field_items as $delta => &$field_item) {
          if (!is_int($delta)) {
            unset($field_items[$delta]);
          }
        }
      }
    }

    // Create a new node with field values
    $node = entity_create('node', array(
      'type'          => $bundle,
      'uid'           => $user->uid,
      'status'        => 0,  // unpublished
      'comment'       => 1, // comments closed
      'promote'       => 0, // not promoted
      'sticky'        => 0, // not sticky
    ) + $fields);

    // Commit node to database
    node_save($node);

  }

}


/**
 * Magical form helper function taken from http://drupal.stackexchange.com/a/69245
 * This attached a working field widget form to an arbitrary form (i.e. not the node edit form for which it was intended)
 */
function living_word_admin_field_attach_form($field_name, $entity_type, $bundle, $entity, &$form, &$form_state, $langcode = NULL) {

  // Set #parents to 'top-level' if it doesn't exist.
  $form += array('#parents' => array());

  // If no language is provided use the default site language.
  $options = array(
    'language' => field_valid_language($langcode),
    'default' => TRUE,
  );

  // Append to the form
  ctools_include('fields');
  $field_instance = field_info_instance($entity_type, $field_name, $bundle);
  $form += (array) ctools_field_invoke_field($field_instance, 'form', $entity_type, $entity, $form, $form_state, $options);

}