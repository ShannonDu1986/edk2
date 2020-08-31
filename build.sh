#!/usr/bin/env bash

FVPATH="Build/OvmfX64/DEBUG_GCC5/FV"

rm -f $FVPATH/OVMF.fd

#OvmfPkg/build.sh -a X64 -D DEBUG_ON_SERIAL_PORT -D CAPSULE_ENABLE -D SMM_REQUIRE
OvmfPkg/build.sh -a X64 -D DEBUG_ON_SERIAL_PORT
