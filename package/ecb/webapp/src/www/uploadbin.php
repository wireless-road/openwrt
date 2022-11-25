<?php
if(session_id() == ''){session_start();}
if ( !isset($_SESSION['username'])) {
  header('Location: login.php');
}
$file_ext=strtolower(end(explode('.',$_FILES['fileToUpload4']['name'])));
$file_name = $_FILES['fileToUpload4']['name'];
$file_size = $_FILES['fileToUpload4']['size'];
$file_tmp = $_FILES['fileToUpload4']['tmp_name'];
$file_ext=strtolower(end(explode('.',$_FILES['fileToUpload4 ']['name'])));
//echo $file_ext;
//echo $file_name;
//echo $file_size;
//echo $file_tmp;
//return;

// Check file size
if ($_FILES["fileToUpload4"]["size"] > 20000000) {
  header('Location: fileupload.php?failBinsize');  
  return;
}

// Allow certain file formats
//if($file_ext != "bin") {
//  header('Location: fileupload.php?failBintype');  
//  return;
//}

// Check if $uploadOk is set to 0 by an error
if (move_uploaded_file($_FILES["fileToUpload4"]["tmp_name"], "/usr/bin/aites")) {
    header('Location: fileupload.php?successBin');
} else {
  header('Location: fileupload.php?failBin');
}
?>
