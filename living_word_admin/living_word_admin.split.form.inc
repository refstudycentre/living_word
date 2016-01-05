<?php

/*
 * Form functions for "Step 3: Moderate LWCs - Split node"
 */

//module_load_include('inc', 'living_word_admin', 'living_word_admin.form');
//module_load_include('inc', 'node', 'node.admin');
module_load_include('inc', 'living_word_admin', 'living_word_admin.add.form');


/**
 * Form builder
 * Much of this is shamelessly copied from node.admin.inc
 */
function living_word_admin_split_form($form, &$form_state, $nid) {

  $bundle = 'living_word_comment';
  global $user;

//  // Create a blank entity for re-using widgets, etc.
//  $node = entity_create('node', array(
//    'type' => $bundle,
//    'uid' => $user->uid,
//    'status' => 0,  // unpublished
//    'comment' => 1, // comments closed
//    'promote' => 0, // not promoted
//  ));

  $node = node_load($nid);
  $form['#source_node'] = &$node;

  // If this is the first time the form is loaded, show only two node forms
  if (!isset($form_state['number_of_destination_nodes'])) {
    $form_state['number_of_destination_nodes'] = 2;
  }

  $form['#tree'] = TRUE;

  $form['#attached']['css'] = array(
    drupal_get_path('module', 'living_word_admin') . '/living_word_admin_split_form.css',
  );

  $form['destination_nodes'] = array(
    '#type'         => 'fieldset',
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
    '#title'        => t('Destination nodes'),
    '#description'  => t('The node will be split into the following nodes, while preserving the original author:'),
    '#attributes'   => array('class' => array('living_word_admin_split_form_destination_nodes')),
  );

  for ($i=0; $i < $form_state['number_of_destination_nodes']; $i++) {

    $form['destination_nodes'][$i] = array(
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
      '#parents'      => array('destination_nodes', $i),
    );

    // add field widgets just like on the node edit form
    living_word_admin_field_attach_form('lw_scripture', 'node', $bundle, $node, $form['destination_nodes'][$i], $form_state);
    living_word_admin_field_attach_form('lw_heading', 'node', $bundle, $node, $form['destination_nodes'][$i], $form_state);
    living_word_admin_field_attach_form('lw_body', 'node', $bundle, $node, $form['destination_nodes'][$i], $form_state);
    living_word_admin_field_attach_form('lw_type', 'node', $bundle, $node, $form['destination_nodes'][$i], $form_state);
    living_word_admin_field_attach_form('lw_detail', 'node', $bundle, $node, $form['destination_nodes'][$i], $form_state);

    // add a remove button if there is more than two in the list
    if ($form_state['number_of_destination_nodes'] > 2) {
      $form['destination_nodes'][$i]['remove'] = array(
        '#type'                     => 'submit',
        '#value'                    => t('Remove LWC @num', array('@num' => "#${i}")),
        '#limit_validation_errors'  => TRUE, // Do not validate the form when clicking
        '#validate'                 => FALSE,
        '#submit'                   => array('living_word_admin_split_form_remove'),
//        '#ajax'                     => array(
//          'wrapper'  => $wrapper,
//          'callback' => 'living_word_admin_split_form_ajax',
//        ),
        '#weight'                   => 100,
      );
    }

  }

  $form['controls'] = array(
    '#type' => 'container',
  );

// TODO: implement add button for having more than two destination nodes
//  // add a button to add more LWCs via AJAX
//  $form['controls']['add'] = array(
//    '#type'                     => 'submit',
//    '#value'                    => t('Add another LWC'),
//    '#limit_validation_errors'  => TRUE, // Do not validate the form when clicking
//    '#validate'                 => FALSE,
//    '#submit'                   => array('living_word_admin_split_form_add'),
//    '#ajax'                     => array(
//      'wrapper'  => $wrapper,
//      'callback' => 'living_word_admin_split_form_ajax',
//    ),
//  );

  // add a button to submit the form and create the nodes
  $form['controls']['save'] = array(
    '#type'   => 'submit',
    '#value'  => t('Split node'),
    // This will use the default submit and validate handlers
  );

  return $form;

}


/**
 * Default submit handler
 */
function living_word_admin_split_form_submit($form, &$form_state) {

  $source_nid = $form['#source_node']->nid;

  // for each destination node
  foreach ($form_state['values']['destination_nodes'] as $delta => $fields) {

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

    // use the source node as a baseline
    $new_node = $form['#source_node'];

    // make sure that a new node is created
    unset($new_node->nid);
    unset($new_node->vid);
    unset($new_node->tnid);
    unset($new_node->changed);
    $new_node->is_new = TRUE;

    // make sure the node is not published
    $new_node->status = NODE_NOT_PUBLISHED;

    // overwrite fields
    foreach($fields as $field_name => &$field_value) {
      $new_node->$field_name = $field_value;
    }

    // save the node
    node_save($new_node);

  }

  // delete the source node
  node_delete($source_nid);

}

