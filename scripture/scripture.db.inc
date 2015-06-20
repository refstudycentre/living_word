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
  return db_select('lw_translations', 't')->fields('t')->execute()->fetchAllAssoc('id');
}

/**
 * Retrieve verse from the db based on vid
 */
function scripture_get_verse($vid, $translation = NULL) {
  if (empty($translation))
    $translation = variable_get('scripture_default_translation', NULL);
  
  if (!empty($translation))
    return db_select('lw_verses', 'v')->fields('v')->condition('vid', $vid)->condition('translation', $translation)->range(0, 1)->execute()->fetchAssoc();
  else
    return array();
}

/**
 * Retrieve all verses in a range from the db based on vid
 */
function scripture_get_verses_between($from_vid, $to_vid, $translation = NULL) {
  
  if (empty($translation))
    $translation = variable_get('scripture_default_translation', NULL);
  
  $from_verse = scripture_get_verse($from_vid, $translation);
  $to_verse = scripture_get_verse($to_vid, $translation);
  $book = scripture_get_book($from_verse);
  
  if (!empty($translation) && !empty($book)) {
    
    if ($from_verse['chapternum'] == $to_verse['chapternum']) {
      
      // The query is much simpler if the range is within one chapter:
      $range_condition = db_and()
        ->condition('chapternum', $from_verse['chapternum'])
        ->condition('versenum', $from_verse['versenum'], '>=')
        ->condition('versenum', $to_verse['versenum'], '<=');
      
    } else {
      
      /*
       * The range spans multiple chapters
       * building this condition:
       * (
       *   (
       *     chapternum > from_chapternum &&
       *     chapternum < to_chapternum
       *   ) ||
       *   (
       *     chapternum = from_chapternum &&
       *     versenum >= from_versenum
       *   ) ||
       *   (
       *     chapternum = to_chapternum &&
       *     versenum <= to_versenum
       *   )
       * )
       */
      
      $middle_chapters = db_and()
        ->condition('chapternum', $from_verse['chapternum'], '>')
        ->condition('chapternum', $to_verse['chapternum'], '<');
      
      $first_chapter = db_and()
        ->condition('chapternum', $from_verse['chapternum'])
        ->condition('versenum', $from_verse['versenum'], '>=');
      
      $last_chapter = db_and()
        ->condition('chapternum', $to_verse['chapternum'])
        ->condition('versenum', $to_verse['versenum'], '<=');
      
      $range_condition = db_or()
        ->condition($middle_chapters)
        ->condition($first_chapter)
        ->condition($last_chapter);
      
    }
    
    return db_select('lw_verses', 'v')
    ->fields('v')
    ->condition('translation', $translation)
    ->condition('booknum', $book['booknum'])
    ->condition($range_condition)
    ->orderBy('chapternum', 'ASC')
    ->orderBy('versenum', 'ASC')
    ->execute()
    ->fetchAllAssoc('vid');
    
  
  } else {
    return array();
  }
}

/**
 * Retrieve matching vid from the db
 */
function scripture_get_vid($translation, $booknum, $chapternum, $versenum) {
  if (empty($translation))
    $translation = variable_get('scripture_default_translation', NULL);
  
  // TODO: vid should be the same accross translations, so the translation condition has to be removed if translation is NULL.
  // If translation is set, this function can be used to check if that verse exists in a specific translation.
  return db_select('lw_verses', 'v')
    ->fields('v', array('vid'))
    ->condition('translation', $translation)
    ->condition('booknum', $booknum)
    ->condition('chapternum', $chapternum)
    ->condition('versenum', $versenum)
    ->range(0, 1)
    ->execute()
    ->fetchField();
}

/**
 * Get all book names, keyed by booknumber, for a specific translation
 */
function scripture_get_books($translation = NULL) {
  if (empty($translation))
    $translation = variable_get('scripture_default_translation', NULL);
  
  if (!empty($translation))
    return db_select('lw_books', 'b')->fields('b')->condition('translation', $translation)->execute()->fetchAllAssoc('booknum');
  else
    return array();
}

/**
 * Get book which contains given verse
 */
function scripture_get_book($verse) {
  if (empty($verse['translation']))
    $translation = variable_get('scripture_default_translation', NULL);
  else
    $translation = $verse['translation'];
  
  if (!empty($translation))
    return db_select('lw_books', 'b')->fields('b')->condition('booknum', $verse['booknum'])->condition('translation', $translation)->range(0, 1)->execute()->fetchAssoc();
  else
    return array();
}