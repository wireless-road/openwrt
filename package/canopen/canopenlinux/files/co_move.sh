#!/bin/sh

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 speed"
    echo "Speed is integer from -1000 to 1000"
    echo "Use negative values to move backward"
    echo "Move. Use Ctrl+C to stop."
fi

for i in $(seq 10 13); do
    # set velocity mode    
    cocomm "$i w 0x6060 0 i8 3"
    # rotate cia402 state machine
    cocomm "$i w 0x6040 0 i16 0x6"
    cocomm "$i w 0x6040 0 i16 0x7"
    cocomm "$i w 0x6040 0 i16 0xf"
done

while true; do
    # set speed
    cocomm $(echo \"10 w 0x60FF 0 i32 $(( $1 * -1))\")
    cocomm $(echo \"11 w 0x60FF 0 i32 $(( $1 * -1))\")
    cocomm $(echo \"12 w 0x60FF 0 i32 $1\")
    cocomm $(echo \"13 w 0x60FF 0 i32 $1\")
done