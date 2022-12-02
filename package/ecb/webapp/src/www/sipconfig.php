<?php
if(session_id() == ''){session_start();}
//header("Refresh:1");
date_default_timezone_set("Asia/Calcutta");
if ( !isset($_SESSION['username'])) {
    header('Location: login.php');
}
    if(isset($_POST['btnCall'])) {
        //Add CALL Related command here
        $host    = "127.0.0.1";
        $port    = 5500;
        $message = "CALL";
        echo "Message To server :".$message;
        // create socket
        $socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
        // connect to server
        $result = socket_connect($socket, $host, $port) or die("Could not connect to server\n");  
        // send string to server
        socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");
        // close socket
        socket_close($socket);        
    } 
    if(isset($_POST['submit'])){
        //NHS
        //$inputfile = file('AppConfig.ini');
        $inputfile = file('/usr/bin/AppConfig.ini');
        $file = fopen("tmpConfig.ini","w");
        $paramName = array();
        $values = array();
        $splitstr = array();
        $i = 1;
        $j = 0;
        $newValues = $_POST['name'];  
        foreach($inputfile as $dl){
            $splitstr = explode("-", $dl);
            $paramName[] = $splitstr[0];
            $values[] = $splitstr[1];
            if ($i == 17) {
                fprintf($file, "%s-%s\n", $splitstr[0], $newValues[4]);
                //$j++;

            }
            else if (($i >= 19 and $i <= 22) ){
                fprintf($file, "%s-%s\n", $splitstr[0], $newValues[$j]);
                $j++;
                
            }
            else if (($i >= 30 and $i <= 33) and in_array(1, $_POST['name']) and count($newValues) > 6){
                //$j++;
                fprintf($file, "%s-%s\n", $splitstr[0], $newValues[$j+2]);
                $j++;
            }
            else if ($i == 41) {
                fprintf($file, "%s-%s\n", $splitstr[0], $newValues[5]);
                //$j++;
            }
            
            else {
                fprintf($file, "%s-%s", $splitstr[0], $splitstr[1]);
            }
            $i++;
        }    
        // delete Appconfig file
        $file_pointer = "/usr/bin/AppConfig.ini"; 
        if (!unlink($file_pointer)) { 
            echo ("$file_pointer cannot be deleted due to an error"); 
        } 
        // Rename new config to App config
        rename("tmpConfig.ini","/usr/bin/AppConfig.ini");
        //$output = shell_exec('sudo chmod 777 AppConfig.ini');
        // $host    = "127.0.0.1";
        // $port    = 5500;
        // $message = "KILL";
        // echo "Message To server :".$message;
        // // create socket
        // $socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
        // // connect to server
        // $result = socket_connect($socket, $host, $port) or die("Could not connect to server\n");  
        // // send string to server
        // socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");
        // // close socket
        // socket_close($socket);        
	    //system('mv /usr/bin/aites.log /usr/bin/aites_last.log');
        system('/etc/init.d/baresip restart');
    } 

    $inputfile = file('/usr/bin/AppConfig.ini');
    $values = array();
    $splitstr = array();
    foreach($inputfile as $dl){
        $splitstr = explode("-", $dl);
        $values[] = $splitstr[1];
    }    
    
    $statusfile=file("ecbstatus");
    $statparam= array();
    $statvalues= array();
    $temp1 = array();
    foreach($statusfile as $d){
        $temp1 = explode("-", $d);
        $statparam[] = $temp1[0];
        $statvalues[] = $temp1[1];
    }
?>

<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="font-awesome.min.css">
<link rel="stylesheet" href="aites.css">
</head>
<body>

    <div class="sidebar">
      <logo-image><img src="logo.png" class="img-fluid"> </logo-image>
      <a href="index.php"><i class="fa fa-fw fa-home"></i> Device Information </a>
      <a href="sipconfig.php"><i class="fa fa-fw fa-phone"></i> SIP configuration </a>
      <a href="networkconfig.php"><i class="fa  fa-sitemap"></i> Network Configuration </a>
      <a href="deviceconfig.php"><i class="fa fa-fw fa-wrench"></i> Device Configuration </a>
      <a href="logserverconfig.php"><i class="fa fa-fw fa-history"></i> Log Configuration </a>
      <?php if ( $_SESSION['username'] == 'admin')
        echo "<a href=\"fileupload.php\"><i class=\"fa fa-fw fa-upload\"></i> File Upload </a>";
        ?>
      <a href="logout.php"><i class="fa fa-fw fa-sign-out"></i> Logout </a>
    </div>
    <div class="header">
    <h2> SIP Configuration
         </h2>
    </div>
    <br><br>
    <div class="main">
    <form method='POST' action="#">
    <table>
        <tr>
        <th>Primary SIP Server </th>
        <th>
        <?php
                if ( $statvalues[0] == 1 ) {
                
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i>";
           
                } else {
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i>";
           
                }
            ?>  
  <?php echo "(  Last Updated :"; echo date('H:i:s d-m-Y'); echo " )";?>
        </th>
        </tr>
        <tr>
        <td>SIP server IP</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[18] ?>/></td>
        </tr>
        <tr>
        <td>SIP server Port</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[19] ?>/></td>
        </tr>
        <tr>
        <td>SIP User ID</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[20] ?>/></td>
        </tr>
        <tr>
        <td>SIP User Password</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[21] ?>/></td>
        </tr>
        <td>SIP Default Dial Number</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[16] ?>/></td>
        </tr>
        <tr>
            <?php if ( $_SESSION['username'] == 'admin') {
        echo "<td>Secondary Server Config </td>";
        echo "<td><input name='name[]' type='text' size=\"50\" value= $values[40] /></td>
        </tr>";
            } ?>
    </table>
    <br><br>
    <?php if ( $values[40] == 0) { 
      //  echo "<button class=\"button\" input type = \"submit\" name = \"submit\" value = \"Submit\">Save</button>";
        echo "<table border=\"0\">
    <tr><td><button class=\"button\" input type = \"submit\" name = \"submit\" value = \"Submit\">Save</button>
    </form></td><td> <form method=\"post\" action=\"#\"><button class=\"button\" input type = \"submit\" name = \"btnCall\" value = \"Submit\">Call</button></form>
       </td></tr>
   </table>";
        echo "<br><br>
    <table>
        <tr><th>
     <i class=\"fa fa-copyright\" style=\"font-size:36px;\"></i> All right reserved by ARYA A-Ites Pvt. Ltd (2022-2025)</th>
</tr>
</table>";
        exit();
    }
    ?>
    <table>
        <tr>
        <th>Secondary SIP Server </th>
        <th>
        <?php
                if ( $statvalues[3] == 1 ) {
                
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i>";
           
                } else {
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i>";
           
                }
            ?>  
        <?php echo "(  Last Updated :"; echo date('H:i:s d-m-Y'); echo " )";?>
        </th>
        </tr>
        <tr>
        <td>SIP server IP</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[29] ?>/></td>
        </tr>
        <tr>
        <td>SIP server Port</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[30] ?>/></td>
        </tr>
        <tr>
        <td>SIP User ID</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[31] ?>/></td>
        </tr>
        <tr>
        <td>SIP User Password</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[32] ?>/></td>
        </tr>
        
    </table>
    <br>
    <br>
    <table border="0">
    <tr><td><button class="button" input type = "submit" name = "submit" value = "Submit">Save</button>
    </form></td><td> <form method="post" action="#"><button class="button" input type = "submit" name = "btnCall" value = "Submit">Call</button></form>
       </td></tr>
   </table>
   <br><br>
    <table>
        <tr><th>
     <i class="fa fa-copyright" style="font-size:36px;"></i> All right reserved by ARYA A-Ites Pvt. Ltd (2022-2025)</th>
</tr>
</table>
    </div>
</body>
</html> 
