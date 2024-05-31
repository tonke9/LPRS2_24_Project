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
#if HANDSPRING
#include <HsExt.h>
#endif
#include "OBDGauge.h"
#include "OBDProto.h"

static LocalID mLocalID = 0;
static ProgressPtr mProgress = NULL;
UInt32 mScreenDepth;
UInt32 mRomVersion;

extern void EnableProgress(ProgressPtr progress)
{
  mProgress = progress;
}

static int GetHardKeys(EventType* pEvent)
{
  UInt16 formID;
  if (pEvent->eType != keyDownEvent)
    return false;
  formID = FrmGetActiveFormID();
  if (formID < Sensor0Form || formID > Sensor3Form)
    return false;
  switch (pEvent->data.keyDown.chr) {
  case hard1Chr:
  case hard2Chr:
  case hard3Chr:
  case hard4Chr:
    SensorPage(pEvent->data.keyDown.chr - hard1Chr);
    return true;
  default:
  }
  return false;
}

static void RegisterNotifications(void)
{
  if (mRomVersion >= 0x03100000)
    if (mLocalID) {
      SysNotifyRegister(0, mLocalID, sysNotifyLateWakeupEvent, NULL,
                        sysNotifyNormalPriority, NULL);
      SysNotifyRegister(0, mLocalID, sysNotifySleepRequestEvent, NULL,
                        sysNotifyNormalPriority, NULL);
    }
}

static void UnregisterNotifications(void)
{
  if (mRomVersion >= 0x03100000)
    if (mLocalID) {
      SysNotifyUnregister(0, mLocalID, sysNotifyLateWakeupEvent,
                          sysNotifyNormalPriority);
      SysNotifyUnregister(0, mLocalID, sysNotifySleepRequestEvent,
                          sysNotifyNormalPriority);
    }
}

extern Boolean AboutFormHandleEvent(EventPtr eventP)
{
  Boolean handled = false;
  FormType *frmP = FrmGetActiveForm();
  switch (eventP->eType) {
  case frmOpenEvent:
    FrmDrawForm(frmP);
    handled = true;
    break;
  case ctlSelectEvent:
    switch (eventP->data.ctlEnter.controlID) {
    case AboutOkButton:
      FrmReturnToForm(0);
      handled = true;
      break;
    default:
      break;
    }
    break;
  default:
  }
  return handled;
}

static Boolean AppHandleEvent(EventPtr eventP)
{
  Boolean handled = false;
  FormType *frmP;
  UInt16 formID;

  switch (eventP->eType) {
  case frmLoadEvent:
    formID = eventP->data.frmLoad.formID;
    if (formID != FrmGetActiveFormID()) {
      frmP = FrmInitForm(formID);
      FrmSetActiveForm(frmP);
    } else {
      frmP = FrmGetActiveForm();
    }
    switch (formID) {
    case Sensor0Form:
    case Sensor1Form:
    case Sensor2Form:
    case Sensor3Form:
      FrmSetEventHandler(frmP, SensorFormHandleEvent);
      handled = true;
      break;
    case CodesForm:
      FrmSetEventHandler(frmP, CodesFormHandleEvent);
      handled = true;
      break;
    case PrefsForm:
      FrmSetEventHandler(frmP, PrefsFormHandleEvent);
      handled = true;
      break;
    case PrefsElmForm:
      FrmSetEventHandler(frmP, PrefsElmFormHandleEvent);
      handled = true;
      break;
    case PrefsMultiplexForm:
      FrmSetEventHandler(frmP, PrefsMultiplexFormHandleEvent);
      handled = true;
      break;
    case AboutForm:
      FrmSetEventHandler(frmP, AboutFormHandleEvent);
      handled = true;
      break;
    case WaitForm:
      FrmSetEventHandler(frmP, WaitFormHandleEvent);

      handled = true;
      break;
    }
    break;
  case ctlSelectEvent:
    break;
  case appStopEvent:
    FrmCloseAllForms();
    handled = true;
    break;
  default:
    break;
  }     
  return handled;
}

static int HandleProgress(EventType* pEvent)
{
  if (!mProgress)
    return false;
  if (!PrgHandleEvent(mProgress, pEvent))
    if (mProgress && PrgUserCancel(mProgress))
      AccelStopTest();
  return true;
}       

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
  switch (cmd) {
  case sysAppLaunchCmdNormalLaunch:
    {
      EventType event;
      UInt16 MenuError;
#if HANDSPRING
      UInt32 hsVersion;
      // fix for Handspring Visor to avoid serial open error
      // from http://www.markspace.com/support/faq_datacord.html
      if (!FtrGet ('hsEx', 0, &hsVersion))
        HsExtKeyboardEnable(false);
#endif
      FtrGet(sysFtrCreator, sysFtrNumROMVersion, &mRomVersion);
      if (mRomVersion >= 0x03503000) {
        WinScreenMode(winScreenModeGet, NULL, NULL, &mScreenDepth, NULL);
        if (mScreenDepth == 1) {
          mScreenDepth =  2; // set to grayscale mode
          WinScreenMode(winScreenModeSet, NULL, NULL, &mScreenDepth, NULL);
        }
      }
      mLocalID = DmFindDatabase(0, "OBD Gauge");
      RegisterNotifications();
      PrefsLoadPrefs();
      SensorLoadPrefs();
      SensorLoadReadings();
      SensorGotoForm();
      do {
        // older Palm need a breather to process UI events
        if (mRomVersion < 0x04003000)
          EvtGetEvent(&event, 1);
        else
          EvtGetEvent(&event, 0);
        if (event.eType == nilEvent) {
            SensorHandleTick();
            ReadHandleTick();
        } else {
          if (! HandleProgress(&event))
            if (! GetHardKeys(&event))
              if (! SysHandleEvent(&event))
                if (! MenuHandleEvent(0, &event, &MenuError))
                  if (! AppHandleEvent(&event))
                    FrmDispatchEvent(&event);
        }
      } while (event.eType != appStopEvent);
      ReadClose();
      SensorSaveReadings();
      SensorSavePrefs();
      PrefsSavePrefs();
      UnregisterNotifications();
    }
    break;
  case sysAppLaunchCmdNotify:
    {
      SysNotifyParamType* notify;
      notify = (SysNotifyParamType *)cmdPBP;
      switch (notify->notifyType) {
      case sysNotifyLateWakeupEvent:
        if (PrefGetPreference(prefDeviceLocked)) {
          if (mLocalID)
            SysNotifyRegister(0, mLocalID,
                              sysNotifyDeviceUnlocked, NULL,
                              sysNotifyNormalPriority, NULL);
          break;
        }
        // fallthrough
      case sysNotifyDeviceUnlocked:
        ReadPowerUp();
        break;
      case sysNotifySleepRequestEvent:
        ReadPowerDown();
        break;
      default:
      }
    }
    break;
  default:
  }
  return 0;
}
