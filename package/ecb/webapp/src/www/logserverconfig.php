<?php
if(session_id() == ''){session_start();}
//header("Refresh:1");
date_default_timezone_set("Asia/Calcutta");
if ( !isset($_SESSION['username'])) {
    header('Location: login.php');
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
            if (($i >= 25 and $i <= 29) ){
                fprintf($file, "%s-%s\n", $splitstr[0], $newValues[$j]);
                $j++;
            }

            else if (($i >= 36 and $i <= 40)  and count($newValues) > 5) {
                fprintf($file, "%s-%s\n", $splitstr[0], $newValues[$j]);
                $j++;
            }
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
        system('mv /usr/bin/aites.log /usr/bin/aites_last.log');
        system('/usr/bin/baresip -f /etc/baresip >> /usr/bin/aites.log &');
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
    <h2> Log Server Configuration  </h2>
    </div>
    <br><br>
    <div class="main">
    <form method='POST' action="#">
    <table>
        <tr>
        <th>Primary Log Server </th>
        <th>
        <?php
                if ( $statvalues[2] == 1 ) {
                
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i>";
           
                } else {
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i>";
           
                }
            ?>  
  <?php echo "(  Last Updated :"; echo date('H:i:s d-m-Y'); echo " )";?>
        </th>
        </tr>
        <tr>
        
        <tr>
        <td>Log Server IP</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[24] ?>/></td>
        </tr>
        <tr>
        <td>Log Server Port</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[25] ?>/></td>
        </tr>
        <tr>
        <td>Time API Path</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[26]; if($_SESSION['username']!='admin') echo "readonly" ;?>/></td>
        </tr>
        <tr>
        <td>Log API Path</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[27]; if($_SESSION['username']!='admin') echo "readonly" ;?>/></td>
        </tr>
        <tr>
        <td>API Secret Key </td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[28]; if($_SESSION['username']!='admin') echo "readonly" ;?>/></td>
        </tr>
        <tr>

    </table>
    <br><br>
    <?php if ( $values[40] == 0) { 
        echo "<button class=\"button\" input type = \"submit\" name = \"submit\" value = \"Submit\">Save</button>";
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
        <th>Secondary Log Server </th>
        <th>
        <?php
                if ( $statvalues[4] == 1 ) {
                
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i>";
           
                } else {
                    echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i>";
           
                }
            ?>  
        <?php echo "(  Last Updated :"; echo date('H:i:s d-m-Y'); echo " )";?>
        </th>
        </tr>
        <tr>
        
        <tr>
        <td>Log Server IP</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[35] ?>/></td>
        </tr>
        <tr>
        <td>Log Server Port</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[36] ?>/></td>
        </tr>
        <tr>
        <td>Time API Path</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[37];if($_SESSION['username']!='admin') echo "readonly" ; ?>/></td>
        </tr>
        <tr>
        <td>Log API Path</td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[38];if($_SESSION['username']!='admin') echo "readonly" ; ?>/></td>
        </tr>
        <tr>
        <td>API Secret Key </td>
        <td><input name='name[]' type='text' size="50" value=<?php echo $values[39];if($_SESSION['username']!='admin') echo "readonly" ; ?>/></td>
        </tr>
    </table>
    <br>
    <br>
    <button class="button" input type = "submit" name = "submit" value = "Submit">Save</button>
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
