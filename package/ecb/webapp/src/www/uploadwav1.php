<?php
if(session_id() == ''){session_start();}
if ( !isset($_SESSION['username'])) {
  header('Location: login.php');
}
$file_ext=strtolower(end(explode('.',$_FILES['fileToUpload1']['name'])));


// Check if file already exists
//if (file_exists($target_file)) {
//  header('Location: fileupload.php?fail');
//  echo "Sorry, file already exists.";
//  $uploadOk = 0;
//}

// Check file size
if ($_FILES["fileToUpload1"]["size"] > 5242880) {
  header('Location: fileupload.php?failAudiosize1');  
  return;
}

// Allow certain file formats
if($file_ext != "wav" ) {
  header('Location: fileupload.php?failAudiotype1');  
  return;
}

// Check if $uploadOk is set to 0 by an error
if (move_uploaded_file($_FILES["fileToUpload1"]["tmp_name"], "/usr/bin/networkcableerr.wav")) {
    header('Location: fileupload.php?successAudio1');
} else {
  header('Location: fileupload.php?failAudio1');
}

?>
