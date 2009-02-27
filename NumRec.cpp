// NumRec.cpp: implementation of the CNumRec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NumRec.h"

#include "ConNumProc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConNumProc g_NumProc;

CNumRec::CNumRec()
{

}

CNumRec::~CNumRec()
{

}

BOOL CNumRec::SetProcFileString( CString& strFilePath )
{
	return g_NumProc.SetProcFileString( strFilePath );
}

CString CNumRec::RecNum( CDIBitmap* pImage )
{
	g_NumProc.m_pImage = pImage;
	CString strRes = g_NumProc.RecNum();
	return strRes;
	//return g_NumProc.RecNum();
}

CString CNumRec::RecNum( CDIBitmap* pImage, BOOL &bJudgeRes )
{
	CString strRes = RecNum( pImage );

	if( strRes >= 11 )
	{
		bJudgeRes = g_NumProc.JudgeResString( strRes );
	}
	else
	{
		bJudgeRes = FALSE;
	}

	return strRes;
}

CString CNumRec::RecTopNum( CDIBitmap* pImage )
{
	g_NumProc.m_pImage = pImage;
	CString strRes = g_NumProc.RecTopViewNum();
	return strRes;
	//return g_NumProc.RecNum();
}

CString CNumRec::RecTopNum( CDIBitmap* pImage, BOOL &bJudgeRes )
{
	CString strRes = RecTopNum( pImage );

	if( strRes >= 11 )
	{
		bJudgeRes = g_NumProc.JudgeResString( strRes );
	}
	else
	{
		bJudgeRes = FALSE;
	}

	return strRes;
}

BOOL CNumRec::GetPosInfo( CRect& rgnABCSeg, CRect& rgnNumSeg, CString& strABCSeg, CString& strNumSeg )
{
	rgnABCSeg = g_NumProc.m_rgnABCSeg;
	rgnNumSeg = g_NumProc.m_rgnNumSeg;
	strNumSeg = g_NumProc.m_strNumSeg;
	strABCSeg = g_NumProc.m_strABCSeg;

	return TRUE;
}

BOOL CNumRec::GetPreNumRects( IMAGE imgGray, ObjRectArray& rcArray )
{
	return g_NumProc.GetPreNumRects(imgGray,rcArray);
}

BOOL CNumRec::GetPreJPNumRects( IMAGE imgGray, ObjRectArray& rcArray )
{
	return g_NumProc.GetPreJPNumRects(imgGray,rcArray);
}

BOOL CNumRec::GetPreVerNumRects( IMAGE imgGray, ObjRectArray& rcArray )
{
	return g_NumProc.GetPreVerNumRects( imgGray, rcArray );
}

BOOL CNumRec::RecNumArray( IMAGE ImgGray, ObjRectArray &charArray, CString& resStr)
{
	return g_NumProc.RecCharArray( ImgGray, charArray, resStr, TRUE );
}

BOOL CNumRec::RecABCArray( IMAGE ImgGray, ObjRectArray &charArray, CString& resStr)
{
	return g_NumProc.RecCharArray( ImgGray, charArray, resStr, FALSE );
}

BOOL CNumRec::GetTheCheckRect(CRect& rcCheck, IMAGE imgGray, int nCharH, int nCharW)
{
	return g_NumProc.GetTheCheckRect( rcCheck, imgGray, nCharH, nCharW);
}

BOOL CNumRec::JudgeResString( CString strRes )
{
	CString strTest = strRes;
	return g_NumProc.JudgeResString(strRes);
}