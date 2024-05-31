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

#define ROWMAX 7
#define PAGEMAX 4
#define STATUSWIDTH 111

typedef struct {
  char *name;
  char *metric;
  char *imperial;
} s_sensor;

typedef struct {
  Int16 value;
  int datacount;
  Int16 data[STATUSWIDTH];
} s_reading;  

enum {
  x11, x0C, x0D, x04, x0E, x0B, x10, x06, x07, x08, x09,
  x0F, x05, x0A, x14, x15, x16, x17, x18, x19, x1A, x1B,
  SENSORMAX
};

enum {ENGLISH, FRENCH, GERMAN, DUTCH, SPANISH, LANGMAX};

typedef struct {
  int Page;
  UInt32 SupportedPids;
  BOOL Imperial;
  UInt8 Sensor[PAGEMAX][ROWMAX];
  UInt8 SensorCount[PAGEMAX];
} SensorPrefs_s;

static SensorPrefs_s mSensorPrefs;
static int mSelectedRow = -1;
static BOOL mMenuDisplayed;
static UInt32 mLastReadTicks;
static DmOpenRef mLogDb;
static int mUpdateCount = 0;
static Int16 mUpdateRate[SENSORMAX];
static BOOL mPrefsChanged = false;

UInt8 activeSensor[SENSORMAX] = {
  x11, x0C, x0D, x04, x0E, x0B, x10, x06, x07, x08, x09,
  x0F, x05, x0A, x14, x15, x16, x17, x18, x19, x1A, x1B
};

UInt8 pidMap[SENSORMAX] = {
  0x11, 0x0c, 0x0d, 0x04, 0x0e, 0x0b, 0x10, 0x06, 0x07, 0x08, 0x09,
  0x0f, 0x05, 0x0a, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b
};

Int8 sensorMap[] = {
  -1,   -1, -1, -1, x04, x05, x06, x07, x08, x09, x0A, x0B, x0C, x0D, x0E, x0F,
  x10, x11, -1, -1, x14, x15, x16, x17, x18, x19, x1A, x1B,  -1,  -1,  -1,  -1
};

s_sensor sensorTitle[LANGMAX][SENSORMAX] = {
        // English
  {
  { "Throttle position", NULL, NULL },
  { "Engine RPM", NULL, NULL },
  { "Vehicle speed", "(km/h)", "(mph)" },
  { "Calculated load value %", NULL, NULL },
  { "Ignition timing advance °", NULL, NULL },
  { "Intake air pressure", "(kPa)", "(inMg)" },
  { "Intake air flow rate", "(g/s)", "(lb/min)" },
  { "Short term fuel trim", NULL, NULL },
  { "Long term fuel trim", NULL, NULL },
  { "Short term fuel trim 2", NULL, NULL },
  { "Long term fuel trim 2", NULL, NULL },
  { "Air temperature", "(°C)", "(°F)" },
  { "Coolant temperature", "(°C)", "(°F)" },
  { "Fuel pressure", "(kPa)", "(inMg)" },
  { "Oxygen Sensor 1", "(mV)", "(mV)" },
  { "Oxygen Sensor 2", "(mV)", "(mV)" },
  { "Oxygen Sensor 3", "(mV)", "(mV)" },
  { "Oxygen Sensor 4", "(mV)", "(mV)" },
  { "Oxygen Sensor 2-1", "(mV)", "(mV)" },
  { "Oxygen Sensor 2-2", "(mV)", "(mV)" },
  { "Oxygen Sensor 2-3", "(mV)", "(mV)" },
  { "Oxygen Sensor 2-4", "(mV)", "(mV)" }
  },
        // French
  {
  { "Position du papillon", NULL, NULL },
  { "Régime moteur", NULL, NULL },
  { "Vitesse du véhicule", "(km/h)", "(mph)" },
  { "Charge calculée moteur %", NULL, NULL },
  { "Avance à l'allumage °", NULL, NULL },
  { "Pression d'air", "(kPa)", "(inMg)" },
  { "Débit d'air (admission)", "(g/s)", "(lb/min)" },
  { "CT correction carburant", NULL, NULL },
  { "LT correction carburant", NULL, NULL },
  { "CT correction carburant 2", NULL, NULL },
  { "LT correction carburant 2", NULL, NULL },
  { "Température d'air", "(°C)", "(°F)" },
  { "Température d'eau", "(°C)", "(°F)" },
  { "Pression carburant", "(kPa)", "(inMg)" },
  { "Sonde à oxygène 1", "(mV)", "(mV)" },
  { "Sonde à oxygène 2", "(mV)", "(mV)" },
  { "Sonde à oxygène 3", "(mV)", "(mV)" },
  { "Sonde à oxygène 4", "(mV)", "(mV)" },
  { "Sonde à oxygène 2-1", "(mV)", "(mV)" },
  { "Sonde à oxygène 2-2", "(mV)", "(mV)" },
  { "Sonde à oxygène 2-3", "(mV)", "(mV)" },
  { "Sonde à oxygène 2-4", "(mV)", "(mV)" }
  },
        // German
  {
  { "Drosselkl. Stellung", NULL, NULL },
  { "Motor U/min", NULL, NULL },
  { "Geschwindigkeit", "(km/h)", "(mph)" },
  { "Berechn. Lastwert %", NULL, NULL },
  { "Zündvoreilung °", NULL, NULL },
  { "Ansaug-Luftdruck", "(kPa)", "(inMg)" },
  { "Ansaug-Luftmenge", "(g/s)", "(lb/min)" },
  { "Kurzz. Kraftst. Trim", NULL, NULL },
  { "Langz. Kraftst. Trim", NULL, NULL },
  { "Kurzz. Kraftst. Trim 2", NULL, NULL },
  { "Langz. Kraftst. Trim 2", NULL, NULL },
  { "Lufttemperatur", "(°C)", "(°F)" },
  { "Kühlmitteltemp.", "(°C)", "(°F)" },
  { "Kraftstoffdruck", "(kPa)", "(inMg)" },
  { "Lambdasonde 1", "(mV)", "(mV)" },
  { "Lambdasonde 2", "(mV)", "(mV)" },
  { "Lambdasonde 3", "(mV)", "(mV)" },
  { "Lambdasonde 4", "(mV)", "(mV)" },
  { "Lambdasonde 2-1", "(mV)", "(mV)" },
  { "Lambdasonde 2-2", "(mV)", "(mV)" },
  { "Lambdasonde 2-3", "(mV)", "(mV)" },
  { "Lambdasonde 2-4", "(mV)", "(mV)" }
  },
        // Dutch
  {
  { "Gaspedaal Positie", NULL, NULL },
  { "Toerental RPM", NULL, NULL },
  { "Snelheid", "(km/h)", "(mph)" },
  { "Berekende Belasting %", NULL, NULL },
  { "Ontstekingstijdstip °", NULL, NULL },
  { "Inlaatlucht Druk", "(kPa)", "(inMg)" },
  { "Inlaatlucht Snelheid", "(g/s)", "(lb/min)" },
  { "Korte Term Br.st. Inst.", NULL, NULL },
  { "Lange Term Br.st. Inst.", NULL, NULL },
  { "Korte Term Br.st. Inst. 2", NULL, NULL },
  { "Lange Term Br.st. Inst. 2", NULL, NULL },
  { "Lucht Temperatuur", "(°C)", "(°F)" },
  { "Koelwater Temperatuur", "(°C)", "(°F)" },
  { "Brandstof Druk", "(kPa)", "(inMg)" },
  { "Zuurstof Opnemer 1", "(mV)", "(mV)" },
  { "Zuurstof Opnemer 2", "(mV)", "(mV)" },
  { "Zuurstof Opnemer 3", "(mV)", "(mV)" },
  { "Zuurstof Opnemer 4", "(mV)", "(mV)" },
  { "Zuurstof Opnemer 2-1", "(mV)", "(mV)" },
  { "Zuurstof Opnemer 2-2", "(mV)", "(mV)" },
  { "Zuurstof Opnemer 2-3", "(mV)", "(mV)" },
  { "Zuurstof Opnemer 2-4", "(mV)", "(mV)" }
  },
        // Spanish
  {
  { "Posicion del acelerador", NULL, NULL },
  { "RPM del motor", NULL, NULL },
  { "Velocidad del vehiculo", "(km/h)", "(mph)" },
  { "% de carga calculada", NULL, NULL },
  { "Avance del tiempo de enc.", NULL, NULL },
  { "Pres.de flujo de aire ad.", "(kPa)", "(inMg)" },
  { "Prop.de flujo de aire ad.", "(g/s)", "(lb/min)" },
  { "Regulac. corta de nafta", NULL, NULL },
  { "Regulac. larga de nafta", NULL, NULL },
  { "Regulac. corta de nafta 2", NULL, NULL },
  { "Regulac. larga de nafta 2", NULL, NULL },
  { "Temperatura del aire", "(°C)", "(°F)" },
  { "Temp. del refrigerante", "(°C)", "(°F)" },
  { "Presión de combustible", "(kPa)", "(inMg)" },
  { "Sensor de oxigeno 1", "(mV)", "(mV)" },
  { "Sensor de oxigeno 2", "(mV)", "(mV)" },
  { "Sensor de oxigeno 3", "(mV)", "(mV)" },
  { "Sensor de oxigeno 4", "(mV)", "(mV)" },
  { "Sensor de oxigeno 2-1", "(mV)", "(mV)" },
  { "Sensor de oxigeno 2-2", "(mV)", "(mV)" },
  { "Sensor de oxigeno 2-3", "(mV)", "(mV)" },
  { "Sensor de oxigeno 2-4", "(mV)", "(mV)" }
  }
};

static s_reading reading[SENSORMAX];

enum { white, lightGray, darkGray, black };

extern UInt32 SensorGetAllActivePids()
{
  int i;
  UInt32 activePids = 0;
  for (i = 0; i < SENSORMAX; i++)
    activePids |= 1L << (32 - pidMap[i]);
  return activePids;
}  

static UInt16 GetSensorFormID()
{
  return Sensor0Form +
    (mPrefs.HidePage ? 1 : 0) +
    (mPrefs.HideMenu ? 2 : 0);
}

extern void SensorGotoForm()
{
  UInt16 formID = GetSensorFormID();
  if (formID != FrmGetActiveFormID())
    FrmGotoForm(formID);
  else
    SensorPage(mSensorPrefs.Page);
}

static void Init(void)
{
  SensorPage(mSensorPrefs.Page);
  if (mSensorPrefs.SupportedPids == 0)
    SensorInit(0xffffffff);
  ReadOpen();
}

extern void SensorPrefsChanged()
{
  mPrefsChanged = true;
}

extern void SensorHandleTick()
{
  if (mPrefsChanged) {
    UInt16 formID = FrmGetActiveFormID();
    if (formID >= Sensor0Form && formID <= Sensor3Form) {
      mPrefsChanged = false;
      if (formID != GetSensorFormID())
        FrmGotoForm(GetSensorFormID());
      else
        Init();
    }
  }
}

extern void SensorSavePrefs()
{
  if (mRomVersion >= 0x02003000)
    PrefSetAppPreferences('obdg', 1, 1, &mSensorPrefs,
                          sizeof(mSensorPrefs), false);
}

extern void SensorLoadPrefs()
{
  UInt16 prefSize = sizeof(mSensorPrefs);
  if (mRomVersion < 0x02003000 ||
      PrefGetAppPreferences('obdg', 1, &mSensorPrefs, &prefSize, false)
      == noPreferenceFound) {
    MemSet(&mSensorPrefs, sizeof(mSensorPrefs), 0);
  }
}

extern void SensorSaveReadings()
{
  if (mRomVersion >= 0x02003000)
    PrefSetAppPreferences('obdg', 2, 1, &reading,
                          sizeof(reading), false);
}

extern void SensorLoadReadings()
{
  UInt16 readingSize = sizeof(reading);
  if (mRomVersion < 0x02003000 ||
      PrefGetAppPreferences('obdg', 2, &reading, &readingSize, false)
      == noPreferenceFound) {
    MemSet(&reading, sizeof(reading), 0);
  }
}

static void OpenRecordDatabase(void)
{
  LocalID localID;
  UInt16 cardNo, attr;
  mLogDb = DmOpenDatabaseByTypeCreator('DATA', 'obdg', dmModeReadWrite);
  if (mLogDb == 0) {
    if (DmCreateDatabase(0, "OBDGaugeLog", 'obdg', 'DATA', false) == errNone) {
      mLogDb = DmOpenDatabaseByTypeCreator('DATA', 'obdg', dmModeReadWrite);
      if (mLogDb != 0) {
        DmOpenDatabaseInfo(mLogDb, &localID, NULL, NULL, &cardNo, NULL);
        if (DmDatabaseInfo(cardNo, localID, NULL, &attr, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL) == errNone) {
          attr |= dmHdrAttrBackup;
          DmSetDatabaseInfo(cardNo, localID, NULL, &attr, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }
      }
    }
  }
}

static void CloseRecordDatabase(void)
{
  if (mLogDb != 0)
    DmCloseDatabase(mLogDb);
  mLogDb = 0;
}

static void ClearRecordDatabase(void)
{
  int numRecords, index;
  BOOL wasOpen = mLogDb != 0;
  if (mLogDb == 0)
    OpenRecordDatabase();
  if (mLogDb != 0) {
    numRecords = DmNumRecords(mLogDb);
    for (index = numRecords - 1; index >= 0; index--)
      DmRemoveRecord(mLogDb, index);
    if (!wasOpen)
      CloseRecordDatabase();
  }
}

extern void SensorWriteLogRecord()
{
  MemHandle recHandle;
  void* recPointer;
  UInt32 seconds, activePids;
  int sensorNum, sensorCount, pid;
  UInt16 index = dmMaxRecordIndex;
  if (mLogDb == 0)
    return;
  seconds = TimGetSeconds();
  activePids = ReadGetActivePids();
  sensorCount = 0;
  for (pid = 1; pid < 32; pid++) {
    if (activePids & (1L << (32 - pid))) {
      sensorNum = sensorMap[pid];
      if (sensorNum >= 0)
        sensorCount++;
    }
  }
  recHandle = DmNewRecord(mLogDb, &index, 8 + sensorCount * 2);
  recPointer = MemHandleLock(recHandle);
  DmWrite(recPointer, 0, &seconds, 4);
  DmWrite(recPointer, 4, &activePids, 4);
  sensorCount = 0;
  for (pid = 1; pid < 32; pid++) {
    if (activePids & (1L << (32 - pid))) {
      sensorNum = sensorMap[pid];
      if (sensorNum >= 0) {
        DmWrite(recPointer, 8 + sensorCount * 2, &reading[sensorNum].value, 2);
        sensorCount++;
        mUpdateCount = 0;
      }
    }
  }
  MemHandleUnlock(recHandle);
  DmReleaseRecord(mLogDb, index, true);
}

static void SelectRow(BOOL select)
{
  const RGBColorType rgbDarkBlue = { 0, 0, 0, 0xcc };
  const RGBColorType rgbBlack = { 0, 0, 0, 0 };
  TableType* tbl = GetObjPtr(SensorStatusTable);
  RectangleType bounds;
  if (mSelectedRow >= 0) {
    TblGetItemBounds(tbl, mSelectedRow, 0, &bounds);
    bounds.extent.y = TblGetRowHeight(tbl, mSelectedRow) - 1;
    if (select)
      if (mRomVersion >= 0x03503000 && mScreenDepth > 2) {
        WinSetForeColor(WinRGBToIndex(&rgbDarkBlue));
        WinDrawRectangleFrame(simpleFrame, &bounds);
        WinSetForeColor(WinRGBToIndex(&rgbBlack));
      } else
        WinDrawRectangleFrame(simpleFrame, &bounds);
    else
      WinEraseRectangleFrame(simpleFrame, &bounds);
  }
}

static void PaintChars(const Char *chars, Int16 len, Coord x, Coord y)
{
  if (mRomVersion >= 0x03503000)
    WinPaintChars(chars, len, x, y);
  else
    WinInvertChars(chars, len, x, y);
}

extern void SensorUpdateStatus(char *message)
{
  RectangleType rect;
  UInt16 formID = FrmGetActiveFormID();
  if (formID == Sensor0Form || formID == Sensor1Form) {
    if (!mPrefs.HideMenu) {
      rect.topLeft.x = 70;
      rect.topLeft.y = 2;
      rect.extent.x = 160 - rect.topLeft.x;
      rect.extent.y = 11;
      WinEraseRectangle(&rect, 0);
      if (message != NULL)
        PaintChars(message, StrLen(message), rect.topLeft.x, rect.topLeft.y);
    }
  }
}

static void UpdateRate(int rate)
{
  int i;
  char s[40];
  UInt32 activePids;
  int sensorCount, pid;
  if (!mPrefs.HideMenu && !mMenuDisplayed) {
    activePids = ReadGetActivePids();
    sensorCount = 0;
    for (pid = 1; pid < 32; pid++) {
      if (activePids & (1L << (32 - pid))) {
        if (sensorMap[pid] >= 0)
          sensorCount++;
      }
    }
    for (i = 0; i < sensorCount; i++)
      s[i] = '\267';
    s[i] = 0;
    s[mUpdateCount] = '\225';
    mUpdateRate[mUpdateCount++] = rate;
    if (mUpdateCount >= sensorCount)
      mUpdateCount = 0;
    rate = 0;
    for (i = 0; i < sensorCount; i++)
      rate += mUpdateRate[i];
    if (sensorCount != 0)
      rate /= sensorCount;
    if (mLogDb != 0) {
      StrCat(s, " Record ");
      StrIToA(s + StrLen(s), DmNumRecords(mLogDb));
    } else {
      StrCat(s, " Scan ");
      if (rate < 0)
        rate = 0;
      if (rate > 99)
        rate = 99;
      i = StrLen(s);
      s[i++] = '0' + rate / 10;
      s[i++] = '.';
      s[i++] = '0' + rate - (rate / 10) * 10;
      StrCopy(s + i, "/sec");
    }
    SensorUpdateStatus(s);
  }
}

char * IToA(Char *s, Int32 i)
{
  //fix StrIToA for negative numbers under PalmOS 1.0
  if (mRomVersion >= 0x02003000 || i > 0)
    return StrIToA(s, i);
  else {
    *s = '-';
    return StrIToA(s + 1, -i);
  }
}

static void UpdateGraph(RectangleType *bounds, Int16 row)
{
  const int widthData = 1;
  const int widthCurrent = 5;
  const int widthNumber = 40;
  const RGBColorType rgbGreen = { 0, 0, 0xff, 0 };
  const RGBColorType rgbDarkGreen = { 0, 0, 0xcc, 0 };
  const RGBColorType rgbBlack = { 0, 0, 0, 0 };
  const RGBColorType rgbYellow = { 0, 0xff, 0xff, 0xcc };
  const RGBColorType rgbGray = { 0, 0x88, 0x88, 0x88 };
  char digits[10];
  char title[40];
  char unit[10];
  RectangleType rect;
  int sensorNum = mSensorPrefs.Sensor[mSensorPrefs.Page][row];
  int value = reading[sensorNum].value;
  Int16 *data = reading[sensorNum].data;
  int datalen = reading[sensorNum].datacount;
  int i, minvalue, maxvalue, scale, units, y, lastY;
  StrCopy(title, sensorTitle[mPrefs.Language][sensorNum].name);
  StrCat(title, " ");
  // use mph for UK
  units = mPrefs.Units;
  if (pidMap[sensorNum] == 0x0D && mPrefs.Units == UNITS_UK)
    units = UNITS_US;
  switch (units) {
  case UNITS_SI:
  case UNITS_UK:
    if (sensorTitle[mPrefs.Language][sensorNum].metric)
      StrCopy(unit, sensorTitle[mPrefs.Language][sensorNum].metric);
    else
      unit[0] = 0;
    StrCat(title, unit);
    break;
  case UNITS_US:
    if (sensorTitle[mPrefs.Language][sensorNum].imperial)
      StrCopy(unit, sensorTitle[mPrefs.Language][sensorNum].imperial);
    else
      unit[0] = 0;
    StrCat(title, unit);
    break;
  }
  rect.topLeft.x = bounds->topLeft.x + 1;
  rect.topLeft.y = bounds->topLeft.y + 1;
  rect.extent.x = bounds->extent.x - 2;
  rect.extent.y = bounds->extent.y - 2;
  if (mRomVersion >= 0x03503000) {
    WinSetForeColor(mScreenDepth > 2 ? WinRGBToIndex(&rgbYellow) : white);
    WinDrawRectangle(&rect, 0);
    WinSetForeColor(mScreenDepth > 2 ? WinRGBToIndex(&rgbBlack) : black);
  } else {
    WinEraseRectangle(&rect, 0);
  }
  if (mRomVersion >= 0x03503000)
    WinSetDrawMode(winOverlay);
  if (mPrefs.GraphType == 0)
    PaintChars(title, StrLen(title), bounds->topLeft.x + 5,
                 bounds->topLeft.y + (bounds->extent.y - FntCharHeight()) / 2);
  if (value != 0x7fff) {
    FntSetFont(ledFont);
    IToA(digits, value);
    PaintChars(digits, StrLen(digits),
               bounds->extent.x - widthNumber,
               bounds->topLeft.y + (bounds->extent.y - FntCharHeight()) / 2);
  }
  WinDrawRectangleFrame(simpleFrame, &rect);
  bounds->extent.x -= widthNumber + 7;
  rect.topLeft.x = bounds->topLeft.x + bounds->extent.x - widthData;
  minvalue = 0x7fff;
  maxvalue = -0x7fff;
  lastY = -1;
  for (i = 0; i < datalen; i++) {
    if (data[i] < minvalue)
      minvalue = data[i];
    if (data[i] > maxvalue && data[i] != 0x7fff)
      maxvalue = data[i];
  }
  if (maxvalue == minvalue) {
    maxvalue += 1;
    minvalue -= 1;
    scale = 3;
  } else
    scale = maxvalue - minvalue;
  for (i = 0; i < datalen; i++) {
    if (i == 0) {
      if (mRomVersion >= 0x03503000)
        WinSetForeColor(mScreenDepth > 2 ?
                        WinRGBToIndex(&rgbGreen) : darkGray);
      rect.extent.x = widthCurrent;
    }
    if (i == 1) {
      if (mRomVersion >= 0x03503000)
        WinSetForeColor(mScreenDepth > 2 ? WinRGBToIndex(&rgbDarkGreen):lightGray);
      rect.extent.x = widthData;
    }
    if (mPrefs.GraphType == 1) {
      if (data[i] == 0x7fff) {
        if (mRomVersion >= 0x03503000)
          WinSetForeColor(mScreenDepth > 2 ? WinRGBToIndex(&rgbGray): darkGray);
        rect.extent.y = bounds->extent.y - 2;
      } else
        rect.extent.y = 1L * (data[i] - minvalue)*(bounds->extent.y - 2) / scale;
      rect.topLeft.y = bounds->topLeft.y + bounds->extent.y - rect.extent.y - 1;
    } else {
      if (data[i] == 0x7fff) {
        if (mRomVersion >= 0x03503000)
          WinSetForeColor(mScreenDepth > 2 ? WinRGBToIndex(&rgbGray): darkGray);
        lastY = -1;
        y = bounds->extent.y - 2;
      } else {
        if (lastY != -1)
          lastY = y;
        y = 1L * (data[i] - minvalue)*(bounds->extent.y - 3) / scale + 1;
        if (lastY == -1)
          lastY = y;
      }
      if (1 && data[i] != 0x7fff) {
        if (lastY == -1) {
          rect.extent.y = 1;
          rect.topLeft.y = bounds->topLeft.y + bounds->extent.y - y - 1;
        } else {
          rect.extent.y = y > lastY ? y - lastY : lastY - y;
          if (rect.extent.y == 0)
            rect.extent.y = 1;
          rect.topLeft.y = bounds->topLeft.y + bounds->extent.y - (y > lastY ? y : lastY) - 1;
        }
      } else {
        rect.extent.y = y;
        rect.topLeft.y = bounds->topLeft.y + bounds->extent.y - y - 1;
      }
    }
    if (i != 0 || data[0] != 0x7fff)
      WinDrawRectangle(&rect, 0);
    if (data[i] == 0x7fff) {
      if (mRomVersion >= 0x03503000)
        WinSetForeColor(mScreenDepth > 2 ? WinRGBToIndex(&rgbDarkGreen):lightGray);
    }
    rect.topLeft.x -= widthData;
    if (rect.topLeft.x <= bounds->topLeft.x)
      break;
  }
  if (mRomVersion >= 0x03503000)
    WinSetForeColor(mScreenDepth > 2 ? WinRGBToIndex(&rgbBlack) : black);
  FntSetFont(stdFont);
  if (mPrefs.GraphType == 1)
    PaintChars(title, StrLen(title), bounds->topLeft.x + 5,
                 bounds->topLeft.y + (bounds->extent.y - FntCharHeight()) / 2);
  if (bounds->extent.y >= FntCharHeight() * 3) {
    if (maxvalue != -0x7fff) {
      IToA(digits, maxvalue);
      PaintChars(digits, StrLen(digits), bounds->topLeft.x + 5,
                    bounds->topLeft.y);
    }
    if (minvalue != 0x7fff) {
      IToA(digits, minvalue);
      PaintChars(digits, StrLen(digits), bounds->topLeft.x + 5,
                    bounds->topLeft.y + bounds->extent.y - FntCharHeight());
    }
  }
  if (mRomVersion >= 0x03503000)
    WinSetDrawMode(winPaint);
}

static void UpdateRow(Int16 row)
{
  TableType* tbl = GetObjPtr(SensorStatusTable);
  RectangleType bounds;
  TblGetItemBounds(tbl, row, 0, &bounds);
  bounds.extent.y = TblGetRowHeight(tbl, row) - 1;
  UpdateGraph(&bounds, row);
}  

static void TableCustomDraw(void* tbl, Int16 row, Int16 column,
                            RectangleType* bounds)
{
  bounds->extent.y -= 1;
  UpdateGraph(bounds, row);
  SelectRow(true);
}

extern void SensorClear()
{
  int i;
  UInt16 formID = FrmGetActiveFormID();
  for (i = 0; i < SENSORMAX; i++) {
    reading[i].value = 0;
    reading[i].datacount = 0;
  }
  if (formID >= Sensor0Form && formID <= Sensor3Form) {
    TblMarkTableInvalid(GetObjPtr(SensorStatusTable));
    TblRedrawTable(GetObjPtr(SensorStatusTable));
  }
}

static void InitSensorTable()
{
  int i;
  RectangleType bounds;
  TableType* tbl = GetObjPtr(SensorStatusTable);
  TblGetBounds(tbl, &bounds);
  TblSetColumnUsable(tbl, 0, true);
  for (i = 0; i < mSensorPrefs.SensorCount[mSensorPrefs.Page]; i++) {
    TblSetRowHeight(tbl, i,
      bounds.extent.y / mSensorPrefs.SensorCount[mSensorPrefs.Page]);
    TblSetRowUsable(tbl, i, true);
    TblSetRowSelectable(tbl, i, false);
    TblSetItemStyle(tbl, i, 0, customTableItem);
   }
  for (; i < ROWMAX; i++)
    TblSetRowUsable(tbl, i, false);
  TblSetCustomDrawProcedure(tbl, 0, TableCustomDraw);
}

static void RedrawSensorTable()
{
  RectangleType bounds;
  mSelectedRow = -1;
  TblMarkTableInvalid(GetObjPtr(SensorStatusTable));
  GetObjBounds(SensorStatusTable, &bounds);
  WinEraseRectangle(&bounds, 0);
  TblRedrawTable(GetObjPtr(SensorStatusTable));
}

static void InitSensorMap()
{
  int i, page, row;
  int count = 0;
  UInt32 activePid;
  for (i = 0; i < SENSORMAX; i++) {
    activePid = 1L << (32 - pidMap[activeSensor[i]]);
    if (mSensorPrefs.SupportedPids & activePid)
      count++;
  }
  for (page = 0; page < PAGEMAX; page++)
    mSensorPrefs.SensorCount[page] = 0;
  row = 0;
  page = 0;
  for (i = 0; i < SENSORMAX; i++) {
    activePid = 1L << (32 - pidMap[activeSensor[i]]);
    if (mSensorPrefs.SupportedPids & activePid) {
      mSensorPrefs.Sensor[page][row] = i;
      mSensorPrefs.SensorCount[page] = ++row;
      if (row >= (count + PAGEMAX - 1) / PAGEMAX) {
        page++;
        row = 0;
      }
    }
  }
}

static void UpdateStatus(int sensorNum, Int16 value)
{
  int row;
  TableType* tbl;
  MenuBarType *mnuP;
  MemMove(&reading[sensorNum].data[1], &reading[sensorNum].data[0],
          reading[sensorNum].datacount * sizeof(reading[0].data[0]));
  if (reading[sensorNum].datacount < STATUSWIDTH-1)
    reading[sensorNum].datacount++;
  reading[sensorNum].value = value;
  reading[sensorNum].data[0] = value;
  tbl = GetObjPtr(SensorStatusTable);
  for (row = 0; row < mSensorPrefs.SensorCount[mSensorPrefs.Page]; row++) {
    if (mSensorPrefs.Sensor[mSensorPrefs.Page][row] == sensorNum) {
      TblMarkRowInvalid(tbl, row);
      mnuP = MenuGetActiveMenu();
      // avoid drawing over menu
      if (!mMenuDisplayed)
        TblRedrawTable(tbl);
    }
  }
}

static void InsertBreakPage(int page)
{
  int row;
  for (row = 0; row < mSensorPrefs.SensorCount[page]; row++)
    UpdateStatus(mSensorPrefs.Sensor[page][row], 0x7fff);
}

extern void SensorInsertBreak()
{
  int page;
  UInt16 formID = FrmGetActiveFormID();
  if (formID >= Sensor0Form && formID <= Sensor3Form)
    for (page = 0; page < PAGEMAX; page++)
      InsertBreakPage(page);
    //    for (i = 0; i < SENSORMAX; i++)
    //      UpdateStatus(i, 0x7fff);
    //  }
}

static void CalculateRate()
{
  UInt32 ticks;
  UInt16 ticksPerSecond;
  ticks = TimGetTicks();
  if (mRomVersion >= 0x02003000)
    ticksPerSecond = SysTicksPerSecond();
  else
    ticksPerSecond = 100;
  if (ticks != mLastReadTicks)
    UpdateRate(ticksPerSecond * 10 / (ticks - mLastReadTicks));
  mLastReadTicks = ticks;
}

extern void SensorReceiveElmObd(UInt8 *message, int length)
{
  int pid;
  UInt16 formID = FrmGetActiveFormID();
  if (formID < Sensor0Form || formID > Sensor3Form)
    return;
  if (length == 0)
    return;
  switch (message[0]) {
  case 0x41:
    if (length < 3) return;
    pid = message[1];
    switch (pid) {
    case 0x04: // calculated load value
      UpdateStatus(x04, (message[2] * 100) / 255);
      break;
    case 0x05: // engine coolent temperature
      switch (mPrefs.Units) {
      case UNITS_SI:
      case UNITS_UK:
        UpdateStatus(x05, message[2] - 40);
        break;
      case UNITS_US:
        UpdateStatus(x05, (message[2] - 40) * 9 / 5 + 32);
        break;
      }
      break;
    case 0x06: // short term fuel trim
    case 0x07: // long term fuel trim
    case 0x08: // short term fuel trim bank 2
    case 0x09: // long term fuel trim bank 2
      UpdateStatus(pid - 0x06 + x06, (message[2] - 128) * 100 / 128);
      break;
    case 0x0a: // fuel pressure
      switch (mPrefs.Units) {
      case UNITS_SI:
      case UNITS_UK:
        UpdateStatus(x0A, message[2] * 3);
        break;
      case UNITS_US:
        UpdateStatus(x0A, (message[2] * 3) * 29 / 100);
        break;
      }
      break;
    case 0x0b: // intake manifold pressure
      switch (mPrefs.Units) {
      case UNITS_SI:
      case UNITS_UK:
        UpdateStatus(x0B, message[2]);
        break;
      case UNITS_US:
        UpdateStatus(x0B, message[2] * 29 / 100);
        break;
      }
      break;
    case 0x0c: // engine RPM
      if (length < 4) return;
      UpdateStatus(x0C, (message[2] * 0x100 + message[3]) / 4);
      break;
    case 0x0d: // vehicle speed
      switch (mPrefs.Units) {
      case UNITS_SI:
      case UNITS_UK:
        UpdateStatus(x0D, message[2]);
        break;
      case UNITS_US:
        UpdateStatus(x0D, message[2] * 62 / 100);
        break;
      }
      break;
    case 0x0e: // timing advance for #1 cylinder
      UpdateStatus(x0E, (message[2] - 128) / 2);
      break;
    case 0x0f: // air intake temperature
      switch (mPrefs.Units) {
      case UNITS_SI:
      case UNITS_UK:
        UpdateStatus(x0F, message[2] - 40);
        break;
      case UNITS_US:
        UpdateStatus(x0F, (message[2] - 40) * 9 / 5 + 32);
        break;
      }
      break;
    case 0x10: // air flow rate
      switch (mPrefs.Units) {
      case UNITS_SI:
      case UNITS_UK:
        UpdateStatus(x10, (message[2] * 0x100 + message[3]) / 100);
        break;
      case UNITS_US:
        UpdateStatus(x10, (message[2] * 0x100 + message[3]) * 132 / 100000);
        break;
      }
      break;
    case 0x11: // absolute throttle position
      UpdateStatus(x11, message[2] * 100 / 255);
      break;
    case 0x14: // O2 bank 1 sensor 1
    case 0x15: // O2 bank 1 sensor 2
    case 0x16: // O2 bank 1 sensor 3
    case 0x17: // O2 bank 1 sensor 4
    case 0x18: // O2 bank 2 sensor 1
    case 0x19: // O2 bank 2 sensor 2
    case 0x1a: // O2 bank 2 sensor 3
    case 0x1b: // O2 bank 2 sensor 4
      UpdateStatus(pid - 0x14 + x14, message[2] * 5);
      break;
    }
    break;
  }
  CalculateRate();
}

static void CalcActivePids()
{
  int page, row;
  UInt32 activePids = 0;
  for (page = 0; page < PAGEMAX; page++) {
    if (mPrefs.Query == 0 || page == mSensorPrefs.Page)
      for (row = 0; row < mSensorPrefs.SensorCount[page]; row++)
        activePids |= 1L <<
          (32 - pidMap[activeSensor[mSensorPrefs.Sensor[page][row]]]);
  }
  ReadSetActivePids(activePids);
}

static void InitPage(void)
{
  UInt16 formID = FrmGetActiveFormID();
  if (formID >= Sensor0Form && formID <= Sensor3Form) {
    InitSensorTable();
    RedrawSensorTable();
  }
  CalcActivePids();
}

extern void SensorInit(UInt32 supportedPids)
{
  if (mSensorPrefs.SupportedPids != supportedPids) {
    mSensorPrefs.SupportedPids = supportedPids;
    InitSensorMap();
  }
  InitPage();
  SensorInsertBreak();
}

static void SelectSensorTableRow(Int16 sx, Int16 sy)
{
  int row;
  RectangleType bounds;
  TableType* tbl = GetObjPtr(SensorStatusTable);
  for (row = 0; row < mSensorPrefs.SensorCount[mSensorPrefs.Page]; row++) {
    TblGetItemBounds(tbl, row, 0, &bounds);
    bounds.extent.y = TblGetRowHeight(tbl, row) - 1;
    if (RctPtInRectangle(sx, sy, &bounds)) {
      if (row == mSelectedRow) {
        SelectRow(false);
        mSelectedRow = -1;
      } else {
        SelectRow(false);
        mSelectedRow = row;
        SelectRow(true);
      }
      break;
    }
  }
}

extern void SensorPage(int page)
{
  int i;
  UInt16 formID = FrmGetActiveFormID();
  SelectRow(false);
  mSelectedRow -= 1;
  mSensorPrefs.Page = page;
  if (formID == Sensor0Form || formID == Sensor2Form)
    for (i = 0; i < PAGEMAX; i++)
      CtlSetValue(GetObjPtr(SensorScreen1Button + i), i == mSensorPrefs.Page);
  if (mPrefs.Query != 0)
    InsertBreakPage(page);
  InitPage();
}

extern Boolean SensorFormHandleEvent(EventPtr eventP)
{
  Boolean handled = false;
  FormType *frmP = FrmGetActiveForm();
  switch (eventP->eType) {
  case frmOpenEvent:
    FrmDrawForm(frmP);
    Init();
    handled = true;
    break;
  case frmUpdateEvent:
    //ReadRestart();
    //Init();
    break;
  case frmCloseEvent:
    ReadClose();
    CloseRecordDatabase();
    break;
  case ctlSelectEvent:
    switch (eventP->data.ctlEnter.controlID) {
    case SensorScreen1Button:
    case SensorScreen2Button:
    case SensorScreen3Button:
    case SensorScreen4Button:
      {
        int page = eventP->data.ctlEnter.controlID - SensorScreen1Button;
        if (page != mSensorPrefs.Page)
          SensorPage(page);
      }
      break;
    default:
    }
    break;
  case penDownEvent:
    SelectSensorTableRow(eventP->screenX, eventP->screenY);
    handled = false;
    break;
  case menuEvent:
    switch (eventP->data.menu.itemID) {
    case MenuPage1:
    case MenuPage2:
    case MenuPage3:
    case MenuPage4:
      MenuEraseStatus(0);
      SensorPage(eventP->data.menu.itemID - MenuPage1);
      handled = true;
      break;
    case MenuUp:
      if (mSelectedRow >= 1) {
        int sensor;
        MenuEraseStatus(0);
        sensor = mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow];
        mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow] =
          mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow-1];
        mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow-1] = sensor;
        SelectRow(false);
        UpdateRow(mSelectedRow);
        mSelectedRow -= 1;
        UpdateRow(mSelectedRow);
        SelectRow(true);
      } else
        FrmAlert(SelectGaugeAlert);
      handled = true;
      break;
    case MenuDown:
      if (mSelectedRow >= 0
          && mSelectedRow < mSensorPrefs.SensorCount[mSensorPrefs.Page]-1) {
        int sensor;
        MenuEraseStatus(0);
        sensor = mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow];
        mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow] =
          mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow+1];
        mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow+1] = sensor;
        SelectRow(false);
        UpdateRow(mSelectedRow);
        mSelectedRow += 1;
        UpdateRow(mSelectedRow);
        SelectRow(true);
      } else
        FrmAlert(SelectGaugeAlert);
      handled = true;
      break;
    case MenuTo1:
    case MenuTo2:
    case MenuTo3:
    case MenuTo4:
      if (mSelectedRow >= 0) {
        int page = eventP->data.menu.itemID - MenuTo1;
        int row;
        if (mSensorPrefs.Page != page
            && mSensorPrefs.SensorCount[page] < ROWMAX) {
          mSensorPrefs.Sensor[page][mSensorPrefs.SensorCount[page]++] =
            mSensorPrefs.Sensor[mSensorPrefs.Page][mSelectedRow];
          for (row = mSelectedRow;
               row < mSensorPrefs.SensorCount[mSensorPrefs.Page]-1; row++)
            mSensorPrefs.Sensor[mSensorPrefs.Page][row] =
              mSensorPrefs.Sensor[mSensorPrefs.Page][row + 1];
          mSensorPrefs.SensorCount[mSensorPrefs.Page] -= 1;
          SensorPage(page);
          mSelectedRow = mSensorPrefs.SensorCount[page]-1;
          SelectRow(true);
        } else
          SndPlaySystemSound(sndInfo);
      } else
        FrmAlert(SelectGaugeAlert);
      handled = true;
      break;
    case MenuRemove:
      if (mSelectedRow >= 0) {
        int row;
        for (row = mSelectedRow;
             row < mSensorPrefs.SensorCount[mSensorPrefs.Page]-1; row++)
          mSensorPrefs.Sensor[mSensorPrefs.Page][row] =
            mSensorPrefs.Sensor[mSensorPrefs.Page][row+1];
        mSensorPrefs.SensorCount[mSensorPrefs.Page] -= 1;
        InitPage();
      } else
        FrmAlert(SelectGaugeAlert);
      handled = true;
      break;
    case MenuClear:
      SensorClear();
      handled = true;
      break;
    case MenuDefault:
      InitSensorMap();
      InitPage();
      handled = true;
      break;
    case MenuRecordStart:
      OpenRecordDatabase();
      handled = true;
      break;
    case MenuRecordStop:
      CloseRecordDatabase();
      handled = true;
      break;
    case MenuRecordClear:
      ClearRecordDatabase();
      handled = true;
      break;
    case MenuCodes:
      FrmPopupForm(CodesForm);
      handled = true;
      break;
    case MenuAccel:
      if (mRomVersion >= 0x03003000)
        AccelStartTest();
      else
        FrmAlert(PalmOSVersionAlert);
      handled = true;
      break;
    case MenuCable:
      ReadEnableBluetooth(false);
      ReadOpen();
      handled = true;
      break;
    case MenuBluetooth:
      ReadEnableBluetooth(true);
      ReadOpen();
      handled = true;
      break;
    case MenuPreferences:
      ReadClose();
      FrmPopupForm(PrefsForm);
      handled = true;
      break;
    case MenuAbout:
      FrmPopupForm(AboutForm);
      handled = true;
      break;
    default:
      break;
    }
    break;
  case winExitEvent:
    if (eventP->data.winExit.exitWindow ==
        (WinHandle)FrmGetFormPtr(GetSensorFormID()))
      mMenuDisplayed = true;
    break;
  case winEnterEvent:
    if (eventP->data.winEnter.enterWindow ==
        (WinHandle)FrmGetFormPtr(GetSensorFormID()))
      mMenuDisplayed = false;
    break;
  default:
  }
  return handled;
}
