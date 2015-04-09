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
 */
function scripture_field_info() {
  $fields = array();

  $fields['scripture'] = array(
    'label'             => t('Scripture reference'),
    'description'       => t('A field that references a range of bible verses from a specific translation'),
    /*'settings'          => array(
      'setting1' => 'default1',
      'setting2' => 'default2',
    ),
    'instance_settings' => array(
      'instance_setting1' => 'instance_default1',
      'instance_setting2' => 'instance_default2',
    ),*/
    'default_widget'    => 'scripture_picker',
    'default_formatter' => 'scripture_formatter_plain',
  );

  return $fields;
}

/**
 * Implements hook_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors).
 * https://api.drupal.org/api/drupal/modules!field!field.api.php/function/hook_field_validate/7
 * Validate this module's field data.
 * If there are validation problems, add to the $errors array (passed by reference). There is no return value.
 */
function scripture_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
	// required fields are already individually checked by FAPI
	
	foreach($items as $delta => &$item) {
		
		if (!empty($item['to_chapter']) && empty($item['to_verse'])) {
			$errors [$field ['field_name']][$langcode][$delta][] = array(
		    'error' => 'scripture_negative_range',
		    'message' => t('%name: You filled in a chapter, but not a verse', array('%name' => $instance ['label'])),
		  );
		}
		
		if (empty($item['to_chapter'])) {
				
			// use the from_chapter value if to_chapter was empty
			$item['to_chapter'] = $item['from_chapter'];
			
			if (empty($item['to_verse'])) {
				// use the from_verse value if both to_chapter and to_verse were empty
				$item['to_verse'] = $item['from_verse'];
			}

		}
		
		// check if the verse range is positive
		if ($item['from_chapter'] > $item['to_chapter'] || ( $item['from_chapter'] == $item['to_chapter'] && $item['from_verse'] > $item['to_verse'] )) {
			$errors [$field ['field_name']][$langcode][$delta][] = array(
		    'error' => 'scripture_negative_range',
		    'message' => t('%name: First verse and chapter must be before the last verse and chapter.', array('%name' => $instance ['label'])),
		  );
		}
		
		// check whether a specific translation was selected
		if (empty($item['translation']) || $item['translation'] == 'N/A') {
			$item['translation'] = NULL;
		}
		
		// get the verse ids using the above information
		$item['from_vid'] = scripture_get_vid($item['translation'],$item['book'],$item['from_chapter'],$item['from_verse']);
		$item['to_vid'] = scripture_get_vid($item['translation'],$item['book'],$item['to_chapter'],$item['to_verse']);
		
		// check if the verses exist in the database
		if (empty($item['from_vid']) || empty($item['to_vid'])) {
			$errors [$field ['field_name']][$langcode][$delta][] = array(
		    'error' => 'scripture_verse_dne',
		    'message' => t('%name: The first or last verse in the range does not exist.', array('%name' => $instance ['label'])),
		  );
		}
		
		// check whether the range is a single verse
		if (empty($item['sid']) || $item['to_vid'] != $item['from_vid']) {
			$item['sid'] = NULL;
		} else {
			// TODO: check if subverse exists
		}
		
	}
	// TODO: can we cache the above results for reuse in the presave function?
}

/**
 * Implements hook_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items).
 * https://api.drupal.org/api/drupal/modules!field!field.api.php/function/hook_field_presave/7
 * Define custom presave behavior for this module's field types.
 * Make changes or additions to field values by altering the $items parameter by reference. There is no return value.
 */
function scripture_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
	
	foreach($items as $delta => &$item) {
	
		if (empty($item['to_chapter'])) {
				
			// use the from_chapter value if to_chapter was empty
			$item['to_chapter'] = $item['from_chapter'];
			
			if (empty($item['to_verse'])) {
				// use the from_verse value if both to_chapter and to_verse were empty
				$item['to_verse'] = $item['from_verse'];
			}

		}
	
		// check whether a specific translation was selected
		if (empty($item['translation']) || $item['translation'] == 'N/A') {
			$item['translation'] = NULL;
		}
		
		// get the verse ids using the above information
		$item['from_vid'] = scripture_get_vid($item['translation'],$item['book'],$item['from_chapter'],$item['from_verse']);
		$item['to_vid'] = scripture_get_vid($item['translation'],$item['book'],$item['to_chapter'],$item['to_verse']);
		
		// check whether the range is a single verse
		if (empty($item['sid']) || $item['to_vid'] != $item['from_vid']) {
			$item['sid'] = NULL;
		}

	}

}


/*
 * FIELD FORMATTER
 * https://api.drupal.org/api/drupal/modules!field!field.api.php/group/field_formatter/7
 */

/**
 * Implements hook_field_formatter_info()
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_formatter_info/7
 * TODO: make configurable setting to show or hide the verse text
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
   * TODO: load multiple verses, single verse, or subverse from database, based on settings.
   * Use db_select:  https://api.drupal.org/api/drupal/includes!database!database.inc/function/db_select/7
   */
}

/**
 * Implements hook_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display)
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_formatter_view/7
 * build a renderable array to display the field value
 * Good reading on renderable arrays:
 * - https://www.drupal.org/node/930760
 * - http://cocoate.com/ddbook/scary-render-array
 * - http://drupal.stackexchange.com/questions/11438/how-to-nest-elements-in-a-render-array
 */
function scripture_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $contents = array();
	
  foreach($items as $delta => $i) {
  	$from_verse = scripture_get_verse($i['from_vid']);
		$to_verse = scripture_get_verse($i['to_vid']);
		
		$contents[$delta] = array(
			'#markup' => scripture_str_verse_range($from_verse, $to_verse, $i['sid']),
		);
	}
	
	return $contents;
}

function scripture_str_verse_range($from_verse,$to_verse,$sid = NULL,$bookname = NULL) {
	if (empty($bookname)) {
		$book = scripture_get_book($from_verse);
		$bookname = $book['bookname'];
	}
	
	if ($from_verse['vid'] == $to_verse['vid']) {
		if (empty($sid)) {
			return "{$book['bookname']} {$from_verse['chapternum']}:{$from_verse['versenum']}";
		} else {
			return "{$book['bookname']} {$from_verse['chapternum']}:{$from_verse['versenum']}.{$sid}";
		}
	} else {
		return "{$book['bookname']} {$from_verse['chapternum']}:{$from_verse['versenum']} - {$to_verse['chapternum']}:{$to_verse['versenum']}";
	}
	
}


/*
 * FIELD WIDGET
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/group/field_widget/7
 */

/**
 * Implements hook_field_widget_info().
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_widget_info/7
 * TODO: will the scripture picker also allow entry of new subverses? probably not...
 */
function scripture_field_widget_info() {
  $widgets = array();

  $widgets['scripture_picker'] = array(
    'label'        => t('Scripture picker'),
    'description'  => t('A compound form element for editing scripture references'),
    'field types'  => array('scripture'),
/*    'settings'     => array(
      'widget_setting1' => 'widget_default1',
      'widget_setting2' => 'widget_default2',
    ),*/
    'behaviours'   => array(
      'multiple values' => FIELD_BEHAVIOR_DEFAULT, // the entire widget accepts one entry
      'default_value'   => FIELD_BEHAVIOR_NONE,    // the widget can have no default value
    ),
  );

  return $widgets;
}

/**
 * Implements hook_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element).
 * https://api.drupal.org/api/drupal/modules%21field%21field.api.php/function/hook_field_widget_form/7
 * create the complex form element that constitutes the widget, using basic form elements.
 */
function scripture_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  
  // this widget only supports single field values. Get the values to populate the form
  if (!empty($items[0])) {
  	$translation = ($items[0]['translation'] > 0) ? $items[0]['translation'] : NULL;
		$from_verse = scripture_get_verse($items[0]['from_vid'],$translation);
		$to_verse = scripture_get_verse($items[0]['to_vid'],$translation);
		$book = $from_verse['booknum'];
		$from_chapter = $from_verse['chapternum'];
		$from_verse = $from_verse['versenum'];
		$to_chapter = $to_verse['chapternum'];
		$to_verse = $to_verse['versenum'];
		$sid = $items[0]['sid'];
	} else {
		$translation = 'N/A';
		$book = NULL;
		$from_chapter = '';
		$from_verse = '';
		$to_chapter = '';
		$to_verse = '';
		$sid = '';
	}
  
  $translations = scripture_get_translations();
	foreach ($translations as $id => &$t) {
		$t = "{$t->abbr} / {$t->lang} / {$t->name}";
	}
	
	$books = scripture_get_books($translation);
	foreach ($books as $booknum => &$b) {
		$b = "{$booknum}. {$b->bookname}";
	}
  
	// TODO: load a preview using AJAX while editing the form
	
  return array(
  	'#type' => 'fieldset',
  	'#title' => $instance['label'],
  	'translation' =>  array(
  		'#type' => 'select',
  		'#title' => t('Translation'),
  		'#options' => array(0=>'N/A') + $translations,
  		'#description' => t('Choose N/A if this field does not reference a specific translation.'),
  		'#required' => false,
  		'#default_value' => $translation,
		),
  	'book' => array(
  		'#type' => 'select',
  		'#title' => t('Book'),
  		'#options' => $books,
  		'#required' => true,
  		'#default_value' => $book,
		),
  	'from_chapter' => array(
  		'#type' => 'textfield',
  		'#title' => t('From chapter'),
  		'#description' => t('First chapter in the range.'),
  		'#size' => 3,
  		'#maxlength' => 3, 
  		'#required' => true,
  		'#default_value' => $from_chapter,
		),
  	'from_verse' => array(
  		'#type' => 'textfield',
  		'#title' => t('From verse'),
  		'#description' => t('First verse in the range.'),
  		'#size' => 3,
  		'#maxlength' => 3,
  		'#required' => true,
  		'#default_value' => $from_verse,
		),
  	'to_chapter' => array(
  		'#type' => 'textfield',
  		'#title' => t('To chapter'),
  		'#description' => t('Last chapter in the range. Leave blank to use the "from chapter" value.'),
  		'#size' => 3,
  		'#maxlength' => 3, 
  		'#required' => false,
  		'#default_value' => $to_chapter,
		),
  	'to_verse' => array(
  		'#type' => 'textfield',
  		'#title' => t('To verse'),
  		'#description' => t('Last verse in the range. Leave this and "To chapter" blank to use the "from verse" value.'),
  		'#size' => 3,
  		'#maxlength' => 3,
  		'#required' => false,
  		'#default_value' => $to_verse,
		),
  	'sid' => array(
  		'#type' => 'textfield',
  		'#title' => t('Subverse'),
  		'#description' => t('This will be ignored if the first and last verses and chapters are not the same.'),
  		'#size' => 1,
  		'#maxlength' => 3,
  		'#required' => false,
  		'#default_value' => $sid,
		),
	);
}


/**
 * Implements hook_field_is_empty($item, $field).
 * https://api.drupal.org/api/drupal/modules!field!field.api.php/function/hook_field_is_empty/7
 * Define what constitutes an empty item for a field type.
 */
function scripture_field_is_empty($item, $field) {
	return empty($item['book']) || empty($item['from_chapter']) || empty($item['from_verse']);
}


