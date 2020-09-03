#ifndef _ASF_PEI_H_
#define _ASF_PEI_H_

#include <Base.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <Ppi/MemoryDiscovered.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Pi/PiStatusCode.h>
#include <Pi/PiPeiCis.h>

#include <Asf.h>

ASF_MSG_NORETRANSMIT gAsfMsgMemoryInitEnd = \
{
  0x04,   // Command
  0x0D,   // Length
  0x16,   //  SubCommand
  0x10,   // VersionNumber
  0x0F,   // EventSensorType
  0x6F,   // EventType
  0x82,   // EventOffset
  0x68,   // EventSourceType
  0x08,   // EventSeverity
  0xFF,   // SensorDevice
  0xFF,   // SensorNumber
  0x20,   // Entity
  0x00,   // EntityInstance
  0x40,   // EventData1
  0x01    // EventData2
};

ASF_MSG_NORETRANSMIT gAsfMsgCacheInit = \
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
  0x03,   // Entity
  0x00,   // EntityInstance
  0x40,   // EventData1
  0x0A    // EventData2
};

ASF_MSG_NORETRANSMIT gAsfMsgMemoryMissing = \
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
  0x20,   // Entity
  0x00,   // EntityInstance
  0x40,   // EventData1
  0x01    // EventData2
};

ASF_STOP_WATCHDOG gAsfMsgStopWatchdogTimer = \
{
  0x02,   // Command
	0x02,   // Length
	0x14,   // SubCommand
	0x10,   // VersionNumber
};

ASF_MSG_NORETRANSMIT gAsfMsgBiosPresent = \
{
  0x04,   // Command
  0x0D,   // Length
  0x16,   // SubCommand
  0x10,   // VersionNumber
  0x25,   // EventSensorType
  0x6F,   // EventType
  0x00,   // EventOffset
  0x68,   // EventSourceType
  0x08,   // EventSeverity
  0xFF,   // SensorDevice
  0xFF,   // SensorNumber
  0x22,   // Entity
  0x00,   // EntityInstance
  0x40,   // EventData1
  0x00    // EventData2
};


#define PEI_MEMORY_NOT_INSTALLED  (EFI_SOFTWARE_PEI_CORE | EFI_SW_PS_PC_INSTALL_PEI_MEMORY)
#define PEI_MEMORY_NOT_DETECTED   (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED)

#endif