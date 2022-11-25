<?php
if(session_id() == ''){session_start();}
if ( !isset($_SESSION['username'])) {
  header('Location: login.php');
}
$file_ext=strtolower(end(explode('.',$_FILES['fileToUpload3']['name'])));

// Check if file already exists
//if (file_exists($target_file)) {
//  header('Location: fileupload.php?fail');
//  echo "Sorry, file already exists.";
//  $uploadOk = 0;
//}

// Check file size
if ($_FILES["fileToUpload3"]["size"] > 1048576) {
  header('Location: fileupload.php?failImagesize');  
  return;
}

// Allow certain file formats
if($file_ext != "png" ) {
  header('Location: fileupload.php?failImagetype');  
  return;
}

// Check if $uploadOk is set to 0 by an error
if (move_uploaded_file($_FILES["fileToUpload3"]["tmp_name"], "logo.png")) {
    header('Location: fileupload.php?successImage');
} else {
  header('Location: fileupload.php?failImage');
}

?>