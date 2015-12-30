<?php


/*
 * TAXONOMY VOCABULARIES
 */

/**
 * Create vocabularies
 */
function living_word_vocab_config() {
  $t = get_t(); // this function might run during install, or any other time
  
  $position = (object) array(
    'name' => $t('LW Position'),
    'description' => $t('Type, or position of a Living Word Comment in the greater commentary.'),
    'machine_name' => 'living_word_position',
  );
  taxonomy_vocabulary_save($position);
  
  $detail = (object) array(
    'name' => $t('LW Detail'),
    'description' => $t('Level of detail for which a Living Word Comment should be displayed.'),
    'machine_name' => 'living_word_detail',
  );
  taxonomy_vocabulary_save($detail);

  /*   Not sure if this is a good idea or not....
  
  $detailterms = array("Basic", "Intermediate", "Advanced");  // use weights: 0, 1, 2
  
  foreach($detailterms as $weight => $desc) {
	$tid = db_query("SELECT td.tid FROM {term_data} td JOIN {term_hierarchy} th ON th.tid = td.tid WHERE td.vid = :vocabid AND td.name= :label",array(
				':vocabid' => $detail->vid, 
				':label' => $desc))->queryField();
	if(!$tid) {
	  $term = array(
		'vid' => $detail->vid, // Voacabulary ID
		'name' => $desc, // Term Name
		'weight' => $weight
	  );
	  taxonomy_save_term($term);
	}
  } // end foreach

  */
  
}

