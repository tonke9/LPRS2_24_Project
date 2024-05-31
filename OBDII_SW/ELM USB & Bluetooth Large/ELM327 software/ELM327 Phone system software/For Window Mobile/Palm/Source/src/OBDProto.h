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

// OBDSerial.c
extern BOOL SerialOpen(int baud, BOOL bluetooth);
extern void SerialClose(void);
extern long SerialCheck(void);
extern BOOL SerialRead(UInt8 *buf, int buflen);
extern void SerialWrite(UInt8* buf, int buflen);

// OBDSensor.c
extern void SensorWriteLogRecord();
extern UInt32 SensorGetAllActivePids();
extern void SensorGotoForm();
extern void SensorPrefsChanged();
extern void SensorHandleTick();
extern void SensorLoadPrefs();
extern void SensorSavePrefs();
extern void SensorLoadReadings();
extern void SensorUpdateStatus(char *message);
extern void SensorSaveReadings();
extern Boolean SensorFormHandleEvent(EventPtr eventP);
extern void SensorInsertBreak();
extern void SensorReceiveElmObd(UInt8 *message, int length);
extern void SensorInit();
extern void SensorClear();
extern void SensorPage(int page);

// OBDCommon.c
void * GetObjPtr(int objID);
void ShowObj(int objID, BOOL show);
void GetObjBounds(int objID, RectangleType *bounds);
void SetObjBounds(int objID, RectangleType *bounds);
void DrawBorder(int objID);
void EraseDrawBorder(int objID);
void EraseObj(int objID);

// OBDRead.c
extern void ReadEnableBluetooth(BOOL enable);
extern void ReadOpen(void);
extern void ReadClose(void);
extern void ReadPause(void);
extern void ReadResume(void);
extern void ReadRestart(void);
extern void ReadSetActivePids(UInt32 activePids);
extern UInt32 ReadGetActivePids();
extern void ReadHandleTick(void);
extern void ReadPowerUp(void);
extern void ReadPowerDown(void);
extern BOOL ReadSerial(void);
extern void ReadReadCodes(void);
extern void ReadClearCodes(void);

// OBDGauge.c
extern UInt32 mScreenDepth;
extern UInt32 mRomVersion;
extern void EnableProgress(ProgressPtr progress);

// OBDCodes.c
extern Boolean CodesFormHandleEvent(EventPtr eventP);
extern void CodesSetMIL(BOOL MIL);
extern void CodesSetClearCodes(void);
extern void CodesReceiveElmObd(UInt8 *message, int length);
extern void CodesReadComplete(void);

// OBDPrefs.c
enum { UNITS_SI, UNITS_US, UNITS_UK };
enum { INTERFACE_ELM, INTERFACE_MULTIPLEX };
enum { BAUD_19200, BAUD_9600 };
enum {
  PROTOCOL_DISABLE,
  PROTOCOL_ISO,
  PROTOCOL_VPW,
  PROTOCOL_PWM,
  PROTOCOL_KWP
};
typedef struct {
  Int8 Units;
  Int8 Query;
  BOOL HidePage;
  BOOL HideMenu;
  Int8 Interface;
  Int8 Baud;
  Int8 Address;
  Int8 Protocol;
  Int8 Language;
  Int8 Timeout;
  Int8 GraphType;
} Prefs_s;
extern Prefs_s mPrefs;
extern Boolean PrefsFormHandleEvent(EventPtr eventP);
extern Boolean PrefsElmFormHandleEvent(EventPtr eventP);
extern Boolean PrefsMultiplexFormHandleEvent(EventPtr eventP);
extern void PrefsLoadPrefs();
extern void PrefsSavePrefs();

// OBDAccel.c
extern BOOL AccelActive();
extern void AccelInit();
extern void AccelStartTest();
extern void AccelStopTest();
extern void AccelFailed();
extern void AccelReceiveElmObd();

// OBDWait.c
extern void DisplayWaitForm(void (*WaitCallback)());
extern Boolean WaitFormHandleEvent(EventPtr eventP);
