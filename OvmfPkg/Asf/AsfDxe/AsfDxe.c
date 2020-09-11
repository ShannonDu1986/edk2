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

  Count = sizeof(mAsfProgressMsgList) / sizeof (ASF_MESSAGE);
  for (Index = 0; Index < Count; Index++) {
    if (mAsfProgressMsgList[Index].StatusCode == Value) {
      Status = AsfPushMessage(mAsfProgressMsgList[Index]);
      if (EFI_ERROR(Status)) {
        break;
      }
    }

    if (mAsfProgressMsgList[Index].Type == MsgHddInit) {
      mIsHddDetected = TRUE;
    }
  }

  Count = sizeof(mAsfErrorMsgList) / sizeof (ASF_MESSAGE);
  for (Index = 0; Index < Count; Index++) {
    if (mAsfErrorMsgList[Index].StatusCode == Value) {
      Status = AsfPushMessage(mAsfErrorMsgList[Index]);
      if (EFI_ERROR(Status)) {
        break;
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
  AsfPushMessage(mAsfMsgStopWatchdogTimer);


  // Start OS Watchdog timer if enabled
  AsfPushMessage(mAsfMsgStartOsWatchdogTimer);

  //Push System State S0 - "Working"
  AsfPushMessage(mAsfSystemStateS0);

  gBS->CloseEvent(Event);
  return;
}

EFI_STATUS
AsfPushMessageMpm (
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
AsfPushMessage (
  IN ASF_MESSAGE  Msg
)
{
  EFI_STATUS    Status = EFI_NOT_FOUND;
  switch (Msg.FormatType) {
    case MsgFormatTypeNoRetransmit:
      Status = AsfPushMessageMpm((UINT8 *)&Msg.Message.MessageNoRetransmit,
                                Msg.Message.MessageNoRetransmit.Length + 2,
                                Msg.MessageString);
      break;
    case MsgFormatTypeStartWatchdog:
      Status = AsfPushMessageMpm((UINT8 *)&Msg.Message.StartWatchdogMessage,
                                Msg.Message.StartWatchdogMessage.Length + 2,
                                Msg.MessageString);
      break;
    case MsgFormatTypeStopWatchdog:
      Status = AsfPushMessageMpm((UINT8 *)&Msg.Message.StopWatchdogMessage,
                                Msg.Message.StopWatchdogMessage.Length + 2,
                                Msg.MessageString);
      break;
    case MsgFormatTypeSystemState:
      Status = AsfPushMessageMpm((UINT8 *)&Msg.Message.SystemState,
                                Msg.Message.SystemState.Length + 2,
                                Msg.MessageString);
      break;
    default:
      break;
  }

  return Status;
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
EFIAPI
AsfEventAllDriversConnectedCallback (
  IN EFI_EVENT      Event,
  IN VOID           *Context
)
{
  EFI_STATUS        Status;
  EFI_HANDLE        *HandleBuffer = NULL;
  UINTN             HandleCount = 0u;
  UINT16            *BootOrderList;
  UINTN             BootOrderListSize;

  DEBUG ((DEBUG_INFO, "Shannon: %a: enter\n", __FUNCTION__));

  Status = gBS->LocateHandleBuffer (ByProtocol,
                                  &gAmdBdsConnectAllDriversDoneGuid,
                                  NULL,
                                  &HandleCount,
                                  &HandleBuffer);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "Failed to locate gAmdBdsConnectAllDriversDoneGuid\n"));
    return;
  }

  if (!mIsHddDetected) {
    AsfPushMessage(mAsfMsgHddFailure);
  }

  GetEfiGlobalVariable2 (EFI_BOOT_ORDER_VARIABLE_NAME, (VOID **) &BootOrderList, &BootOrderListSize);
  if (BootOrderList == NULL) {
    Status = EFI_NOT_FOUND;
    AsfPushMessage(mAsfMsgNoBootMedia);
  } else {
    DEBUG ((DEBUG_INFO, "%a: BootOrderListSize=0x%x\n", __FUNCTION__, BootOrderListSize / sizeof (UINT16)));
  }

  return;
}

extern EFI_GUID gAmdBdsConnectAllDriversDoneGuid;

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
  VOID          *Registration;

  // Check MPM Enable

  // Push "Motherboard init" message
  AsfPushMessage(mAsfMsgMotherBoardInit);

  // Start BIOS Watchdog timer if needed
  AsfPushMessage(mAsfMsgStartBiosWatchdogTimer);

  // Register RscHandler protocol
  Status = gBS->LocateProtocol(&gEfiRscHandlerProtocolGuid, NULL, (VOID *)&RscHandler);
  if (!EFI_ERROR(Status)) {
    RscHandler->Register (AsfRscHandlerCallBack, TPL_HIGH_LEVEL);
  } else {
    return Status;
  }

  // Creaet BDS All Drivers connected when device are ready
  EfiCreateProtocolNotifyEvent (
    &gAmdBdsConnectAllDriversDoneGuid,
    TPL_CALLBACK,
    AsfEventAllDriversConnectedCallback,
    NULL,
    &Registration
    );

  // Create ReadyToBoot event call back for ASF bootoverride and SMBios
  Status = EfiCreateEventReadyToBootEx (
                TPL_CALLBACK,
                AsfReadyToBootCallBack,
                NULL,
                &AsfReadyToBoot);

  return EFI_SUCCESS;
}