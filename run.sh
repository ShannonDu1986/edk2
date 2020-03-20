#!/usr/bin/env bash

FVPATH="Build/OvmfX64/DEBUG_GCC5/FV"
APPPATH="Build/OvmfX64/DEBUG_GCC5/X64"
DISKIMG="Qemu/Images/disk.img"
LOGFILE="Qemu/Log/debug.log"
MNTPNT="/mnt/disk_image"
CPSPATH="$APPPATH/CapsuleApp.efi"
SHLPATH="Qemu/Shell/Shell.efi"

rm -f $FVPATH/OVMF.fd

OvmfPkg/build.sh -a X64 -D DEBUG_ON_SERIAL_PORT -D CAPSULE_ENABLE

if [[ -f $FVPATH/SYSTEMFIRMWAREUPDATECARGO.Fv ]]; then
	echo "Copy images and applications to disk.img"

	if [[ -f $DISKIMG ]]; then
		echo "$DISKIMG exists"
	else
		echo "$DISKIMG doesn't exist"
		echo "Create $DISKIMG"
		dd if=/dev/zero of=$DISKIMG obs=512 ibs=512 bs=512 count=16384
		echo "o
n
p
1


t
c
w" | fdisk $DISKIMG

		mkfs.vfat $DISKIMG

	fi

	mkdir $MNTPNT
	mount $DISKIMG $MNTPNT
	cp $FVPATH/OVMFFIRMWAREUPDATECAPSULEFMPPKCS7.Cap $MNTPNT
	cp $CPSPATH $MNTPNT
	cp $SHLPATH $MNTPNT
	umount $MNTPNT
	rmdir $MNTPNT
	echo "Done"
fi

if [[ -f Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd ]]; then
	echo "rum qemu"
	cp $FVPATH/OVMF.fd Qemu/BIOS/
	qemu-system-x86_64 -bios Qemu/BIOS/OVMF.fd -serial file:$LOGFILE -machine q35 -hda $DISKIMG
fi

echo "Please find the BIOS log at Qemu/Log/debug.log"
cp $LOGFILE ./
