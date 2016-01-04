<?php

/*
 * Form functions for "Step 2: Proofread LWCs"
 */

module_load_include('inc', 'living_word_admin', 'living_word_admin.form');


/**
 * Form builder
 */
function living_word_admin_proofread_form($form, &$form_state) {

  global $user;

  if (empty($form_state['nodes_to_proofread'])) {
    $form_state['nodes_to_proofread'] = living_word_admin_get_nodes_for_proofreading();
  }
  $nids = &$form_state['nodes_to_proofread'];
  $n = count($nids);

  $form['progress'] = array(
    '#prefix' => "<p>",
    '#suffix' => '</p>',
  );

  if (count($nids)) {

    $form['progress']['#markup'] = t('There are !num LWCs left to proofread.', array('!num' => $n));

    // Load the first node to be proofread
    $node = node_load($nids[0]);

    $form['preview'] = array(
      '#prefix' => '<blockquote>',
      'node' => node_view($node, 'living_word_admin_proofreading'),
      '#suffix' => '</blockquote>',
    );

  } else {

    $form['progress']['#markup'] = t('There is nothing left to proofread.');

  }

  return $form;

}


/**
 * Default form validation handler
 */
function living_word_admin_proofread_form_validate($form, &$form_state) {



}


/**
 * Default form submit handler
 */
function living_word_admin_proofread_form_submit($form, &$form_state) {



}


/**
 * Get the node ids of all LWCs that need proofreading
 */
function living_word_admin_get_nodes_for_proofreading() {

  $bundle = 'living_word_comment';

  /*
 * EntityFieldQuery does not support isNull, therefore it omits fields that
 * have no value. We have to do a custom query. See:
 *  - https://www.drupal.org/node/1157006
 *  - http://drupal.stackexchange.com/a/65136/8452
 */
//  $query = new EntityFieldQuery();
//  $result = $query
//    ->entityCondition('entity_type', 'node')
//    ->entityCondition('bundle', $bundle)
//    ->fieldCondition('lw_workflow', 'value', 'proofread', '<>')
//    ->propertyOrderBy('changed', 'ASC')
//    ->execute();

  $subquery = db_select('field_data_lw_workflow', 'f');
  $subquery->addField('f', 'entity_id', 'nid');
  $subquery->condition('f.lw_workflow_value', 'proofread');

  $query = db_select('node', 'n');
  $query->fields('n', array('nid', 'changed'));
  $query->orderBy('changed', 'ASC'); // nodes that have been waiting the longest are serviced first
  $query->condition('n.type', $bundle, '=');
  $query->condition('n.nid', $subquery, 'NOT IN');

  return $query->execute()->fetchCol(0);

}