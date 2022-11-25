<?php
if(session_id() == ''){session_start();}
header("Refresh:10");
//echo $_SESSION['username'];
date_default_timezone_set("Asia/Calcutta");
if ( ! isset($_SESSION['username'])) {
    header('Location: login.php');
}
$_SESSION['application_running'] = shell_exec("pidof  aitesbaresip");
if(isset($_POST['btnReboot'])){
    shell_exec('reboot');    
}

    $devicefile=file("deviceinfo");
    $devparam= array();
    $devvalues= array();
    $temp = array();
    foreach($devicefile as $d){
        $temp = explode("-", $d);
        $devparam[] = $temp[0];
        $devvalues[] = $temp[1];
    }

    $inputfile = file('/usr/bin/AppConfig.ini');
    $paramName = array();
    $values = array();
    $splitstr = array();
    foreach($inputfile as $dl){
        $splitstr = explode("-", $dl);
        $paramName[] = $splitstr[0];
        $values[] = $splitstr[1];
    }
    if($values[17] == '1')
      $isPrimary = "Yes";
    else
      $isPrimary = "No";

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
<link rel="stylesheet" href="fa-solid.min.css">
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
    <h2> Device Information</h2>
    
    </div>
    <br><br>
    <div class="main">
    <form method='POST' action="#">
    <table>
        <tr>
        <th colspan="7">Device Model Information</th>
        <th></th>
        </tr>
        <tr>
        <td><b>Device Model</b></td><td><b>Hardware Version</b></td><td><b>Software Version</b></td><td colspan="2"><b>Device Description</b></td><td><b>Serial No </b></td><td><b>Manufacturing Date</b></td>
        <td ></td>
        </tr>
        <tr>
            <td><?php echo $devvalues[0] ?></td>
            <td><?php echo $devvalues[1] ?></td>
            <td><?php echo $devvalues[2] ?></td>    
            <td colspan="2"><?php echo $devvalues[3] ?></td>
            <td><?php echo $devvalues[4] ?></td>
            <td><?php echo $devvalues[5] ?></td>
        </tr>
        
    </table>
    <br><br>
    <table>
        <tr>
        <th colspan="5">Network Information</th>
        </tr>
        <tr>
        <td><b>IP Address</b> </td> <td><b>Mac Address</b> </td><td><b>Gateway</b> </td><td><b>Subnet mask</b></td><td><b>DNS</b></td>
        </tr>
        <tr>
            <td><?php echo $values[7] ?> </td>
            <td><?php echo $values[8] ?></td>
            <td><?php echo $values[9] ?></td> 
            <td><?php echo $values[10] ?></td>
            <td><?php echo $values[11] ?></td>   
        </tr>
        
    </table>
    <br>
    <br>
    <table>
        <tr>
        <th colspan="5">Primary SIP Server Information</th>
        </tr>
        <tr>
        <td><b>SIP Server IP</b> </td> <td><b>SIP Server Port</b> </td><td><b>SIP User ID</b> </td><td></td><td></td>
        </tr>
        <tr>
            <td><?php echo $values[18] ?> </td>
            <td><?php echo $values[19] ?></td>
            <td><?php echo $values[20] ?></td> 
            <td></td>
            <td></td>
        </tr>
    
    </table>
    <br>
    <br>
    <table>
        <tr>
        <th colspan="5">Primary Log Server Information</th>
        </tr>
        <tr>
        <td><b>Log Server IP</b> </td> <td><b>Log Server Port</b> </td><td></td><td></td><td></td>
        </tr>
        <tr>
            <td><?php echo $values[24] ?> </td>
            <td><?php echo $values[25] ?></td>
            
            <td></td>
            <td></td>
        </tr>
    
    </table>
    <br><br>
    <?php if ($values[40] == 1 ) {
       //$var=<<<'EOD'
       echo" <table> <tr>
        <th colspan=\"5\">Secondary SIP Server Information</th>
        </tr>
        <tr>
        <td><b>SIP Server IP</b> </td> <td><b>SIP Server Port</b> </td><td><b>SIP User ID</b> </td><td></td><td></td>
        </tr>
        <tr>
        <td> $values[29]"; 
        //EOD;
          
        //$var1=<<<'EOD'
         echo "</td>  $values[30] <td>" ;
            //EOD;
             
        //$var2=<<<'EOD'
          echo"</td><td> $values[31]";
            //EOD; 
            // 
        //$var3=<<<'EOD'
            echo "</td> 
            <td></td>
            <td></td>
            </tr> </table>
            <br>
            <br>
            <table>
            <tr>
            <th colspan=\"5\">Secondary Log Server Information</th>
            </tr>
            <tr>
            <td><b>Log Server IP</b> </td> <td><b>Log Server Port</b> </td><td></td><td></td><td></td>
            </tr>
            <tr> 
            <td>$values[35]" ;
            //EOD;
           //   
    //$var4=<<<'EOD'
            echo "</td><td>$values[36]";
            //EOD;
         //     
    //$var5=<<<'EOD'
             echo "</td>
            
            <td></td>
            <td></td>
        </tr>
    
    </table>";
    
    //EOD;

    //echo $var ; echo $values[29]; echo $var1;echo $values[30]; echo $var2;echo $values[31];echo $var3;echo $values[35];echo $var4;echo $values[36];echo $var5;
    }
    ?>
    <br><br>
    <table>
        <tr><th colspan="3">ECB Status <?php echo "(  Last Updated :"; echo date('H:i:s d-m-Y'); echo " )";?></th></tr>
        <tr><td text-align='center'>
        <?php 
            if ( isset($_SESSION['application_running'] )) {
                
                echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:green;align:center\"></i> </td><td><b>Application Running...</b>";
           
            } else {
                echo "<i class=\"fa fa-circle\" style=\"font-size:40px;color:red;align:center\"></i> </td><td> <b>Application Stopped...</b>";
           
            }
        ?>
        </td>
        <td>
            <form name="reboot" method="post" action="#">
                <button class="button" input type = "submit" name = "btnReboot" value = "Submit">Reboot</button> 
            </form>
        </td>
        </tr>
        <tr> 
            <?php
                if ( $statvalues[0] == 1 ) {
                
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i> </td><td><b>Sip Registered...</b></td>";
           
                } else {
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i> </td><td> <b>Sip Not Registered...</b></td>";
           
                }
            ?>
            <td></td>
        </tr>
        <tr>
        <?php
                if ( $statvalues[1] == 1 ) {
                
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i> </td><td><b>Network Available...</b></td>";
           
                } else {
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i> </td><td> <b>Network Not Availabale...</b></td>";
           
                }
            ?>
            <td></td>
        </tr>

        <tr>
        <?php
                if ( $statvalues[2] == 1 ) {
                
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i> </td><td><b>Log Server Available...</b></td>";
           
                } else {
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i> </td><td> <b>Log Server Not Availabale...</b></td>";
           
                }
            
            echo "<td>Last Log Sent:"; echo "\n";echo readfile('lastlog'); echo "</td>";
            ?>
        </tr>
        <?php
        if ($values[40] == 1) {
        echo "<tr>";
        
                if ( $statvalues[3] == 1 ) {
                
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i> </td><td><b>Secondary Sip Registered...</b></td>";
           
                } else {
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i> </td><td> <b>Secondary Sip Not Registered...</b></td>";
           
                }
            
            echo "<td></td>";
        echo "</tr>";
        echo "<tr>";
        
                if ( $statvalues[4] == 1 ) {
                
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:green\"></i> </td><td><b>Secondary Log Server Available...</b></td>";
           
                } else {
                    echo "<td><i class=\"fa fa-circle\" style=\"font-size:40px;color:red\"></i> </td><td> <b>Secondary Log Server Not Available...</b></td>";
           
                }
            
                echo "<td>Last Log Sent:"; echo "\n";echo readfile('lastlog'); echo "</td>";
        echo "</tr>";
            }
            ?>
    </table>
    <!--<button class="button" input type = "submit" name = "submit" value = "Submit">Save</button> -->
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
