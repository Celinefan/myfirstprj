// CharCutFuc.h : interface of CharCut Function
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(SCU_III_CHARCUTFUC_INCLUDED_)
#define SCU_III_CHARCUTFUC_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMGFUC.h"
#include "PlateRecFuc.h"

BOOL TackleCongluatination( ObjRectArray &rects, IMAGE imgGray, int nh, int nw );
BOOL TackleCong_ByConf( ObjRectArray &rects, ObjRectArray &allrcs, IMAGE imgGray, int nh, int nw );
BOOL TackleCongluatination( ObjRectArray &rects, ObjRectArray &allrcs, IMAGE imgGray, int nh, int nw );
BOOL TackleVerCongluatination( ObjRectArray &rects, IMAGE imgGray, int nh, int nw );
BOOL TackleCongluatinationForTopView( ObjRectArray &rects, IMAGE imgGray, int nh, int nw );
BOOL ReCutRgn( CRect rcRgn, IMAGE imgGray, int nh, int nw, ObjRectArray &rcChars );
BOOL SearchObjInRgn( IMAGE imgReCut_Bin, int nh, int nw, ObjRectArray &rcObj );
BOOL ReCutVerRgn(CRect rcRgn, IMAGE imgGray, int nh, int nw, ObjRectArray &rcChars );
BOOL ReCutRgnForTopView( CRect rcRgn, IMAGE imgGray, int nh, int nw, ObjRectArray &rcChars );

BOOL MergePolicy( ObjRectArray& charArray, ObjRectArray& allRcsArray, int w, int h, int nRcDis );
BOOL EsRect( ObjRectArray& rcAnaRcsArray, int w, int h, CRect& rcMerge );

BOOL SetCutCharsDebugInfoPath( CString& strDebugDir, CString& strDebugFname );
#endif // !defined(SCU_III_CHARCUTFUC_INCLUDED_)