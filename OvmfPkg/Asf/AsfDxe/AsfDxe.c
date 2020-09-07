#include "AsfDxe.h"

// VOID
// EFIAPI
// AsfEventCallBack (
//   IN EFI_EVENT  Event,
//   IN VOID*      Context
// )
// {
//   // EFI_STATUS                Status;
//   // EFI_DATA_HUB_PROTOCOL     *DataHub;
//   // EFI_DATA_RECORD_HEADER    *Record;
//   // // DATA_HUB_STATUS_CODE_DATA_RECORD  *StatusRecord;
//   // UINTN                     Index;
//   // UINT64                    MonotonicCount;
// }

EFI_STATUS
EFIAPI
AsfRscHandlerCallBack (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data
)
{
  EFI_STATUS            Status = EFI_SUCCESS;
  UINTN                 Index;
  UINTN                 Count;

  DEBUG ((DEBUG_INFO, "%a: CodeType=0x%x, Value=0x%x\n", __FUNCTION__, CodeType, Value));
  // if () {
  //   AsfPushMessage();
  // }
  Count = sizeof(mAsfProgressMsgList) / sizeof (ASF_MESSAGE);
  for (Index = 0; Index < Count; Index++) {
    if (mAsfProgressMsgList[Index].StatusCode == Value) {
      Status = AsfPushMessage((UINT8 *)&mAsfProgressMsgList[Index].Message.MessageNoRetransmit,
                      mAsfProgressMsgList[Index].Message.MessageNoRetransmit.Length + 2,
                      mAsfProgressMsgList[Index].MessageString);
      if (EFI_ERROR(Status)) {
        return Status;
      }
    }
  }

  Count = sizeof(mAsfErrorMsgList) / sizeof (ASF_MESSAGE);
  for (Index = 0; Index < Count; Index++) {
    if (mAsfErrorMsgList[Index].StatusCode == Value) {
      Status = AsfPushMessage((UINT8 *)&mAsfErrorMsgList[Index].Message.MessageNoRetransmit,
                      mAsfErrorMsgList[Index].Message.MessageNoRetransmit.Length + 2,
                      mAsfErrorMsgList[Index].MessageString);
      if (EFI_ERROR(Status)) {
        return Status;
      }
    }
  }

  return Status;
}

VOID
EFIAPI
AsfReadyToBootCallBack (
  IN EFI_EVENT      Event,
  IN VOID           *Context
)
{
  DEBUG ((DEBUG_INFO, "%a\n", __FUNCTION__));
  // ASF boot override

  // Create ASF SMBIOS Structure Type 129

  // Stop BIOS Watchdog timer if needed
  AsfPushMessage((UINT8 *)&mAsfMsgStopWatchdogTimer.Message.StopWatchdogMessage,
                  mAsfMsgStopWatchdogTimer.Message.StopWatchdogMessage.Length + 2,
                  mAsfMsgStopWatchdogTimer.MessageString);

  // Start OS Watchdog timer if enabled
  AsfPushMessage((UINT8 *)&mAsfMsgStartOsWatchdogTimer.Message.StartWdtMessage,
                  mAsfMsgStartOsWatchdogTimer.Message.StartWdtMessage.Length + 2,
                  mAsfMsgStartOsWatchdogTimer.MessageString);

  //Push System State S0 - "Working"
  AsfPushMessage((UINT8 *)&mAsfSystemStateS0.Message.SystemState,
                  mAsfSystemStateS0.Message.SystemState.Length + 2,
                  mAsfSystemStateS0.MessageString);

  gBS->CloseEvent(Event);
  return;
}

EFI_STATUS
AsfPushMessage (
  IN UINT8        *Msg,
  IN UINT32       Size,
  IN const UINT16 *MsgStr OPTIONAL
)
{
  EFI_STATUS        Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "%a\n", __FUNCTION__));

  if (StrLen(MsgStr) > 0) {
    DEBUG ((DEBUG_INFO, "ASF Message Name: %s\n", MsgStr));
  }

  {
    // only for debug
    UINTN         Index;
    DEBUG ((DEBUG_INFO, "    "));
    for (Index = 0; Index < Size; Index++) {
      DEBUG ((DEBUG_INFO, "0x%02x ", Msg[Index]));
    }
    DEBUG ((DEBUG_INFO, "\n\n"));
  }

  // Status = BiosMpmAlertMsg(Msg, Size); TODO: uncomment it when merge to AMD CODE

  return Status;
}

EFI_STATUS
AsfGetBootOption ()
{
  EFI_STATUS          Status = EFI_SUCCESS;
  ASF_BOOT_OPTION     BootOption;

  SetMem(&BootOption, sizeof(ASF_BOOT_OPTION), 0x00);

  // Call MPM Interface to get boot option

  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }

  if (BootOption.SubCommand == ASFMSG_SUBCMD_RET_BOOT_OPTION) {
    CopyMem(&mAsfBootOption, &BootOption, sizeof(ASF_BOOT_OPTION));

    AsfPushMessage((UINT8 *)&mAsfMsgClearBootOption.Message.BootOption,
                    mAsfMsgClearBootOption.Message.BootOption.Length + 2,
                    mAsfMsgClearBootOption.MessageString);

  }

  return Status;
}

VOID
AsfConfigurationDataReset()
{
  // UINTN         Length;
  // UINT32        Attribute;
  // EFI_STATUS    Status;

  if (mAsfBootOption.BootOptionBit[0] & ASF_BOP_BIT_CONFIGURATION_DATA_RESET) {
    // reset setup data
    ;
  }
}

VOID
EFIAPI
LegacyBiosLockLocalKeyboard ()
{
#if (defined(CSM_SUPPORT) && (CSM_SUPPORT != 0))
#endif
}

BOOLEAN
IsUsbDevice (
  IN EFI_DEVICE_PATH_PROTOCOL *Node
)
{
  if(Node != NULL) {
    if ((Node->Type == MESSAGING_DEVICE_PATH) && \
        (Node->SubType == MSG_USB_DP)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsGenericSioDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *Node
)
{
  // TODO:

  return FALSE;
}

VOID
EFIAPI
EventConInStartedCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
)
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        *SimpleTextInHandle;
  UINTN                             NumberOfHandle;
  UINTN                             Index;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePathNode;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *SimpleTextIn;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEx;

  // Hook IN 9h for locking local keyboard
  LegacyBiosLockLocalKeyboard();

  // Uninstall all the console spliter drivers except SOL
  Status = gBS->LocateHandleBuffer ( ByProtocol,
                                    &gEfiSimpleTextInProtocolGuid,
                                    NULL,
                                    &NumberOfHandle,
                                    &SimpleTextInHandle
                                    );
  if (EFI_ERROR(Status)) {
    return;
  }

  for (Index = 0; Index < NumberOfHandle; Index++) {
    Status = gBS->HandleProtocol (SimpleTextInHandle[Index],
                                  &gEfiDevicePathProtocolGuid,
                                  (VOID *)&DevicePath
                                  );
    if (EFI_ERROR(Status)) {
      return;
    }

    DevicePathNode = DevicePath;
    while (!IsDevicePathEndType(DevicePathNode)) {
      // Is USB device path or gereric SIO device path (SouthBridge)
      if (IsUsbDevice(DevicePathNode) || IsGenericSioDevicePath(DevicePathNode)) {
        Status = gBS->HandleProtocol (
                            SimpleTextInHandle[Index],
                            &gEfiSimpleTextInProtocolGuid,
                            (VOID **)&SimpleTextIn
                            );
        if (EFI_ERROR(Status)) {
          break;
        }
        Status = gBS->UninstallProtocolInterface (
                            SimpleTextInHandle[Index],
                            &gEfiSimpleTextInProtocolGuid,
                            SimpleTextIn
                            );
        if (EFI_ERROR(Status)) {
          break;
        }

        Status = gBS->HandleProtocol (
                            SimpleTextInHandle[Index],
                            &gEfiSimpleTextInputExProtocolGuid,
                            (VOID **)&SimpleTextInEx
                            );
        if (EFI_ERROR(Status)) {
          break;
        }
        Status = gBS->UninstallProtocolInterface (
                            SimpleTextInHandle[Index],
                            &gEfiSimpleTextInputExProtocolGuid,
                            SimpleTextInEx
                            );
        if (EFI_ERROR(Status)) {
          break;
        }
      }
      DevicePathNode = NextDevicePathNode(DevicePathNode);
    }
  }

  return;
}

VOID
AsfLockLocalKeyboard ()
{
  EFI_STATUS      Status;
  EFI_EVENT       EvtConInStarted;
  VOID            *RegisterConInStarted;

  if (mAsfBootOption.BootOptionBit[0] & ASF_BOP_BIT_LOCK_KEYBOARD) {
    Status = gBS->CreateEvent (
                        EVT_NOTIFY_SIGNAL,
                        TPL_CALLBACK,
                        EventConInStartedCallback,
                        NULL,
                        &EvtConInStarted
                        );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (
                        &gEfiSimpleTextInProtocolGuid,
                        EvtConInStarted,
                        &RegisterConInStarted
                        );
    ASSERT_EFI_ERROR (Status);
  }
}

EFI_STATUS
EFIAPI
AsfDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS                Status;
  EFI_RSC_HANDLER_PROTOCOL  *RscHandler;
  // EFI_EVENT                 AsfEvent;
  EFI_EVENT                 AsfReadyToBoot;
  // UINT64                    DataClass;

  // Check MPM Enable

  // ASF Get Boot Option
  AsfGetBootOption();

  // Configuration Data Reset if needed
  AsfConfigurationDataReset();

  // Lock lock keyboard if needed
  AsfLockLocalKeyboard();

  // Push "Motherboard init" message
  AsfPushMessage((UINT8 *)&mAsfMsgMotherBoardInit.Message.MessageNoRetransmit,
                  mAsfMsgMotherBoardInit.Message.MessageNoRetransmit.Length + 2,
                  mAsfMsgMotherBoardInit.MessageString);

  // Start BIOS Watchdog timer if needed

  // // Locate DataHub protocol
  // Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, (VOID *)&DataHub);
  // if (EFI_ERROR(Status)) {
  //   return EFI_UNSUPPORTED;
  // }

  // // Create Progress/Error class code of data hub event for ASF messages
  // Status = gBS->CreateEvent ( EVT_NOTIFY_SIGNAL,
  //                             TPL_CALLBACK,
  //                             AsfEventCallBack,
  //                             NULL,
  //                             &AsfEvent );

  // if (EFI_ERROR(Status)) {
  //   return Status;
  // }
  // // Register Filter Driver for Progress/Error Class Code of Data Hub
  // DataClass = EFI_DATA_RECORD_CLASS_ERROR | EFI_DATA_RECORD_CLASS_PROGRESS_CODE;
  // Status = DataHub->RegisterFilterDriver (DataHub, AsfEvent, TPL_APPLICATION, DataClass, NULL);
  // if (EFI_ERROR(Status)) {
  //   return Status;
  // }

  // Register RscHandler protocol
  Status = gBS->LocateProtocol(&gEfiRscHandlerProtocolGuid, NULL, (VOID *)&RscHandler);
  if (!EFI_ERROR(Status)) {
    RscHandler->Register (AsfRscHandlerCallBack, TPL_HIGH_LEVEL);
  } else {
    return Status;
  }

  // Creaet BDS All Drivers connected when device are ready

  // Create ReadyToBoot event call back for ASF bootoverride and SMBios
  Status = EfiCreateEventReadyToBootEx (
                TPL_CALLBACK,
                AsfReadyToBootCallBack,
                NULL,
                &AsfReadyToBoot);

  return EFI_SUCCESS;
}