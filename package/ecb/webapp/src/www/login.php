<?php
   
   session_start();
   ob_start();
  $inputfile = file('users');
 
 $paramName = array();
 $values = array();
 $splitstr = array();
 $i = 1;
 //$newValues = $_POST['name'];
 foreach($inputfile as $dl){
    $splitstr = explode("-", $dl);
    $paramName[] = $splitstr[0];
    $values[] = $splitstr[1];
     $i++;
 }
 $_SESSION['valid_user'] = $values[0];
 $_SESSION['valid_pass'] = $values[1];
 $_SESSION['admin_user'] = $values[2];
 $_SESSION['admin_pass'] = $values[3];

 /*echo $_SESSION['valid_user'];
 echo " == ";
 echo $_SESSION['valid_pass'];
 echo "\n";
 echo $_SESSION['admin_user'];
 echo " == ";
 echo $_SESSION['admin_pass'];*/
?>

<?
   // error_reporting(E_ALL);
   // ini_set("display_errors", 1);
?>

<html lang = "en">
   
   <head>
      <title>eKawach - ECB</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
      <link href = "aites.css" rel = "stylesheet">
  </head>
	
   <body>
      <div align='center'>
      <logo-image><img src="logo.png" class="img-fluid"> </logo-image>
</dev>
      <h2 class="header2" align='center'>Login</h2> 
      <div class = "main">
         
         <?php
            $msg = '';
            
            if (isset($_POST['login']) && !empty($_POST['username']) 
               && !empty($_POST['password'])) {
                   $usr = trim($_POST['username']);
               /* echo $_POST['username'] ;
                echo $_POST['password'] ;
                echo "==";
                echo $_SESSION['valid_user'];
                echo $_SESSION['valid_pass'];
                echo "======== \n";*/
               if ((($_POST['username'] == trim($_SESSION['valid_user']) && 
                  $_POST['password'] == trim($_SESSION['valid_pass'])) )
                  || ($_POST['username'] == trim($_SESSION['admin_user']) && 
                  $_POST['password'] == trim($_SESSION['admin_pass']))) {
                  $_SESSION['valid'] = true;
                  $_SESSION['timeout'] = time();
                  $_SESSION['username'] = $usr;//trim($_POST['username']);
                  echo 'You have entered valid use name and password';
                  header('Location: index.php');
                  //echo 'You have entered valid use name and password';
               }else {
                   /*echo $_POST['username'] ;
                   echo "==";
                   echo $_POST['password'];*/
                  $msg = 'Wrong username or password';
               }
            }
         ?>
      </div> <!-- /container -->
      
      <div >
      <h4  align='center'><?php echo $msg; ?></h4>
      <br>
         <form  action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);  ?>" method = "post">
            
            <table>
                <tr><th>Login</th><th></th>
                </tr>
                <tr><td align="right">User Name </td>
                    <td>
                        <input type = "text" class = "form-control" name = "username" placeholder = "username = ecb" required autofocus>
                    </td>
                </tr>
               <tr>
                   <td>Password </td>
                   <td> <input type = "password" class = "form-control" name = "password" placeholder = "password = Ecb@123" required>
                   </td>
                </tr>
            <tr><td></td><td></td>
         </form></tr>   
        </table><br>  
        <button class = "button" type = "submit" 
               name = "login">Login</button> 
            
			
         <!--Click here to clean <a href = "logout.php" tite = "Logout">Session. -->
         
      </div> 
      
   </body>
</html>