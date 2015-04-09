<?php

/**
 * Get all known translations
 * @return array:
 *   An array like follows:
 *   array(
 *     id => object(
 *       id   => 'translation id'
 *       abbr => 'translation abbreviation',
 *       name => 'translation name',
 *       lang => 'langcode',
 *     ),
 *     ...,
 *   )
 */
function scripture_get_translations() {
  return db_select('lw_translations','t')
		->fields('t')
		->execute()
		->fetchAllAssoc('id');
}

/**
 * Retrieve verses from the db based on vid
 * @return array:
 *   An array like follows:
 *   array(
 *     vid => object(
 *       vid   => 'verse id',
 *       
 *     ),
 *     ...,
 *   )
 */
function scripture_get_verse($vid,$translation = NULL) {
	if (empty($translation))
		$translation = variable_get('scripture_default_translation',NULL);
	
	if (!empty($translation))
	  return db_select('lw_verses','v')
			->fields('v')
			->condition('vid',$vid)
			->condition('translation',$translation)
			->range(0,1)
			->execute()
			->fetchAssoc();
	else
		return array();
	
}

/**
 * Retrieve matching vid from the db
 */
function scripture_get_vid($translation,$booknum,$chapternum,$versenum) {
	if (empty($translation))
		$translation = variable_get('scripture_default_translation',NULL);
		
  return db_select('lw_verses','v')
		->fields('v',array('vid'))
		->condition('translation',$translation)
		->condition('booknum',$booknum)
		->condition('chapternum',$chapternum)
		->condition('versenum',$versenum)
		->range(0,1)
		->execute()
		->fetchField();
}

/**
 * Get all book names, keyed by booknumber, for a specific translation
 */
function scripture_get_books($translation = NULL) {
	if (empty($translation))
		$translation = variable_get('scripture_default_translation',NULL);
	
	if (!empty($translation))
	  return db_select('lw_books','b')
			->fields('b')
			->condition('translation',$translation)
			->execute()
			->fetchAllAssoc('booknum');
	else
		return array();
}

/**
 * Get book which contains given verse
 */
function scripture_get_book($verse) {
	if (empty($verse['translation']))
		$translation = variable_get('scripture_default_translation',NULL);
	else
		$translation = $verse['translation'];
	
	if (!empty($translation))
	  return db_select('lw_books','b')
			->fields('b')
			->condition('booknum',$verse['booknum'])
			->condition('translation',$translation)
			->range(0,1)
			->execute()
			->fetchAssoc();
	else
		return array();
}