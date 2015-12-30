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
	
	/* 	Return a table like this:
	=============================
	Scripture Section	Intro	Eksegese	Application
    Genesis 1:1-31	    1 note	34 notes	21 notes
    Genesis 2:1-55	    1 note	8 notes	    11 notes
    ...
	
	Difficult problem, because LWC has Ranges, and Chunks has Ranges.  So they could have multiple nodes... */
	
	$rst = db_query("SELECT c.from_vid, c.to_vid, v1.chapternum as chap1, v1.versenum as vers1, v2.chapternum as chap2, v2.versenum as vers2,
					  SUM(CASE WHEN GREATEST(from_vid, lw_scripture_from_vid) > LEAST(c.to_vid, lw_scripture_to_vid) THEN 0 ELSE 1 END) AS NrNotes
					FROM lw_browse_chunks c
					INNER JOIN lw_verses v1 ON (v1.translation= :trans ) AND (v1.vid=from_vid)
					INNER JOIN lw_verses v2 ON (v2.translation= :trans ) AND (v2.vid=to_vid)
					INNER JOIN (
					  SELECT lw_scripture_from_vid, lw_scripture_to_vid
					  FROM field_data_lw_scripture
					  WHERE lw_scripture_from_vid BETWEEN (:booknum - 1)*726353 and (:booknum * 726353)
					) nodes
					/* last inner join without a join condition => full */
					WHERE c.booknum=:booknum
					GROUP BY c.id", array( 
		':trans' => $translation, 
		':booknum' => $booknum));

	$content .= "<div id='lwc-browse-book'><ul>";
	foreach ($rst as $row) {
		if ($row->chap1==$row->chap2) {
			$range = "{$row->chap1}:{$row->vers1}-{$row->vers2}";
		} else {
			$range = "{$row->chap1}:{$row->vers1}-{$row->chap2}:{$row->vers2}";
		}
		$content .= "<li class='lwc-browse-count-{$row->NrNotes}'><a href='/lwc/{$row->from_vid}-{$row->to_vid}'>{$bookname} {$range} <span class='lwc-browse-count'>({$row->NrNotes})</span></a></li>";
	}			   
	$content .= "</ul></div>";
	
	return $content;
}





function living_word_ui_browsemain() {
	# add the css for this page
	drupal_add_css(drupal_get_path('module', 'living_word_ui').'/living_word_ui.css');	
	
	$content = t("Listing the number of Living Word Comments for each bible book that is currently in the system");

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
