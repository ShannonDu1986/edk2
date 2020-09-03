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

  // Configuration Data Reset if needed

  // Lock lock keyboard if needed

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