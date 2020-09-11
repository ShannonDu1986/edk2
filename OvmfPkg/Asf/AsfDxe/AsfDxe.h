#ifndef _ASF_DXE_H_
#define _ASF_DXE_H_


// #include <Protocol/DataHub.h>
#include <Protocol/ReportStatusCodeHandler.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Guid/GlobalVariable.h>
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <Pi/PiStatusCode.h>

#include <Asf.h>

#ifndef EFI_CU_CHIPSET_PC_INTRUDER_DETECT
#define EFI_CU_CHIPSET_PC_INTRUDER_DETECT   (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#endif

#ifndef EFI_SW_DXE_BS_EC_NO_BOOT_MEDIA
#define EFI_SW_DXE_BS_EC_NO_BOOT_MEDIA      (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#endif

#ifndef DXE_BDS_CONNECT_DRIVERS
#define DXE_BDS_CONNECT_DRIVERS (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BEGIN_CONNECTING_DRIVERS)
#endif

static const ASF_MESSAGE mAsfMsgHddInit = \
{
  MsgHddInit,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x04,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x02    // EventData2
    }
  },
  (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_DETECT),
  L"ASF - PROGRESS - HDD INIT"
};

static const ASF_MESSAGE mAsfMsgApInit = \
{
  MsgApInit,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x03,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x03    // EventData2
    }
  },
  (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_AP_INIT),
  L"ASF - PROGRESS - AP Init"
};

static const ASF_MESSAGE mAsfMsgUserInitSetup = \
{
  MsgUserInitSetup,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x22,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x05    // EventData2
    }
  },
  (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_USER_SETUP),
  L"ASF - PROGRESS - User Init Setup"
};

static const ASF_MESSAGE mAsfMsgUsbResouceConfig = \
{
  MsgUsbResourceConfig,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x22,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x06    // EventData2
    }
  },
  (EFI_IO_BUS_USB | EFI_P_PC_ENABLE),
  L"ASF - PROGRESS - USB Resource Config"
};

static const ASF_MESSAGE mAsfMsgPciResourceConfig = \
{
  MsgPciResourceConfig,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x22,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x07    // EventData2
    }
  },
  (EFI_IO_BUS_PCI | EFI_IOB_PCI_BUS_ENUM),
  L"ASF - PROGRESS - PCI Resource Config"
};

static const ASF_MESSAGE mAsfMsgVideoInit = \
{
  MsgVideoInit,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x0b,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x09    // EventData2
    }
  },
  (EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_INIT),
  L"ASF - PROGRESS - Video Init"
};

static const ASF_MESSAGE mAsfMsgKbcInit = \
{
  MsgKbcInit,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x07,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x0C    // EventData2
    }
  },
  (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_INIT),
  L"ASF - PROGRESS - KBC Init"
};

static const ASF_MESSAGE mAsfMsgKbcTest = \
{
  MsgKbcTest,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x07,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x17    // EventData2
    }
  },
  (EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_SELF_TEST),
  L"ASF - PROGRESS - KBC Teset"
};

static const ASF_MESSAGE mAsfMsgOsBoot = \
{
  MsgOSBoot,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x00,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x13    // EventData2
    }
  },
  (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT),
  L"ASF - PROGRESS - OS Boot"
};

static const ASF_MESSAGE mAsfMsgMotherBoardInit = \
{
  MsgMotherBoardInit,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x02,   // EventOffset
      0x68,   // EventSourceType
      0x01,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x07,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x14    // EventData2
    }
  },
  (EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_PC_INIT_BEGIN),
  L"ASF - PROGRESS - Mother Board Init"
};

static const ASF_MESSAGE mAsfMsgNoVideo = \
{
  MsgNoVideo,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x00,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x0B,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x0A    // EventData2
    }
  },
  (EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_DETECTED),
  L"ASF - ERROR - NO Vdieo"
};

static const ASF_MESSAGE mAsfMsgKbdFailure = \
{
  MsgKbdFailure,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x00,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x07,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x07    // EventData2
    }
  },
  (EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NOT_DETECTED),
  L"ASF - ERROR - Keyboard Failure"
};

static const ASF_MESSAGE mAsfMsgHddFailure = \
{
  MsgHddFailure,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x0F,   // EventSensorType
      0x6F,   // EventType
      0x00,   // EventOffset
      0x68,   // EventSourceType
      0x20,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x04,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x03    // EventData2
    }
  },
  (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NOT_DETECTED),
  L"ASF - ERROR - HDD Failure"
};

static const ASF_MESSAGE mAsfMsgChassisIntrusion = \
{
  MsgChassisIntrusion,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0B,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x05,   // EventSensorType
      0x6F,   // EventType
      0x00,   // EventOffset
      0x68,   // EventSourceType
      0x01,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x07,   // Entity
      0x00    // EntityInstance
    }
  },
  (EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_CHIPSET_PC_INTRUDER_DETECT),
  L"ASF - ERROR - Chassis Intrusion"
};

static const ASF_MESSAGE mAsfMsgUserPasswordInvalid = \
{
  MsgUserPasswordInvalid,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0B,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x06,   // EventSensorType
      0x6F,   // EventType
      0x01,   // EventOffset
      0x68,   // EventSourceType
      0x08,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x22,   // Entity
      0x00    // EntityInstance
    }
  },
  (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_INVALID_PASSWORD),
  L"ASF - ERROR - User Password Invalid"
};

static const ASF_MESSAGE mAsfMsgNoBootMedia = \
{
  MsgNoBootMedia,
  MsgFormatTypeNoRetransmit,
  {
    {
      0x04,   // Command
      0x0D,   // Length
      0x16,   // SubCommand
      0x10,   // VersionNumber
      0x1E,   // EventSensorType
      0x6F,   // EventType
      0x00,   // EventOffset
      0x68,   // EventSourceType
      0x20,   // EventSeverity
      0xFF,   // SensorDevice
      0xFF,   // SensorNumber
      0x00,   // Entity
      0x00,   // EntityInstance
      0x40,   // EventData1
      0x00    // EventData2
    }
  },
  (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_NO_BOOT_MEDIA),
  L"ASF - ERROR - No Boot Media"
};

ASF_MESSAGE mAsfSystemStateS0 = \
{
  MsgSystemStateS0,
  MsgFormatTypeSystemState,
  {
    {
      0x01,   // Command
      0x03,   // Length
      0x18,   // SubCommand
      0x10,   // VersionNumber
      0x00,   // SystemState
    }
  },
  0,        // Not valid
  L"ASF - MESSAGE - System State S0"
};

ASF_MESSAGE mAsfSystemStateS3 = \
{
  MsgSystemStateS3,
  MsgFormatTypeSystemState,
  {
    {
      0x01,   // Command
      0x03,   // Length
      0x18,   // SubCommand
      0x10,   // VersionNumber
      0x03,   // SystemState
    }
  },
  0,        // Not valid
  L"ASF - MESSAGE - System State S3"
};

ASF_MESSAGE mAsfSystemStateS4 = \
{
  MsgSystemStateS4,
  MsgFormatTypeSystemState,
  {
    {
      0x01,   // Command
      0x03,   // Length
      0x18,   // SubCommand
      0x10,   // VersionNumber
      0x04,   // SystemState
    }
  },
  0,        // Not valid
  L"ASF - MESSAGE - System State S4"
};

ASF_MESSAGE mAsfSystemStateS5 = \
{
  MsgSystemStateS5,
  MsgFormatTypeSystemState,
  {
    {
      0x01,   // Command
      0x03,   // Length
      0x18,   // SubCommand
      0x10,   // VersionNumber
      0x05,   // SystemState
    }
  },
  0,        // Not valid
  L"ASF - MESSAGE - System State S5"
};

#define ASF_BIOS_WATCHDOG_TIMER_VALUE       0x100u
#define ASF_OS_WATCHDOG_TIMER_VALUE         0x100u

static const ASF_MESSAGE mAsfMsgStartBiosWatchdogTimer = \
{
  MsgStartBiosWatchdogTimer,
  MsgFormatTypeStartWatchdog,
  {
    {
      0x02,  // Command
      0x0F,  // Length
      0x13,  // SubCommand
      0x10,  // VersionNumber
      ASF_BIOS_WATCHDOG_TIMER_VALUE & 0xFF,  // TimeoutValue low
      (ASF_BIOS_WATCHDOG_TIMER_VALUE >> 8) & 0xFF,  // TimeoutValue high
      0x23,  // EventSensorType
      0x6F,  // EventType
      0x00,  // EventOffset
      0x00,  // EventSourceType
      0x10,  // EventSeverity
      0xFF,  // SensorDevice
      0xFF,  // SensorNumber
      0x00,  // Entity
      0x00,  // EntityInstance
      0x40,  // EventData1
      0x04   // EventData2
    }
  },
  (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NOT_DETECTED),
  L"ASF - Start System Firmware Watchdog Timer"
};

static const ASF_MESSAGE mAsfMsgStartOsWatchdogTimer = \
{
  MsgStartOsWatchdogTimer,
  MsgFormatTypeStartWatchdog,
  {
    {
      0x02,  // Command
      0x0F,  // Length
      0x13,  // SubCommand
      0x10,  // VersionNumber
      ASF_OS_WATCHDOG_TIMER_VALUE & 0xFF,  // TimeoutValue low
      (ASF_OS_WATCHDOG_TIMER_VALUE >> 8) & 0xFF,  // TimeoutValue high
      0x23,  // EventSensorType
      0x6F,  // EventType
      0x00,  // EventOffset
      0x00,  // EventSourceType
      0x10,  // EventSeverity
      0xFF,  // SensorDevice
      0xFF,  // SensorNumber
      0x00,  // Entity
      0x00,  // EntityInstance
      0x40,  // EventData1
      0x02   // EventData2
    }
  },
  (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NOT_DETECTED),
  L"ASF - Start OS Watchdog Timer"
};


static const ASF_MESSAGE mAsfMsgStopWatchdogTimer = \
{
  MsgStopWatchdogtimer,
  MsgFormatTypeStopWatchdog,
  {
    {
      0x02,  // Command
      0x02,  // Length
      0x14,  // SubCommand
      0x10   // VersionNumber
    }
  },
  (EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NOT_DETECTED),
  L"ASF - Stop Watchdog Timer"
};

ASF_MESSAGE mAsfProgressMsgList[] = {
  mAsfMsgHddInit,
  mAsfMsgApInit,
  mAsfMsgUserInitSetup,
  mAsfMsgUsbResouceConfig,
  mAsfMsgPciResourceConfig,
  mAsfMsgVideoInit,
  mAsfMsgKbcInit,
  mAsfMsgKbcTest,
  mAsfMsgKbcTest,
  mAsfMsgOsBoot,
  mAsfMsgMotherBoardInit
};

ASF_MESSAGE mAsfErrorMsgList[] = {
  mAsfMsgNoVideo,
  mAsfMsgKbdFailure,
  mAsfMsgHddFailure,
  mAsfMsgChassisIntrusion,
  mAsfMsgUserPasswordInvalid,
  mAsfMsgNoBootMedia
};

BOOLEAN         mIsHddDetected = FALSE;


EFI_STATUS
AsfPushMessageMpm (
  IN UINT8        *Msg,
  IN UINT32       Size,
  IN const UINT16       *MsgStr    OPTIONAL
);

EFI_STATUS
AsfPushMessage (
  IN ASF_MESSAGE  Msg
);

#endif