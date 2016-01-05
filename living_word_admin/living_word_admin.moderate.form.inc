<?php

/*
 * Form functions for "Step 3: Moderate LWCs"
 */

module_load_include('inc', 'living_word_admin', 'living_word_admin.form');
module_load_include('inc', 'node', 'node.admin');


/**
 * Form builder
 * Much of this is shamelessly copied from node.admin.inc
 */
function living_word_admin_moderate_form($form, &$form_state) {

  $bundle = 'living_word_comment';

  if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
    return node_multiple_delete_confirm($form, $form_state, array_filter($form_state['values']['nodes']));
  }
  $form['#submit'][] = 'node_filter_form_submit';
  $form['admin'] = living_word_admin_moderate_form_table($form, $form_state);

  return $form;

}


/**
 * Form builder: Builds the node administration overview.
 * Taken from node.admin.inc and modified
 */
function living_word_admin_moderate_form_table($form, &$form_state) {

  $bundle = 'living_word_comment';

  // Enable language column if translation module is enabled or if we have any
  // node with language.
  $multilanguage = (module_exists('translation') || db_query_range("SELECT 1 FROM {node} WHERE language <> :language", 0, 1, array(':language' => LANGUAGE_NONE))->fetchField());

  // Build the sortable table header.
  $header = array(
    'author' => t('Author'),
    'changed' => array('data' => t('Updated'), 'field' => 'n.changed', 'sort' => 'asc'),
//    'status' => array('data' => t('Status'), 'field' => 'n.status'),
//    'lw_workflow' => t('LW workflow'),
    'lw_heading' => t('Heading'),
    'lw_body' => t('Text'),
    'lw_scripture' => t('Scripture'),
    'lw_type' => t('Type / position'),
    'lw_detail' => t('Level of detail'),
  );
  if ($multilanguage) {
    $header['language'] = array('data' => t('Language'), 'field' => 'n.language');
  }
  $header['operations'] = array('data' => t('Operations'));

  $nids = living_word_admin_get_nodes_for_moderation();
  $nodes = node_load_multiple($nids);

  // Prepare the list of nodes.
  $languages = language_list();
  $destination = drupal_get_destination();
  $options = array();

  foreach ($nodes as $node) {

    $langcode = entity_language('node', $node);
    $l_options = $langcode != LANGUAGE_NONE && isset($languages[$langcode]) ? array('language' => $languages[$langcode]) : array();

//    $field_lw_workflow = field_view_field('node', $node, 'lw_workflow', array(
//      'label' => 'hidden',
//      'type'  => 'text_plain',
//    ));
    $field_lw_heading = field_view_field('node', $node, 'lw_heading', array(
      'label' => 'hidden',
      'type'  => 'text_plain',
    ));
    $field_lw_body = field_view_field('node', $node, 'lw_body', array(
      'label' => 'hidden',
      'type'  => 'text_plain',
    ));
    $field_lw_scripture = field_view_field('node', $node, 'lw_scripture', array(
      'label'     => 'hidden',
      'type'      => ' scripture_formatter_plain',
      'settings'  => array(
        'show_text' => 1,
        'show_ref'  => 1,
      ),
    ));
    $field_lw_type = field_view_field('node', $node, 'lw_type', array(
      'label' => 'hidden',
      'type'  => 'taxonomy_term_reference_plain',
    ));

    $form_lw_detail = drupal_get_form('living_word_admin_detail_form', $node);

    $options[$node->nid] = array(
      'author' => theme('username', array('account' => $node)),
      'changed' => format_date($node->changed, 'short'),
//      'status' => $node->status ? t('published') : t('not published'),
//      'lw_workflow' => drupal_render($field_lw_workflow),
      'lw_heading' => drupal_render($field_lw_heading),
      'lw_body' => drupal_render($field_lw_body),
      'lw_scripture' => drupal_render($field_lw_scripture),
      'lw_type' => drupal_render($field_lw_type),
      'lw_detail' => drupal_render($form_lw_detail), // This is a hack. We should be using something like http://e9p.net/theming-drupal-7-form-elements-table
    );

    if ($multilanguage) {
      if ($langcode == LANGUAGE_NONE || isset($languages[$langcode])) {
        $options[$node->nid]['language'] = $langcode == LANGUAGE_NONE ? t('Language neutral') : t($languages[$langcode]->name);
      }
      else {
        $options[$node->nid]['language'] = t('Undefined language (@langcode)', array('@langcode' => $langcode));
      }
    }
    // Build a list of all the accessible operations for the current node.
    $operations = array();
    $operations['edit'] = array(
      'title' => t('edit'),
      'href' => 'node/' . $node->nid . '/edit',
      'query' => $destination,
    );
    $operations['delete'] = array(
      'title' => t('delete'),
      'href' => 'node/' . $node->nid . '/delete',
      'query' => $destination,
    );
    $operations['split'] = array(
      'title' => t('split'),
      'href' => 'admin/structure/lw/moderate/split/' . $node->nid,
      'query' => $destination,
    );
    $options[$node->nid]['operations'] = array();
    // Render an unordered list of operations links.
    $options[$node->nid]['operations'] = array(
      'data' => array(
        '#theme' => 'links__node_operations',
        '#links' => $operations,
        '#attributes' => array('class' => array('links', 'inline')),
      ),
    );

  }

  // Use tableselect to allow the current user to perform operations.
  $form['nodes'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('There are no more nodes to moderate.'),
  );

  $form['pager'] = array('#markup' => theme('pager'));

  $form['controls'] = array(
    'done'    => array(
      '#type'     => 'submit',
      '#value'    => t('Mark as done'),
      '#validate' => FALSE, // Do not validate the form when clicking
      '#submit'   => array('living_word_admin_moderate_form_table_done'),
    ),
  );

  return $form;

}


/**
 * AJAX callback for the select lists
 */
function living_word_admin_moderate_form_table_ajax_lw_detail($form, &$form_state) {

  // load the node
  $node = node_load($form_state['build_info']['args'][0]->nid);

  // get the form value
  $lw_detail_tid = $form_state['values']['lw_detail'];

  /** @var EntityDrupalWrapper $wrapper An EntityMetadataWrapper for the node */
  $wrapper = entity_metadata_wrapper('node', $node);

  // make sure the node is not published
  $wrapper->status->set(NODE_NOT_PUBLISHED);

  // update the node
  $wrapper->lw_detail->set($lw_detail_tid);
  $wrapper->save();

}


/**
 * Submit handler for the 'Mark as done' button
 */
function living_word_admin_moderate_form_table_done($form, &$form_state) {

  // get the selected nodes
  foreach($form_state['values']['nodes'] as $nid => $checkbox_value) {
    if ($checkbox_value > 0) {

      // load the node
      $node = node_load($nid);
      /** @var EntityDrupalWrapper $wrapper An EntityMetadataWrapper for the node */
      $wrapper = entity_metadata_wrapper('node', $node);

      // mark node as "moderated" in the workflow
      $wrapper->lw_workflow[] = 'moderated';

      // make sure the node is not published
      $wrapper->status->set(NODE_NOT_PUBLISHED);

      // save
      $wrapper->save();

    }
  }

}


/**
 * Get the node ids of all LWCs that need moderation
 */
function living_word_admin_get_nodes_for_moderation() {

  $bundle = 'living_word_comment';

  // query all LWCs that have been proofread
  $subquery_proofread = db_select('field_data_lw_workflow', 'f');
  $subquery_proofread->addField('f', 'entity_id', 'nid');
  $subquery_proofread->condition('f.lw_workflow_value', 'proofread');

  // query all LWCs that have been moderated
  $subquery_moderated = db_select('field_data_lw_workflow', 'f');
  $subquery_moderated->addField('f', 'entity_id', 'nid');
  $subquery_moderated->condition('f.lw_workflow_value', 'moderated');

  $query = db_select('node', 'n') // from node n
    ->extend('PagerDefault');     // add a pager

  // Only show LWC nodes
  $query->condition('type', $bundle);

  // Only show nodes that have been proofread
  $query->condition('nid', $subquery_proofread, 'IN');

  // Only show nodes that have not been moderated
  $query->condition('nid', $subquery_moderated, 'NOT IN');

  // nodes that have been waiting the longest are serviced first
  $query->orderBy('changed', 'ASC');

  $nids = $query
    ->fields('n',array('nid'))
    ->limit(20)
    ->execute()
    ->fetchCol(0);

  return $nids;

}