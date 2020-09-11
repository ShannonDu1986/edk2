#include "AsfPei.h"

EFI_STATUS
AsfPushMessage (
  IN UINT8        *Msg,
  IN UINT32       Size
);

EFI_STATUS
EFIAPI
AsfMemoryCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
);

static EFI_PEI_NOTIFY_DESCRIPTOR AsfMemoryMessageNofify[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
    &gEfiPeiMemoryDiscoveredPpiGuid, \
    AsfMemoryCallback,
  }
};

EFI_STATUS
EFIAPI
AsfMemoryCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
)
{
  EFI_STATUS      Status;
  EFI_BOOT_MODE   BootMode;
  DEBUG ((DEBUG_INFO, "%a\n", __FUNCTION__));

  // Push "Memory Init End" Message
  DEBUG ((DEBUG_INFO, "%a, push memory init end message\n", __FUNCTION__));
  AsfPushMessage((UINT8 *)&gAsfMsgMemoryInitEnd, gAsfMsgMemoryInitEnd.Length + 2);

  // Push "Cache Init" Message
  DEBUG ((DEBUG_INFO, "%a, push cache init message\n", __FUNCTION__));
  AsfPushMessage((UINT8 *)&gAsfMsgCacheInit, gAsfMsgCacheInit.Length + 2);

  // Push "System State of S0" if resuming from S3
  Status = PeiServicesGetBootMode(&BootMode);
  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    ;
  }

  return Status;
}

EFI_STATUS
AsfPushMessage (
  IN UINT8        *Msg,
  IN UINT32       Size
)
{
  EFI_STATUS        Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "%a\n", __FUNCTION__));

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
AsfPeiStatusCodeListner (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_STATUS_CODE_TYPE         Type,
  IN EFI_STATUS_CODE_VALUE        Value,
  IN UINT32                       Instance,
  IN EFI_GUID                     *CallerId,
  IN EFI_STATUS_CODE_DATA         *Data
)
{
  EFI_STATUS      Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "%a, Type=0x%x, Value=0x%x\n", __FUNCTION__, Type, Value));

  if (((Type & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) && \
      ((Value == PEI_MEMORY_NOT_INSTALLED) || \
      (Value == PEI_MEMORY_NOT_DETECTED))) {
    // Report the memory missing
    DEBUG ((DEBUG_INFO, "Shannon: Report Memory Missing Message\n"));
    Status = AsfPushMessage((UINT8 *)&gAsfMsgMemoryMissing, gAsfMsgMemoryMissing.Length + 2);
  }

  return Status;
}

EFI_STATUS
EFIAPI
AsfPeiEntry (
  IN        EFI_PEI_FILE_HANDLE   FileHanle,
  IN CONST  EFI_PEI_SERVICES      **PeiServices
)
{
  EFI_STATUS                Status;
  EFI_PEI_RSC_HANDLER_PPI   *RscHandler;

  DEBUG ((DEBUG_INFO, "%a enter\n", __FUNCTION__));

  Status = PeiServicesNotifyPpi(AsfMemoryMessageNofify);
  ASSERT_EFI_ERROR(Status);

  Status = PeiServicesLocatePpi (
                    &gEfiPeiRscHandlerPpiGuid,
                    0,
                    NULL,
                    (VOID **) &RscHandler
                    );
  if (!EFI_ERROR (Status)) {
    Status = RscHandler->Register((EFI_PEI_RSC_HANDLER_CALLBACK)AsfPeiStatusCodeListner);
  } else {
    return Status;
  }

  // Push "Stop watchdog timer" message
  DEBUG ((DEBUG_INFO, "%a, push stop watchdog timer message\n", __FUNCTION__));
  AsfPushMessage((UINT8 *)&gAsfMsgStopWatchdogTimer, gAsfMsgStopWatchdogTimer.Length + 2);

  // Push "BIOS Present" message 
  DEBUG ((DEBUG_INFO, "%a, push BIOS present message\n", __FUNCTION__));
  AsfPushMessage((UINT8 *)&gAsfMsgBiosPresent, gAsfMsgBiosPresent.Length + 2);


  return Status;
}