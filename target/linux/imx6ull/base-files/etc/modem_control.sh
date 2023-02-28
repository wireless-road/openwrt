#!/bin/sh

case "$1" in
start)
    echo 8 > /sys/class/gpio/export
    echo out > /sys/class/gpio/gpio8/direction
    echo 1 > /sys/class/gpio/gpio8/value
    sleep 5
    ifup wwan
    ;;
stop)
    ifdown wwan
    echo 0 > /sys/class/gpio/gpio8/value
    echo 8 > /sys/class/gpio/unexport
    ;;
*)
    echo "Usage $0 start|stop"
    ;;
esac