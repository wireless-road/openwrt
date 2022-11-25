<?php
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
?>
