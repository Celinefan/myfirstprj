// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1E4E2AE4_E1F2_4A75_9863_CF7B072A83F1__INCLUDED_)
#define AFX_STDAFX_H__1E4E2AE4_E1F2_4A75_9863_CF7B072A83F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include "SCUU.h"
#include "IMGFUC.h"
#include "PlateRecFuc.h"
#include "CharCutFuc.h"
#include "DongleCheck.h"
#include <fstream.h>
#include <io.h>

#include <DataArray.h>


//#define TIME_LIMITED
#define TEST_4331_FORMAT
#define TEST_VER_FORMAT2
#define TEST_CUT_CONF
#define TEST_CUT_CONF_V2
#define TEST_PROJ
//#define TEST_CUT_CONF_V3

#define TEST_HMM//ADDED 20081119 -- FOR HMM CHAR CUT CONF ANA
#ifdef TEST_HMM
#include "BfPropa.h"
#endif
// #define SAVE_HMM_MID_RES//ADDED 20081119 -- FOR TEST_HMM MID RES
// #ifdef SAVE_HMM_MID_RES
// #define SAVE_DEBUGINFO_FILES
// #endif

#define TEST_HMMCONF//ADDED 20090325 -- For HMM Segmentation Confidence Calculation
#define DEBUG_HMMCONF//ADDED 20090326 -- For HMM Segmentation Confidence Debugging
#ifdef DEBUG_HMMCONF
#define SAVE_DEBUGINFO_FILES
#endif

#define ONLY_TEST_HMMCONF


//#define SAVE_DEBUGINFO_FILES
//#define SAVE_PLATEPOS_INFO
//#define SAVE_CUTCHAR_INFO
//#define SAVE_CHARS
//#define SAVE_CHECKCHAR_INFO

//#define SAVE_TC_INFO//Debug TackleCongluatination

//#define SHOW_CHARPOS

//#define ERROR_NEED_MEND
//#define CHECK_ERROR
//#define CHECK_MEM_LEAK
//#define CHECK_DOG
//#define AD_PERFORMANCE

//#define TEST_VERALIGN_ONLY
//#define TEST_VERPLATEPOS_ONLY
//#define TEST_PLATEPOS_ONLY
//#define TEST_HORALIGN_ONLY
//#define TEST_HORALIGNxTH_ONLY



//#ifndef _AFX_NO_OLE_SUPPORT
//#include <afxole.h>         // MFC OLE classes
//#include <afxodlgs.h>       // MFC OLE dialog classes
//#include <afxdisp.h>        // MFC Automation classes
//#endif // _AFX_NO_OLE_SUPPORT
//
//
//#ifndef _AFX_NO_DB_SUPPORT
//#include <afxdb.h>			// MFC ODBC database classes
//#endif // _AFX_NO_DB_SUPPORT
//
//#ifndef _AFX_NO_DAO_SUPPORT
//#include <afxdao.h>			// MFC DAO database classes
//#endif // _AFX_NO_DAO_SUPPORT
//
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1E4E2AE4_E1F2_4A75_9863_CF7B072A83F1__INCLUDED_)
