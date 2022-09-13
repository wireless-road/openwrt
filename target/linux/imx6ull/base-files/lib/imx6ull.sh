#!/bin/sh
#
# Copyright (C) 2010-2013 OpenWrt.org
#

IMX6_BOARD_NAME=
IMX6_MODEL=

imx6ull_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	"LoraWan Gateway Ethernet")
		name="lorawan_gateway_ethernet";
		;;
	"LoraWan Gateway 3G")
		name="lorawan_gateway_3g";
		;;
	"LoraWan Gateway WiFi")
		name="lorawan_gateway_wifi";
		;;
	"Video Stream Ethernet")
		name="video_stream_ethernet";
		;;
	"Video Stream 3G")
		name="video_stream_3g";
		;;
	"Video Stream WiFi")
		name="video_stream_wifi";
		;;
	"Tensorflow WiFi")
		name="tensorflow_wifi";
		;;
	"Tensorflow WiFi Dev")
		name="tensorflow_wifi_dev";
		;;
	"Audio Stream Ethernet")
		name="audio_stream_ethernet";
		;;
	"Audio Stream 3G")
		name="audio_stream_3g";
		;;
	"Audio Stream WiFi")
		name="audio_stream_wifi";
		;;
	"FlexCAN Ethernet")
		name="flexcan_ethernet";
		;;
	"FlexCAN 3G")
		name="flexcan_3g";
		;;
	"FlexCAN WiFi")
		name="flexcan_wifi";
		;;
	"Amazon Voice Service Ethernet")
		name="amazon_voice_service_ethernet";
		;;
	"Amazon Voice Service 3G")
		name="amazon_voice_service_3g";
		;;
	"Amazon Voice Service WiFi")
		name="amazon_voice_service_wifi";
		;;
	"ICPCON")
		name="icpcon";
		;;
	*)

		name="generic"
		;;
	esac

	if [ "$name" != "generic" ];then
		model="$name";
	fi;

	[ -z "$IMX6_BOARD_NAME" ] && IMX6_BOARD_NAME="$name"
	[ -z "$IMX6_MODEL" ] && IMX6_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$IMX6_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$IMX6_MODEL" > /tmp/sysinfo/model
}
