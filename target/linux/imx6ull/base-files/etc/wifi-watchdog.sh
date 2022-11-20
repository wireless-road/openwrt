#!/bin/sh
. /usr/share/libubox/jshn.sh
json_init
let _rc++
logger -t wmon "STA: Carrier lost on ${1}, startimg recovery.."

while [ ${_rc} -le 30 ]
    do
	    json_load "$(ubus -S call network.device status '{ "name": "sta0" }')"
        json_get_var status carrier
        logger -t wmon "STA:  TRY: ${_rc} STATUS: ${status}"

        if [ ${status} = '1' ]; then
              logger -t wmon "STA: Carrier acquired, stop monitoring"
	      /etc/init.d/network reload
              break
        fi
        sleep 1
        let ++_rc
    done
if [ ${status} = '0' ]; then
    logger -t wmon "STA: Association timeout! Reconfiguring for AP-only mode."
    uci set wireless.@wifi-iface[0].disabled='1'
    uci commit wireless
    wifi down && wifi up
    /etc/init.d.network reload
    uci set wireless.@wifi-iface[0].disabled='0'
    uci commit wireless
fi
json_cleanup
