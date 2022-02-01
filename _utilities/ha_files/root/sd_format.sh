#!/bin/ash
block info
read -r -p "Is /dev/mmcblk0p1 your sdcard? [y/N] " response
case "$response" in
    [yY][eE][sS]|[yY]) 
        echo "Format sdcard"
	mkfs.f2fs /dev/mmcblk0p1
        ;;
    *)
        exit
        ;;
esac

echo "set sdcard as overlay"
eval $(block info /dev/mmcblk0p1 | grep -o -e "UUID=\S*")
uci -q delete fstab.overlay
uci set fstab.overlay="mount"
uci set fstab.overlay.uuid="${UUID}"
uci set fstab.overlay.target="/overlay"
uci commit fstab
echo "Start copy overlay"
mkdir -p /tmp/cproot
mount --bind /overlay /tmp/cproot
mount /dev/mmcblk0p1 /mnt
tar -C /tmp/cproot -cvf - . | tar -C /mnt -xf -	
umount /tmp/cproot /mnt
echo "All complete. Rebooting"
reboot