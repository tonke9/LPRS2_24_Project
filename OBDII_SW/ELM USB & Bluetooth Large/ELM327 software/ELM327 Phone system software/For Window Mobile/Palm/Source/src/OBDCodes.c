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

#define DTCINDEXMAX 15
#define DTCTYPEACTIVE 0
#define DTCTYPEPENDING 1
#define DTCTYPEMAX 2

static UInt16 mDtcCode[DTCTYPEMAX][DTCINDEXMAX];
static char mDtcText[DTCTYPEMAX][DTCINDEXMAX][6];
static char *mDtcList[DTCTYPEMAX][DTCINDEXMAX];
static int mDtcCount[DTCTYPEMAX];
static BOOL mMIL = false;
static DmOpenRef dbHandle = 0;

static void SetDescriptionText(char *text)
{
  FieldPtr txtP;
  MemHandle txtH, oldTxtH;
  char *s;
  txtP = GetObjPtr(CodesDescriptionText);
  oldTxtH = FldGetTextHandle(txtP);
  txtH = MemHandleNew(StrLen(text) + 1);
  s = MemHandleLock(txtH);
  StrCopy(s, text);
  MemHandleUnlock(txtH);
  FldSetTextHandle(txtP, txtH);
  if (oldTxtH != NULL)
    MemHandleFree(oldTxtH);
  FldDrawField(txtP);
}

static void GetDescription(UInt16 dtc)
{
  UInt16 index;
  MemHandle recHandle;
  UInt32 recSize;
  char *recPointer;
  char dtcText[120];
  if (dbHandle == 0)
    dbHandle = DmOpenDatabaseByTypeCreator('dtc1', 'obdg', dmModeReadOnly);
  if (dbHandle == 0)
    SetDescriptionText("Please load trouble code database on Palm using"
                       " HotSync to display the trouble code description.");
  else if (DmFindRecordByID(dbHandle, dtc, &index) == 0) {
    recHandle = DmQueryRecord(dbHandle, index);
    recSize = MemHandleSize(recHandle);
    recPointer = MemHandleLock(recHandle);
  if (mRomVersion >= 0x02003000)
    StrNCopy(dtcText, recPointer, sizeof(dtcText));
  else
    StrCopy(dtcText, recPointer);
    MemHandleUnlock(recHandle);
    SetDescriptionText(dtcText);
  } else
    SetDescriptionText("Please see appropriate service manual"
                       " for manufacturer defined trouble codes.");
}

static void ClearLists()
{
  MemSet(&mDtcCount, sizeof(mDtcCount), 0);
  mMIL = false;
  CodesReadComplete();
}

extern Boolean CodesFormHandleEvent(EventPtr eventP)
{
  Boolean handled = false;
  FormType *frmP = FrmGetActiveForm();
  switch (eventP->eType) {
  case frmOpenEvent:
    FrmDrawForm(frmP);
        // disable on/off buttons programmatically
        // because Palm Development Suite does not allow design time disable
    CtlSetEnabled(GetObjPtr(CodesMILOnButton), false);
    CtlSetEnabled(GetObjPtr(CodesMILOffButton), false);
    ClearLists();
    DisplayWaitForm(NULL);
    ReadReadCodes();
    handled = true;
    break;
  case frmCloseEvent:
    if (dbHandle != 0)
      DmCloseDatabase(dbHandle);
    break;
  case ctlSelectEvent:
    switch (eventP->data.ctlEnter.controlID) {
    case CodesClearButton:
      if (FrmAlert(ClearCodesAlert) == 0) {
        ClearLists();
        DisplayWaitForm(NULL);
        ReadClearCodes();
      }
      handled = true;
      break;
    case CodesOkButton:
      if (dbHandle != 0)
        DmCloseDatabase(dbHandle);
      FrmReturnToForm(0);
      ReadRestart();
      handled = true;
      break;
    default:
      break;
    }
    break;
  case lstSelectEvent:
    switch (eventP->data.ctlEnter.controlID) {
    case CodesActiveList:
      LstSetSelection(GetObjPtr(CodesPendingList), -1);
      GetDescription(mDtcCode[DTCTYPEACTIVE]
                     [eventP->data.lstSelect.selection]);
      break;
    case CodesPendingList:
      LstSetSelection(GetObjPtr(CodesActiveList), -1);
      GetDescription(mDtcCode[DTCTYPEPENDING]
                     [eventP->data.lstSelect.selection]);
      break;
    }
    break;
  default:
    break;
  }
  return handled;
}

extern void CodesSetMIL(BOOL MIL)
{
  mMIL = MIL;
}

extern void CodesSetClearCodes(void)
{

}

extern void CodesReceiveElmObd(UInt8 *message, int length)
{
  static char *prefix = "PCBU";
  static char *code = "0123456789ABCDEF";
  int dtcType, i;
  UInt16 dtcCode;
  char *dtcText;
  if (length < 7)
    return;
  if (message[0] == 0x43)
    dtcType = DTCTYPEACTIVE;
  else if (message[0] == 0x47)
    dtcType = DTCTYPEPENDING;
  else
    return;
  for (i = 0; i < 3; i++)
    if (mDtcCount[dtcType] < DTCINDEXMAX) {
      dtcCode = (message[i * 2 + 1] << 8) | message[i * 2 + 2];
      if (dtcCode != 0) {
        mDtcCode[dtcType][mDtcCount[dtcType]] = dtcCode;
        dtcText = mDtcText[dtcType][mDtcCount[dtcType]];
        mDtcList[dtcType][mDtcCount[dtcType]] = dtcText;
        *dtcText++ = prefix[(dtcCode >> 14) & 0x03];
        *dtcText++ = code[(dtcCode >> 12) & 0x03];
        *dtcText++ = code[(dtcCode >> 8) & 0x0f];
        *dtcText++ = code[(dtcCode >> 4) & 0x0f];
        *dtcText++ = code[dtcCode & 0x0f];
        *dtcText = 0;
        mDtcCount[dtcType]++;
      }
    }
}

extern void CodesReadComplete(void)
{
  ControlType *ctl;
  ListType *lst;
  if (CodesForm != FrmGetActiveFormID())
    FrmReturnToForm(CodesForm);
  ctl = GetObjPtr(CodesMILOnButton);
  CtlSetValue(ctl, mMIL);
  ctl = GetObjPtr(CodesMILOffButton);
  CtlSetValue(ctl, !mMIL);
  lst = GetObjPtr(CodesActiveList);
  LstSetListChoices(lst, mDtcList[DTCTYPEACTIVE], mDtcCount[DTCTYPEACTIVE]);
  if (mDtcCount[DTCTYPEACTIVE] > 0) {
    LstSetSelection(lst, 0);
    GetDescription(mDtcCode[DTCTYPEACTIVE][0]);
  } else
    LstSetSelection(lst, -1);
  LstDrawList(lst);
  lst = GetObjPtr(CodesPendingList);
  LstSetListChoices(lst, mDtcList[DTCTYPEPENDING], mDtcCount[DTCTYPEPENDING]);
  LstSetSelection(lst, -1);
  LstDrawList(lst);
}
