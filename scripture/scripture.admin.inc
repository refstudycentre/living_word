<?php


/*
 * ADMIN PAGE CALLBACKS
 * Three different strategies for page callbacks: http://www.drupalcoder.com/blog/how-to-define-page-callbacks-that-you-dont-want-to-render-using-page-template-in-drupal.html
 * Build the callbacks using renderable arrays. Good reading on renderable arrays:
 * - https://www.drupal.org/node/930760
 * - http://cocoate.com/ddbook/scary-render-array
 * - http://drupal.stackexchange.com/questions/11438/how-to-nest-elements-in-a-render-array
 */

/**
 * Page callback for admin/structure/scripture[/status]
 */
function scripture_callback_status() {
  $content = array();
  
  // TODO: summarize what is going on in the Scripture db tables, and show it here.
  $content['foo'] = t('Status page');
  
  return $content;
}

/**
 * Page callback for admin/structure/scripture/translations
 */
function scripture_callback_translations() {
  $form = array();
	
	$translations = scripture_get_translations();
	foreach ($translations as $id => &$t) {
		$t = "{$t->abbr} / {$t->lang} / {$t->name}";
	}
	
  $form['scripture_default_translation'] = array(
		'#type' => 'select',
		'#title' => t('Translation'),
		'#options' => $translations,
		'#description' => t('Choose the default translation to use when none is specified.'),
		'#required' => true,
		'#default_value' => array(variable_get('scripture_default_translation',NULL)),
	);
  
  return system_settings_form($form);
}

/**
 * Page callback for admin/structure/scripture/subverses
 */
function scripture_callback_subverses() {
  $content = array();
  
  // TODO: build a form to manage (add, modify, delete) the subverses
  // Make use of the verse picker widget!
  $content['foo'] = t('TODO: admin interface to manage subverses');
  
  return $content;
}

