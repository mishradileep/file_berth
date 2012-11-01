<h1>Alt Tag</h1>
<?php print $file_berth['file_berth_alt_tag']?>

<h1>Description</h1>
<?php print $file_berth['file_berth_description']?>

<h1>MIME Type</h1>
<?php print $file_berth['file_berth_title']?>

<h1>Size</h1>
<?php print format_size($file_berth['filesize']) ?>

<h1>File Berth Sort Code</h1>
<textarea>
<file_berth fid="<?php print $file_berth['fid']?>" alt="<?php print $file_berth['file_berth_alt_tag']?>" desc="<?php print $file_berth['file_berth_description']?>" inline="yes">
</textarea>

<h1>Output</h1>
<?php print change_shortcode_to_file($file_berth['fid']); ?>