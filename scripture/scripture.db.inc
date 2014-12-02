<?php


/*
 * TRANSLATIONS
 */

/**
 * Get all known translations
 * @param boolean $installed
 *   NULL = do not filter
 *   TRUE = only get installed translations
 *   FALSE = only get non-installed translatiosn
 * @return array:
 *   An array like follows:
 *   array(
 *     name => array(
 *       fullname  => 'full name of translation',
 *       installed => boolean,
 *       language  => 'langcode',
 *     ),
 *     ...,
 *   )
 */
function scripture_get_translations($installed = NULL) {
  /*
   * TODO: perform a SELECT query and return an array
   */
  return array();
}

/**
 * Imports a bible translation and adds it to the database
 */
function scripture_import_translation() {
  /*
   * TODO: Design decision:
   * 1: Import sword files directly on the website
   * 1.1: ... by creating a bridge between The SWORD Engine (written in CPP; http://www.crosswire.org/sword/develop/index.jsp) and PHP
   * 1.2: ... by implementing the SWORD API in PHP
   * 2: Write a CPP app that uses the SWORD Engine, extracts the bible info, and generates a csv or sql file which we can import
   * 3: Abandon this functionality
   *
   * No 2 is probably MUCH less work.
   * Open bible translations available here: http://www.crosswire.org/sword/modules/ModDisp.jsp?modType=Bibles
   * 
   * Don't implement this yet, I'm considering No3.
   */

  /*
   * TODO: Accept file name as parameter. Use a batch operation
   * (docs: https://api.drupal.org/api/drupal/includes%21form.inc/group/batch/7)
   * (example: https://www.drupal.org/node/180528)
   * The file should be uploaded in the admin interface. This function should
   * be called when the upload form succeeds.
   * Create an entry for this translation in the translations table if it does not yet exist.
   * If the translation is already installed, do nothing
   * Otherwise, create a verses table and a subverses table using db_create_table (https://api.drupal.org/api/drupal/includes%21database%21database.inc/function/db_create_table/7)
   * Fill in the verses table using the imported sword data.
   * Mark the translation as installed.
   * Remove the imported file
   */
}


/*
 * VERSES & SUBVERSES
 */

/**
 * Adds new subverses to the db
 * @param array $new
 *   An array of new subverses
 *   TODO: specify a format for this array that is convenient to pass to the fields method after db_insert
 *   see heading "Multi-insert form" at https://www.drupal.org/node/310079
 * @param boolean $overwrite
 *   Specifies whether existing subverses should be modified or not
 */
function scripture_subverses_add(array $new = array(), $overwrite = TRUE) {
  /*
   * TODO:
   * If overwrite is true
   *   DELETE existing subverses that conflict with those in $new
   * else
   *   unset existing subverses from the $new array
   * Do a multiple INSERT with db_insert
   */
}

/**
 * Retrieves subverses from the db, based on criteria
 * @param array $crit
 *   An array of optional WHERE criteria
 *   TODO: specify a format which is convenient for passing to db_query (apparently faster than db_select)
 *   see https://www.drupal.org/node/310072
 *   Note on static vs. dynamic queries in Drupal: https://www.drupal.org/node/310075
 * @return array:
 *   An array like follows:
 *   array(
 *     vid => array(
 *       sid => text,
 *       ...,
 *     ),
 *     ...,
 *   )
 */
function scripture_subverses_get(array $crit = array()) {
  /*
   * TODO: perform a SELECT and return an array
   */
  return array();
}

/**
 * Retrieve available verses from the db, with or without their subverses
 * @param array $crit
 *   
 * @return array:
 *   An array like follows:
 *   array(
 *     vid => array(
 *       'bookname'   => bookname,
 *        'booknumber' => booknumber,
 *        'chapter'    => chapter,
 *       'verse'      => verse,
 *        'subverses'  => array( // or NULL if not loaded; or empty array if none available
 *          sid => text,
 *         ...,
 *       ),
 *     ),
 *     ...,
 *   )
 */
function scripture_verses_get(array $crit = array(), $subverses = TRUE) {
  /*
   * TODO: perform a SELECT query and return an array
   */
  return array();
}

/**
 * 
 * @param string $translation
 *   name of the translation, e.g. 'esv' or 'aov'
 * @return array:
 *   An array like follows:
 *   array( // ordered by booknumber
 *     bookname => array(
 *       chapter => array(
 *         verse,
 *         ...,
 *       ),
 *       ...,
 *     ),
 *     ...,
 *   )
 */
function scripture_get_structure($translation = "") {
  /*
   * TODO: perform a SELECT query (or queries?) and return an array
   */
  return array();
}

