#!/usr/bin/env bash

BUILDPATH="Build"
FVPATH="Build/OvmfX64/DEBUG_GCC5/FV"
APPPATH="Build/OvmfX64/DEBUG_GCC5/X64"
DISKIMG="Qemu/Images/disk.img"
LOGFILE="Qemu/Log/debug.log"
MNTPNT="/mnt/disk_image"
CPSPATH="$APPPATH/CapsuleApp.efi"
SHLPATH="Qemu/Shell/Shell.efi"
BIOSIMG="$FVPATH/OVMF.fd"

MANOS="manOS"
MANOS_SRC_PATH="Qemu/ManOS"
MANOS_DST_PATH="$MNTPNT/EFI/AMD"

start_time=`date +%s%N`

rm -f $BIOSIMG
rm -f $FVPATH/SYSTEMFIRMWAREUPDATECARGO.Fv

#OvmfPkg/build.sh -a X64 -D DEBUG_ON_SERIAL_PORT -D CAPSULE_ENABLE -D SMM_REQUIRE
OvmfPkg/build.sh -a X64 -D DEBUG_ON_SERIAL_PORT 

if [[ ! -f $BIOSIMG ]]; then
  echo "Build error"
  exit
else

	echo "Copy images and applications to disk.img"

	if [[ -f $DISKIMG ]]; then
		echo "$DISKIMG exists"
	else
		echo "$DISKIMG doesn't exist"
		echo "Create $DISKIMG"
		dd if=/dev/zero of=$DISKIMG obs=512 ibs=512 bs=512 count=65536
		echo "o
n
p
1


t
c
w" | fdisk $DISKIMG

		mkfs.vfat $DISKIMG

	fi

  rm -rf $MNTPNT
	mkdir $MNTPNT
	mount $DISKIMG $MNTPNT
  if [[ -f $FVPATH/OVMFFIRMWAREUPDATECAPSULEFMPPKCS7.Cap ]]; then
	  cp $FVPATH/OVMFFIRMWAREUPDATECAPSULEFMPPKCS7.Cap $MNTPNT
  fi
  if [[ -f $CPSPATH ]]; then
	  cp $CPSPATH $MNTPNT
  fi
  if [[ -f $SHLPATH ]]; then
	  cp $SHLPATH $MNTPNT
  fi
  if [[ -f $MANOS_SRC_PATH/$MANOS ]]; then
    if [[ ! -d $MANOS_DST_PATH ]]; then
      mkdir -p $MANOS_DST_PATH
    fi
    cp $MANOS_SRC_PATH/$MANOS $MANOS_DST_PATH
  fi
	umount $MNTPNT
	rm -rf $MNTPNT
	echo "Done"

	echo "rum qemu"
	cp $FVPATH/OVMF.fd Qemu/BIOS/
	# qemu-system-x86_64 -bios Qemu/BIOS/OVMF.fd -serial file:$LOGFILE -machine q35 -hda $DISKIMG
	qemu-system-x86_64 -drive file=Qemu/BIOS/OVMF.fd,format=raw,if=pflash \
                     -global ICH9-LPC.disable_s3=1 \
                     -serial file:$LOGFILE \
                     -machine q35 \
                     -hda $DISKIMG
fi

find $BUILDPATH | xargs chown shannon

echo "Please find the BIOS log at Qemu/Log/debug.log"
cp $LOGFILE ./

end_time=`date +%s%N`
time_used=`echo $end_time $start_time | awk '{print($1 - $2)/1000000000}'`

echo "Total time used: $time_used seconds"