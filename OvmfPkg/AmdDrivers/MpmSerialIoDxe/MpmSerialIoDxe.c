/*****************************************************************************
 *
 * Copyright 2020 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting You permission to use this software and documentation (if
 * any) (collectively, the "Software") pursuant to the terms and conditions of
 * the Software License Agreement included with the Software. If You do not have
 * a copy of the Software License Agreement, contact Your AMD representative for
 * a copy.
 * 
 * You agree that You will not reverse engineer or decompile the Software, in
 * whole or in part, except as allowed by applicable law.
 * 
 * WARRANTY DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND. AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
 * BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT AND WARRANTIES
 * ARISING FROM CUSTOM OF TRADE OR COURSE OF USAGE WITH RESPECT TO THE SOFTWARE,
 * INCLUDING WITHOUT LIMITATION, THAT THE SOFTWARE WILL RUN UNINTERRUPTED OR
 * ERROR-FREE. THE ENTIRE RISK ASSOCIATED WITH THE USE OF THE SOFTWARE IS
 * ASSUMED BY YOU. Some jurisdictions do not allow the exclusion of implied
 * warranties, so the above exclusion may not apply to You, but only to the
 * extent required by law.
 * 
 * LIMITATION OF LIABILITY AND INDEMNIFICATION: TO THE EXTENT NOT PROHIBITED BY
 * APPLICABLE LAW, AMD AND ITS LICENSORS WILL NOT, UNDER ANY CIRCUMSTANCES BE
 * LIABLE TO YOU FOR ANY PUNITIVE, DIRECT, INCIDENTAL, INDIRECT, SPECIAL OR
 * CONSEQUENTIAL DAMAGES ARISING FROM POSSESSION OR USE OF THE SOFTWARE OR
 * OTHERWISE IN CONNECTION WITH ANY PROVISION OF THIS AGREEMENT EVEN IF AMD AND
 * ITS LICENSORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. THIS
 * INCLUDES, WITHOUT LIMITATION, DAMAGES DUE TO LOST OR MISAPPROPRIATED DATA,
 * LOST PROFITS OR CONFIDENTIAL OR OTHER INFORMATION, FOR BUSINESS INTERRUPTION,
 * FOR PERSONAL INJURY, FOR LOSS OF PRIVACY, FOR FAILURE TO MEET ANY DUTY
 * INCLUDING OF GOOD FAITH OR REASONABLE CARE, FOR NEGLIGENCE AND FOR ANY OTHER
 * PECUNIARY OR OTHER LOSS WHTSOEVER. In no event shall AMD's total liability to
 * You for all damages, losses, and causes of action (whether in contract, tort
 * (including negligence) or otherwise) exceed the amount of $50 USD. You agree
 * to defend, indemnify and hold harmless AMD, its subsidiaries and affiliates
 * and their respective licensors, directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your
 * possession or use of the Software or violation of the terms and conditions of
 * this Agreement.
 * 
 * U.S. GOVERNMENT RESTRICTED RIGHTS: Notice to U.S. Government End Users. The
 * Software and related documentation are "commercial items", as that term is
 * defined at 48 C.F.R. Section 2.101, consisting of "commercial computer
 * software" and "commercial computer software documentation", as such terms are
 * used in 48 C.F.R. Section 12.212 and 48 C.F.R. Section 227.7202,
 * respectively. Consistent with 48 C.F.R. Section 12.212 or 48 C.F.R. Sections
 * 227.7202-1 through 227.7202-4, as applicable, the commercial computer
 * software and commercial computer software documentation are being licensed to
 * U.S. Government end users: (a) only as commercial items, and (b) with only
 * those rights as are granted to all other end users pursuant to the terms and
 * conditions set forth in this Agreement. Unpublished rights are reserved under
 * the copyright laws of the United States.
 * 
 * EXPORT RESTRICTIONS:  You shall adhere to all applicable U.S. import/export
 * laws and regulations, as well as the import/export control laws and
 * regulations of other countries as applicable. You further agree You will not
 * export, re-export, or transfer, directly or indirectly, any product,
 * technical data, software or source code received from AMD under this license,
 * or the direct product of such technical data or software to any country for
 * which the United States or any other applicable government requires an export
 * license or other governmental approval without first obtaining such licenses
 * or approvals, or in violation of any applicable laws or regulations of the
 * United States or the country where the technical data or software was
 * obtained. You acknowledges the technical data and software received will not,
 * in the absence of authorization from U.S. or local law and regulations as
 * applicable, be used by or exported, re-exported or transferred to: (i) any
 * sanctioned or embargoed country, or to nationals or residents of such
 * countries; (ii) any restricted end-user as identified on any applicable
 * government end-user list; or (iii) any party where the end-use involves
 * nuclear, chemical/biological weapons, rocket systems, or unmanned air
 * vehicles.  For the most current Country Group listings, or for additional
 * information about the EAR or Your obligations under those regulations, please
 * refer to the website of the U.S. Bureau of Industry and Security at
 * http://www.bis.doc.gov/.
 *******************************************************************************
 **/

#include "MpmSerialIo.h"

static BOOLEAN myTestMode = TRUE;

SERIAL_DEVICE_PATH mSerialDevicePath = {
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, { sizeof (VENDOR_DEVICE_PATH), 0} },
    EDKII_SERIAL_PORT_LIB_VENDOR_GUID
  },
  {
    { MESSAGING_DEVICE_PATH, MSG_UART_DP, { sizeof (UART_DEVICE_PATH), 0} },
    0,  // Reserved
    0,  // BaudRate
    0,  // DataBits
    0,  // Parity
    0 // StopBits
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 }
  }
};


EFI_SERIAL_IO_MODE mSerialIoMode = {
  0,        // ControlMask
  1000000,  // Timeout
  115200,   // BaudRate
  1,        // ReceiveFifoDepth
  0,        // DataBits
  0,        // Parity
  0         // StopBits
};

EFI_SERIAL_IO_PROTOCOL mSerialIoTemplate = {
  SERIAL_IO_INTERFACE_REVISION,
  SerialReset,
  SerialSetAttributes,
  SerialSetControl,
  SerialGetControl,
  SerialWrite,
  SerialRead,
  &mSerialIoMode
};


/**
  Check if the RingBuffer is empty or not

  @param  [in] RingBuffer   The pointer of MPM_TEXT_RING_BUFFER structure

  @retval TRUE              The RingBuffer is empty
  @retval FALSE             The RingBuffer is not empty

**/
BOOLEAN
IsMpmTextRingBufferEmpty(
  IN MPM_TEXT_RING_BUFFER *RingBuffer
)
{
  return RingBuffer->ReadPointer == RingBuffer->WritePointer ? TRUE : FALSE;
}


/**
  Read one byte from the RingBuffer

  @param  [in] RingBuffer   The pointer of MPM_TEXT_RING_BUFFER structure
  @param  [in] RingBufferSize The size of the RingBuffer
  @param  [out] Data        The data read from the RingBuffer

  @retval EFI_SUCCESS
  @retval EFI_NOT_READY     RingBuffer is empty

**/
EFI_STATUS
ReadOneByteFromRingBuffer (
  IN MPM_TEXT_RING_BUFFER *RingBuffer,
  IN UINT32               RingBufferSize,
  IN OUT UINT8            *Data
)
{
  if (IsMpmTextRingBufferEmpty(RingBuffer)) {
    return EFI_NOT_READY;
  }

  *Data = RingBuffer->Data[RingBuffer->ReadPointer];
  RingBuffer->ReadPointer = (RingBuffer->ReadPointer + 1) % RingBufferSize;

  return EFI_SUCCESS;
}

/**
  Write one byte to the RingBuffer

  @param  [in] RingBuffer   The pointer of MPM_TEXT_RING_BUFFER structure
  @param  [in] RingBufferSize The size of the RingBuffer
  @param  [out] Data        The data to be written to the RingBuffer

  @retval EFI_SUCCESS

**/
EFI_STATUS
WriteOneByteToRingBuffer (
  IN MPM_TEXT_RING_BUFFER *RingBuffer,
  IN UINT32               RingBufferSize,
  IN UINT8                Data
)
{
  RingBuffer->WritePointer = (RingBuffer->WritePointer + 1) % RingBufferSize;
  RingBuffer->Data[RingBuffer->WritePointer] = Data;

  return EFI_SUCCESS;
}

/**
  Send one byte data to MPM via PSP MpmMbox interface

  @param  This        Protocol instance pointer.
  @param  [in] Data   The Data to be sent to MPM

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
EFI_STATUS
MpmTextWriteOneByte(
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN UINT8    Data
)
{
  EFI_STATUS    Status;
  MPM_SERIAL_IO_DEV *SerialIoDev;

  SerialIoDev = MPM_SERIAL_IO_DEV_FROM_THIS(This);

  Status = WriteOneByteToRingBuffer((MPM_TEXT_RING_BUFFER *)SerialIoDev->TxBuffer,
                                    SerialIoDev->TxBufferSize,
                                    Data);
  return Status;
}

/**
  Read one byte data from MPM via PSP MpmMbox interface

  @param  This        Protocol instance pointer.
  @param  [in] *Data  The Data read from MPM

  @retval EFI_SUCCESS
  @retval EFI_NOT_READY

**/
EFI_STATUS
MpmTextReadyOneByte (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN OUT UINT8   *Data
)
{
  EFI_STATUS    Status;
  MPM_SERIAL_IO_DEV *SerialIoDev;

  SerialIoDev = MPM_SERIAL_IO_DEV_FROM_THIS(This);

  Status = ReadOneByteFromRingBuffer((MPM_TEXT_RING_BUFFER *)SerialIoDev->RxBuffer,
                                      SerialIoDev->RxBufferSize,
                                      Data);
  return Status;
}

/**
  Reset the serial device.

  @param  This              Protocol instance pointer.

  @retval EFI_SUCCESS       The device was reset.
  @retval EFI_DEVICE_ERROR  The serial device could not be reset.

**/
EFI_STATUS
EFIAPI
SerialReset (
  IN EFI_SERIAL_IO_PROTOCOL *This
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "%a\n", __FUNCTION__));

  //
  // Go set the current attributes
  //
  Status = This->SetAttributes (
                   This,
                   This->Mode->BaudRate,
                   This->Mode->ReceiveFifoDepth,
                   This->Mode->Timeout,
                   (EFI_PARITY_TYPE) This->Mode->Parity,
                   (UINT8) This->Mode->DataBits,
                   (EFI_STOP_BITS_TYPE) This->Mode->StopBits
                   );

  //
  // The serial device may not support some of the attributes. To prevent
  // later failure, always return EFI_SUCCESS when SetAttributes is returning
  // EFI_INVALID_PARAMETER.
  //
  if (Status == EFI_INVALID_PARAMETER) {
    return EFI_SUCCESS;
  }

  SerialSetControl (This, EFI_SERIAL_REQUEST_TO_SEND | EFI_SERIAL_DATA_TERMINAL_READY);

  return Status;
}

/**
  Sets the baud rate, receive FIFO depth, transmit/receive time out, parity,
  data bits, and stop bits on a serial device.

  @param  This             Protocol instance pointer.
  @param  BaudRate         The requested baud rate. A BaudRate value of 0 will use the the
                           device's default interface speed.
  @param  ReceiveFifoDepth The requested depth of the FIFO on the receive side of the
                           serial interface. A ReceiveFifoDepth value of 0 will use
                           the device's default FIFO depth.
  @param  Timeout          The requested time out for a single character in microseconds.
                           This timeout applies to both the transmit and receive side of the
                           interface. A Timeout value of 0 will use the device's default time
                           out value.
  @param  Parity           The type of parity to use on this serial device. A Parity value of
                           DefaultParity will use the device's default parity value.
  @param  DataBits         The number of data bits to use on the serial device. A DataBits
                           value of 0 will use the device's default data bit setting.
  @param  StopBits         The number of stop bits to use on this serial device. A StopBits
                           value of DefaultStopBits will use the device's default number of
                           stop bits.

  @retval EFI_SUCCESS           The device was reset.
  @retval EFI_INVALID_PARAMETER One or more attributes has an unsupported value.
  @retval EFI_DEVICE_ERROR      The serial device is not functioning correctly.

**/
EFI_STATUS
EFIAPI
SerialSetAttributes (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN UINT64                 BaudRate,
  IN UINT32                 ReceiveFifoDepth,
  IN UINT32                 Timeout,
  IN EFI_PARITY_TYPE        Parity,
  IN UINT8                  DataBits,
  IN EFI_STOP_BITS_TYPE     StopBits
  )
{
  MPM_SERIAL_IO_DEV *SerialIoDev;

  SerialIoDev = MPM_SERIAL_IO_DEV_FROM_THIS(This);
  //
  // Update the device path
  //
  SerialIoDev->SerialDevicePath.Uart.BaudRate = BaudRate;
  SerialIoDev->SerialDevicePath.Uart.DataBits = DataBits;
  SerialIoDev->SerialDevicePath.Uart.Parity   = (UINT8) Parity;
  SerialIoDev->SerialDevicePath.Uart.StopBits = (UINT8) StopBits;

  return EFI_SUCCESS;
}

/**
  Set the control bits on a serial device

  @param  This             Protocol instance pointer.
  @param  Control          Set the bits of Control that are settable.

  @retval EFI_SUCCESS      The new control bits were set on the serial device.
  @retval EFI_UNSUPPORTED  The serial device does not support this operation.
  @retval EFI_DEVICE_ERROR The serial device is not functioning correctly.

**/
EFI_STATUS
EFIAPI
SerialSetControl (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN UINT32                 Control
  )
{
  MPM_SERIAL_IO_DEV *SerialIoDev;

  SerialIoDev = MPM_SERIAL_IO_DEV_FROM_THIS(This);
  //
  // The control bits that can be set are :
  //     EFI_SERIAL_DATA_TERMINAL_READY: 0x0001  // WO
  //     EFI_SERIAL_REQUEST_TO_SEND: 0x0002  // WO
  //     EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE: 0x1000  // RW
  //     EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE: 0x2000  // RW
  //     EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE: 0x4000 // RW
  //


  if ((Control & (~(EFI_SERIAL_REQUEST_TO_SEND | EFI_SERIAL_DATA_TERMINAL_READY |
                    EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE | EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE | 
                    EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE))) != 0) {
    DEBUG ((DEBUG_INFO, "%a Control(0x%x) is not allowed to be set\n", __FUNCTION__, Control));
    return EFI_UNSUPPORTED;
  }
  DEBUG ((DEBUG_INFO, "%a Control=0x%x\n", __FUNCTION__, Control));


  if ((Control & EFI_SERIAL_DATA_TERMINAL_READY) == EFI_SERIAL_DATA_TERMINAL_READY) {
    SerialIoDev->SerialControl |= EFI_SERIAL_DATA_TERMINAL_READY;
  }

  if ((Control & EFI_SERIAL_REQUEST_TO_SEND) == EFI_SERIAL_REQUEST_TO_SEND) {
    SerialIoDev->SerialControl |= EFI_SERIAL_REQUEST_TO_SEND;
  }

  if ((Control & EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) == EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) {
    SerialIoDev->SerialControl |= EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE;
  }

  if ((Control & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) == EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
    SerialIoDev->SerialControl |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
  }

  if ((Control & EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) == EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) {
    SerialIoDev->SerialControl |= EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE;
  }

  return EFI_SUCCESS;
}

/**
  Retrieves the status of the control bits on a serial device

  @param  This              Protocol instance pointer.
  @param  Control           A pointer to return the current Control signals from the serial device.

  @retval EFI_SUCCESS       The control bits were read from the serial device.
  @retval EFI_DEVICE_ERROR  The serial device is not functioning correctly.

**/
EFI_STATUS
EFIAPI
SerialGetControl (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  OUT UINT32                *Control
  )
{
  MPM_SERIAL_IO_DEV *SerialIoDev;

  SerialIoDev = MPM_SERIAL_IO_DEV_FROM_THIS(This);
  *Control = SerialIoDev->SerialControl;
  return EFI_SUCCESS;
}

/**
  Writes data to a serial device.

  @param  This              Protocol instance pointer.
  @param  BufferSize        On input, the size of the Buffer. On output, the amount of
                            data actually written.
  @param  Buffer            The buffer of data to write

  @retval EFI_SUCCESS       The data was written.
  @retval EFI_DEVICE_ERROR  The device reported an error.
  @retval EFI_TIMEOUT       The data write was stopped due to a timeout.

**/
EFI_STATUS
EFIAPI
SerialWrite (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN OUT UINTN              *BufferSize,
  IN VOID                   *Buffer
  )
{
  EFI_STATUS  Status;
  UINTN       Index;

  
  for (Index = 0; Index < *BufferSize; Index++) {
    if (myTestMode) {
      Status = EFI_SUCCESS;
      DEBUG ((DEBUG_ERROR, "Shannon: w<%x>\n", ((UINT8 *)Buffer)[Index]));
    } else {
      Status = MpmTextWriteOneByte(This, ((UINT8 *)Buffer)[Index]);
    }
    if (EFI_ERROR(Status)) {
      *BufferSize = Index;
      return Status;
    }
  }

  return Status;
}

/**
  Reads data from a serial device.

  @param  This              Protocol instance pointer.
  @param  BufferSize        On input, the size of the Buffer. On output, the amount of
                            data returned in Buffer.
  @param  Buffer            The buffer to return the data into.

  @retval EFI_SUCCESS       The data was read.
  @retval EFI_DEVICE_ERROR  The device reported an error.
  @retval EFI_TIMEOUT       The data write was stopped due to a timeout.

**/
EFI_STATUS
EFIAPI
SerialRead (
  IN EFI_SERIAL_IO_PROTOCOL *This,
  IN OUT UINTN              *BufferSize,
  OUT VOID                  *Buffer
  )
{
  EFI_STATUS    Status;
  MPM_SERIAL_IO_DEV *SerialIoDev;

  SerialIoDev = MPM_SERIAL_IO_DEV_FROM_THIS(This);

  if (myTestMode) {
    return EFI_SUCCESS;
  }

  *BufferSize = 0;

  Status = MpmTextReadyOneByte(This, (UINT8 *)Buffer);
  if (EFI_ERROR(Status)) {
    *BufferSize = 0;
    SerialIoDev->SerialControl |= EFI_SERIAL_INPUT_BUFFER_EMPTY;
  } else {
    *BufferSize = 1;
    SerialIoDev->SerialControl &= ~EFI_SERIAL_INPUT_BUFFER_EMPTY;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
InitMpmSerialIo (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS            Status;
  EFI_HANDLE            SerialHandle = NULL;
  MPM_SERIAL_IO_DEV     *SerialIoDev;

  DEBUG ((DEBUG_INFO, "%a\n", __FUNCTION__));

  // allocate private data
  SerialIoDev = &mMpmSerialIoDev;
  gBS->SetMem(SerialIoDev, sizeof(MPM_SERIAL_IO_DEV), 0x00);

  // setup device instance
  SerialIoDev->Signature = MPM_SERIAL_IO_DEV_SIGNATURE;

  SerialIoDev->SerialIo = mSerialIoTemplate;
  SerialIoDev->SerialIo.Mode = &mSerialIoMode;
  SerialIoDev->SerialDevicePath = mSerialDevicePath;

  SerialIoDev->SerialDevicePath.Uart.BaudRate = PcdGet64 (PcdUartDefaultBaudRate);
  SerialIoDev->SerialDevicePath.Uart.DataBits = PcdGet8 (PcdUartDefaultDataBits);
  SerialIoDev->SerialDevicePath.Uart.Parity   = PcdGet8 (PcdUartDefaultParity);
  SerialIoDev->SerialDevicePath.Uart.StopBits = PcdGet8 (PcdUartDefaultStopBits);

  SerialIoDev->SerialIo.Mode->BaudRate = PcdGet64 (PcdUartDefaultBaudRate);
  SerialIoDev->SerialIo.Mode->DataBits = (UINT32) PcdGet8 (PcdUartDefaultDataBits);
  SerialIoDev->SerialIo.Mode->Parity   = (UINT32) PcdGet8 (PcdUartDefaultParity);
  SerialIoDev->SerialIo.Mode->StopBits = (UINT32) PcdGet8 (PcdUartDefaultStopBits);
  SerialIoDev->SerialIo.Mode->ReceiveFifoDepth = (UINT32)PcdGet16 (PcdUartDefaultReceiveFifoDepth);

  //
  // Issue a reset to initialize the Serial Port
  //
  Status = SerialIoDev->SerialIo.Reset(&SerialIoDev->SerialIo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SerialIoDev->SerialControl = EFI_SERIAL_DATA_TERMINAL_READY \
                                | EFI_SERIAL_REQUEST_TO_SEND \
                                | EFI_SERIAL_CARRIER_DETECT \
                                | EFI_SERIAL_RING_INDICATE \
                                | EFI_SERIAL_DATA_SET_READY \
                                | EFI_SERIAL_OUTPUT_BUFFER_EMPTY \
                                | EFI_SERIAL_INPUT_BUFFER_EMPTY;

  DEBUG ((DEBUG_INFO, "MpmSerialIoDriverStart SerialDevicePath: %s\n",
                      ConvertDevicePathToText ((EFI_DEVICE_PATH *)&SerialIoDev->SerialDevicePath, FALSE, FALSE)));

  //
  // Make a new handle with Serial IO protocol and its device path on it.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &SerialHandle,
                  &gEfiSerialIoProtocolGuid,
                  &SerialIoDev->SerialIo,
                  &gEfiDevicePathProtocolGuid,
                  &SerialIoDev->SerialDevicePath,
                  NULL
                  );

  return Status;
}