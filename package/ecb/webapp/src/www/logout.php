<?php
   session_start();
   unset($_SESSION["username"]);
   unset($_SESSION["password"]);
   
   header('Refresh: 2; URL = login.php');
   echo 'Logging Out !!!!';
?>
