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
  $form['admin'] = living_word_admin_moderate_form_table();

  return $form;

}


/**
 * Form builder: Builds the node administration overview.
 * Taken from node.admin.inc and modified
 */
function living_word_admin_moderate_form_table() {

  $bundle = 'living_word_comment';

  // Build the 'Update options' form.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options = array();
  foreach (module_invoke_all('node_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'approve',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#validate' => array('node_admin_nodes_validate'),
    '#submit' => array('node_admin_nodes_submit'),
  );

  // Enable language column if translation module is enabled or if we have any
  // node with language.
  $multilanguage = (module_exists('translation') || db_query_range("SELECT 1 FROM {node} WHERE language <> :language", 0, 1, array(':language' => LANGUAGE_NONE))->fetchField());

  // Build the sortable table header.
  $header = array(
    'author' => t('Author'),
    'status' => array('data' => t('Status'), 'field' => 'n.status'),
    'changed' => array('data' => t('Updated'), 'field' => 'n.changed', 'sort' => 'desc'),
    'lw_workflow' => array('data' => t('LW workflow'), 'field' => 'lw_workflow', 'sort' => 'desc'),
    'lw_heading' => array('data' => t('Heading'), 'field' => 'lw_heading', 'sort' => 'desc'),
    'lw_body' => array('data' => t('Text'), 'field' => 'lw_body', 'sort' => 'desc'),
  );
  if ($multilanguage) {
    $header['language'] = array('data' => t('Language'), 'field' => 'n.language');
  }
  $header['operations'] = array('data' => t('Operations'));

  $query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');

  // Only show LWC nodes
  $query->condition('type', $bundle);

  $nids = $query
    ->fields('n',array('nid'))
    ->limit(50)
    ->orderByHeader($header)
    ->addTag('node_access')
    ->execute()
    ->fetchCol();
  $nodes = node_load_multiple($nids);

  // Prepare the list of nodes.
  $languages = language_list();
  $destination = drupal_get_destination();
  $options = array();
  foreach ($nodes as $node) {
    $langcode = entity_language('node', $node);
    $l_options = $langcode != LANGUAGE_NONE && isset($languages[$langcode]) ? array('language' => $languages[$langcode]) : array();
    $field_lw_workflow = field_view_field('node', $node, 'lw_workflow', array(
      'label' => 'hidden',
      'type' => 'text_plain',
    ));
    $field_lw_heading = field_view_field('node', $node, 'lw_heading', array(
      'label' => 'hidden',
      'type' => 'text_plain',
    ));
    $field_lw_body = field_view_field('node', $node, 'lw_body', array(
      'label' => 'hidden',
      'type' => 'text_plain',
    ));
    $options[$node->nid] = array(
      'author' => theme('username', array('account' => $node)),
      'status' => $node->status ? t('published') : t('not published'),
      'changed' => format_date($node->changed, 'short'),
      'lw_workflow' => drupal_render($field_lw_workflow),
      'lw_heading' => drupal_render($field_lw_heading),
      'lw_body' => drupal_render($field_lw_body),
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
    if (node_access('update', $node)) {
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => 'node/' . $node->nid . '/edit',
        'query' => $destination,
      );
    }
    if (node_access('delete', $node)) {
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => 'node/' . $node->nid . '/delete',
        'query' => $destination,
      );
    }
    $options[$node->nid]['operations'] = array();
    if (count($operations) > 1) {
      // Render an unordered list of operations links.
      $options[$node->nid]['operations'] = array(
        'data' => array(
          '#theme' => 'links__node_operations',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      );
    }
    elseif (!empty($operations)) {
      // Render the first and only operation as a link.
      $link = reset($operations);
      $options[$node->nid]['operations'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => $link['title'],
          '#href' => $link['href'],
          '#options' => array('query' => $link['query']),
        ),
      );
    }
  }

  // Use tableselect to allow the current user to perform operations.
  $form['nodes'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No content available.'),
  );

  $form['pager'] = array('#markup' => theme('pager'));
  return $form;
}


/**
 * Default form validation handler
 */
function living_word_admin_moderate_form_validate($form, &$form_state) {



}


/**
 * Default form submit handler
 */
function living_word_admin_moderate_form_submit($form, &$form_state) {



}

