#!/bin/ash
read -r -p "Install Homeassistant? [y/N] " response
case "$response" in
    [yY][eE][sS]|[yY]) 
        opkg update
	opkg install ha-all
        ;;
    *)
        exit
        ;;
esac