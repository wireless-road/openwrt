<?php
if(session_id() == ''){session_start();}
//header("Refresh:1");
date_default_timezone_set("Asia/Calcutta");
if ( !isset($_SESSION['username'])) {
    header('Location: login.php');
}
$iperror="";
$macerror="";

    if(isset($_POST['btnSave'])){
        $devicefile=file("deviceinfo");
        $file=fopen("tmpdev","w");
        $devparam= array();
        $devvalues= array();
        $temp = array();
        $newvalues = $_POST['name'];
        $i=0;
        foreach($devicefile as $d){
            $temp = explode("-", $d);
            $devparam[] = $temp[0];
            $devvalues[] = $temp[1];
            fprintf($file, "%s-%s\n", $temp[0], $newvalues[$i]);
            $i++;
        }
        $file_pointer = "deviceinfo"; 
        if (!unlink($file_pointer)) { 
            echo ("$file_pointer cannot be deleted due to an error"); 
        }
        rename("tmpdev","deviceinfo");
    }
    if(isset($_POST['btnSaveDevice'])){
        
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
            if ($i >= 13 and $i < 17) {
                          
                    fprintf($file, "%s-%s\n", $splitstr[0], $newValues[$i - 13]);
            }else if($i == 5)
                fprintf($file, "%s-%s\n", $splitstr[0], $newValues[4]);
            else if($i == 7)
                fprintf($file, "%s-%s\n", $splitstr[0], $newValues[5]);
            else
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
        system('mv /usr/bin/aites.log /usr/bin/aites_last.log');
        system('/etc/init.d/baresip restart');
    } 
    $inputfile = file('/usr/bin/AppConfig.ini');
    $values = array();
    $splitstr = array();
    foreach($inputfile as $dl){
        $splitstr = explode("-", $dl);
        $values[] = $splitstr[1];
    }

    $inputfile1 = file('deviceinfo');
    $devvalues = array();
    $splitstr1 = array();
    foreach($inputfile1 as $dl){
        $splitstr1 = explode("-", $dl);
        $devvalues[] = $splitstr1[1];
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
        <th>Device Model Configuration</th>
        <th></th>
        </tr>
        <tr>
        <td>Model </td>
        <td><input name='name[]' type='text' size='30' value=<?php  echo  "\"$devvalues[0]\""  ;?> <?php if ( $_SESSION['username'] != 'admin') echo "readonly"?> /></td>
        </tr>
        <tr>
        <td>HW Version </td>
        <td><input name='name[]' type='text' size='30' value=<?php echo $devvalues[1];?> <?php if ( $_SESSION['username'] != 'admin') echo "readonly"?> /></td>
        </tr>
        <tr>
        <td>Software Version </td>
        <td><input name='name[]' type='text' size='30' value=<?php echo $devvalues[2];?> <?php if ( $_SESSION['username'] != 'admin') echo "readonly"?> /></td>
        </tr>
        <tr>
        <td>Device Description</td>
        <td><input name='name[]' type='text' size='30' value=<?php echo "\"$devvalues[3]\"";?> <?php if ( $_SESSION['username'] != 'admin') echo "readonly"?> /></td>
        </tr>
        <tr>
        <td>Serial Number</td>
        <td><input name='name[]' type='text' size='30' value=<?php echo $devvalues[4];?> <?php if ( $_SESSION['username'] != 'admin') echo "readonly"?> /></td>
        </tr>
        <tr>
        <td>Manufacturing Date (DD/MM/YYYY)</td>
        <td><input name='name[]' type='text' size='30' value=<?php echo $devvalues[5];?> <?php if ( $_SESSION['username'] != 'admin') echo "readonly"?> /></td>
        </tr>
    </table>
    <br>
    <br>
    <button class="button" input type = "submit" name = "btnSave" value = "Submit" <?php if ( $_SESSION['username'] != 'admin') echo "disabled" ?>>Save</button> 
    <br><br><br>
</form>
    <form method='POST' action="#">
    <table>
        <tr>
        <th style='font-size:16px'>Device Other Configuration </th>
        <th></th>
        </tr>
        <tr>
        <tr>
        <td>Data Log Interval </td>
        <td><input name='name[]' type='text' size='30' value=<?php echo $values[12] ?>/></td>
        </tr>
        <tr>
        <td>Data Query Char </td>
        <td><input name='name[]' type='text' size='30' value=<?php echo $values[13] ?>/></td>
        </tr>
        <tr>
        <td>Data Start Char </td>
        <td><input name='name[]' type='text' size='30' value=<?php echo $values[14] ?>/></td>
        </tr>
        <tr>
        <td>Data End Char </td>
        <td><input name='name[]' type='text' size='30' value=<?php echo $values[15] ?>/></td>
        </tr>
        <tr>
        <td>Speaker Volume </td>
        <td><input name='name[]' type='number' min="1" max="10" size='30' value=<?php echo $values[4] ?>/></td>
        </tr>
        <tr>
        <td>Error Audio play count  </td>
        <td><input name='name[]' type='number'  min="1" max="5" size='30' value=<?php echo $values[6] ?>/></td>
        </tr>
    </table>
    <br>
    <br>
    <button class="button" input type = "submit" name = "btnSaveDevice" value = "Submit">Save</button>
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
