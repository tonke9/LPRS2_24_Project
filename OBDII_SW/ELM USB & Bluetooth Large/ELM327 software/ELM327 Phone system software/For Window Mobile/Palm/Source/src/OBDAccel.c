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

typedef enum {
  STAGEINIT, STAGECANCEL, STAGESTOP, STAGEGO,
  STAGETIME, STAGERESULT, STAGEFAILED
} e_Stage;

static ProgressPtr mProgress;
static e_Stage mStage;
static UInt32 mTimerTicks;
static int mReadings;

static Boolean ProgressCallback(PrgCallbackDataPtr cbP)
{
  static UInt32 iconTicks;
  static int iconState;
  char *text = NULL;
  UInt32 icon = 0;
  int stage = cbP->stage;
  if (!cbP)
    return false;
  if (cbP->canceled)
    stage = STAGECANCEL;
  switch (stage) {
  case STAGEINIT:
    text = "Initializing, please wait...";
    icon = YieldIcon;
    break;
  case STAGECANCEL:
    text = "Canceled";
    break;
  case STAGESTOP:
    text = "Bring vehicle to a complete stop";
    icon = StopSignIcon;
    break;
  case STAGEGO:
    text = mPrefs.Units == UNITS_SI ?
      "Accelerate to 100\240km/h" : "Accelerate to 60\240mph";
    icon = TrafficLightIcon;
    iconTicks = 0;
    iconState = 0;
    break;
  case STAGETIME:
    text = "Timer running...         ";
    icon = StopWatchIcon1 + iconState;
    if (TimGetTicks() - iconTicks > SysTicksPerSecond()) {
      iconTicks = TimGetTicks();
      iconState = iconState < 7 ? iconState + 1 : 0;
    }
    break;
  case STAGERESULT:
    // there is no easy way to control where line breaks occur
    // so \240 is used in place of space to avoid a line break
    // and multiple spaces are used to try to force a line break
    text = mPrefs.Units == UNITS_SI ?
      "0-100\240km/h\240in               " :
      "0-60\240mph\240in                 ";
    icon = FinishFlagIcon;
    break;
  case STAGEFAILED:
    text = "Test aborted because OBD connection lost";
    icon = NoEntryIcon;
    break;
  }
  if (text != NULL)
    StrNCopy(cbP->textP, text, cbP->textLen);
  else
    cbP->textP[0] = 0;
  if (cbP->message != NULL)
    StrNCat(cbP->textP, cbP->message, cbP->textLen);
  cbP->bitmapId = icon;
  return true;
}

static void SwitchStage(e_Stage stage, char *message)
{
  mStage = stage;
  PrgUpdateDialog(mProgress,
                  (stage == STAGEFAILED || stage == STAGERESULT) ? 1 : 0,
                  stage, message, true);
}

extern BOOL AccelActive()
{
  return mProgress != NULL;
}

extern void AccelInit()
{
  ReadSetActivePids(1L<<(32 - 0x0D)); // speed only
}

extern void AccelStartTest()
{
  mProgress = PrgStartDialogV31("Acceleration Timer", ProgressCallback);
  EnableProgress(mProgress);
  SwitchStage(STAGEINIT, NULL);
  ReadOpen();
}

extern void AccelStopTest()
{
  if (mProgress != NULL) {
    PrgStopDialog(mProgress, false);
    mProgress = NULL;
    EnableProgress(NULL);
    SensorGotoForm();
  }
}

extern void AccelFailed()
{
  if (mProgress != NULL && mStage != STAGEINIT && mStage != STAGERESULT) {
    SwitchStage(STAGEFAILED, NULL);
    SndPlaySystemSound(sndInfo);
    AccelStopTest();
  }
}

extern void AccelReceiveElmObd(UInt8 *message, int length)
{
  int pid;
  int speed;
  static char mMessage[64];
  if (!mProgress || length == 0)
    return;
  switch (message[0]) {
  case 0x41:
    if (length < 3) return;
    pid = message[1];
    switch (pid) {
    case 0x0d: // vehicle speed
      mReadings++;
      if (mPrefs.Units == UNITS_SI)
        speed = message[2];
      else
        speed = message[2] * 62 / 100;
      switch (mStage) {
      case STAGEINIT:
        SwitchStage(speed == 0 ? STAGEGO : STAGESTOP, NULL);
        break;
      case STAGESTOP:
        if (speed == 0) {
          SwitchStage(STAGEGO, NULL);
          SndPlaySystemSound(sndInfo);
        }
        break;
      case STAGEGO:
        if (speed > 0) {
          mTimerTicks = TimGetTicks();
          mReadings = 0;
          SwitchStage(STAGETIME, NULL);
          SndPlaySystemSound(sndInfo);
        }
        break;
      case STAGETIME:
        if (speed == 0) {
          SwitchStage(STAGEGO, NULL);
          SndPlaySystemSound(sndInfo);
        } else if (speed < (mPrefs.Units == UNITS_SI ? 100 : 60)) {
          int ticks, secs, huns;
          ticks = TimGetTicks() - mTimerTicks;
          secs = ticks / SysTicksPerSecond();
          huns = (ticks % SysTicksPerSecond())*100/SysTicksPerSecond();
          StrPrintF(mMessage, mPrefs.Units == UNITS_SI ?
                    "Speed:\240%d\240km/h Time:\240%d.%2d\240seconds" :
                    "Speed:\240%d\240mph Time:\240%d.%2d\240seconds",
                    speed, secs, huns);
          SwitchStage(STAGETIME, mMessage);
        } else {
          int ticks, secs, huns, errsecs, errhuns;
          ticks = TimGetTicks() - mTimerTicks;
          secs = ticks / SysTicksPerSecond();
          huns = (ticks % SysTicksPerSecond())*100/SysTicksPerSecond();
          ticks = ticks / mReadings;
          errsecs = ticks / SysTicksPerSecond();
          errhuns = (ticks % SysTicksPerSecond())*100/SysTicksPerSecond();
          StrPrintF(mMessage,
             "\240%d.%2d\240seconds (\261%d.%2d\240seconds)",
                    secs, huns, errsecs, errhuns);
          SwitchStage(STAGERESULT, mMessage);
          SndPlaySystemSound(sndInfo);
          AccelStopTest();
        }
        break;
      default:
      }
      break;
    }
    break;
  }
}
