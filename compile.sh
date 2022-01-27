#!/bin/bash

#Put Openwrt config for new board to './openwrt-configs' directory with '.config' suffix
# so this script can work with it.


name="$0";
boardsdir="./openwrt-configs";
USER_DIR=$( getent passwd "$USER" | cut -d: -f6 )
FINAL_PATH=$USER_DIR"/build/bin/"$(date '+%Y_%m_%d_%H_%M_%S/');

show_boards() {
	echo -n "Available board names:";
	local itr=0;
	for l in $(ls -1 $boardsdir/*.config|sed -e 's/\.config//g'|sed -e 's/.*\///g');do
		if [ $itr == "0" ];then
		    echo ; echo "     ";
		    itr=5;
		fi;
		echo -n "$l ";
		itr=$(expr $itr - 1);
	done
	echo;echo;
};

show_usage() {
	echo;
	echo "USAGE:";
	echo "     $name [command or boardname]";
	echo ;
	echo "Where [command] can be one of:";
	echo "     list       to list available boards configs to compile.";
	echo "     help       to show this help.";
	echo "     usage      to show this help.";
	echo ;
	echo "Example:";
	echo "     $name list";
	echo "  or";
	echo "     $name zbt-we1326";
	echo;
};

compiled_successful_flag=0;

compilei() {
	compiled_successful_flag=0;
	if make -j $(nproc); then
		compiled_successful_flag=1;
		return
	else
		return
	fi
}

compile_board() {
	compiled_successful_flag=0;
	local boardname="$1";
	local configfile="$boardsdir/$boardname.config";

	if [ -d "build_dir/target-arm_cortex-a7+neon-vfpv4_musl_eabi/linux-imx6ull_cortexa7" ];then
		echo "-- removing all built packages.";
		rm -rf ./build_dir/target-arm_cortex-a7+neon-vfpv4_musl_eabi/linux-imx6ull_cortexa7;
	fi
	
	if [ ! -f "$configfile" ];then
		echo;
		echo "   No such config file found. Use 'list' argument to see available configs.";
		echo;
		return;
	fi
	
	if [ ! -d "./feeds" ];then
		./scripts/feeds update -a
		./scripts/feeds install -a
	fi
	
	cp "$configfile" ./.config;
	yes "" | make -j $(nproc) oldconfig;
	while [ $compiled_successful_flag -eq 0 ];
	do
		compilei;
		sleep 1;
	done
	if [ $? -eq 0 ] && [ -n "$FINAL_PATH" ]; then
		cp -R ./bin/targets/imx6ull/cortexa7/ "$FINAL_PATH"
		cp -R ./bin/packages/arm_cortex-a7_neon-vfpv4/ "$FINAL_PATH"/packages
	fi

	compiled_successful_flag=1;
	echo "   Done.";
};

compile_all()
{
	local board

	for l in $boardsdir/*.config;do
		compiled_successful_flag=0;
		board="$(echo $l | sed -e 's:.*/::g' -e 's:\..*::')"
		# Rebuild linux kernel to prevent incompatibility list between targets.
		echo "------------------ Compile $board configuration";
		compile_board "$board"; 
	done
}

compile()
{
	local boardname=$1
	echo "$FINAL_PATH"
	mkdir -p "$FINAL_PATH"

	if [ "$boardname" == "all" ]; then
		compile_all
	else
		compile_board "$boardname"
	fi
}


if [ X"$1" = X"" ]; then
	show_usage;
	exit 0;
fi

case "$1" in
list)
	show_boards;
	;;
help|usage)
	show_usage;
	;;
*)
	compile "$1";
	;;
esac




