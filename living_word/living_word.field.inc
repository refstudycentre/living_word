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
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    ));
  }

  $fieldname = "{$prefix}_workflow";
  if (!in_array($fieldname, $fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'list_text',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'settings'    => array(
        'allowed_values' => array(
          'proofread' => '01. Proofread (i.e. checked for linguistic errors)',
          'moderated' => '02. Moderated (i.e. content has been checked and level of detail has been set)',
          'reviewed'  => '03. Reviewed (i.e. content is ordered and ready for publication)',
        ),
      ),
    ));
  }

  $fieldname = "{$prefix}_listing_order";
  if (!in_array($fieldname, $fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'list_text',
      'cardinality' => 1,
      'settings'    => array(
        'allowed_values' => array(
          LIVING_WORD_LISTING_ORDER_PREDOMINANT_MULTI_VERSE => $t('Put predominant multiple verse range first'),
          LIVING_WORD_LISTING_ORDER_SINGLE_SUB_VERSE => $t('Put single verse or subverse first'),
          LIVING_WORD_LISTING_ORDER_APPLICATION_TAGS => $t('Order by application tags'),
        ),
      ),
    ));
  }
  

  $fieldname = "{$prefix}_subverse";
  if (!in_array($fieldname, $fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'living_word_quote',
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
      'widget'      => array('weight' => 10),
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
      'widget'      => array('weight' => 20),
    ));
    $instances[] = $fieldname;
  }

  $fieldname = "{$prefix}_weight";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'    => $fieldname,
      'label'         => $t('Weight'),
      'entity_type'   => 'node',
      'bundle'        => $bundle,
      'description'   => $t('This is a measure of importance, and is used to order items within a listing.'),
      'required'      => true,
      'display'       => array(
        'default' => array(
          'type' => 'hidden',
        ),
      ),
      'widget'        => array('weight' => 30),
      'default_value' => array(array('value' => 0)),
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
        'weight'   => 40,
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

  $fieldname = "{$prefix}_subverse";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'  => $fieldname,
      'label'       => $t('Subverse'),
      'entity_type' => 'node',
      'bundle'      => $bundle,
      'description' => $t('Quote a selection of the scripture verse'),
      'required'    => true,
      'widget'      => array(
        'weight'  => 50,
        'type'    => 'living_word_quote_widget_simple',
      ),
      'display'       => array(
        'default' => array(
          'type' => 'hidden',
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
            'weight' => 60,
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
            'weight' => 70,
          ),
        ));
        $instances[] = $fieldname;
      }
  
    } else {
      drupal_set_message(t('Cannot create instance of non-existing field <strong>@fieldname</strong> on taxonomy bundle <strong>@bundle</strong>.',array('@fieldname'=>$fieldname,'@bundle'=>$vocab->machine_name)),'error');
    }
  
  }

  $fieldname = "{$prefix}_workflow";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'    => $fieldname,
      'label'         => $t('Workflow'),
      'entity_type'   => 'node',
      'bundle'        => $bundle,
      'description'   => $t('The progress of this LWC node through the content workflow.'),
      'required'      => FALSE,
      'display'       => array(
        'default' => array(
          'type' => 'hidden',
        ),
      ),
      'widget'        => array(
        'weight'  => 80,
        'type'    => 'options_buttons',
      ),
    ));
    $instances[] = $fieldname;
  }

  $fieldname = "{$prefix}_listing_order";
  if (!in_array($fieldname, $instances) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'    => $fieldname,
      'label'         => $t('Listing order'),
      'entity_type'   => 'taxonomy_term',
      'bundle'        => 'living_word_position',
      'description'   => $t('How nodes in this position should be sorted.'),
      'required'      => FALSE,
      'display'       => array(
        'default' => array(
          'type' => 'hidden',
        ),
      ),
      'widget'        => array(
        'weight'  => 90,
        'type'    => 'options_buttons',
      ),
    ));
    $instances[] = $fieldname;
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
