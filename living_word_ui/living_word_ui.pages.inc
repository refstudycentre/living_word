<?php

/*
 * USER PAGE CALLBACKS
 */

/**
 * Page callback for browse function in living word
 * Summary of all nodes
 */
function living_word_ui_browsepage($param) {
  $content = "Hierso kom die browsepage";

  return $content;
}





function living_word_ui_browsemain($param) {
  $content = "Hierdie is die main page";
  // Get the user default translation.  
  // Then list the book names, with the number of lwcomments for each book
  //   select booknum, bookname, (booknum-1)*726353 +1 as MinVID, (booknum)*726353 as MaxVID  from lw_books where translation='ESV' ;
  // detail table:  s
  // Links on each bible book to /browse/[n] where [n]= biblebookid
  
  return $content;
}
