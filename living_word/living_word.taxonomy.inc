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

  $nrterms = db_query("SELECT count(*) FROM {taxonomy_term_data} td WHERE td.vid = :vocabid",array(':vocabid' => $position->vid))->fetchField();
  if ($nrterms==0) {
  
    $terms = array("Introduction", "Structure", "Timeline and Charts", "Technical", "Exposition", "Application");  
    foreach($terms as $weight => $desc) {
	  $term = (object) array(
		'vid' => $position->vid, // Voacabulary ID
		'name' => $desc, // Term Name
		'weight' => $weight
	  );
	  taxonomy_term_save($term);
    } // end foreach
  }

  
  $detail = (object) array(
    'name' => $t('LW Detail'),
    'description' => $t('Level of detail for which a Living Word Comment should be displayed.'),
    'machine_name' => 'living_word_detail',
  );
  taxonomy_vocabulary_save($detail);

  $nrterms = db_query("SELECT count(*) FROM {taxonomy_term_data} td WHERE td.vid = :vocabid",array(':vocabid' => $detail->vid))->fetchField();
  if ($nrterms==0) {
  
    $terms = array("Basic", "Intermediate", "Advanced");  
    foreach($terms as $weight => $desc) {
	  $term = (object) array(
		'vid' => $detail->vid, // Voacabulary ID
		'name' => $desc, // Term Name
		'weight' => $weight
	  );
	  taxonomy_term_save($term);
    } // end foreach
  }


  
}

