#ifndef _AMD_ASF_H_
#define _AMD_ASF_H_

#include <Base.h>
// #include <Ppi/ReportStatusCodeHandler.h>
#include <Pi/PiStatusCode.h>

#define ASF_BIOS_WATCHDOG_TIMER           0       // Platform firmware
#define ASF_OS_WATCHDOG_TIMER             0x48    // OS vendor
#define START_TIMER                       TRUE
#define STOP_TIMER                        FALSE

#define EVENT_TYPE_PROGRESS               0
#define EVENT_TYPE_ERROR                  1
#define EVENT_TYPE_SYSTEM_MANAGEMENT      2

#define ASFMSG_CMD_SENSOR_DEV_SYSSTATE    1	 // Sensor Device and System State
#define ASFMSG_SUBCMD_GET_EVENT_DATA	    0x12 // Get Event Data
#define ASFMSG_SUBCMD_GET_EVENT_STS		    0x12 // Get Event Status
#define ASFMSG_SUBCMD_DEV_TYPE_POLL		    0x13 // Device Type Poll
#define ASFMSG_SUBCMD_SET_SYS_STATE		    0x18 // Set System State(00010100??))

#define ASFMSG_CMD_MGM_CTRL               2	 // Management Control
#define ASFMSG_SUBCMD_START_WD_TIMER	    0x13 // Start Watchdog Timer
#define ASFMSG_SUBCMD_STOP_WD_TIMER		    0x14 // Stop Watchdog Timer

#define ASFMSG_CMD_CONFIG                 3	 // ASF Configuration
#define ASFMSG_SUBCMD_CLR_BOOT_OPTION     0x15 // Clear Boot Options
#define ASFMSG_SUBCMD_RET_BOOT_OPTION	    0x16 // Return Boot Options
#define ASFMSG_SUBCMD_NO_BOOT_OPTION	    0x17 // No Boot Options

#define ASFMSG_CMD_MSG                    4	 // ASF Messaging
#define ASFMSG_SUBCMD_MSG_RETRANSMIT	    0x15 // Push Message with Retransmission
#define ASFMSG_SUBCMD_MSG_NORETRANSMIT	  0x16 // Push Message without Retransmission
#define ASFMSG_SUBCMD_SET_ALRT_CFG		    0x11 // Set Alert Configuration
#define ASFMSG_SUBCMD_GET_ALRT_CFG		    0x12 // Get Alert Configuration

#define ASFMSG_SYSTEM_STATE_S0		        0 // S0/G0 "Working"
#define ASFMSG_SYSTEM_STATE_S1		        1 // S1
#define ASFMSG_SYSTEM_STATE_S2		        2 // S2
#define ASFMSG_SYSTEM_STATE_S3		        3 // S3
#define ASFMSG_SYSTEM_STATE_S4		        4 // S4
#define ASFMSG_SYSTEM_STATE_S5		        5 // S5/G2 "Soft-off"

//Asf Boot Option special cmd
#define ASF_SCMD_NOP                        0
#define ASF_SCMD_FORCE_PXE_BOOT             1
#define ASF_SCMD_FORCE_HARD_DRIVE_BOOT      2
#define ASF_SCMD_FORCE_HARD_DRIVE_SAFE_BOOT 3
#define ASF_SCMD_FORCE_DIAGNOSTIC_BOOT      4
#define ASF_SCMD_FORCE_CD_BOOT              5

//Asf Boot Option Bit
#define ASF_BOP_BIT_LOCK_POWER_BUTTON           0x02    //BIT1
#define ASF_BOP_BIT_LOCK_RESET_BUTTON           0x04    //BIT2
#define ASF_BOP_BIT_LOCK_KEYBOARD               0x20    //BIT5
#define ASF_BOP_BIT_LOCK_SLEEP_BUTTON           0x40    //BIT6
#define ASF_BOP_BIT_PASSWORD_BYPASS             0x800  //BIT11
#define ASF_BOP_BIT_FORCE_PROGRESS_EVENT        0x1000  //BIT12
#define ASF_BOP_BIT_FIRMWARE_VERBOSITY          0x6000  //BIT13+BIT14
#define ASF_BOP_BIT_CONFIGURATION_DATA_RESET    0x8000  //BIT15

/**
  This is the ASF Message Type structure.
**/
typedef enum {
  MsgHddInit,
  MsgApInit,
  MsgUserInitSetup,
  MsgUsbResourceConfig,
  MsgPciResourceConfig,
  MsgVideoInit,
  MsgKbcInit,
  MsgKbcTest,
  MsgOSBoot,
  MsgMotherBoardInit,
  MsgNoVideo,
  MsgKbdFailure,
  MsgHddFailure,
  MsgChassisIntrusion,
  MsgUserPasswordInvalid,
  MsgNoBootMedia,
  MsgSystemStateS0,
  MsgSystemStateS3,
  MsgSystemStateS4,
  MsgSystemStateS5,
  MsgStartBiosWatchdogTimer,
  MsgStartOsWatchdogTimer,
  MsgStopWatchdogtimer,
  MsgClearBootOption
} ASF_MESSAGE_TYPE;

#pragma pack (1)
/*
  ASF Boot Option message structure.
*/
typedef struct _ASF_BOOT_OPTION
{
  UINT8       Command;
  UINT8       Length;
	UINT8       SubCommand;
	UINT8       VersionNumber;
	UINT8       IANAId[4];
  UINT8       SpecialCmd;
  UINT8       SpecialCmdParamHighByte;
  UINT8       SpecialCmdParamLowByte;
  UINT8       BootOptionBit[2];
	UINT8       OEMParam[2];
} ASF_BOOT_OPTION;


/*
  ASF NO-Retransmit message structure
*/
typedef struct _ASF_MSG_NORETRANSMIT {
  UINT8       Command;
  UINT8       Length;
  UINT8       SubCommand;
  UINT8       VersionNumber;
  UINT8       EventSensorType;
  UINT8       EventType;
  UINT8       EventOffset;
  UINT8       EventSourceType;
  UINT8       EventSeverity;
  UINT8       SensorDevice;
  UINT8       SensorNumber;
  UINT8       Entity;
  UINT8       EntityInstance;
  UINT8       EventData1;
  UINT8       EventData2;
  UINT8       EventData3;
  UINT8       EventData4;
  UINT8       EventData5;
  UINT8       EventData6;
} ASF_MSG_NORETRANSMIT;

/*
  ASF Start Watchdog message structure
*/
typedef struct _ASF_START_WATCHDOG {
	UINT8       Command;
	UINT8       Length;
	UINT8       SubCommand;
	UINT8       VersionNumber;
  UINT16      TimeoutValue;
	UINT8       EventSensorType;
	UINT8       EventType;
	UINT8       EventOffset;
	UINT8       EventSourceType;
	UINT8       EventSeverity;
	UINT8       SensorDevice;
	UINT8       SensorNumber;
	UINT8       Entity;
	UINT8       EntityInstance;
  UINT8       EventData1;
  UINT8       EventData2;
  UINT8       EventData3;
  UINT8       EventData4;
  UINT8       EventData5;
  UINT8       EventData6;
} ASF_START_WATCHDOG;

/*
  ASF Stop Watchdog message structure
*/
typedef struct _ASF_STOP_WATCHDOG
{
	UINT8       Command;
	UINT8       Length;
	UINT8       SubCommand;
	UINT8       VersionNumber;
} ASF_STOP_WATCHDOG;

/*
  ASF System state messgae structure.
*/
typedef struct _ASF_SYSTEM_STATE
{
  UINT8       Command;
  UINT8       Length;
  UINT8       SubCommand;
  UINT8       VersionNumber;
  UINT8       SystemState;
} ASF_SYSTEM_STATE;

/**
  This is the Message Data Hub Map Structure.
**/
typedef struct {
  ASF_MESSAGE_TYPE            MessageType;
  EFI_STATUS_CODE_VALUE       StatusCode;
} MESSAGE_DATA_HUB_MAP;


#pragma pack ()

#pragma pack (2)

/**
  This is the ASF System Firmware Event Structure.
**/
typedef struct {
  ASF_MESSAGE_TYPE            Type;
  union {
    ASF_MSG_NORETRANSMIT      MessageNoRetransmit;
    ASF_START_WATCHDOG        StartWdtMessage;
    ASF_STOP_WATCHDOG         StopWatchdogMessage;
    ASF_SYSTEM_STATE          SystemState;
  } Message;
  EFI_STATUS_CODE_VALUE       StatusCode;
  UINT16                      MessageString[128];
} ASF_MESSAGE;

#pragma pack ()

#endif