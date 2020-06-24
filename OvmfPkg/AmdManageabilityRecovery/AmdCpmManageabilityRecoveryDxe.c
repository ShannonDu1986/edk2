/*****************************************************************************
 *
 * Copyright 2012 - 2020 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your AMD
 * representative for a copy.
 * 
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 * 
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 * 
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall AMD's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless AMD and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 * 
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of AMD's proprietary rights in them.
 * 
 * EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
 * stated in the Software License Agreement.
 ******************************************************************************
 */
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Protocol/LoadedImage.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/DevicePathLib.h>
#include <Library/FileHandleLib.h>

//
// Definitions
//
#define MAN_OS_RECOVERY_BACKUP_FILE_NAME      L"\\EFI\\AMD\\manOS"
#define MAN_OS_MAX_SIZE                       0x800000

#define IS_USB_DEVICEPATH(dp)       (DevicePathType (dp) == MESSAGING_DEVICE_PATH && DevicePathSubType (dp) == MSG_USB_DP)

#pragma pack (1)
typedef struct {
  UINT32  CurVersion;
  UINT32  TargetVersion;
  UINT64  ImageAddress;
  UINT8   NeedRecovery;
  UINT8   ImageReadyInMem;
  UINT8   VarInitialized;
} MANOS_RECOVERY_CONFIG;
#pragma pack ()

CHAR16 mVariableName[] = L"AmdManOsRecoveryConfig";
#define AMD_MANOS_RECOVERY_CONFIG_NAME L"AmdManOsRecoveryConfig"
extern EFI_GUID gAmdManOSRecoveryConfigGuid;

/////////////////////////////////////////////////////////////////////////////

VOID deadloop();

//
// Function definitions
//

BOOLEAN
IsManOSBackUpImageAvailable (
);

BOOLEAN
IsManOSRecoveryRequired (
);

EFI_STATUS
LoadManOSBackUpImage (
  IN OUT UINT8      *FileBuffer,
  IN OUT UINTN      *FileBufferSize,
  IN     CHAR16     *FileName,
  IN BOOLEAN        SearchUsb
);

EFI_STATUS
SaveManOSImageAsBackup (
  IN UINT8        *FileBuffer,
  IN UINTN        *FileBufferSize,
  IN CHAR16       *FileName
);

EFI_STATUS
LoadManOSImageFromFlash (
  IN OUT UINT8    *FileBuffer,
  IN OUT UINTN    *FileBufferSize
);

EFI_STATUS
WriteManOSImageToFlash (
  IN UINT8        *FileBuffer,
  IN UINTN        *FileBufferSize
);

EFI_STATUS
LoadManOSRecovery (
);

VOID
EFIAPI
PrepareManOSRecovery (
  IN EFI_EVENT    Event,
  IN VOID         *Context
);

EFI_STATUS
ProcessManOSRecovery (
);
//////////////////////////////////////////////////////////////////////////


VOID deadloop()
{
  DEBUG ((EFI_D_ERROR, "deadloop\n"));
  gBS->Stall (1000*1000);
  volatile UINTN p = 0;
  while (p < 10) p = 0;
}

VOID
DumpMemory (
  IN VOID *Addr,
  IN UINTN Length
)
{
  UINTN   Index;
  UINT8   *pBuf;
  DEBUG((EFI_D_ERROR, "\n\n\n---------------------------------------------------------\n"));
  DEBUG((EFI_D_ERROR, "-------  Dump Memory at 0x%llx, length is %d  -------\n", Addr, Length));
  DEBUG((EFI_D_ERROR, "---------------------------------------------------------\n"));

  DEBUG ((EFI_D_ERROR, "| Offset  | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B | 0C | 0D | 0E | 0F |"));
  pBuf = (UINT8 *)Addr;
  for (Index = 0; Index < Length; Index++)
  {
    if ((Index % 16) == 0)
    {
      DEBUG ((EFI_D_ERROR, "\n|  %04x   |", Index));
    }
    DEBUG ((EFI_D_ERROR, " %02x |", *(pBuf + Index)));
  }

  DEBUG((EFI_D_ERROR, "\n\n-----------------------  DONE    -----------------------\n"));
  DEBUG((EFI_D_ERROR, "---------------------------------------------------------\n"));
}

BOOLEAN
IsUsbDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
)
{
  BOOLEAN       Result = FALSE;

  if (DevicePath != NULL)
  {
    while (!IsDevicePathEnd (DevicePath) && !Result)
    {
      Result = (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (DevicePath) == MSG_USB_DP);
      DevicePath = NextDevicePathNode (DevicePath);
    }
  }
  return Result;
}

BOOLEAN
IsUsbDeviceType (
  IN EFI_HANDLE   *Handle
)
{
  EFI_STATUS                Status = EFI_NOT_STARTED;
  BOOLEAN                   IsFileSystemOnUsb = FALSE;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath = NULL;

  if (Handle != NULL)
  {
    Status = gBS->HandleProtocol(Handle, &gEfiDevicePathProtocolGuid, (VOID**)&DevicePath);
    if (Status == EFI_SUCCESS)
    {
      IsFileSystemOnUsb = IsUsbDevicePath(DevicePath);
    }
  }
  return IsFileSystemOnUsb;
}

EFI_FILE_HANDLE
OpenRoot (
   IN EFI_HANDLE        DeviceHandle
)
{
   EFI_STATUS                      Status = EFI_NOT_STARTED;
   EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume = NULL;
   EFI_FILE_HANDLE                 File = NULL;

   if (DeviceHandle != NULL)
   {
      // Find the file system interface to the device
      Status = gBS->HandleProtocol (DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**)&Volume);
      if (Status == EFI_SUCCESS)
      {
         Volume->OpenVolume (Volume, &File);
      }
   }
   return File;
}

BOOLEAN
IsManOSBackUpImageAvailable (
)
{
  return TRUE;
}


BOOLEAN
IsImageReadyInMem (
)
{
  return FALSE;
}

BOOLEAN
IsManOSRecoveryRequired (
)
{
  return TRUE;
}

EFI_STATUS
LoadManOSBackUpImage (
  IN OUT UINT8      *FileBuffer,
  IN OUT UINTN      *FileBufferSize,
  IN     CHAR16     *FileName,
  IN BOOLEAN        SearchUsb
)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  EFI_HANDLE        *HandleBuffer = NULL;
  EFI_FILE_HANDLE   FileHandle  = NULL;
  UINTN             HandleCount = 0u;
  UINTN             FileSize;
  UINTN             Index;
  BOOLEAN           FoundFile = FALSE;
  EFI_FILE_HANDLE   Dir = NULL;

  DEBUG ((EFI_D_ERROR, "LoadManOSBackUpImage: enter\n"));

  Status = gBS->LocateHandleBuffer (ByProtocol, 
                                    &gEfiSimpleFileSystemProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &HandleBuffer);
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "LoadManOSBackUpImage: failed to locate gEfiSimpleFileSystemProtocolGuid\n"));
    return Status;
  }

  DEBUG ((EFI_D_ERROR, "LoadManOSBackUpImage: HandleCount=%d\n", HandleCount));
  for (Index = 0; (Index < HandleCount) && !FoundFile; Index++) {
    if (!SearchUsb) {
      if (IsUsbDeviceType(HandleBuffer[Index])) {
        continue;
      }
    }

    Dir = OpenRoot (HandleBuffer[Index]);
    if (Dir != NULL) {
      Status = Dir->Open (Dir, &FileHandle, FileName, EFI_FILE_MODE_READ, 0u);
      if (!EFI_ERROR(Status)) {
        Status = FileHandleGetSize(FileHandle, &FileSize);
        if (!EFI_ERROR(Status) && (FileSize <= MAN_OS_MAX_SIZE)) {
          Status = FileHandleRead(FileHandle, &FileSize, FileBuffer);
          if (!EFI_ERROR(Status)) {
            FoundFile = TRUE;
            break;
          }
        } else {
          FileHandleClose(FileHandle);
        }
      }
    }
  }

  Status = FoundFile ? EFI_SUCCESS : EFI_NOT_FOUND;
  DEBUG ((EFI_D_ERROR, "LoadManOSBackUpImage: return Status = %r\n", Status));
  return Status;
}

EFI_STATUS
SaveManOSImageAsBackup (
  IN UINT8        *FileBuffer,
  IN UINTN        *FileBufferSize,
  IN CHAR16       *FileName
)
{
  // EFI_STATUS        Status;
  if ((FileBuffer == NULL) || (FileBufferSize == NULL) || (*FileBufferSize == 0) || (*FileBufferSize >= MAN_OS_MAX_SIZE)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

VOID
EFIAPI
CreateVariable (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS            Status;
  MANOS_RECOVERY_CONFIG Config;

  Config.CurVersion = 0;
  Config.TargetVersion = 0;
  Config.ImageAddress = 0x0ul;
  Config.NeedRecovery = 0;
  Config.ImageReadyInMem = 0;
  Config.VarInitialized = 1;

  Status = gRT->SetVariable (
                  AMD_MANOS_RECOVERY_CONFIG_NAME,
                  &gAmdManOSRecoveryConfigGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(MANOS_RECOVERY_CONFIG),
                  (VOID *) &Config
        );
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Initialized the variable failed, Status = %r\n", Status));
  } else {
    DEBUG ((EFI_D_ERROR, "Initialized the variable successfully, Status = %r\n", Status));
  }
}

VOID
InitManRecoveryConfigVariable ()
{
  EFI_STATUS            Status;
  MANOS_RECOVERY_CONFIG Config;
  UINTN                 Size;
  VOID                  *Registration;

  Size = sizeof (MANOS_RECOVERY_CONFIG);
  Status = gRT->GetVariable (
                  AMD_MANOS_RECOVERY_CONFIG_NAME,
                  &gAmdManOSRecoveryConfigGuid,
                  NULL,
                  &Size,
                  (VOID *) &Config
      );

  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Variable is not available, Status=%r\n", Status));

    if (Status == EFI_NOT_FOUND) {
      // the variable is not created, so register callback to create the variable
      EfiCreateProtocolNotifyEvent (
        &gEfiVariableWriteArchProtocolGuid,
        TPL_NOTIFY,
        CreateVariable,
        NULL,
        &Registration
        );
    }
  } else {
    DEBUG ((EFI_D_ERROR, "Variable is available, Config.VarInitialized=%d\n", Config.VarInitialized));
  }
}

VOID
EFIAPI
PrepareManOSRecovery (
  IN EFI_EVENT    Event,
  IN VOID         *Context
)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINT8         *FileBuffer;
  UINTN         FileBufferSize;
  UINTN         Pages;
  EFI_PHYSICAL_ADDRESS Address;

  DEBUG ((EFI_D_ERROR, "PrepareManOSRecovery :enter  Status=%r\n", Status));

  FileBufferSize = 0x800000;
  Pages = EFI_SIZE_TO_PAGES (FileBufferSize);
  Address = 0xFFFFFFFF;

  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  Pages,
                  &Address
      );
  FileBuffer = (UINT8 *) (UINTN) Address;
  ZeroMem (FileBuffer, FileBufferSize);


  //FileBuffer = (UINT8 *)AllocateZeroPool(FileBufferSize);
  DumpMemory(FileBuffer, 0x300);
  DEBUG ((EFI_D_ERROR, "PrepareManOSRecovery : FileBuffer=%p\n", FileBuffer));
  Status = LoadManOSBackUpImage(FileBuffer, &FileBufferSize, MAN_OS_RECOVERY_BACKUP_FILE_NAME, TRUE);
  DEBUG ((EFI_D_ERROR, "PrepareManOSRecovery : FileBufferSize=%d, Status=%r\n", FileBufferSize, Status));
  DumpMemory(FileBuffer, 0x300);

  return;
}



EFI_STATUS
ProcessManOSRecovery (
)
{
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
AmdCpmManageabilityDxeRecoveryEntry (
  IN      EFI_HANDLE                      ImageHandle,
  IN      EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  VOID          *Registration;

  DEBUG((EFI_D_ERROR, "AmdCpmManageabilityDxeRecoveryEntry - enter\n"));

  InitManRecoveryConfigVariable();

  if (!IsManOSBackUpImageAvailable()) {
    DEBUG ((EFI_D_ERROR, "Need to backup manOS binary\n"));
    // Status = SaveManOSImageAsBackup();
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "Failed to backup manOS binary\n"));
      return Status;
    }
  }


  DEBUG((EFI_D_ERROR, "AmdCpmManageabilityDxeRecoveryEntry - 1\n"));
  if (IsImageReadyInMem()) {
    Status = ProcessManOSRecovery();
  }
  DEBUG((EFI_D_ERROR, "AmdCpmManageabilityDxeRecoveryEntry - 2\n"));

  if (IsManOSRecoveryRequired()) {
    EfiCreateProtocolNotifyEvent (
        &gEfiSimpleFileSystemProtocolGuid,
        TPL_CALLBACK,
        PrepareManOSRecovery,
        NULL,
        &Registration
        );
  }
  DEBUG((EFI_D_ERROR, "AmdCpmManageabilityDxeRecoveryEntry - 3\n"));

  DEBUG((EFI_D_ERROR, "AmdCpmManageabilityDxeRecoveryEntry - exit, Status = %r\n", Status));
  return Status;
}
