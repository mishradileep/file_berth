<?php
function list_file_berth() {
	
	$form['filter'] = file_berth_filter_form();
	$form['#submit'][] = 'node_filter_form_submit';
	$form['admin'] = file_berth_list();
	return $form;
	
}

function settings_file_berth() {
	return 'asdas';
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
	
	
	$query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
	//file_berth_build_filter_query($query);
	// Build the sortable table header.
	$header = array(
		'title' => array('data' => t('Title'), 'field' => 'n.title'),
		'type' => array('data' => t('Type'), 'field' => 'n.type'),
		'author' => t('Author'),
		'status' => array('data' => t('Status'), 'field' => 'n.status'),
		'changed' => array('data' => t('Updated'), 'field' => 'n.changed', 'sort' => 'desc')
	);
	$nids = $query
	->fields('n',array('nid'))
	->limit(50)
	->orderByHeader($header)
	->addTag('node_access')
	->execute()
	->fetchCol();
	$nodes = node_load_multiple($nids);
	foreach ($nodes as $node) {
		$langcode = entity_language('node', $node);
		$l_options = $langcode != LANGUAGE_NONE && isset($languages[$langcode]) ? array('language' => $languages[$langcode]) : array();
		$options[$node->nid] = array(
				'title' => array(
						'data' => array(
								'#type' => 'link',
								'#title' => $node->title,
								'#href' => 'node/' . $node->nid,
								'#options' => $l_options,
								'#suffix' => ' ' . theme('mark', array('type' => node_mark($node->nid, $node->changed))),
						),
				),
				'type' => check_plain(node_type_get_name($node)),
				'author' => theme('username', array('account' => $node)),
				'status' => $node->status ? t('published') : t('not published'),
				'changed' => format_date($node->changed, 'short'),
		);
		
		// Build a list of all the accessible operations for the current node.
		
		$options[$node->nid]['operations'] = array();
		if (count($operations) > 1) {
			// Render an unordered list of operations links.
			$options[$node->nid]['operations'] = array(
					'data' => array(
							'#theme' => 'links__node_operations',
							'#links' => $operations,
							'#attributes' => array('class' => array('links', 'inline')),
					),
			);
		}
		elseif (!empty($operations)) {
			// Render the first and only operation as a link.
			$link = reset($operations);
			$options[$node->nid]['operations'] = array(
					'data' => array(
							'#type' => 'link',
							'#title' => $link['title'],
							'#href' => $link['href'],
							'#options' => array('query' => $link['query']),
					),
			);
		}
	}
	// Only use a tableselect when the current user is able to perform any
	// operations.
	if ($admin_access) {
		$form['nodes'] = array(
				'#type' => 'tableselect',
				'#header' => $header,
				'#options' => $options,
				'#empty' => t('No content available.'),
		);
	}
	// Otherwise, use a simple table.
	else {
		$form['nodes'] = array(
				'#theme' => 'table',
				'#header' => $header,
				'#rows' => $options,
				'#empty' => t('No content available.'),
		);
	}
	
	$form['pager'] = array('#markup' => theme('pager'));
	return $form;
}

function upload_file_berth() {
	$form['file_berth_file'] = array(
		'#title' => t('File'),
		'#type' => 'managed_file',
		'#description' => t('The uploaded file will be allocated to a berth and managed.'),
		'#upload_location' => 'public://file_berth/',
		//'#element_validate' => array('_validate_file_berth_upload'),
		'#upload_validators' => array(
			'file_validate_extensions' => array('png jpeg gif jpg'),
			// Pass the maximum file size in bytes
			//'file_validate_size' => array(200),
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
		
	}
	else {
		drupal_set_message('File birth can\'t be allocated.', 'error');
	}
	
}