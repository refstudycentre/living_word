<?php

/*
 * Form functions for "Step 4: Review LWCs"
 */

module_load_include('inc', 'living_word_admin', 'living_word_admin.form');


/**
 * Form builder
 */
function living_word_admin_review_form($form, &$form_state) {

  $bundle = 'living_word_comment';
  global $user;



  return $form;

}


/**
 * Default form validation handler
 */
function living_word_admin_review_form_validate($form, &$form_state) {



}


/**
 * Default form submit handler
 */
function living_word_admin_review_form_submit($form, &$form_state) {



}

