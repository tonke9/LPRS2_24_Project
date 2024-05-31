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
#include <SerialMgrOld.h>
#include "OBDGauge.h"
#include "OBDProto.h"
#include <BtCommVdrv.h>

static UInt16 SerL = 0;
static int mNewSerial = false;

extern BOOL SerialOpen(int baud, BOOL bluetooth)
{
  UInt32 value;
  Err err;
  SerSettingsType serSettings;
  if (SerL)
    return true;
  mNewSerial = false;
  if (FtrGet(sysFileCSerialMgr, sysFtrNewSerialPresent, &value) == 0)
     if (value != 0)
       mNewSerial = true;
  if (mNewSerial) {
    if (bluetooth) {
      // try opening built-in Bluetooth
      err = SrmOpen(sysFileCVirtRfComm, baud, &SerL);
      if (err == serErrBadPort)
        // try opening TDK BlueM or Blue5 Bluetooth
        err = SrmOpen('BTBS', baud, &SerL);
    } else {
      err = SrmOpen(serPortCradleRS232Port, baud, &SerL);
      if (err == serErrBadPort) //!= errNone
        err = SrmOpen(serPortCradlePort, baud, &SerL);
    }
    if (err == errNone) {
      UInt32 flags = srmSettingsFlagBitsPerChar8 | srmSettingsFlagStopBits1;
      UInt16 paramSize = sizeof(flags);
      if (SrmControl(SerL, srmCtlSetFlags, &flags, &paramSize) == errNone)
        return true;
    } else {
      char s[maxStrIToALen];
      FrmCustomAlert(CustomAlert, "Failed to open communications port",
                     " Err:", StrIToA(s, err));
    }
  } else {
    SysLibFind("Serial Library", &SerL);
    if (SerL) {
      if (SerOpen(SerL, 0, baud) == 0) {
        SerGetSettings(SerL, &serSettings);
        serSettings.flags = serSettingsFlagBitsPerChar8
          | serSettingsFlagStopBits1;
        SerSetSettings(SerL, &serSettings);
        return true;
      }
    }
  }
  return false;
}

extern void SerialClose(void)
{
  if (SerL) {
    if (mNewSerial)
      SrmClose(SerL);
    else
      SerClose(SerL);
  }
  SerL = 0;
}

extern long SerialCheck(void)
{
  long n = -1;
  if (SerL != 0) {
    if (mNewSerial) {
      if (SrmReceiveCheck(SerL, &n) != errNone)
        SrmClearErr(SerL);
    } else {
      if (SerReceiveCheck(SerL, &n))
        SerClearErr(SerL);
    }
  }
  return n;
}

extern BOOL SerialRead(UInt8 *buf, int buflen)
{
  Err err;
  if (SerL) {
    if (mNewSerial) {
      SrmReceive(SerL, buf, buflen, 1, &err);
      if (err != errNone)
        SrmClearErr(SerL);
      else
        return true;
    } else {
      if (SerReceive10(SerL, buf, buflen, -1))
        SerClearErr(SerL);
      else
        return true;
    }
  }
  return false;
}

extern void SerialWrite(UInt8* buf, int buflen)
{
  Err err;
  if (SerL) {
    if (mNewSerial) {
      SrmSend(SerL, buf, buflen, &err);
      if (err != errNone)
        SrmClearErr(SerL);
    } else {
      if (SerSend10(SerL, buf, buflen))
        SerClearErr(SerL);
    }
  }
}
