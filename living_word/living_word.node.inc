<?php


/*
 * CONTENT TYPES
 */

/**
 * Implements hook_node_info().
 * https://api.drupal.org/api/drupal/modules!node!node.api.php/function/hook_node_info/7
 * Defines content types
 */
function living_word_node_info() {
  $types = array();

  $types['living_word_comment'] = array(
    'name'           => t('Living Word Comment'),
    'base'           => 'living_word_comment',
    'description'    => t('The smallest unit of information stored by the Living Word modules'),
    'locked'         => TRUE, // admin cannot change machine name
    'has_title'      => FALSE,
  );

  return $types;
}

/**
 * Implements hook_form($node, &$form_state).
 * https://api.drupal.org/api/drupal/modules!node!node.api.php/function/hook_form/7
 *
 * Allows the content type defined above to show up at 'admin/structure/types'
 * TODO: do we actually want this? disable?
 */
function living_word_comment_form($node, $form_state) {
  return node_content_form($node, $form_state);
}

/**
 * (re)set content type settings
 */
function living_word_content_type_settings() {
  variable_set("node_preview_living_word_comment", 1);                  // Optional preview
  variable_set("node_options_living_word_comment", array());           // default unpublished
  variable_set("node_submitted_living_word_comment", 0);                // Unset 'Display author and date information'.
  variable_set("comment_living_word_comment", COMMENT_NODE_CLOSED);     // close comments
  variable_set("menu_options_living_word_comment", array());           // no available menus
}

