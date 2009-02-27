// ConNumRecDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#include "NumRec_C.h"
#include "NumRec.h"
#include "DongleCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE ConNumRecDllDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("CONNUMRECDLL.DLL Initializing!\n");

#ifdef CHECK_DOG
		if( !CheckDog() ) return 0;
#endif
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(ConNumRecDllDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(ConNumRecDllDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("CONNUMRECDLL.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(ConNumRecDllDLL);
	}
	return 1;   // ok
}

CNumRec *g_NumRec = NULL;
CDIBitmap g_Image;
int g_width = 0;
int g_height = 0;

extern "C" _declspec(dllexport) int _stdcall RecConNum( LPBYTE pImg, int nW, int nH, LPCONNUMINFO pInfo)
{
	if( g_width != nW || g_height != nH)
	{
		g_Image.Create( nW, nH, 24 );
		g_width = nW;
		g_height = nH;
	}

	int nLen = nW * nH * 3;
	BYTE *pDest = g_Image.GetBitsAddress();

   	memcpy(pDest, pImg, nLen);
	BOOL bJudgeRes = FALSE;
	CString strRes = g_NumRec->RecNum( &g_Image, bJudgeRes );
	strcpy(pInfo->ConNum, strRes.GetBuffer(strRes.GetLength() + 1));
	pInfo->bJudge = bJudgeRes;

	if( strRes != "" && strRes != "error")
	{
		CRect rcSubRgn1;
		CRect rcSubRgn2;
		CString strSubString1;
		CString strSubString2;
		g_NumRec->GetPosInfo(rcSubRgn1,rcSubRgn2,strSubString1,strSubString2);

		pInfo->ABCRegion[0] = rcSubRgn1.left;
		pInfo->ABCRegion[1] = rcSubRgn1.top;
		pInfo->ABCRegion[2] = rcSubRgn1.right;
		pInfo->ABCRegion[3] = rcSubRgn1.bottom;

		pInfo->NumRegion[0] = rcSubRgn2.left;
		pInfo->NumRegion[1] = rcSubRgn2.top;
		pInfo->NumRegion[2] = rcSubRgn2.right;
		pInfo->NumRegion[3] = rcSubRgn2.bottom;
	}
	else
	{
		return 0;
	}
	
	return 1;
}