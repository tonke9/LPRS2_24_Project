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

Prefs_s mPrefs;

static char *mLanguage[] = { "English", "French", "German", "Dutch", "Spanish" };
static char *mDisplay[] = {
  "Show both", "Hide menu bar", "Hide page numbers", "Hide both" };
static char *mUnits[] = { "Metric", "U.S.", "U.K." };
static char *mQueries[] = { "All sensors", "Displayed sensors" };
static char *mGraphTypes[] = { "Line", "Bar" };
static char *mInterfaces[] = { "ELM", "Multiplex" };
static char *mBaud[] = { "19200", "9600" };
static char *mAddress[255];
static char mAddressText[255][3];
static char *mProtocol[] = { "Disabled", "ISO", "VPW", "PWM", "KWP" };
static char *mTimeout[] = {
  "200 ms", "175 ms", "150 ms", "125 ms", "100 ms", "75 ms", "50 ms", "25 ms" };

extern void PrefsSavePrefs()
{
  if (mRomVersion >= 0x02003000)
    PrefSetAppPreferences('obdg', 0, 1, &mPrefs, sizeof(mPrefs), false);
}

extern void PrefsLoadPrefs()
{
  UInt16 prefSize = sizeof(Prefs_s);
  MemSet(&mPrefs, prefSize, 0);
  if (mRomVersion >= 0x02003000)
    PrefGetAppPreferences('obdg', 0, &mPrefs, &prefSize, false);
}

static void SetPopup(int listObjID, int popupObjID,
                     char **list, int listSize, int selection)
{
  ControlPtr ctlP;
  ListType* lstP = GetObjPtr(listObjID);
  LstSetListChoices(lstP, list, listSize);
  LstSetSelection(lstP, selection);
  LstMakeItemVisible(lstP, selection);
  ctlP = (ControlPtr)GetObjPtr(popupObjID);
  CtlSetLabel(ctlP, list[selection]);
}

extern Boolean PrefsFormHandleEvent(EventPtr eventP)
{
  Boolean handled = false;
  FormType *frmP = FrmGetActiveForm();
  int i;

  switch (eventP->eType) {
  case frmOpenEvent:
    FrmDrawForm(frmP);
    SetPopup(PrefsLanguageList, PrefsLanguagePopup, mLanguage,
             sizeof(mLanguage)/sizeof(mLanguage[0]), mPrefs.Language);
    SetPopup(PrefsDisplayList, PrefsDisplayPopup, mDisplay,
             sizeof(mDisplay)/sizeof(mDisplay[0]),
             (mPrefs.HidePage ? 2 : 0) | (mPrefs.HideMenu ? 1 : 0));
    SetPopup(PrefsUnitsList, PrefsUnitsPopup, mUnits,
             sizeof(mUnits)/sizeof(mUnits[0]), mPrefs.Units);
    SetPopup(PrefsQueryList, PrefsQueryPopup, mQueries,
             sizeof(mQueries)/sizeof(mQueries[0]), mPrefs.Query);
    SetPopup(PrefsGraphTypeList, PrefsGraphTypePopup, mGraphTypes,
             sizeof(mGraphTypes)/sizeof(mGraphTypes[0]), mPrefs.GraphType);
    SetPopup(PrefsInterfaceList, PrefsInterfacePopup, mInterfaces,
             sizeof(mInterfaces)/sizeof(mInterfaces[0]), mPrefs.Interface);
    handled = true;
    break;
  case ctlSelectEvent:
    switch (eventP->data.ctlEnter.controlID) {
    case PrefsOkButton:
      mPrefs.Language = LstGetSelection(GetObjPtr(PrefsLanguageList));
      mPrefs.Units = LstGetSelection(GetObjPtr(PrefsUnitsList));
      mPrefs.Query = LstGetSelection(GetObjPtr(PrefsQueryList));
      mPrefs.GraphType = LstGetSelection(GetObjPtr(PrefsGraphTypeList));
      mPrefs.Interface = LstGetSelection(GetObjPtr(PrefsInterfaceList));
      mPrefs.HidePage = (LstGetSelection(GetObjPtr(PrefsDisplayList)) & 2) != 0;
      mPrefs.HideMenu = (LstGetSelection(GetObjPtr(PrefsDisplayList)) & 1) != 0;
      SensorPrefsChanged();
      FrmReturnToForm(0);
      handled = true;
      break;
    case PrefsCancelButton:
      SensorPrefsChanged();
      FrmReturnToForm(0);
      handled = true;
      break;
    case PrefsSetupButton:
      switch (LstGetSelection(GetObjPtr(PrefsInterfaceList))) {
      case INTERFACE_ELM:
        FrmPopupForm(PrefsElmForm);
        break;
      case INTERFACE_MULTIPLEX:
        FrmPopupForm(PrefsMultiplexForm);
        break;
      }
      handled = true;
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  return handled;
}

extern Boolean PrefsElmFormHandleEvent(EventPtr eventP)
{
  Boolean handled = false;
  FormType *frmP = FrmGetActiveForm();
  int i;

  switch (eventP->eType) {
  case frmOpenEvent:
    FrmDrawForm(frmP);
    SetPopup(PrefsTimeoutList, PrefsTimeoutPopup, mTimeout,
             sizeof(mTimeout)/sizeof(mTimeout[0]), mPrefs.Timeout);
    handled = true;
    break;
  case ctlSelectEvent:
    switch (eventP->data.ctlEnter.controlID) {
    case PrefsOkButton:
      mPrefs.Timeout = LstGetSelection(GetObjPtr(PrefsTimeoutList));
      FrmReturnToForm(0);
      handled = true;
      break;
    case PrefsCancelButton:
      FrmReturnToForm(0);
      handled = true;
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  return handled;
}

extern Boolean PrefsMultiplexFormHandleEvent(EventPtr eventP)
{
  static char *hex = "0123456789ABCDEF";
  Boolean handled = false;
  FormType *frmP = FrmGetActiveForm();
  int i;

  switch (eventP->eType) {
  case frmOpenEvent:
    FrmDrawForm(frmP);
    for (i = 0; i < 255; i++) {
      mAddress[i] = &mAddressText[i][0];
      mAddressText[i][0] = hex[(i+1) >> 4];
      mAddressText[i][1] = hex[(i+1) & 0xf];
      mAddressText[i][2] = 0;
    }
    SetPopup(PrefsBaudList, PrefsBaudPopup, mBaud,
             sizeof(mBaud)/sizeof(mBaud[0]), mPrefs.Baud);
    SetPopup(PrefsAddressList, PrefsAddressPopup, mAddress,
             sizeof(mAddress)/sizeof(mAddress[0]),
             mPrefs.Address == 0 ? 0x24 : mPrefs.Address - 1);
    SetPopup(PrefsProtocolList, PrefsProtocolPopup, mProtocol,
             sizeof(mProtocol)/sizeof(mProtocol[0]), mPrefs.Protocol);
    handled = true;
    break;
  case ctlSelectEvent:
    switch (eventP->data.ctlEnter.controlID) {
    case PrefsOkButton:
      mPrefs.Baud = LstGetSelection(GetObjPtr(PrefsBaudList));
      mPrefs.Address = LstGetSelection(GetObjPtr(PrefsAddressList)) + 1;
      mPrefs.Protocol = LstGetSelection(GetObjPtr(PrefsProtocolList));
      FrmReturnToForm(0);
      handled = true;
      break;
    case PrefsCancelButton:
      FrmReturnToForm(0);
      handled = true;
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  return handled;
}
