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
  
}

