<?php

/*
 * Form functions for "Step 2: Proofread LWCs"
 */

module_load_include('inc', 'living_word_admin', 'living_word_admin.form');


/**
 * Form builder
 */
function living_word_admin_proofread_form($form, &$form_state) {

  // Wrap the form to allow AJAX replacement
  if (empty($form_state['wrapper'])) {
    $form_state['wrapper'] = drupal_html_id("living_word_admin_proofread_node");
  }
  $form['#prefix'] = "<div id=${form_state['wrapper']}>";
  $form['#suffix'] = "</div>";

  // If there are no nodes in the queue, request them from the database
  if (empty($form_state['nodes_to_proofread'])) {
    $form_state['nodes_to_proofread'] = living_word_admin_get_nodes_for_proofreading();
    $form_state['skip'] = 0;
  }

  $n = count($form_state['nodes_to_proofread']);
  if ($n) {

    // Load the first node to be proofread
    $skip = $form_state['skip'] % $n;
    $form['#nid'] = $form_state['nodes_to_proofread'][$skip];
    $node = node_load($form['#nid']);
    $node_url = drupal_get_path_alias("node/${form['#nid']}");

    // build the form

    $form['instructions'] = array(
      '#prefix' => '<p>',
      '#markup' => t('Please proofread the following LWC for grammatical, spelling and linguistic errors.'),
      '#suffix' => '</p>',
    );

    $form['progress'] = array(
      '#prefix' => '<p>',
      '#markup' => format_plural($n, 'This is the last LWC to be proofread.', 'There are @count LWCs left to proofread.'),
      '#suffix' => '</p>',
    );

    $form['original_fields'] = array(
      '#prefix'   => '<div class="living-word-admin-original-fields">',
      'heading'   => array(
        '#prefix'   => "Original heading: <blockquote>",
        'content'   => field_view_field('node', $node, 'lw_heading', array(
          'label' => 'hidden',
          'type'  => 'text_plain',
        )),
        '#suffix'   => '</blockquote>',
      ),
      'body'      => array(
        '#prefix'   => "Original text: <blockquote cite='${node_url}'>",
        'content'   => field_view_field('node', $node, 'lw_body', array(
          'label' => 'hidden',
          'type'  => 'text_default',
        )),
        '#suffix'   => '</blockquote>',
      ),
      'scripture' => array(
        '#prefix' => "Scripture reference: <blockquote>",
        'content' => field_view_field('node', $node, 'lw_scripture', array(
          'label' => 'hidden',
          'type'  => 'scripture_formatter_plain',
        )),
        '#suffix' => '</blockquote>',
      ),
      '#suffix'   => '</div>',
    );

    // check if edit-mode is active
    if (!empty($form_state['edit'])) {

      // collect fields from node
//      $original_heading = field_get_items('node', $node, 'lw_heading')[0];
//      $original_body = field_get_items('node', $node, 'lw_body')[0];

      // build the edit form

      $form['new_fields'] = array(
        '#prefix'   => '<div class="living-word-admin-new-fields">',
        '#suffix'   => '</div>',
      );

      // add field widgets just like on the node edit form
      living_word_admin_field_attach_form('lw_heading', 'node', $node->type, $node, $form['new_fields'], $form_state);
      living_word_admin_field_attach_form('lw_body', 'node', $node->type, $node, $form['new_fields'], $form_state);

    }

    $form['controls'] = array(
      '#prefix' => "<p>".t('Are there any errors?')."</p>",
      'edit' => array(
        '#type'                     => 'submit',
        '#value'                    => t('Yes, edit'),
        '#limit_validation_errors'  => TRUE, // Do not validate the form when clicking
        '#validate'                 => FALSE,
        '#submit'                   => array('living_word_admin_proofread_form_edit_button_submit'),
        '#ajax'                     => array(
          'wrapper'   => $form_state['wrapper'],
          'callback'  => 'living_word_admin_proofread_form_ajax',
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
          'wrapper'   => $form_state['wrapper'],
          'callback'  => 'living_word_admin_proofread_form_ajax',
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
          'wrapper'   => $form_state['wrapper'],
          'callback'  => 'living_word_admin_proofread_form_ajax',
          'effect'    => 'fade',
        ),
        '#weight'                   => 30,
      ),
    );

  } else {

    $form['progress'] = array(
      '#prefix' => "<p>",
      '#markup' => t('There is nothing left to proofread.'),
      '#suffix' => '</p>',
    );

  }

  return $form;

}


/**
 * Submit handler for the "edit" button
 */
function living_word_admin_proofread_form_edit_button_submit($form, &$form_state) {
  $form_state['edit'] = TRUE;
  $form_state['rebuild'] = TRUE;
}


/**
 * Submit handler for the "skip" button
 */
function living_word_admin_proofread_form_skip_button_submit($form, &$form_state) {
  $form_state['skip']++;
  $form_state['skip'] %= count($form_state['nodes_to_proofread']);
  $form_state['rebuild'] = TRUE;
}


/**
 * Submit handler for the "next" button
 */
function living_word_admin_proofread_form_next_button_submit($form, &$form_state) {

  // load node
  $node = node_load($form['#nid']);
  /** @var EntityDrupalWrapper $wrapper An EntityMetadataWrapper for the node */
  $node_wrapper = entity_metadata_wrapper('node', $node);

  // mark node as "proofread" in the workflow
  $node_wrapper->lw_workflow[] = 'proofread';

  // save the node
  $node_wrapper->save();

  // remove node from the list of pending nodes and re-index
  $skip = $form_state['skip'] % count($form_state['nodes_to_proofread']);
  unset($form_state['nodes_to_proofread'][$skip]);
  $form_state['nodes_to_proofread'] = array_values($form_state['nodes_to_proofread']);

  // request a rebuilt form
  $form_state['rebuild'] = TRUE;

}


/**
 * AJAX callback to replace the entire form
 */
function living_word_admin_proofread_form_ajax($form, &$form_state) {
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