<?php

/**
  * @file
  * functions for info_manager module
  */ 
  
/**
 * Main Information Manager Page
 * @see display_data()
 *
 */
function info_manager_input_data($form, &$form_state) {
  
  $form['description'] = array(
    '#type' => 'item',
    '#title' => t('Data Input'),
  );
  
  // This is the form elements.
  $form['name'] = array(
    '#type' => 'fieldset',
    '#title' => t('Personal Information'),
	 // Make the fieldset collapsible.
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  ); 
  $form['name']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First Name'),
	'#size' => 50,
    '#maxlength' => 50,
  );
  $form['name']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
	'#size' => 50,
    '#maxlength' => 50,
  );
  $form['name']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
	'#size' => 100,
    '#maxlength' => 100,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );
  return $form;
}

/**
 * Submit handler for input_data().
 */
function info_manager_input_data_submit($form, &$form_state) {

 try {
    $id = db_insert('info_manager')
	->fields(array(
	  'first_name' => $form_state['values']['first_name'],
	  'last_name' => $form_state['values']['last_name'],
	  'email' => $form_state['values']['email'],
	))
	->execute();
	
	drupal_set_message(t('The form has been submitted. name="@first_name @last_name", Email="@email"',
	  array(
		'@first_name' => $form_state['values']['first_name'],
		'@last_name' => $form_state['values']['last_name'],
		'@email' => $form_state['values']['email'],
	  )
	));  
    return $id;
  }
  catch (Exception $e) {
    // Something went wrong somewhere, so roll back now.
    $txn->rollback();
    // Log the exception to watchdog.
    watchdog_exception('type', $e);
  }
}

/**
 * display the data.
 */
function info_manager_display_data($form, &$form_state) {
  //Use Database API to retrieve records.
  $max_num = variable_get('info_manager_max', 3);
  $query = db_select('info_manager', 'n')
    ->fields('n', array('id', 'first_name', 'last_name', 'email'))
	->extend('PagerDefault')
	->limit($max_num)
    ->orderBy('id', 'DESC'); //Most recent first.
  $result = $query->execute();    
 
  //Iterate over the resultset and format them.
  $header = array('', 'ID', 'First Name', 'Last Name', 'Email');
  $rows = array();
  foreach ($result as $row){
    $rows[] = array('<input type="checkbox" value="1/' . $row->id . '" onclick=""><span style="margin:5px;"><a href="http://' . $_SERVER['HTTP_HOST'] . '/info_manager/data/process/1/'. $row->id . '">delete</a></span><a href="http://' . $_SERVER['HTTP_HOST'] . '/info_manager/data/process/3/'. $row->id . '">update</a>',
	                $row->id,
                    $row->first_name,
                    $row->last_name,
                    $row->email,
    );
  }
 
// Create a render array ($build) which will be themed as a table with a pager.
  $build['pager_table'] = array(
	'#theme' => 'table',
	'#header' => $header,
	'#rows' => $rows,
	'#empty' => t('There are no date formats found in the db'),
  );
// Attach the pager theme.
  $build['pager_pager'] = array('#theme' => 'pager');
  return $build; 
}

/**
 * delete data.
 */
function info_manager_delete_data($form, &$form_state, $arg0) {

 try {
   $id = $arg0;
   $id_deleted = db_delete('info_manager')
     ->condition('id', $id)
     ->execute();
	
	drupal_set_message(t('The No. ' . $arg0. ' record has been deleted.'));
	
	$page_array['info_manager_arguments'] = array(
	  //Title serves as page subtitle
	  '#title' => t('Go back to data display'),
	  '#markup' => t('<a href="http://' . $_SERVER['HTTP_HOST'] . '/info_manager/data/process">click to go back to data display page</a>'),
    );
    return $page_array;	
  }
  catch (Exception $e) {
    // Something went wrong somewhere, so roll back now.
    $txn->rollback();
    // Log the exception to watchdog.
    watchdog_exception('type', $e);
  }
}


/**
 * get data for update.
 */
function info_manager_update_data($form, &$form_state, $arg0) {

 try {
   $id = $arg0;
   
   //Use Database API to retrieve records.
   $query = db_select('info_manager', 'n')
	 ->fields('n', array('id', 'first_name', 'last_name', 'email'))
	 ->condition('id', $arg0, '=')
	 ->orderBy('id', 'DESC'); //Most recent first.
	 	 
    $result = $query->execute();  
  }
  catch (Exception $e) {
    // Something went wrong somewhere, so roll back now.
    $txn->rollback();
    // Log the exception to watchdog.
    watchdog_exception('type', $e);
  }
  
  foreach ($result as $row){
	$form['description'] = array(
	  '#type' => 'item',
	  '#title' => t('Data Update for the ID# ' . $row->id . ' record'),
	);
	// This is the form elements.
	$form['name'] = array(
	  '#type' => 'fieldset',
	  '#title' => t('Personal Information'),
	   // Make the fieldset collapsible.
	  '#collapsible' => TRUE,
	  '#collapsed' => FALSE,
	); 
	$form['name']['id'] = array(
	  '#type' => 'hidden',
	  '#title' => t('ID'),
	  '#value' => $row->id,
	  '#readonly' => 'readonly',
	);	
	$form['name']['first_name'] = array(
	  '#type' => 'textfield',
	  '#title' => t('First Name'),
	  '#size' => 50,
	  '#maxlength' => 50,
	  '#default_value' => $row->first_name,
	);
	$form['name']['last_name'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Last Name'),
	  '#size' => 50,
	  '#maxlength' => 50,
	  '#default_value' => $row->last_name,
	);
	$form['name']['email'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Email'),
	  '#size' => 100,
	  '#maxlength' => 100,
	  '#default_value' => $row->email,
	);
	
	$form['submit'] = array(
	  '#type' => 'submit',
	  '#value' => 'Submit',
	);
  };
  return $form; 
}

/**
 * update data.
 */
function info_manager_update_data_submit($form, &$form_state) {

 try { 
   $id = $form_state['values']['id'];
   $id_updated = db_update('info_manager') // Table name no longer needs {}
     ->fields(array(
	  'first_name' => $form_state['values']['first_name'],
	  'last_name' => $form_state['values']['last_name'],
	  'email' => $form_state['values']['email'],
    ))
     ->condition('id', $id, '=')
     ->execute();
	
   drupal_set_message(t('The No. ' .  $id. ' record has been updated.'));
  }
  catch (Exception $e) {
    // Something went wrong somewhere, so roll back now.
    $txn->rollback();
    // Log the exception to watchdog.
    watchdog_exception('type', $e);
  }
}

/**
 * search the data.
 */
function info_manager_search_data($form, &$form_state){
 
  if (!empty($form_state['results_table'])) {
    $form['results_table'] = array('#markup' => $form_state['results_table']);
  }     
  $form['description'] = array(
    '#type' => 'item',
    '#title' => t('Data Search'),
  );   
  // This is the form elements.
  $form['name'] = array(
    '#type' => 'fieldset',
    '#title' => t('Personal Information'),
	 // Make the fieldset collapsible.
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  ); 
  $form['name']['id'] = array(
	  '#type' => 'textfield',
	  '#title' => t('ID'),
	  '#size' => 50,
	  '#maxlength' => 50,
  );  
  $form['name']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First Name'),
	'#size' => 50,
    '#maxlength' => 50,
  );
  $form['name']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
	'#size' => 50,
    '#maxlength' => 50,
  );
  $form['name']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
	'#size' => 100,
    '#maxlength' => 100,
  ); 
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Search',
  );     
  return $form;
}

function info_manager_search_data_submit($form, &$form_state) {
  drupal_goto('http://' . $_SERVER['HTTP_HOST'] . '/info_manager/data/process/4/1', array('query'=>array(
	'id'=>$form_state['values']['id'],
	'first_name'=>$form_state['values']['first_name'],
	'last_name'=>$form_state['values']['last_name'],
	)));
}

function info_manager_search_data_results($form, &$form_state) {
  $arg0 = $_GET["id"];
  $arg1 = $_GET["first_name"];
  $arg2 = $_GET["last_name"];
  $arg3 = $_GET["email"];
  $max_num = variable_get('info_manager_max', 3);

  $query = db_select('info_manager', 'n')->extend('PagerDefault')
	->fields('n', array('id', 'first_name', 'last_name', 'email'));
  if(!empty(arg0))		
	$query->condition('id', '%' . db_like($arg0) . '%', 'LIKE');
  if(!empty(arg1))	
	$query->condition('first_name', '%' . db_like($arg1) . '%', 'LIKE');
  if(!empty(arg2))		
	$query->condition('last_name', '%' . db_like($arg2) . '%', 'LIKE');
  if(!empty(arg3))	
	$query->condition('email', '%' . db_like($arg3) . '%', 'LIKE');    
  $query->limit($max_num);
  $query->orderBy('id', 'DESC');  //Most recent first.
  
  $result = $query->execute();      
 
  //Iterate over the resultset and format them.
  $header = array('', 'ID', 'First Name', 'Last Name', 'Email');
  $rows = array();
  foreach ($result as $row){
    $rows[] = array('<input type="checkbox" value="1/' . $row->id . '" onclick=""><span style="margin:5px;"><a href="http://' . $_SERVER['HTTP_HOST'] . '/info_manager/data/process/1/'. $row->id . '">delete</a></span><a href="http://' . $_SERVER['HTTP_HOST'] . '/info_manager/data/process/3/'. $row->id . '">update</a>',
	                $row->id,
                    $row->first_name,
                    $row->last_name,
                    $row->email,
    );
  }
 
  // Create a render array ($build) which will be themed as a table with a pager.
  $build['pager_table'] = array(
	'#theme' => 'table',
	'#header' => $header,
	'#rows' => $rows,
	'#empty' => t('There are no date formats found in the db'),
  );
  // Attach the pager theme.
  $build['pager_pager'] = array('#theme' => 'pager');
  return $build; 
} 

function info_manager_update_batch($form, &$form_state) {
     
  $form['description'] = array(
    '#type' => 'item',
    '#title' => t('Batch Update'),
  );   
  // This is the form elements.
  $form['field_no'] = array(
    '#type' => 'select',
    '#title' => t('Which field to update?'),
    // The options here are integers, but since all the action here happens
    // using the DOM on the client, we will have to use strings to work with them.
    '#options' => array(
      1 => t('First Name'),
      2 => t('Last Name'),
      3 => t('Email'),
    ),
  );  
  $form['old_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Old Value'),
	'#size' => 50,
    '#maxlength' => 50,
  );  
  $form['new_value'] = array(
    '#type' => 'textfield',
    '#title' => t('New Value'),
	'#size' => 50,
    '#maxlength' => 50,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Update',
  );     
  return $form; 
}

function info_manager_update_batch_submit($form, &$form_state) { 

  $field_no = $form_state['values']['field_no'];
  $new_value = trim($form_state['values']['new_value']);
  $old_value = trim($form_state['values']['old_value']);

  try { 
    $id_updated = db_update('info_manager');
   
    switch($field_no) {
 	  case 1:
		$id_updated->fields(array(
		  'first_name' => $new_value,
		)); 
		if($old_value == '' || $old_value == NULL) 
		  $id_updated->isNULL('first_name');
		else
		  $id_updated->condition('first_name', $old_value, '=');
		break;
 	  case 2:
   	    $id_updated->fields(array(
		    'last_name' => $new_value,
		  ));
	    if($old_value == '' || $old_value == NULL) 
		  $id_updated->isNULL('last_name');
		else  
		  $id_updated->condition('last_name', $old_value, '=');
		break;
	  case 3:
        $id_updated->fields(array(
		    'email' => $new_value,
		  ));
	    if($old_value == '' || $old_value == NULL) 
		  $id_updated->isNULL('email');
		else
		  $id_updated->condition('email', $old_value, '=');
		beark;  
    } 

//   $id_updated->condition('id', $id, '=');
   $id_updated->execute();	
   drupal_set_message(t('The Field No. ' .  $field_no . ' has been updated.'));
  }
  catch (Exception $e) {
	// Something went wrong somewhere, so roll back now.
	$txn->rollback();
	// Log the exception to watchdog.
	watchdog_exception('type', $e);
  }
}
