#!/bin/sh

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 old_id new_id"
    exit 0
fi

# Try to read actual id.
echo "Reading current id"
cocomm "$1 r 0x200D 0 i16"
echo "Setting new id"
cocomm "$1 w 0x200D 0 i16 $2"
echo "Resetting node"
cocomm "$1 reset node"