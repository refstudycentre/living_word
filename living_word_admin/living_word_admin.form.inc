<?php

/*
 * COMMON FORM HELPER FUNCTIONS
 */


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


/**
 * Get the 'options' array for a select list on a taxonomy term reference field, given the field name
 */
function living_word_admin_get_taxonomy_field_options($field_name) {

  $vocabulary_machine_name = field_info_field($field_name)['settings']['allowed_values'][0]['vocabulary'];

  $query = new EntityFieldQuery();
  $result = $query
    ->entityCondition('entity_type', 'taxonomy_term')
    ->entityCondition('bundle', $vocabulary_machine_name)
    ->execute();

  $options = array();
  if (!empty($result['taxonomy_term'])) {
    foreach(taxonomy_term_load_multiple(array_keys($result['taxonomy_term'])) as $tid => $term) {
      $options[$tid] = $term->name;
    }
  }

  return $options;
}


/**
 * Build a form for lw_detail, which will be included in a table on the moderate page
 */
function living_word_admin_detail_form($form, &$form_state, $node) {

  static $options;
  if (!isset($options)) {
    $options = living_word_admin_get_taxonomy_field_options('lw_detail');
  }

  $default = empty(field_get_items('node', $node, 'lw_detail')[0]['tid']) ? '' : field_get_items('node', $node, 'lw_detail')[0]['tid'];

  $form['lw_detail'] = array(
    '#type'           => 'select',
    '#default_value'  => $default,
    '#options'        => $options,
    '#required'       => FALSE,
    '#empty_value'    => '',
    '#empty_option'   => t('- None -'),
  );

  $form['lw_detail']['#ajax'] = array(
    'callback'  => 'living_word_admin_moderate_form_table_ajax_lw_detail',
    'event'     => 'change',
    'progress'  => array(
      'type'      => 'throbber',
      'message'   => t('Saving...'),
    ),
  );

  return $form;

}


/**
 * Take fields from form and add it to a given node object
 */
function living_word_admin_form_field_node_update($fields, &$node) {

  foreach($fields as $field_name => &$field_value) {

    // Remove anything that is not an actual field value (e.g. "add another" buttons)
    foreach ($field_value as $lang => &$field_items) {
      foreach ($field_items as $delta => &$field_item) {
        if (!is_int($delta)) {
          unset($field_items[$delta]);
        }
      }
    }

    // overwrite fields on the node
    $node->$field_name = $field_value;

  }

}

