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
#include "OBDProto.h"

extern void * GetObjPtr(int objID)
{
  FormType *frmP = FrmGetActiveForm();
  return FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, objID));
}

extern void ShowObj(int objID, BOOL show)
{
  FormType *frmP = FrmGetActiveForm();
  if (show)
    FrmShowObject(frmP, FrmGetObjectIndex(frmP, objID));
  else
    FrmHideObject(frmP, FrmGetObjectIndex(frmP, objID));
}

extern void GetObjBounds(int objID, RectangleType *bounds)
{
  FormType *frmP = FrmGetActiveForm();
  FrmGetObjectBounds(frmP, FrmGetObjectIndex(frmP, objID), bounds);
}

extern void SetObjBounds(int objID, RectangleType *bounds)
{
  FormType *frmP = FrmGetActiveForm();
  FrmSetObjectBounds(frmP, FrmGetObjectIndex(frmP, objID), bounds);
}

extern void DrawBorder(int objID)
{
  RectangleType bounds;
  GetObjBounds(objID, &bounds);
  WinDrawRectangleFrame(simpleFrame, &bounds);
}

extern void EraseDrawBorder(int objID)
{
  RectangleType bounds;
  GetObjBounds(objID, &bounds);
  WinEraseRectangle(&bounds, 0);
  WinDrawRectangleFrame(simpleFrame, &bounds);
}

extern void EraseObj(int objID)
{
  RectangleType bounds;
  GetObjBounds(objID, &bounds);
  WinEraseRectangle(&bounds, 0);
}

extern BOOL GetCtlValue(int objID)
{
  return CtlGetValue(GetObjPtr(objID)) != 0;
}

