#!/usr/bin/env bash


rm -f Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd

OvmfPkg/build.sh -a X64 -D DEBUG_ON_SERIAL_PORT -D CAPSULE_ENABLE

if [[ -f Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd ]]; then
	echo "rum qemu"
	cp Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd Qemu/BIOS/
	qemu-system-x86_64 -bios Qemu/BIOS/OVMF.fd -serial file:Qemu/Log/debug.log -machine q35 -hda Qemu/Images/disk.img
fi

echo "Please find the BIOS log at Qemu/Log/debug.log"
