// OBD Gauge
// Copyright (C) 2004 Dana Peters

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <PalmOS.h>
#include "OBDGauge.h"
#include "OBDProto.h"

#define RXBUFSIZE 32

typedef enum {
  MODE_NULL,
  MODE_INIT,
  MODE_ECHO_OFF,
  MODE_LINEFEED_OFF,
  MODE_SET_TIMEOUT,
  MODE_READ_PIDS,
  MODE_READ_SENSORS,
  MODE_READ_MIL,
  MODE_READ_ACTIVE_CODES,
  MODE_READ_PENDING_CODES,
  MODE_CLEAR_CODES
} eMode;

static UInt32 mSupportedPids;
static UInt32 mActivePids;
static BOOL mReadOpen;
static UInt32 mTicks;
static eMode mMode = MODE_NULL;
static BOOL mBluetooth = false;
static BOOL mReadCodes = false;
static BOOL mNeedInit = false;

static void SendObdMultiplex(UInt8 *message, int length, BOOL multi)
{
  char command[15];
  int i, cs;
  command[0] = mPrefs.Address;
  switch (mPrefs.Protocol) {
  case PROTOCOL_ISO:
    command[1] = mNeedInit ? 0x01 : (multi ? 0x10 : 0x81);
    command[3] = 0x68;
    command[4] = 0x6a;
    break;
  case PROTOCOL_VPW:
    command[1] = multi ? 0x02 : 0x82;
    command[3] = 0x68;
    command[4] = 0x6a;
    break;
  case PROTOCOL_PWM:
    command[1] = multi ? 0x04 : 0x84;
    command[3] = 0x61;
    command[4] = 0x6a;
    break;
  case PROTOCOL_KWP:
    command[1] = mNeedInit? 0x85 : (multi ? 0x88 : 0x89);
    command[3] = 0xc7;
    command[4] = 0x33;
    break;
  default:
    return;
  }
  command[2] = length + 3;
  command[5] = 0xf1;
  for (i = 0; i < 8; i++)
    command[i + 6] = i < length ? message[i] : 0;
  cs = 0;
  for (i = 1; i < 14; i++)
    cs += command[i];
  command[14] = cs & 0xff;
  SerialWrite(command, 15);
}

static void SendAsciiElm(char *message)
{
  SerialWrite(message, StrLen(message));
}

static void SendObdElm(UInt8 *message, int length)
{
  static char *hex = "0123456789ABCDEF";
  char str[30], *sp = str;
  int i;
  for (i = 0; i < length; i++) {
    *sp++ = hex[message[i] >> 4];
    *sp++ = hex[message[i] & 0xf];
  }
  *sp++ = '\r';
  *sp = 0;
  SerialWrite(str, StrLen(str));
}

static void SendObd(UInt8 *message, int length)
{
  if (mPrefs.Interface == INTERFACE_MULTIPLEX)
    SendObdMultiplex(message, length, false);
  else
    SendObdElm(message, length);
}

static void SendObdMulti(UInt8 *message, int length)
{
  if (mPrefs.Interface == INTERFACE_MULTIPLEX)
    SendObdMultiplex(message, length, true);
  else
    SendObdElm(message, length);
}

static void SendPidQuery(void)
{
  UInt8 msg[2];
  msg[0] = 1;
  msg[1] = 0;
  SendObd(msg, 2);
}

static void SendSensorQuery(void)
{
  static int bitnum = 0;
  UInt8 msg[2];
  if (mActivePids != 0) {
    for (;; bitnum++) {
      if (bitnum > 31) {
        SensorWriteLogRecord();
        bitnum = 0;
      }
      if (mActivePids & (1L << (31 - bitnum))) {
        msg[0] = 1;
        msg[1] = ++bitnum;
        SendObd(msg, 2);
        break;
      }
    }
  }
}

static void SendMILQuery(void)
{
  UInt8 msg[2];
  msg[0] = 1;
  msg[1] = 1;
  SendObd(msg, 2);
}

static void SendActiveCodeQuery(void)
{
  UInt8 msg[1];
  msg[0] = 3;
  SendObdMulti(msg, 1);
}

static void SendPendingCodeQuery(void)
{
  UInt8 msg[1];
  msg[0] = 7;
  SendObdMulti(msg, 1);
}

static void SendClearCodesCommand(void)
{
  UInt8 msg[1];
  msg[0] = 4;
  SendObd(msg, 1);
}

static void SendSetTimeout(void)
{
  char s[10];
  static char *timeout[] = { "32", "2B", "25", "1F", "19", "13", "0D", "06" };
  StrCopy(s, "ATST ");
  StrCat(s, timeout[mPrefs.Timeout]);
  StrCat(s, "\r");
  SendAsciiElm(s);
}

static void SwitchMode(eMode mode)
{
  mTicks = TimGetTicks();
  mMode = mode;
  switch (mMode) {
  case MODE_NULL:
    // do nothing
    break;
  case MODE_INIT:
    if (AccelActive())
      AccelFailed();
    mSupportedPids = 0;
    mNeedInit = true;
    if (mPrefs.Interface == INTERFACE_MULTIPLEX)
      SwitchMode(MODE_READ_PIDS);
    else
      SendAsciiElm("ATZ\r");
    SensorUpdateStatus("Initializing...");
    break;
  case MODE_SET_TIMEOUT:
    SendSetTimeout();
    break;
  case MODE_ECHO_OFF:
    SendAsciiElm("ATE0\r");
    break;
  case MODE_LINEFEED_OFF:
    SendAsciiElm("ATL0\r");
    break;
  case MODE_READ_PIDS:
    SendPidQuery();
    break;
  case MODE_READ_SENSORS:
#if FAST
    SendSensorQuery();
#endif
    break;
  case MODE_READ_MIL:
    break;
  case MODE_READ_ACTIVE_CODES:
    SendActiveCodeQuery();
    break;
  case MODE_READ_PENDING_CODES:
    SendPendingCodeQuery();
    break;
  case MODE_CLEAR_CODES:
    SendClearCodesCommand();
    break;
  }
}

static BOOL SecsHaveElapsed(int secs)
{
  UInt16 ticksPerSecond;
  if (mRomVersion >= 0x02003000)
    ticksPerSecond = SysTicksPerSecond();
  else
    ticksPerSecond = 100;
  if (TimGetTicks() - mTicks > ticksPerSecond * secs) {
    mTicks = TimGetTicks();
    return true;
  }
  return false;
}

extern void ReadSetActivePids(UInt32 activePids)
{
  mActivePids = activePids;
}

extern UInt32 ReadGetActivePids()
{
  return mActivePids;
}

static void ReadElmAscii(char *message)
{
}

static void ReadObd(UInt8 *message, int length)
{
   if (length == 0)
    return;
  switch (message[0]) {
  case 0x41: // sensor report
    if (length < 3) return;
    switch (message[1]) {
    case 0x00: // supported PIDs
      if (length < 6) return;
      MemMove(&mSupportedPids, message + 2, 4);
      mActivePids = 0;
      if (AccelActive())
        AccelInit(mSupportedPids);
      else
        SensorInit(mSupportedPids);
      SwitchMode(MODE_READ_SENSORS);
      break;
    case 0x01: // trouble codes count
      if (length < 3) return;
      CodesSetMIL((message[2] & 0x80) != 0);
      break;
    default:
      mTicks = TimGetTicks();
      // keep Palm powered up while reading sensors
      EvtResetAutoOffTimer();
      if (AccelActive())
        AccelReceiveElmObd(message, length);
      else
        SensorReceiveElmObd(message, length);
    }
    break;
  case 0x43: // active trouble codes
  case 0x47: // pending trouble codes
    CodesReceiveElmObd(message, length);
    break;
  case 0x44: // clear trouble codes
    CodesSetClearCodes();
    break;
  }
}

static void ObdReplyComplete(void)
{
  switch (mMode) {
  case MODE_NULL:
    // do nothing
    break;
  case MODE_INIT:
    if (mPrefs.Interface == INTERFACE_ELM)
      SwitchMode(MODE_ECHO_OFF);
    break;
  case MODE_ECHO_OFF:
    SwitchMode(MODE_LINEFEED_OFF);
    break;
  case MODE_LINEFEED_OFF:
    SwitchMode(MODE_SET_TIMEOUT);
    break;
  case MODE_SET_TIMEOUT:
    SwitchMode(MODE_READ_PIDS);
    break;
  case MODE_READ_PIDS:
    break;
  case MODE_READ_SENSORS:
    mNeedInit = false;
    if (mReadCodes) {
      mReadCodes = false;
      SendMILQuery();
      SwitchMode(MODE_READ_MIL);
    } else
      SendSensorQuery();
    break;
  case MODE_READ_MIL:
    SwitchMode(MODE_READ_ACTIVE_CODES);
    break;
  case MODE_READ_ACTIVE_CODES:
    SwitchMode(MODE_READ_PENDING_CODES);
    break;
  case MODE_READ_PENDING_CODES:
    SwitchMode(MODE_NULL);
    CodesReadComplete();
    break;
  case MODE_CLEAR_CODES:
    SwitchMode(MODE_INIT);
    ReadReadCodes();
  }
}

static void ReceiveElm(UInt8 *message)
{
  int i;
  int n = 0;
  char c;
  BOOL ascii = false; 
  BOOL odd = false;
  UInt8 b;
  for (i = 0; message[i] != 0 && !ascii; i++) {
    c = message[i];
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
      odd = !odd;
    else
      ascii = (c != ' ' && c != '>');
  }
  if (ascii || odd) {
    ReadElmAscii((char *)message);
  } else {
    n = 0;
    b = 0;
    odd = false;
    for (i = 0; message[i] != 0; i++) {
      c = message[i];
      if (c >= '0' && c <= '9')
        b = (b << 4) + c - '0';
      else if (c >= 'A' && c <= 'F')
        b = (b << 4) + c - 'A' + 10;
      else
        continue;
      odd = !odd;
      if (!odd) {
        message[n++] = b;
        b = 0;
      }
    }
    ReadObd(message, n);
  }
}

extern BOOL ReadSerialMultiplex(void)
{
  static UInt8 rxbuf[RXBUFSIZE];
  static int rxindex = 0;
  int i;
  long n;

  n = SerialCheck();
  if (n <= 0)
    return false;
  if (n + rxindex > RXBUFSIZE)
    n = RXBUFSIZE - rxindex;
  SerialRead(&rxbuf[rxindex], n);
  i = rxindex;
  rxindex += n;
  if (rxindex >= 14) {
    if (rxbuf[0] == 0x40
        && (rxbuf[1] == 0x81 || rxbuf[1] == 0x82 || rxbuf[1] == 0x84)
        && (rxbuf[2] == 0x48 || rxbuf[2] == 0x41)
        && rxbuf[3] == 0x6b)
      ReadObd(rxbuf + 5, 8);
    ObdReplyComplete();
    rxindex -= 14;;
    MemMove(rxbuf, rxbuf + 14, rxindex);
  }
  return true;
}

extern BOOL ReadSerialElm(void)
{
  static int start = 0;
  static int rxindex = 0;
  static UInt8 rxbuf[RXBUFSIZE];
  static UInt32 lastTicks;
  UInt32 ticks;
  int i;
  long n;

  n = SerialCheck();
  if (n <= 0)
    return false;
  ticks = TimGetTicks();
  if (lastTicks + 50 < ticks)
    rxindex = 0;
  lastTicks = ticks;
  if (n + rxindex > RXBUFSIZE)
    n = RXBUFSIZE - rxindex;
  SerialRead(&rxbuf[rxindex], n);
  i = rxindex;
  rxindex += n;
  for (; i < rxindex; i++) {
    switch (rxbuf[i]) {
    case 0x0d:
      rxbuf[i] = 0;
#ifndef FAST
      if (i > start)
        ReceiveElm(&rxbuf[start]);
      start = i + 1;
#endif
      break;
    case '>':
      ObdReplyComplete();
#if FAST
      if (i > start)
        ReceiveElm(&rxbuf[start]);
#endif
      start = i + 1;
      break;
    default:
      break;
    }
  }
  if (start > 0) {
    rxindex -= start;
    MemMove(rxbuf, rxbuf + start, rxindex);
    start = 0;
  }
  return true;
}

extern BOOL ReadSerial(void)
{
  switch (mPrefs.Interface) {
  case INTERFACE_ELM:
    return ReadSerialElm();
  case INTERFACE_MULTIPLEX:
    return ReadSerialMultiplex();
  }
  return false;
}

static int GetBaud()
{
  switch (mPrefs.Interface) {
  case INTERFACE_ELM:
    return 9600;
  case INTERFACE_MULTIPLEX:
    switch (mPrefs.Baud) {
    case BAUD_9600:
      return 9600;
    case BAUD_19200:
      return 19200;
    }
  }
  return 0;
}

extern void ReadHandleTick(void)
{
  ReadSerial();
  switch (mMode) {
  case MODE_NULL:
    // do nothing
    break;
  case MODE_INIT:
    if (mPrefs.Interface == INTERFACE_ELM)
      if (SecsHaveElapsed(4))
        SendAsciiElm("ATZ\r");
    break;
  case MODE_ECHO_OFF:
  case MODE_LINEFEED_OFF:
  case MODE_READ_PIDS:
  case MODE_READ_SENSORS:
    if (SecsHaveElapsed(4))
      SwitchMode(MODE_INIT);
    break;
  case MODE_READ_MIL:
  case MODE_READ_ACTIVE_CODES:
  case MODE_READ_PENDING_CODES:
  case MODE_CLEAR_CODES:
    break;
  }
}

extern void ReadEnableBluetooth(BOOL enable)
{
  mBluetooth = enable;
}

extern void ReadOpen(void)
{
  if (mReadOpen)
    ReadClose();
  mReadOpen = true;
  SerialOpen(GetBaud(), mBluetooth);
  SwitchMode(MODE_INIT);
}

extern void ReadClose(void)
{
  mReadOpen = false;
  SwitchMode(MODE_NULL);
  SerialClose();
}

extern void ReadRestart(void)
{
  if (mMode != MODE_READ_SENSORS)
    SwitchMode(MODE_INIT);
}

extern void ReadPowerUp(void)
{
  if (mReadOpen) {
    SerialOpen(GetBaud(), mBluetooth);
    SwitchMode(MODE_INIT);
  }
}

extern void ReadPowerDown(void)
{
  if (mReadOpen)
    SerialClose();
}

extern void ReadReadCodes(void)
{
  mReadCodes = true;
}

extern void ReadClearCodes(void)
{
  SwitchMode(MODE_CLEAR_CODES);
}
