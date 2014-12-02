<?php

// Field API docs: https://api.drupal.org/api/drupal/modules%21field%21field.module/group/field/7
// Bird's eye view of creating a custom field type: http://clikfocus.com/blog/how-set-custom-field-type-using-drupal-7-fields-api


/*
 * FIELD TYPE
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/group/field_types/7
 */

/**
 * Implements hook_field_info().
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_info/7
 * TODO: default settings
 */
function scripture_field_info() {
  $fields = array()

  $fields['scripture'] = array(
    'label'             => t('Scripture reference'),
    'description'       => t('A field that references a range of bible verses from a specific translation'),
    'settings'          => array(
      'setting1' => 'default1',
      'setting2' => 'default2',
    ),
    'instance_settings' => array(
      'instance_setting1' => 'instance_default1',
      'instance_setting2' => 'instance_default2',
    ),
    'default_widget'    => 'scripture_picker',
    'default_formatter' => 'scripture_formatter_plain',
  );

  return $fields
}


/*
 * FIELD FORMATTER
 * https://api.drupal.org/api/drupal/modules!field!field.api.php/group/field_formatter/7
 */

/**
 * Implements hook_field_formatter_info()
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_formatter_info/7
 * TODO: default settings
 */
function scripture_field_formatter_info() {
  $formatters = array();

  $formatters['scripture_formatter_plain'] = array(
    'label'       => t('Plain-text scripture reference'),
    'description' => t('A plain-text formatter for scripture references'),
    'field types' => array('scripture'),
    'settings'    => array(
      'formatter_setting1' => 'formatter_default1',
      'formatter_setting2' => 'formatter_default2',
    ),
  );

  return $formatters;
}

/**
 * Implements hook_field_formatter_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items, $displays)
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_formatter_prepare_view/7
 */
function scripture_field_formatter_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items, $displays) {
  /*
   * TODO: load subverse from database if applicable.
   * Use db_select:  https://api.drupal.org/api/drupal/includes!database!database.inc/function/db_select/7
   */
}

/**
 * Implements hook_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display)
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_formatter_view/7
 */
function scripture_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  /*
   * TODO: build a renderable array to display the field value
   * Good reading on renderable arrays:
   * - https://www.drupal.org/node/930760
   * - http://cocoate.com/ddbook/scary-render-array
   * - http://drupal.stackexchange.com/questions/11438/how-to-nest-elements-in-a-render-array
   */
}


/*
 * FIELD WIDGET
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/group/field_widget/7
 */

/**
 * Implements hook_field_widget_info().
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_widget_info/7
 * TODO: default settings
 * TODO: will the scripture picker also allow entry of new subverses?
 */
function scripture_field_widget_info() {
  $widgets = array();

  $widgets['scripture_picker'] = array(
    'label'        => t('Scripture picker'),
    'description'  => t('A compound form element for editing scripture references'),
    'field types'  => array('scripture'),
    'settings'     => array(
      'widget_setting1' => 'widget_default1',
      'widget_setting2' => 'widget_default2',
    ),
    'behaviours'   => array(
      'multiple values' => FIELD_BEHAVIOUR_DEFAULT, // the entire widget accepts one entry
      'default_value'   => FIELD_BEHAVIOUR_NONE,    // the widget can have no default value
    ),
  );

  return $widgets;
}

/**
 * Implements hook_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element).
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_widget_form/7
 */
function scripture_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  /*
   * TODO: create the complex form element that constitutes the widget, using basic form elements.
   * You will probably need a custom validation function: http://drupal.stackexchange.com/a/64056
   * Drupal Form API reference: https://api.drupal.org/api/drupal/developer%21topics%21forms_api_reference.html/7
   */
}

