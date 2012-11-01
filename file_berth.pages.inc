<?php
function file_berth_view($fid) {
	$query = db_select('file_berth', 'berth')
	->fields ('berth', array (
			'fid',
			'file_berth_title',
			'file_berth_alt_tag',
			'file_berth_description'
	));
	$query->leftJoin('file_managed', 'files', 'files.fid = berth.fid');
	$query->condition('berth.fid', $fid)
	->fields ('files', array (
			'filename',
			'uri',
			'filemime',
			'filesize',
			'status',
			'timestamp'
	));
	$result = $query->execute()->fetchAssoc();
	
	drupal_set_title($result['file_berth_title']);
	return theme('file_berth_view', array('file_berth' => $result));
}

function change_shortcode_to_file($fid) {
	$query = db_select('file_berth', 'berth')
	->fields ('berth', array (
			'fid',
			'file_berth_title',
			'file_berth_alt_tag',
			'file_berth_description'
	));
	$query->leftJoin('file_managed', 'files', 'files.fid = berth.fid');
	$query->condition('berth.fid', $fid)
	->fields ('files', array (
			'filename',
			'uri',
			'filemime',
			'filesize',
			'status',
			'timestamp'
	));
	$result = $query->execute()->fetchAssoc();
	
	$image['style_name'] = 'thumbnail';
	$image['path'] = $result['uri'];
	$image['alt'] = $result['file_berth_alt_tag'];
	$image['title'] = $result['file_berth_title'];
	$picture = theme('image_style', $image);
	
	return $picture;
}

