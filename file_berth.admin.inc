<?php
function list_file_berth() {
	
	$form['filter'] = file_berth_filter_form();
	$form['#submit'][] = 'node_filter_form_submit';
	$form['admin'] = file_berth_list();
	return $form;
	
}

function settings_file_berth() {
	return drupal_get_form('file_berth_settings');
}

function file_berth_filter_form() {
	$form['filters'] = array(
		'#type' => 'fieldset',
		'#title' => t('Show only items where'),
		'#theme' => 'exposed_filters__node',
	);
	return $form;
}

function file_berth_list() {
	
	$header = array(
			'file_berth_title' => array('data' => t('Title'), 'field' => 'berth.file_berth_title'),
			'filemime' => array('data' => t('MIME Type'), 'field' => 'files.filemime'),
			'filesize' => array('data' => t('Size'), 'field' => 'files.filesize'),
			'operations' => array('data' => t('Operations')),
	);
	
	$query = db_select('file_berth', 'berth')
	->extend('PagerDefault')
	->limit(10)
	->extend('TableSort')
	->orderByHeader($header)
	->fields ('berth', array (
			'fid',
			'file_berth_title',
			'file_berth_alt_tag',
			'file_berth_description'
	));
	 
	$query->leftJoin('file_managed', 'files', 'files.fid = berth.fid');
	$query->fields ('files', array (
			'filename',
			'uri',
			'filemime',
			'filesize',
			'status',
			'timestamp'
	));
	
	$result = $query->execute()->fetchAll();
	
	$form['options'] = array(
			'#type' => 'fieldset',
			'#title' => t('Update options'),
			'#attributes' => array('class' => array('container-inline')),
	);
	$options = array();
	foreach (module_invoke_all('file_berth_operations') as $operation => $array) {
		$options[$operation] = $array['label'];
	}
	$form['options']['operation'] = array(
			'#type' => 'select',
			'#title' => t('Operation'),
			'#title_display' => 'invisible',
			'#options' => $options,
			'#default_value' => 'unblock',
	);
	$options = array();
	$form['options']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Update'),
	);
	
	$destination = drupal_get_destination();
	foreach ($result as $berth) {
		
		
		
		$options[$berth->fid] = array(
				'file_berth_title' => array(
	        'data' => array(
	          '#type' => 'link',
	          '#title' => $berth->file_berth_title,
	          '#href' => 'file-berth/' . $berth->fid,
	        ),
      	), 
				'filemime' =>  $berth->filemime,
				'filesize' => format_size($berth->filesize),
		);
		
		$operations = array();
		$operations['edit'] = array(
				'title' => t('edit'),
				'href' => "file_berth/$berth->fid/edit",
				'query' => $destination,
		);
		$operations['delete'] = array(
				'title' => t('delete'),
				'href' => "file-berth/$berth->fid/delete",
				'query' => $destination,
		);
		$options[$berth->fid]['operations'] = array(
				'data' => array(
						'#theme' => 'links__node_operations',
						'#links' => $operations,
						'#attributes' => array('class' => array('links', 'inline')),
				),
		);
	}
	
	$form['links'] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $options,
			'#empty' => t('No files available.'),
	);
	$form['pager'] = array('#markup' => theme('pager'));
	return $form;
}

function upload_file_berth() {
	$form['file_berth_file'] = array(
		'#title' => t('File'),
		'#type' => 'managed_file',
		'#description' => t('The uploaded file will be allocated to a berth and managed.'),
		'#upload_location' => 'public://file_berth/',
		'#upload_validators' => array(
			'file_validate_extensions' => array('png jpeg gif jpg'),
		),
		'#required' => TRUE,
	);
	$form['file_berth_title'] = array(
		'#type' => 'textfield',
		'#title' => t('Title'),
		'#size' => 60,
		'#maxlength' => 128,
		'#required' => TRUE,
	);
	$form['file_berth_alt_text'] = array(
		'#type' => 'textfield',
		'#title' => t('Alt Tag'),
		'#size' => 60,
		'#maxlength' => 128,
	);
	$form['file_berth_description'] = array(
		'#type' => 'textfield',
		'#title' => t('Description'),
		'#size' => 60,
		'#maxlength' => 128,
	);
	$form['file_berth_file_submit'] = array(
		'#type' => 'submit',
		'#value' => 'Upload',
	);
	
	return $form;
}

function upload_file_berth_submit($form, &$form_state) {
	
	// If fid is not 0 we have a valid file.
	if ($form_state['values']['file_berth_file'] != 0) {
		$file = file_load($form_state['values']['file_berth_file']);
		$file->status = FILE_STATUS_PERMANENT;
		file_save($file);
		db_insert('file_berth')
		->fields(array(
				'fid' => $file->fid,
				'file_berth_title' => $form_state['values']['file_berth_title'],
				'file_berth_alt_tag' => $form_state['values']['file_berth_alt_text'],
				'file_berth_description' => $form_state['values']['file_berth_description']
			))->execute();
		drupal_set_message('File uploaded and allocated berth.', 'status');
		drupal_goto('admin/structure/file_berth');
		
	}
	else {
		drupal_set_message('File birth can\'t be allocated.', 'error');
	}
	
}

function file_berth_settings() {
	
	$file_berth_settings = variable_get('file_berth_settings');
	
	$form['file_berth_allowed_extensions'] = array(
		'#type' => 'textfield',
		'#title' => t('Allowed file extensions'),
		'#description' => 'Separate extensions with a space or comma and do not include the leading dot.',
		'#default_value' => $file_berth_settings['allowed_file_ext'],
		'#size' => 60,
		'#maxlength' => 128,
		'#required' => TRUE,
	);
	$form['file_berth_file_directory'] = array(
		'#type' => 'textfield',
		'#title' => t('File directory'),
		'#description' => 'Optional subdirectory within the upload destination where files will be stored. Do not include preceding or trailing slashes.',
		'#default_value' => $file_berth_settings['file_berth'],
		'#size' => 60,
		'#maxlength' => 128,
	);
	$form['file_berth_file_size'] = array(
		'#type' => 'textfield',
		'#title' => t('Maximum upload size'),
		'#description' => 'Enter a value in bytes.',
		'#default_value' => $file_berth_settings['upload_size'],
		'#size' => 60,
		'#maxlength' => 128,
	);
	$form['file_berth_settings_submit'] = array(
		'#type' => 'submit',
		'#value' => 'Submit',
	);
	return $form;
}

function file_berth_settings_validate($form_id,&$form_values) {
	
	if(trim(preg_match('/[^A-z0-9]/', trim($form_values['input']['file_berth_file_directory'])) && $form_values['input']['file_berth_file_directory']) != '' ) {
		form_set_error('file_berth_file_directory', t('File directory should contain only alphanumeric string.'));
	}
	
	if(preg_match('/[^0-9]/', trim($form_values['input']['file_berth_file_size'])) && trim($form_values['input']['file_berth_file_size']) != '') {
		form_set_error('file_berth_file_size', t('Maximum upload size should be in numbers (bytes) only.'));
	}
}

function file_berth_settings_submit($form_id,&$form_values) {
	
	$settings['allowed_file_ext'] = trim($form_values['input']['file_berth_allowed_extensions']);
	$settings['file_berth'] = trim($form_values['input']['file_berth_file_directory']);
	$settings['upload_size'] = trim($form_values['input']['file_berth_file_size']);
	variable_set('file_berth_settings', $settings);
	drupal_set_message('File berth settings seved sucessfully.','status');
}