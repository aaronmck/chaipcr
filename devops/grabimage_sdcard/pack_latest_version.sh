#!/bin/bash
#
# Chai PCR - Software platform for Open qPCR and Chai's Real-Time PCR instruments.
# For more information visit http://www.chaibio.com
#
# Copyright 2016 Chai Biotechnologies Inc. <info@chaibio.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

if ! id | grep -q root; then
	echo "must be run as root"
	exit 1
fi

if [ -e /dev/mmcblk1p4 ] ; then
	sdcard_dev="/dev/mmcblk0"
	eMMC="/dev/mmcblk1"
fi

if [ -e /dev/mmcblk0p4 ] ; then
	sdcard_dev="/dev/mmcblk1"
	eMMC="/dev/mmcblk0"
fi

if [ ! -e "${eMMC}p4" ]
then
        echo "Proper eMMC partitionining not found!"
	exit 1
fi

#check_running_system
echo timer > /sys/class/leds/beaglebone\:green\:usr0/trigger
sdcard="/tmp/sdcard"

image_filename_upgrade1="${sdcard}/eMMC_part1.img"
image_filename_upgrade2="${sdcard}/eMMC_part2.img"
image_filename_upgrade_temp1="${sdcard}/eMMC_part1.img.tmp"
image_filename_upgrade_temp2="${sdcard}/eMMC_part2.img.tmp"


echo "Packing eMMC image.."

if [ -e  ${sdcard} ]
then
	echo "$sdcard: exits!"
else
	mkdir -p ${sdcard}
fi

#umount ${sdcard} >/dev/null || true
mount ${sdcard_dev}p1 ${sdcard} || true

if [ -e $image_filename_upgrade1 ]
then
	rm $image_filename_upgrade1
fi
if [ -e $image_filename_upgrade2 ]
then
        rm $image_filename_upgrade2
fi

if [ -e $image_filename_upgrade_temp1 ]
then
	rm $image_filename_upgrade_temp1
fi
if [ -e $image_filename_upgrade_temp2 ]
then
        rm $image_filename_upgrade_temp2
fi

if [ ! -e ${sdcard}/tmp ]
then
        mkdir -p ${sdcard}/tmp/
fi

echo "Copying eMMC from $eMMC"
sync
sleep 2

dd  if=${eMMC} bs=16M of=$image_filename_upgrade_temp1 count=120
if [ $? -gt 0 ]
then
        exit 1
fi

dd  if=${eMMC} bs=16M of=$image_filename_upgrade_temp2 skip=120
if [ $? -gt 0 ]
then
	exit 1
fi

sleep 5
sync

echo "Finalizing: $image_filename_upgrade1, and $image_filename_upgrade2"
mv $image_filename_upgrade_temp1 $image_filename_upgrade1
mv $image_filename_upgrade_temp2 $image_filename_upgrade2

echo "Finished.. image parts are at: $image_filename_upgrade1, and  $image_filename_upgrade2"

if [ -e ${sdcard}/pack_resume_autorun.flag ]
then
	rm ${sdcard}/pack_resume_autorun.flag>/dev/null || true
fi

sync
echo default-on > /sys/class/leds/beaglebone\:green\:usr0/trigger

alldone () {
	if [ -e /sys/class/leds/beaglebone\:green\:usr0/trigger ] ; then
		echo default-on > /sys/class/leds/beaglebone\:green\:usr0/trigger
		echo default-on > /sys/class/leds/beaglebone\:green\:usr1/trigger
		echo default-on > /sys/class/leds/beaglebone\:green\:usr2/trigger
		echo default-on > /sys/class/leds/beaglebone\:green\:usr3/trigger
	fi

	echo "Done!"

	echo "Halt..."
	sync
	
	halt -d 5
	sleep 180
	exit 0
}


if [ -e $image_filename_upgrade1 ]
then
	alldone
	exit 0
fi

exit 1