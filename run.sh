#!/usr/bin/env bash


rm -f Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd

OvmfPkg/build.sh -a X64 -D DEBUG_ON_SERIAL_PORT

cd Build/OvmfX64/DEBUG_GCC5/FV
if [[ -f OVMF.fd ]]; then
	echo "rum qemu"
	qemu-system-x86_64 -bios OVMF.fd -serial file:debug.log -machine q35
fi
cd -
