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
 */
function living_word_field_instance_config() {
  $t = get_t(); // this function might run during install, or any other time
  $prefix = "lw";
  $bundle = 'living_word_comment';
  $fields = array_keys(field_info_fields());
  $instances = array_keys(field_info_instances('node',$bundle));
  
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
    ));
    $instances[] = $fieldname;
  }

  $fieldname = "{$prefix}_heading";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'  => $fieldname,
      'label'       => $t('Heading'),
      'entity_type' => 'node',
      'bundle'      => $bundle,
      'description' => $t('An optional heading for this commentary (nodes will be grouped by this if present)'),
      'required'    => true,
      'settings'    => array('text_processing' => 0), // disable rich text formats
      'display'     => array(
        'default' => array(
          'label' => 'hidden',
        ),
      ),
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
      'description' => $t('Weight used to order items within a listing'),
      'required'    => true,
      'display'     => array(
        'default' => array(
          'type' => 'hidden',
        ),
      ),
    ));
    $instances[] = $fieldname;
  }

  $settings = variable_get("living_word_vocabs",NULL);
  if (!empty($settings)) {

    $label = array(
      'type'   => $t('Type'),
      'detail' => $t('Detail'),
    );
    
    $description = array(
      'type'   => $t('Type, or position of node in the greater commentary'),
      'detail' => $t('Level of detail'),
    );

    // taxonomy reference field instances
    foreach($settings['vocabs'] as $field => $vid) {             // for each taxonomy reference field
      if ($vid > 0) {                                             // if a vocabulary has been configured
        $vocab = taxonomy_vocabulary_load($vid);
        $fieldname = "{$prefix}_{$field}_{$vocab->machine_name}"; // determine what this field is called
        if (in_array($fieldname, $fields)) {                      // if the field exists

          if (!in_array($fieldname, $instances)) { // if the instance does not exist yet
            field_create_instance(array(           // create the instance
              'field_name'  => $fieldname,
              'label'       => $label[$field],
              'entity_type' => 'node',
              'bundle'      => $bundle,
              'description' => $description[$field],
              'required'    => false,
              'display'     => array(
                'default' => array(
                  'type' => 'hidden',
                ),
              ),
              'widget'      => array('type' => 'options_select'),
            ));
            $instances[] = $fieldname;
          }

        } else {
          drupal_set_message(t('Cannot create instance of non-existing field <strong>@fieldname</strong> on taxonomy bundle <strong>@bundle</strong>.',array('@fieldname'=>$fieldname,'@bundle'=>$vocab->machine_name)),'error');
        }
      }
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
