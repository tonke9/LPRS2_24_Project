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

static void (*mWaitCallback)();

extern void DisplayWaitForm(void (*WaitCallback)())
{
  mWaitCallback = WaitCallback;
  FrmPopupForm(WaitForm);
}

extern Boolean WaitFormHandleEvent(EventPtr eventP)
{
  Boolean handled = false;
  FormType *frmP = FrmGetActiveForm();

  switch (eventP->eType) {
    case frmOpenEvent:
      FrmDrawForm(frmP);
      if (mWaitCallback)
        mWaitCallback();
      handled = true;
      break;
  default:
    break;
  }
  return handled;
}
