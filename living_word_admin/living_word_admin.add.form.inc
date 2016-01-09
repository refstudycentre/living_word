<?php

/*
 * Form functions for "Step 1: Add LWCs"
 */

module_load_include('inc', 'living_word_admin', 'living_word_admin.form');


/**
 * Form builder
 */
function living_word_admin_add_form($form, &$form_state) {

  $bundle = 'living_word_comment';
  global $user;
  $ajax_nodes = array(
    'wrapper'  => 'living-word-admin-add-form-nodes',
    'callback' => 'living_word_admin_add_form_ajax_nodes',
  );
  $ajax_common = array(
    'wrapper'  => 'living-word-admin-add-form-common',
    'callback' => 'living_word_admin_add_form_ajax_common',
  );

  // If this is the first time the form is loaded, show only one new node
  if (!isset($form_state['number_of_nodes'])) {
    $form_state['number_of_nodes'] = 1;
  }

  // Create the nodes which provide the fields widgets with their default values
  $form_state['nodes'] = array();
  for($delta=0; $delta<$form_state['number_of_nodes']; $delta++) {

    // create the node
    $form_state['nodes'][$delta] = living_word_admin_add_form_build_node($form_state, $delta);

    /*
     * Because we don't save these node to the db using node_save, make sure at
     * least the field_presave hooks are called.
     */
    field_attach_presave('node', $form_state['nodes'][$delta]);

  }

  $form['#tree'] = TRUE;

  $form['common'] = array(
    '#type'         => 'fieldset',
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
    '#title'        => t('Common values'),
    '#description'  => t('These fields are the same for all LWCs added below.'),
    '#parents'      => array('common'),
    '#id'           => $ajax_common['wrapper'],
  );

  // add scripture field widget just like on the node edit form
  living_word_admin_field_attach_form('lw_scripture', 'node', $bundle, $form_state['nodes'][0], $form['common'], $form_state);
  $form['common']['lw_scripture']['#weight'] = 10;

  if (empty($form_state['enable_subverse'])) {

    // add a button to enable the subverse
    $form['common']['enable_subverse'] = array(
      '#type'                     => 'submit',
      '#value'                    => t('Choose a subverse...'),
//      '#validate'                 => array(),
      '#limit_validation_errors' => array(
        array('common'), // only validate $form_state['values']['common']
      ),
      '#submit'                   => array('living_word_admin_add_form_enable_subverse_button_submit'),
      '#ajax'                     => $ajax_common,
      '#weight'                   => 20,
    );

  } else {

    // prevent the scripture field widget from being changed
    $form['common']['lw_scripture']['#disabled'] = TRUE;

    // add subverse field widget just like on the node edit form
    living_word_admin_field_attach_form('lw_subverse', 'node', $bundle, $form_state['nodes'][0], $form['common'], $form_state);
    $form['common']['lw_subverse']['#weight'] = 30;

  }

  $form['unique'] = array(
    '#type'         => 'fieldset',
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
    '#title'        => t('Unique values'),
    '#description'  => t('These fields are unique for every LWC.'),
  );

  $form['unique']['nodes'] = array(
    '#type' => 'container',
    '#id'   => $ajax_nodes['wrapper'],
  );

  foreach ($form_state['nodes'] as $delta => &$node) {

    $form['unique']['nodes'][$delta] = array(
      '#type'         => 'fieldset',
      '#title'        => "LWC #${delta}",
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
      '#parents'      => array('unique', 'nodes', $delta),
    );

    // add field widgets just like on the node edit form
    living_word_admin_field_attach_form('lw_type',    'node', $bundle, $node, $form['unique']['nodes'][$delta], $form_state);
    living_word_admin_field_attach_form('lw_heading', 'node', $bundle, $node, $form['unique']['nodes'][$delta], $form_state);
    living_word_admin_field_attach_form('lw_body',    'node', $bundle, $node, $form['unique']['nodes'][$delta], $form_state);

    $form['unique']['nodes'][$delta]['lw_type']['#weight'] = -100;

    // add a remove button if there is more than one in the list
    if ($form_state['number_of_nodes'] > 1) {
      $form['unique']['nodes'][$delta]['remove'] = array(
        '#type'                     => 'submit',
        '#value'                    => t('Remove LWC @num', array('@num' => "#${delta}")),
        '#validate'                 => array(),
        '#limit_validation_errors'  => array(),
        '#submit'                   => array('living_word_admin_add_form_remove_button_submit'),
        '#ajax'                     => $ajax_nodes,
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
    '#validate'                 => array(),
    '#limit_validation_errors'  => array(),
    '#submit'                   => array('living_word_admin_add_form_add_button_submit'),
    '#ajax'                     => $ajax_nodes,
  );

  // add a button to submit the form and create the nodes
  $form['unique']['controls']['save'] = array(
    '#type'   => 'submit',
    '#value'  => t('Create nodes'),
    // This will use the default submit and validate handlers
  );

  return $form;

}


/**
 * Submit handler for enable_subverse button
 */
function living_word_admin_add_form_enable_subverse_button_submit($form, &$form_state) {
  $form_state['enable_subverse'] = TRUE;
  $form_state['rebuild'] = TRUE;
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
 * Default form validation handler
 */
function living_word_admin_add_form_validate($form, &$form_state) {
  // The individual widgets will be validated after this function, so there is nothing left to do here.
}


/**
 * Default form submit handler
 */
function living_word_admin_add_form_submit($form, &$form_state) {

  // Collect information and create new nodes
  foreach (array_keys($form_state['values']['unique']['nodes']) as $delta) {

    // Create node
    $node = living_word_admin_add_form_build_node($form_state, $delta);

    // Commit node to database
    node_save($node);

  }

}


/**
 * Build a new node, given the form_state and a delta value
 */
function living_word_admin_add_form_build_node($form_state, $delta) {

  $bundle = 'living_word_comment';
  global $user;

  // Create a new node
  $node = entity_create('node', array(
    'type'    => $bundle,
    'uid'     => $user->uid,
    'status'  => 0, // unpublished
    'comment' => 1, // comments closed
    'promote' => 0, // not promoted
    'sticky'  => 0, // not sticky
  ));

  if (!empty($form_state['values'])) {

    // Collect fields
    $fields = array();

    dpm($form_state['values']);

    if (!empty($form_state['values']['common'])) {
      $fields['lw_scripture'] = $form_state['values']['common']['lw_scripture'];
    }

    if (!empty($form_state['values']['unique']['nodes'][$delta])) {
      $fields['lw_heading'] = $form_state['values']['unique']['nodes'][$delta]['lw_heading'];
      $fields['lw_body']    = $form_state['values']['unique']['nodes'][$delta]['lw_body'];
      $fields['lw_type']    = $form_state['values']['unique']['nodes'][$delta]['lw_type'];
    }

    // Add the fields to the node
    living_word_admin_form_field_node_update($fields, $node);

  } // else the form has not been submitted yet

  return $node;

}


/**
 * AJAX callback for replacing the list of nodes
 */
function living_word_admin_add_form_ajax_nodes($form, &$form_state) {
  // Return the part of the form being replaced
  return $form['unique']['nodes'];
}


/**
 * AJAX callback for replacing the common fields
 */
function living_word_admin_add_form_ajax_common($form, &$form_state) {
  // Return the part of the form being replaced
  return $form['common'];
}