<?php


/*
 * FIELDS
 */

/**
 * Create fields
 */
function living_word_field_config() {
  $t = get_t(); // this function might run during install, or any other time

  $fieldnames = array_keys(field_info_fields());
  $prefix = 'lw';

  $fieldname = "{$prefix}_heading";
  if (!in_array($fieldname,$fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'text',
      'cardinality' => 1,
    ));
  }

  $fieldname = "{$prefix}_body";
  if (!in_array($fieldname,$fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'text_long',
      'cardinality' => 1,
    ));
  }

  $fieldname = "{$prefix}_weight";
  if (!in_array($fieldname,$fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'number_integer',
      'cardinality' => 1,
    ));
  }

  $fieldname = "{$prefix}_type";
  if (!in_array($fieldname, $fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'taxonomy_term_reference',
      'cardinality' => 1,
      'settings'    => array(
        'allowed_values' => array(
          array(
            'vocabulary' => 'living_word_position',
            'parent'     => '0',
          ),
        ),
      ),
    ));
  }
  
  $fieldname = "{$prefix}_detail";
  if (!in_array($fieldname, $fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'taxonomy_term_reference',
      'cardinality' => 1,
      'settings'    => array(
        'allowed_values' => array(
          array(
            'vocabulary' => 'living_word_detail',
            'parent'     => '0',
          ),
        ),
      ),
    ));
  }
  
  $fieldname = "{$prefix}_scripture";
  if (!in_array($fieldname,$fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'scripture',
      'cardinality' => 1,
    ));
  }
  
}


/**
 * Configure field instances
 * To order, set [instance][widget][weight] property (see https://api.drupal.org/comment/13254#comment-13254 )
 */
function living_word_field_instance_config() {
  $t = get_t(); // this function might run during install, or any other time
  $prefix = "lw";
  $bundle = 'living_word_comment';
  $fields = array_keys(field_info_fields());
  $instances = array_keys(field_info_instances('node',$bundle));

  $fieldname = "{$prefix}_heading";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'  => $fieldname,
      'label'       => $t('Heading'),
      'entity_type' => 'node',
      'bundle'      => $bundle,
      'description' => $t('An optional heading for this commentary (nodes will be grouped by this if present)'),
      'required'    => FALSE,
      'settings'    => array('text_processing' => 0), // disable rich text formats
      'display'     => array(
        'default' => array(
          'label' => 'hidden',
        ),
      ),
      'widget'      => array('weight' => 0),
    ));
    $instances[] = $fieldname;
  }
  
  $fieldname = "{$prefix}_body";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'  => $fieldname,
      'label'       => $t('Body'),
      'entity_type' => 'node',
      'bundle'      => $bundle,
      'description' => $t('The actual piece of commentary'),
      'required'    => true,
      'settings'    => array('text_processing' => 0), // disable rich text formats
      'display'     => array(
        'default' => array(
          'label' => 'hidden',
        ),
      ),
      'widget'      => array('weight' => 1),
    ));
    $instances[] = $fieldname;
  }

  $fieldname = "{$prefix}_weight";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'  => $fieldname,
      'label'       => $t('Weight'),
      'entity_type' => 'node',
      'bundle'      => $bundle,
      'description' => $t('This is a measure of importance, and is used to order items within a listing.'),
      'required'    => true,
      'display'     => array(
        'default' => array(
          'type' => 'hidden',
        ),
      ),
      'widget'      => array('weight' => 2),
    ));
    $instances[] = $fieldname;
  }

  $fieldname = "{$prefix}_scripture";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'  => $fieldname,
      'label'       => $t('Scripture'),
      'entity_type' => 'node',
      'bundle'      => $bundle,
      'description' => $t('Reference one or more verses from the Bible'),
      'required'    => true,
      'widget'      => array(
        'weight'   => 3,
//         'type'     => 'scripture_picker',
//         'settings' => array(
//         ),
      ),
      'display'     => array(
        'default' => array(
          'label'    => 'hidden',
          'type'     => 'scripture_formatter_plain',
          'settings' => array(
            'settings' => array(
              'show_text' => SCRIPTURE_SHOW_TEXT_TRUNC,
              'show_ref'  => 1
            )
          ),
        ),
      ),
    ));
    $instances[] = $fieldname;
  }

  $settings = variable_get("living_word_vocabs",NULL);
  
  // load vocabulary for position
  // TODO: should this machine name be configurable?
  $vocab = taxonomy_vocabulary_machine_name_load('living_word_position');
  if (!empty($vocab)) {
  
    // create taxonomy reference field instance
    $fieldname = "{$prefix}_type";
    if (in_array($fieldname, $fields)) {                      // if the field exists

      if (!in_array($fieldname, $instances)) { // if the instance does not exist yet
        field_create_instance(array(           // create the instance
          'field_name'  => $fieldname,
          'label'       => $t('Type'),
          'entity_type' => 'node',
          'bundle'      => $bundle,
          'description' => $t('Type, or position of node in the greater commentary'),
          'required'    => false,
          'display'     => array(
            'default' => array(
              'type' => 'hidden',
            ),
          ),
          'widget'      => array(
            'type'   => 'options_select',
            'weight' => 4,
          ),
        ));
        $instances[] = $fieldname;
      }

    } else {
      drupal_set_message(t('Cannot create instance of non-existing field <strong>@fieldname</strong> on taxonomy bundle <strong>@bundle</strong>.',array('@fieldname'=>$fieldname,'@bundle'=>$vocab->machine_name)),'error');
    }
  
  }
  
  // load vocabulary for detail
  // TODO: should this machine name be configurable?
  $vocab = taxonomy_vocabulary_machine_name_load('living_word_detail');
  if (!empty($vocab)) {
  
    // create taxonomy reference field instance
    $fieldname = "{$prefix}_detail";
    if (in_array($fieldname, $fields)) {                      // if the field exists
  
      if (!in_array($fieldname, $instances)) { // if the instance does not exist yet
        field_create_instance(array(           // create the instance
          'field_name'  => $fieldname,
          'label'       => $t('Detail'),
          'entity_type' => 'node',
          'bundle'      => $bundle,
          'description' => $t('Level of detail'),
          'required'    => false,
          'display'     => array(
            'default' => array(
              'type' => 'hidden',
            ),
          ),
          'widget'      => array(
            'type'   => 'options_select',
            'weight' => 5,
          ),
        ));
        $instances[] = $fieldname;
      }
  
    } else {
      drupal_set_message(t('Cannot create instance of non-existing field <strong>@fieldname</strong> on taxonomy bundle <strong>@bundle</strong>.',array('@fieldname'=>$fieldname,'@bundle'=>$vocab->machine_name)),'error');
    }
  
  }
  
}

/**
 * Delete unused fields (fields without instances) that start with lw_
 */
function living_word_field_cleanup() {
  $fields = array_keys(field_info_fields());
  $used_fields = array_keys(field_info_field_map());
  $unused_fields = array_diff($fields,$used_fields);

  foreach ($unused_fields as $field) {
    if (substr($field,0,3) === 'lw_') {
      field_delete_field($field);
    }
  }
}
