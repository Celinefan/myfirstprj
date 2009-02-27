// NumRec.h: interface for the CNumRec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUMREC_H__35E92797_38A2_4C9A_9952_748B28281B42__INCLUDED_)
#define AFX_NUMREC_H__35E92797_38A2_4C9A_9952_748B28281B42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CNumRec  
{
public:
	CNumRec();
	virtual ~CNumRec();
	BOOL SetProcFileString( CString& strFilePath );
	CString RecTopNum( CDIBitmap* pImage );
	CString RecTopNum( CDIBitmap* pImage, BOOL &bJudgeRes );
	CString RecNum( CDIBitmap* pImage );
	CString RecNum( CDIBitmap* pImage, BOOL &bJudgeRes );
	BOOL GetPosInfo( CRect& rgnNumSeg, CRect& rgnABCSeg, CString& strNumSeg, CString& strABCSeg );

	BOOL GetPreNumRects( IMAGE imgGray, ObjRectArray& rcArray );
	BOOL GetPreJPNumRects( IMAGE imgGray, ObjRectArray& rcArray );
	BOOL GetPreVerNumRects( IMAGE imgGray, ObjRectArray& rcArray );
	BOOL RecNumArray( IMAGE ImgGray, ObjRectArray &charArray, CString& resStr);
	BOOL RecABCArray( IMAGE ImgGray, ObjRectArray &charArray, CString& resStr);
	BOOL GetTheCheckRect(CRect& rcCheck, IMAGE imgGray, int nCharH, int nCharW);
	BOOL JudgeResString( CString strRes );
};

#endif // !defined(AFX_NUMREC_H__35E92797_38A2_4C9A_9952_748B28281B42__INCLUDED_)
