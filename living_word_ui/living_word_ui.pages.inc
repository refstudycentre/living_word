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

	$vocab = taxonomy_vocabulary_machine_name_load('living_word_position');
	$tree = taxonomy_get_tree($vocab->vid);
	
	$fields = '';
	$aggr ='';
	foreach ($tree as $term) {
			$fields .= ", Nr_{$term->tid}";
			$aggr .=  ", SUM(case when lw_type_tid = {$term->tid} then 1 else 0 end) as Nr_{$term->tid} ";
	}	
	/* 	Return a table like this:
	=============================
	Scripture Section	Intro	Exposition	Application
    Genesis 1:1-31	    1 note	34 notes	21 notes
    Genesis 2:1-55	    1 note	8 notes	    11 notes
    ...
	
	Difficult problem, because LWC has Ranges, and Chunks has Ranges.  So they could have multiple nodes... */
	
	$rst = db_query("SELECT c.from_vid, c.to_vid, v1.chapternum as chap1, v1.versenum as vers1, v2.chapternum as chap2, v2.versenum as vers2, 
					COALESCE(nodes.NrNotes, 0) as NrNotes {$fields}
					FROM lw_browse_chunks c
					INNER JOIN lw_verses v1 ON (v1.translation= :trans ) AND (v1.vid=from_vid)
					INNER JOIN lw_verses v2 ON (v2.translation= :trans ) AND (v2.vid=to_vid)
					LEFT JOIN (
					  SELECT id, lw_scripture_from_vid, lw_scripture_to_vid, count(*) as NrNotes {$aggr}
					  FROM lw_browse_chunks
					  INNER JOIN field_data_lw_scripture s on GREATEST(from_vid, lw_scripture_from_vid) <= LEAST(to_vid, lw_scripture_to_vid) # the two ranges overlap
					  INNER JOIN field_data_lw_type t ON s.entity_id=t.entity_id
					  WHERE (lw_scripture_from_vid BETWEEN (:booknum - 1)*726353 and (:booknum * 726353 ) ) and (booknum = :booknum)  # optimization
					  GROUP BY id
					) nodes using (id)
					WHERE c.booknum=:booknum ", array( 
		':trans' => $translation, 
		':booknum' => $booknum));

/*  even better ?  Need to test the performance of the queries against one another when we have some data.

					SELECT c.from_vid, c.to_vid, v1.chapternum, v1.versenum, v2.chapternum, v2.versenum, coalesce(nodes.NrNotes, 0) as NrNotes, 
					  NrIntro, NrApplication
					FROM lw_browse_chunks c
					INNER JOIN lw_verses v1 ON (v1.translation='ESV') AND (v1.vid=from_vid)
					INNER JOIN lw_verses v2 ON (v2.translation='ESV') AND (v2.vid=to_vid)
					LEFT JOIN (
					  SELECT id, lw_scripture_from_vid, lw_scripture_to_vid, count(*) as NrNotes,
					  SUM(case when lw_type_tid in (4,5,6) then 1 else 0 end) as NrIntro,
					  SUM(case when lw_type_tid in (7,8,9) then 1 else 0 end) as NrApplication
					  FROM lw_browse_chunks
					  INNER JOIN field_data_lw_scripture s on GREATEST(from_vid, lw_scripture_from_vid) <= LEAST(to_vid, lw_scripture_to_vid) # the two ranges overlap
					  INNER JOIN field_data_lw_type t ON s.entity_id=t.entity_id
					  WHERE (lw_scripture_from_vid BETWEEN (1-1)*726353 and 1*726353) and (booknum=1)  # optimization
					  GROUP BY id
					) nodes using (id)
					WHERE (c.booknum=1)


*/		

	$content .= "<table id='lwc-browse-book'><tr><th>Scripture</th><th>Total notes</th>";
	foreach ($tree as $term) {
		$content .= "<th>{$term->name}</th>";
	}
	$content .= "</tr>";
	foreach ($rst as $row) {
		$content .= "<tr class='lwc-browse-count-{$row->NrNotes}'>";   # for optionally hiding empty rows
		if ($row->chap1==$row->chap2) {
			$range = "{$row->chap1}:{$row->vers1}-{$row->vers2}";
		} else {
			$range = "{$row->chap1}:{$row->vers1}-{$row->chap2}:{$row->vers2}";
		}
		$content .= "<td>".l("{$bookname} {$range}", "lwc/{$row->from_vid}-{$row->to_vid}")."</td>";
		$content .= "<td>{$row->NrNotes}</td>";
		
		foreach ($tree as $term) {
			$field = "Nr_{$term->tid}";
			$val = $row->$field;
			if (empty($val)) $val=0;
			$content .= "<td>{$val}</td>";
		}
		$content .= "</tr>";
		
	}			   
	$content .= "</table>";
	
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
				   WHERE translation = :trans 
				   ORDER BY booknum", array(':trans' => $translation));

  $content .= "<div id='lwc-browse-main'><ul>";
  foreach ($rst as $row) {
    $content .= "<li class='lwc-browse-count-{$row->NrLWC}'>". l("{$row->bookname}", "browse/{$row->booknum}")."<span class='lwc-browse-count'>({$row->NrLWC})</span></li>";
  }			   
  $content .= "</ul></div>";
				   
  // detail table:  s
  // Links on each bible book to /browse/[n] where [n]= biblebookid
  
  return $content;
}

function living_word_ui_chunkpage($paramfirst = NULL, $second = NULL, $third=NULL, $fourth=NULL) {
  $range = scripture_decode_range($paramfirst);
  $rangetext = scripture_vidrange2text($range['from_vid'], $range['to_vid']);
  
  $content = "<h1>{$rangetext}</h1>";
  # var_dump($range);

  
  
/*  
  SELECT s.entity_id, lw_scripture_from_vid, lw_scripture_to_vid
  FROM field_data_lw_scripture s
  # INNER JOIN field_data_lw_type t ON s.entity_id=t.entity_id  # for position filter
  # INNER JOIN field_data_lw_detail d ON s.entity_id=d.entity_id
  WHERE
    GREATEST(181, lw_scripture_from_vid) <= LEAST(18000, lw_scripture_to_vid) # the query range overlaps with these nodes
  # AND t.lw_type_tid = 5     # the term ids for the sections this falls under
  # AND d.lw_detail_tid <= 1    # 1=Basic, 2=Intermediate, 3=Advanced
*/  


  return $content;
  
  
}

