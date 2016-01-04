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

  if (empty($form_state['node_wrapper'])) {
    $form_state['node_wrapper'] = drupal_html_id("living_word_admin_proofread_node");
  }
  $wrapper = &$form_state['node_wrapper'];

  if (empty($form_state['nodes_to_proofread'])) {
    $form_state['nodes_to_proofread'] = living_word_admin_get_nodes_for_proofreading();
    $form_state['skip'] = 0;
  }
  $nids = &$form_state['nodes_to_proofread'];
  $n = count($nids);
  $skip = $form_state['skip'] % $n;

  $form['progress'] = array(
    '#prefix' => "<p>",
    '#suffix' => '</p>',
  );

  if (count($nids)) {

    // Load the first node to be proofread
    $node = node_load($nids[$skip]);
    $nid = $node->nid;
    $node_url = drupal_get_path_alias("node/${nid}");

    $form['progress']['#markup'] = t('There are !num LWCs left to proofread.', array('!num' => $n));

    $form['instructions'] = array(
      '#prefix' => '<p>',
      '#markup' => t('Please proofread the following LWC for grammatical, spelling and linguistic errors.'),
      '#suffix' => '</p>',
    );

    $form['node'] = array(
      '#prefix'   => "<div id=$wrapper>",
      'heading'   => array(
        '#prefix'   => "Heading: <blockquote>",
        'content'   => field_view_field('node', $node, 'lw_heading', array(
          'label' => 'hidden',
          'type'  => 'text_plain',
        )),
        '#suffix'   => '</blockquote>',
      ),
      'body'      => array(
        '#prefix'   => "Body text: <blockquote cite='${node_url}'>",
        'content'   => field_view_field('node', $node, 'lw_body', array(
          'label' => 'hidden',
          'type'  => 'text_default',
        )),
        '#suffix'   => '</blockquote>',
      ),
      'scripture' => array(
        '#prefix' => "Scripture reference: <blockquote>",
        'content' => field_view_field('node', $node, 'lw_scripture', array(
          'label'     => 'hidden',
          'type'      => 'scripture_formatter_plain',
        )),
        '#suffix'   => '</blockquote>',
      ),
      '#suffix'   => "</div>",
    );

    $form['controls'] = array(
      '#prefix' => "<p>".t('Are there any errors?')."</p>",
      'edit' => array(
        '#type'                     => 'submit',
        '#value'                    => t('Yes, edit'),
        '#limit_validation_errors'  => TRUE, // Do not validate the form when clicking
        '#validate'                 => FALSE,
        '#submit'                   => array('living_word_admin_proofread_form_edit_button_submit'),
        '#ajax'                     => array(
          'wrapper'   => $wrapper,
          'callback'  => 'living_word_admin_add_form_edit_button_ajax',
          'effect'    => 'fade',
        ),
        '#weight'                   => 10,
      ),
      'skip' => array(
        '#type'                     => 'submit',
        '#value'                    => t('Unsure, skip'),
        '#limit_validation_errors'  => TRUE, // Do not validate the form when clicking
        '#validate'                 => FALSE,
        '#submit'                   => array('living_word_admin_proofread_form_skip_button_submit'),
        '#ajax'                     => array(
          'wrapper'   => $wrapper,
          'callback'  => 'living_word_admin_add_form_skip_button_ajax',
          'effect'    => 'fade',
        ),
        '#weight'                   => 20,
      ),
      'next' => array(
        '#type'                     => 'submit',
        '#value'                    => t('No, next'),
        '#limit_validation_errors'  => TRUE, // Do not validate the form when clicking
        '#validate'                 => FALSE,
        '#submit'                   => array('living_word_admin_proofread_form_next_button_submit'),
        '#ajax'                     => array(
          'wrapper'   => $wrapper,
          'callback'  => 'living_word_admin_add_form_next_button_ajax',
          'effect'    => 'fade',
        ),
        '#weight'                   => 30,
      ),
    );

  } else {

    $form['progress']['#markup'] = t('There is nothing left to proofread.');

  }

  return $form;

}


/**
 * Submit handler for the "edit" button
 */
function living_word_admin_proofread_form_edit_button_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}


/**
 * AJAX callback for the "edit" button
 */
function living_word_admin_add_form_edit_button_ajax($form, &$form_state) {

}


/**
 * Submit handler for the "skip" button
 */
function living_word_admin_proofread_form_skip_button_submit($form, &$form_state) {
  $form_state['skip']++;
  $form_state['rebuild'] = TRUE;
}


/**
 * AJAX callback for the "skip" button
 */
function living_word_admin_add_form_skip_button_ajax($form, &$form_state) {
  return $form['node'];
}


/**
 * Submit handler for the "next" button
 */
function living_word_admin_proofread_form_next_button_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}


/**
 * AJAX callback for the "next" button
 */
function living_word_admin_add_form_next_button_ajax($form, &$form_state) {

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