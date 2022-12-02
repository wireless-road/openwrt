<?php
if(session_id() == ''){session_start();}
if ( !isset($_SESSION['username'])) {
    header('Location: login.php');
}
    $audioRet1 = "";
    $audioRet2 = "";
    $imageRet = "";
    $binRet = "";
    if(isset($_GET['failAudiosize1'])){
        $audioRet1 = "Audio file size is large - Max size is 5MB ";
    }
    if(isset($_GET['failAudiotype1'])){
        $audioRet1 = "Audio file type is invalid - .wav files are allowed ";
    }
    if(isset($_GET['successAudio1'])){
        $audioRet1 = "Successfully uploaded audio file ";
    }
    if(isset($_GET['failAudio1'])){
        $audioRet1 = "Failed to upload audio file";
    }

    if(isset($_GET['failAudiosize2'])){
        $audioRet2 = "Audio file size is large - Max size is 5MB ";
    }
    if(isset($_GET['failAudiotype2'])){
        $audioRet2 = "Audio file type is invalid - .wav files are allowed ";
    }
    if(isset($_GET['successAudio2'])){
        $audioRet2 = "Successfully uploaded audio file ";
    }
    if(isset($_GET['failAudio2'])){
        $audioRet2 = "Failed to upload audio file";
    }


    if(isset($_GET['failImagefile'])){
        $imageRet = "Image file is not valid image file";
    }
    if(isset($_GET['failImagesize'])){
        $imageRet = "Image file size is large - Max size is 1MB ";
    }
    if(isset($_GET['failImagetype'])){
        $imageRet = "Image file type is invalid - logo.png file only allowed";
    }
    if(isset($_GET['successImage'])){
        $imageRet = "Successfully uploaded Logo file ";
    }
    if(isset($_GET['failImage'])){
        $imageRet = "Failed to upload Logo file";
    }

    if(isset($_GET['failBinsize'])){
        $binRet = "Firmware file size is large - Max size is 20MB ";
    }
    if(isset($_GET['failBintype'])){
        $binRet = "Firmware file type is invalid - .bin files are allowed ";
    }
    if(isset($_GET['successBin'])){
        $binRet = "Successfully uploaded Firmware file ";
        // $host    = "127.0.0.1";
        // $port    = 5500;
        // $message = "KILL";
        // //echo "Message To server :".$message;
        // // create socket
        // $socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
        // // connect to server
        // $result = socket_connect($socket, $host, $port) or die("Could not connect to server\n");  
        // // send string to server
        // socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");
        // // close socket
        // socket_close($socket);
        // system('mv /usr/bin/aites.log /usr/bin/aites_last.log');
        $file_pointer = "/usr/bin/baresip"; 
        if (!unlink($file_pointer)) { 
            echo ("$file_pointer cannot be deleted due to an error"); 
        } 
        // Rename new config to App config
        rename("/usr/bin/aites","/usr/bin/baresip");
        system('chmod 770 /usr/bin/baresip');
        system('mv /usr/bin/aites.log /usr/bin/aites_last.log');
        system('/usr/bin/baresip -f /etc/baresip >> /usr/bin/aites.log &');

    }
    if(isset($_GET['failBin'])){
        $binRet = "Failed to upload Firmware file";
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
    <h2> File Upload  </h2>
    </div>
    <div class="main">
    <br>
    <h3>Network Cable not connected Audio File upload </h3>
    <form action="uploadwav1.php" method="post" enctype="multipart/form-data">
        Select .WAV file to upload:
        <br>
        <br>
        <input type="file" name="fileToUpload1" id="fileToUpload1">
        <input type="submit" value="Upload File" name="submit">
        <br>
    <br>    
    <?php echo $audioRet1 ?>
    </form>
    <br>
    <br>
    <h3>Server not reachable Audio File upload </h3>
    <form action="uploadwav2.php" method="post" enctype="multipart/form-data">
        Select .WAV file to upload:
        <br>
        <br>
        <input type="file" name="fileToUpload2" id="fileToUpload2">
        <input type="submit" value="Upload File" name="submit">
        <br>
    <br>    
    <?php echo $audioRet2 ?>
    </form>
    <br>
    <br>
    <h3> Logo file upload </h3>
    <form action="uploadimg.php" method="post" enctype="multipart/form-data">
        Select "logo.png" file to upload:
        <br>
        <br>
        <input type="file" name="fileToUpload3" id="fileToUpload3">
        <input type="submit" value="Upload File" name="submit">
    </form>
    <br>
    <?php echo $imageRet ?>
    <br>    
    <h3>Application upgrade file upload </h3>
    <form action="uploadbin.php" method="post" enctype="multipart/form-data">
        Select .bin file to upload:
        <br>
        <br>
        <input type="file" name="fileToUpload4" id="fileToUpload4">
        <input type="submit" value="Upload File" name="submit">
    </form>
    <br>
    <?php echo $binRet ?>

    <br><br>
    <table>
        <tr><th>
     <i class="fa fa-copyright" style="font-size:36px;"></i> All right reserved by ARYA A-Ites Pvt. Ltd (2022-2025)</th>
</tr>
</table>
    </div>
</body>
</html> 
