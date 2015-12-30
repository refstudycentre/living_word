<?php

/*
 * USER PAGE CALLBACKS
 */

/**
 * Page callback for browse function in living word
 * Summary of all nodes
 */
function living_word_ui_browsepage($param) {
	
	$booknum = $param;
	# assert 1<= $booknum <= 66;
    $translation = variable_get('scripture_default_translation', NULL);
	
	$bookname = db_query("SELECT bookname FROM lw_books WHERE translation=:trans AND booknum=:booknum", array( 
		':trans' => $translation, 
		':booknum' => $booknum))->fetchField();
	
	$content = "<div class='lwc-browse-book'>".t("Browsing Living Word Comments for")." {$bookname}.</div>";
	
	

	return $content;
}





function living_word_ui_browsemain() {
	# add the css for this page
	drupal_add_css(drupal_get_path('module', 'living_word_ui').'/living_word_ui.css');	
	
	$content = t("Listing the number of Living Word Comments for each bible book that is currently in the system");

  //TODO:  Get the user default translation, and replace the hardcoded ESV below with the users' chosen book names
    $translation = variable_get('scripture_default_translation', NULL);
  
  // Then list the book names, with the number of lwcomments for each book
  $rst = db_query("SELECT booknum, bookname, coalesce(NrLWC, 0) as NrLWC                   
				   FROM lw_books 
				   LEFT JOIN (
						SELECT ceiling(lw_scripture_from_vid / 726353) as booknum, count(*) as NrLWC 
						FROM field_data_lw_scripture 
						GROUP BY booknum) cbb USING (booknum)
				   WHERE translation='{$translation}' 
				   ORDER BY booknum");

  $content .= "<div id='lwc-browse-main'><ul>";
  foreach ($rst as $row) {
    $content .= "<li class='lwc-browse-count-{$row->NrLWC}'><a href='/browse/{$row->booknum}'>{$row->bookname} <span class='lwc-browse-count'>({$row->NrLWC})</span></a></li>";
  }			   
  $content .= "</ul></div>";
				   
  // detail table:  s
  // Links on each bible book to /browse/[n] where [n]= biblebookid
  
  return $content;
}
