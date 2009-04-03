// ConNumProc.h: interface for the CConNumProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNUMPROC_H__9C6F791E_289C_4621_A3C9_39FAC07BD61E__INCLUDED_)
#define AFX_CONNUMPROC_H__9C6F791E_289C_4621_A3C9_39FAC07BD61E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Featurefusion.h"

class CConNumProc
{
public:
	CConNumProc();
	virtual ~CConNumProc();

	BOOL ReduceFrame();

	BOOL SetProcFileString( CString& strFilePath );
	CString RecNum();
	CString RecTopViewNum();
	BOOL GetPreNumRects( IMAGE imgGray, ObjRectArray& rcArray, BOOL bBlack = FALSE );
	BOOL GetPreJPNumRects( IMAGE imgGray, ObjRectArray& rcArray, BOOL bBlack = FALSE );
	BOOL GetPreVerNumRects( IMAGE imgGray, ObjRectArray& rcArray );
	BOOL GetPrePosStrategy( IMAGE imgGray, ObjRectArray& rcArray, BOOL bBlack, int nthTry );
	BOOL RecCharArray( IMAGE ImgGray, ObjRectArray &charArray, CString& resStr, BOOL bNums, int nTrys = 0 );
	BOOL RecCharArrayForTopView( IMAGE ImgGray, ObjRectArray &charArray, CString& resStr, BOOL bNums);
	BOOL JudgeResString( CString strRes );
	BOOL GetTheCheckRect( CRect& rcCheck, IMAGE imgGray, int nCharH, int nCharW );
	BOOL GetTheCheckRectForTopView( CRect& rcCheck, IMAGE imgGray, int nCharH, int nCharW );
	
	CDIBitmap* m_pImage;
	CDIBitmap* m_pImageDis;

	CRect m_rgnNumSeg;
	CRect m_rgnABCSeg;

	CString m_strNumSeg;
	CString m_strABCSeg;

	ObjRectArray m_NumCharArray;
	ObjRectArray m_ABCCharArray;

private:
#ifdef TEST_CUT_CONF
	ObjRectArray m_NumCharArray_ByConf;
	ObjRectArray m_ABCCharArray_ByConf;
	float m_fCutConf;
#endif
	
#ifdef CHECK_DOG
	int m_bCheckDog;
#endif

#ifdef CHECK_MEM_LEAK
	int nAllocTimes;
	int nNewTimes;

	IMAGE ImageAlloc( int width, int height )
	{
		nAllocTimes++;
		return ::ImageAlloc(width,height);
	};

	void ImageFree( IMAGE img )
	{
		nAllocTimes--;
		::ImageFree( img );
		img = NULL;
	};

	int GetLeakCnt()
	{
		return nAllocTimes;
	};

	int GetNewTimes()
	{
		return nNewTimes;
	};
#endif

	CString m_strDebugDir;
	CString m_strSubDebugDir;
	CString m_strCurDebugDir;
	CString m_strCurDebugFile;
	fstream m_CurInfoFile;
#ifdef SAVE_HMM_MID_RES
	CString m_strHMMDebugDir;
#endif

	int m_nCurProcID;
	CString m_strWholePath;
	ObjRectArray m_RectArray;
	BOOL m_bFindNumSeg;

	CECongnize m_ecoConChar;

	void InitParas();
	void InitParasForDebug();
	BOOL TryGetPreVerRgns( IMAGE imgGray, ObjRectArray& rcArray, int nTimes );
	BOOL RectsReDraw(IMAGE imgGray, ObjRectArray& charArray, int nCharH, int nCharW, int nRcDis );
	BOOL TopRectsAnalyze( ObjRectArray &charArray );
	BOOL HorRgnRcsAnaStrategy( ObjRectArray& charArray, ObjRectArray& allRcsArray, ObjRectArray& resArray, int& nType, int nCharW, int nCharH, int nRcDis, CRect rcNumRgn, BOOL bBlack );
	BOOL RectsAnalyze( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray);
#ifdef TEST_CUT_CONF
	BOOL RectsAnalyze_ByProb( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray, CArray<int,int>& disArray);
	BOOL PatternAnalyze_ByConf( CArray<int,int>& intArray, CArray<int,int>& disArray, int& nType, ObjRectArray& charArray );
	int GetNumSeqType_ByConf( CArray<int,int>& intArray, CArray<int,int>& disArray, int &nDrop );
#endif
	BOOL RectsAnalyzeForTopView( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray );
	BOOL MergeApartRcs( ObjRectArray& charArray, ObjRectArray& allRcsArray, int w, int h, int nRcDis );
	BOOL MergeApartRcsByScan( ObjRectArray& allRcsArray, ObjRectArray& resArray, int w, int h );
	BOOL MerVerApartRcs( ObjRectArray& charArray, ObjRectArray& allRcsArray, int w, int h );
	BOOL MergeSplittedChars( ObjRectArray& charArray, ObjRectArray& allRcsArray, int w, int h, int nRcDis );//Michael Add 2008-04-16
	//BOOL EsRect( ObjRectArray& rcAnaRcsArray, int w, int h, CRect& rcMerge );
	BOOL RectsAnalyzeByScan( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray);
	BOOL RectsVerAnalyze( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray);
	BOOL RectsMulAnalyze( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray );	
	BOOL RemoveABCRgnNoise( ObjRectArray &charArray, ObjRectArray &allRcsArray );
	BOOL DigOutSeq( ObjRectArray &charArray, ObjRectArray &subSeqArray, ObjRectArray &leftCharArray );
	int GetNumSeqType( CArray<int,int>& intArray );
	int GetNumSeqType( CArray<int,int>& intArray, int &nDrop );
	int GetNumSeqTypeForTopView( CArray<int,int>& intArray );
	int GetMulNumSeqType( CArray<int,int>& intArray );
	BOOL PatternAnalyze( CArray<int,int>& intArray, int& nType, ObjRectArray& charArray );
#ifdef TEST_HMM
	BOOL PatternAnalyze_ByHMM( CArray<int,int>& intArray, CArray<int,int>& disArray, int& nType, ObjRectArray& charArray );//Michael Add 20071120
#endif
#ifdef TEST_CUT_CONF
	BOOL PatternAnalyze_ByConf( CArray<int,int>& intArray, int& nType, ObjRectArray& charArray );
	int GetNumSeqType_ByConf( CArray<int,int>& intArray, int &nDrop );
	BOOL ABCRgnAna_ByConf( ObjRectArray& charArray );
#endif
	BOOL CheckAbnorm( ObjRectArray& charArrayAna );
	BOOL GetMisPosForWhole( CArray<int,int>& intAnaArray, int &nMissPos, float &fConf );
	BOOL SetMemCharArrayByConf( ObjRectArray& charArrayAna, int nMissPos );

	BOOL GetABCSeqRgn(CRect &rcABCRgn, CRect rcNumRgn, ObjRectArray& NumCharArray, int nPicWidth, int nPicHeight );
	BOOL GetLABCSeqRgn(CRect &rcABCRgn, CRect rcNumRgn, ObjRectArray& NumCharArray, int nPicWidth, int nPicHeight );
	BOOL GetABCSeqRgnForV2(CRect &rcABCRgn, ObjRectArray& NumCharArray, int nPicWidth, int nPicHeight );//For Vertically Aligned Format 2
	BOOL CutABCRgn( IMAGE imgGrayOrg, CRect rcABCRgn, BOOL bBlack, int nCharW, int nCharH, int nCharDis );
	BOOL CutABCRgn( IMAGE imgGrayOrg, CRect rcABCRgn, BOOL bBlack  );
	BOOL CutABCRgnByLLT( IMAGE imgGrayOrg, CRect rcABCRgn, BOOL bBlack  );
	BOOL CutABCRgnByLLT( IMAGE imgGrayOrg, CRect rcABCRgn, BOOL bBlack, int nCharW, int nCharH, int nCharDis );
	
	BOOL EliminCheckCharFrame(IMAGE imgReCut_Bin);

	BOOL ReDrawRect( IMAGE imgGray, CRect& rcChar );//Michael Add 2008-04-19
	BOOL ReDrawRectByTopValueBin( IMAGE imgGray, CRect& rcChar, float fTRatio );//Michael Add 2008-05-06
	BOOL TopValueBin( IMAGE imgGray, IMAGE imgBin, CRect rcChar, float fTopRatio );//Michael Add 2008-05-06

	BOOL CutForHorRgn( IMAGE imgGrayOrg, BOOL bBlack, BOOL bCutFor3LsOnly = FALSE );
	BOOL CutForChars( CRect rcNumRgn, IMAGE imgGrayOrg, BOOL bBlack );
	BOOL HorRgnCutStrategy( IMAGE imgNumSeq_Gray, IMAGE imgNumSeq, CRect rcNumRgn, ObjRectArray& resArray, int &nType, BOOL bBlack );


	BOOL CopyCharPos( ObjRectArray& charArray, CRect rcNumRgn, int nType, IMAGE imgGrayOrg, BOOL bBlack );
	BOOL SearchABCChars( IMAGE imgGrayOrg, BOOL bBlack );

	BOOL CutForTopViewChars( CRect rcNumRgn, IMAGE imgGray, BOOL bBlack );

	BOOL CutForCharsByLLT( CRect rcNumRgn, IMAGE imgGrayOrg, BOOL bBlack, ObjRectArray& charArrayRes, ObjRectArray& allRcsArray, CArray<int,int>& intArray );
	int AnaNestedRcs( ObjRectArray& charArray, ObjRectArray& allRcArray );

	BOOL Get3SeqArray( ObjRectArray& rcArray3Seqs, ObjRectArray& allRcArray );
	BOOL CutForMulRgns( ObjRectArray &rcRgns, IMAGE imgGrayrcCurRgn, ObjRectArray& rcArrayRes );
	BOOL CutForMulRgnsByLLT( ObjRectArray &rcRgns, IMAGE imgGray, ObjRectArray& rcArrayRes );
	BOOL CutForMulRgnsPolicy( ObjRectArray &rcRgns, IMAGE imgGray, ObjRectArray& rcArrayRes, int nthBinMed, BOOL bBlack = FALSE );
	BOOL CutForMulRgnChars( IMAGE imgGrayOrg );
	BOOL CutForVerRgn( CRect rcNumRgn, IMAGE imgGray, ObjRectArray& rcArrayRes, BOOL bBlack = FALSE);
	BOOL GetCharsInVerRgn( IMAGE imgGray, IMAGE imgCurRgnBin, ObjRectArray& rcArrayRes, CRect rcRgn );
#ifdef TEST_VER_FORMAT2
	BOOL CutForV2ABCRgn( IMAGE imgGray, CRect rcABCRgn, ObjRectArray& charArray );
	BOOL V2ABCRgnCutStrategy( IMAGE imgGray, CRect rcABCRgn, ObjRectArray& charArray, int nthTry );
#endif


	BOOL GetSubRgns( ObjRectArray& NumCharArray, ObjRectArray& ABCCharArray, CRect& rcNumRgn, CRect& rcABCRgn );

	BOOL m_bBlack;

#ifdef SAVE_DEBUGINFO_FILES
	void WriteRectsAnaRes( fstream& curInfoFile, CString& strTitle, CRect rcRgn, ObjRectArray& charArray, int nCharW, int nCharH, int nRcDis
								    , CArray<int, int>* pnSeqCharsArray = NULL, CArray<int, int>* pnSeqDisArray = NULL );
	void WriteIntArray( fstream& curInfoFile, CString& strTitle, CArray<int, int>* pnArray );
#endif


#ifdef TEST_HMMCONF
	BOOL GetHMMConf( ObjRectArray& allobjs, float &fHMMConf );
	BOOL GetLines( ObjRectArray& allobjs, int& nlines );
	BOOL CheckInsertRect( ObjRectArray& allobjs );

	float** m_HMMDevArray;//Current Deviation Array
	CStModel m_HMMDevST;//Deviation Statistics

	BOOL InsNodes_HMMConfAna( float &fHMMConf, ObjRectArray& resArray, ObjRectArray& allobjs, CArray<int, int>& inputSeqCharsArray, CArray<int, int>& inputSeqDistsArray );
#endif

};

#endif // !defined(AFX_CONNUMPROC_H__9C6F791E_289C_4621_A3C9_39FAC07BD61E__INCLUDED_)
