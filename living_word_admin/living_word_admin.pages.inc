<?php

/*
 * USER PAGE CALLBACKS
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