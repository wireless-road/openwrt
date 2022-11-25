<?php
if(session_id() == ''){session_start();}
//header("Refresh:1");
date_default_timezone_set("Asia/Calcutta");
if ( !isset($_SESSION['username'])) {
    header('Location: login.php');
}
$iperror="";
$macerror="";

function checkPassword() {
    echo '<script type="text/javascript"> ';
    echo 'var inputname = prompt("Please enter Password", "");';
    echo 'alert(inputname);';
    echo '</script>';
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

    if(isset($_POST['btnSaveNetwork'])){
        //$iperror="";
        //$macerror="";
        //checkPassword();
        $inputfile = file('/usr/bin/AppConfig.ini');
        $file = fopen("tmpConfig.ini","w");
        $paramName = array();
        $values = array();
        $splitstr = array();
        $i = 1;
        $newValues = $_POST['name'];
        foreach($inputfile as $dl){
            $splitstr = explode("-", $dl);
            $paramName[] = $splitstr[0];
            $values[] = $splitstr[1];
            if ($i >= 8 and $i <= 12){
                if ( in_array($i,[8,10,11,12])) {    
                    if (filter_var($newValues[$i - 8], FILTER_VALIDATE_IP, FILTER_FLAG_IPV4)){
                        fprintf($file, "%s-%s\n", $splitstr[0], $newValues[$i - 8]);
                        $iperror="";
                    } else{
                        fprintf($file, "%s-%s", $splitstr[0], $splitstr[1]);
                        $iperror="Invalid_IP_Address";
                    }
                } 
                else {
                    if (filter_var($newValues[$i - 8], FILTER_VALIDATE_MAC)){
                        fprintf($file, "%s-%s\n", $splitstr[0], $newValues[$i - 8]);
                        $macerror="";
                    } else {
                        fprintf($file, "%s-%s", $splitstr[0], $splitstr[1]);
                        $macerror="Invalid_MAC_Address";
                    }
                } 
                
        
           }else
                fprintf($file, "%s-%s", $splitstr[0], $splitstr[1]);
            $i++;
        }
        // delete Appconfig file
        $file_pointer = "/usr/bin/AppConfig.ini";
        if (!unlink($file_pointer)) {
            echo ("$file_pointer cannot be deleted due to an error");
        }
        // Rename new config to App config
        rename("tmpConfig.ini","/usr/bin/AppConfig.ini");
        $output = shell_exec('sudo chmod 777 /usr/bin/AppConfig.ini');
        $host    = "127.0.0.1";
        $port    = 5500;
        $message = "KILL";
        echo "Message To server :".$message;
        // create socket
        $socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
        // connect to server
        $result = socket_connect($socket, $host, $port) or die("Could not connect to server\n");
        // send string to server
        socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");
        // close socket
        socket_close($socket);
        $myfile = fopen("/usr/bin/ipaddr", "w") or die("Unable to open file!");
        //$myfile = fopen("ipaddr", "w") or die("Unable to open file!");
        fprintf($myfile, "%s\n", $newValues[0]);
        fprintf($myfile, "%s\n", $newValues[1]);
        fprintf($myfile, "%s\n", $newValues[2]);
        fprintf($myfile, "%s\n", $newValues[3]);
        fprintf($myfile, "%s\n", $newValues[4]);
        fclose($myfile);
        system('/etc/init.d/aites_net start');
        system('mv /usr/bin/aites.log /usr/bin/aites_last.log');
        system('/usr/bin/aitesbaresip -f /etc/baresip >> /usr/bin/aites.log &');
      }
?>

<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
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
    <h2> Device Configuration </h2>
    </div>
    <div class="main">
        <br><br>
    <form method='POST' action="#">
    <table>
        <tr>
        <th>Network Information</th>
        <th>
        <?php
                if ( $statvalues[1] == 1 ) {
                
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i>";
           
                } else {
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i>";
           
                }
            ?>  
          <?php echo "(  Last Updated :"; echo date('H:i:s d-m-Y'); echo " )";?>
        </th>
        </tr>
        <tr>
        <td>IP Address </td>
        <td><input name='name[]' type='text' value=<?php if( empty($iperror)) echo $values[7]; else echo $iperror; ?>/></td>
        </tr>
        <tr>
        <td>Mac Address </td>
        <td><input name='name[]' type='text' value=<?php if( empty($macerror)) echo $values[8]; else echo $macerror; ?> /></td>
        </tr>
        <tr>
        <td>Gateway </td>
        <td><input name='name[]' type='text' value=<?php if(empty($iperror)) echo $values[9]; else echo $iperror;?> /></td>
        </tr>
        <tr>
        <td>Subnet mask</td>
        <td><input name='name[]' type='text' value=<?php if(empty($iperror)) echo $values[10]; else echo $iperror;?> /></td>
        </tr>
        <tr>
        <td>DNS</td>
        <td><input name='name[]' type='text' value=<?php if(empty($iperror)) echo $values[11]; else echo $iperror;?> /></td>
        </tr>
    </table>
    <br>
    <br>
    <button class="button" input type = "submit" name = "btnSaveNetwork" value = "Submit">Save</button> 
    
</form>
       <br><br>
    <table>
        <tr><th>
     <i class="fa fa-copyright" style="font-size:36px;"></i> All right reserved by ARYA A-Ites Pvt. Ltd (2022-2025)</th>
</tr>
</table>
    </div>
</body>
</html> 
