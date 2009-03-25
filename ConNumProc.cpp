// ConNumProc.cpp: implementation of the CConNumProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConNumProc.h"
#include "ConNumChecker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char drive[_MAX_DRIVE];
char dir[_MAX_DIR];
char fname[_MAX_FNAME];
char ext[_MAX_EXT];

CConNumProc::CConNumProc() : m_ecoConChar(CString("data"))
{
#ifdef SAVE_DEBUGINFO_FILES
	InitParasForDebug();
#endif

#ifdef SAVE_HMM_MID_RES
	InitParasForDebug();
	m_strHMMDebugDir = m_strDebugDir + _T("HMM_Debug_Info\\");
	CreateDirectory( m_strHMMDebugDir, NULL );
#endif

	m_strWholePath = _T("");

	InitParas();
#ifdef CHECK_MEM_LEAK
	nAllocTimes = 0;
	nNewTimes = 0;
#endif

	m_pImageDis = new CDIBitmap;
#ifdef CHECK_MEM_LEAK
	nNewTimes++;
#endif

#ifdef CHECK_DOG
	m_bCheckDog = TRUE;
#endif
}

void CConNumProc::InitParas()
{
	m_bFindNumSeg = FALSE;
	m_bBlack = FALSE;
	m_NumCharArray.RemoveAll();
	m_ABCCharArray.RemoveAll();
	m_strNumSeg = "";
	m_strABCSeg = "";
	m_rgnABCSeg = CRect(0,0,0,0);
	m_rgnNumSeg = CRect(0,0,0,0);

#ifdef TEST_CUT_CONF
	m_NumCharArray_ByConf.RemoveAll();
	m_ABCCharArray_ByConf.RemoveAll();
	m_fCutConf = 0.0f;
#endif
}

void CConNumProc::InitParasForDebug()
{
	m_strDebugDir = "识别模块调试信息\\";
	CreateDirectory( m_strDebugDir, NULL );
	m_strCurDebugDir = "";
	m_strCurDebugFile = "";
	m_strSubDebugDir = m_strDebugDir;
	m_nCurProcID = 0;
}

CConNumProc::~CConNumProc()
{
	if( m_pImageDis != NULL )
	{
		delete m_pImageDis;
#ifdef CHECK_MEM_LEAK
		nNewTimes--;
#endif		
	}
}

BOOL CConNumProc::SetProcFileString( CString& strFilePath )
{
	m_strWholePath = strFilePath;
	_splitpath( strFilePath, drive, dir, fname, ext );
	if( fname != "" )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConNumProc::JudgeResString( CString strRes )
{
	CString strJudge = "";
	int nCnt = strRes.GetLength();
	if( nCnt >= 11 )
	{
		for( int i = 0; i < nCnt; i++ )
		{
			char c = strRes.GetAt(i);
#ifdef TEST_CUT_CONF
			if( c == '#' )
			{
				return FALSE;
			}
#endif
			if( c == '-' || c == '_' )
			{
				continue;
			}
			CString strAdd;
			strAdd.Format("%c",c);
			strJudge += strAdd;
		}
	}
	nCnt = strJudge.GetLength();
	if( nCnt != 11 )
		return FALSE;
	else
		return JudgeRecRes( strJudge );
}

BOOL CConNumProc::ReduceFrame()
{
	int nPicWidth = 0;
	int nPicHeight = 0;
	nPicWidth = m_pImage->GetWidth();
	nPicHeight = m_pImage->GetHeight();

	if( nPicWidth > nPicHeight * 2 )
	{
		return FALSE;
	}

	IMAGE imgR = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgG = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgB = ImageAlloc( nPicWidth , nPicHeight );

	CRect rcPic;
	m_pImage->GetRect(rcPic);
	m_pImage->GetImage( rcPic , imgR[0] , imgG[0] , imgB[0] );	

	nPicHeight = nPicHeight / 2;
	m_pImage->Create( nPicWidth, nPicHeight, 24);

	IMAGE imgR_r = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgG_r = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgB_r = ImageAlloc( nPicWidth , nPicHeight );

	int i,j;
	for( i = 0; i < nPicHeight; i++ )
	{
		for( j = 0; j < nPicWidth; j++ )
		{
			imgR_r[i][j] = imgR[i*2][j];
			imgG_r[i][j] = imgG[i*2][j];
			imgB_r[i][j] = imgB[i*2][j];
		}
	}

	m_pImage->GetRect(rcPic);
	m_pImage->PutImage(rcPic,imgR_r[0],imgG_r[0],imgB_r[0]);

	ImageFree( imgR_r );
	ImageFree( imgG_r );
	ImageFree( imgB_r );

	ImageFree( imgR );
	ImageFree( imgG );
	ImageFree( imgB );

	return TRUE;
}

CString CConNumProc::RecTopViewNum()
{
#ifdef TIME_LIMITED
	CTime TimeCur;
	TimeCur = CTime::GetCurrentTime();
	CTime TimeExpire( 2008, 9, 16, 0, 0, 0 );
	if( TimeCur >= TimeExpire )
	{
		CString strExpired = "error";
		return strExpired;
 	}
#endif

#ifdef CHECK_MEM_LEAK
	int nOrgNewTimes = GetNewTimes();
#endif

	ReduceFrame();

	InitParas();
	int nNums = m_NumCharArray.GetSize();
	int nABCs = m_NumCharArray.GetSize();
	ASSERT( nNums == 0 );
	ASSERT( nABCs == 0 );

	BOOL bCutBingle = FALSE;	
	CString strRes = "";

	int i = 0;
	int nPicWidth = 0;
	int nPicHeight = 0;
	nPicWidth = m_pImage->GetWidth();
	nPicHeight = m_pImage->GetHeight();

	IMAGE imgR = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgG = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgB = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgGray = ImageAlloc( nPicWidth, nPicHeight );
	IMAGE imgGrayOrg = ImageAlloc( nPicWidth, nPicHeight );

	CRect rcPic;
	m_pImage->GetRect(rcPic);
	m_pImage->GetImage( rcPic , imgR[0] , imgG[0] , imgB[0] );

	for( int jj = 0; jj < nPicHeight; jj++ )
	{
		for( int ii = 0; ii < nPicWidth; ii++ )
		{
			imgGray[jj][ii] = (imgR[jj][ii] + imgG[jj][ii] + imgB[jj][ii]) / 3;
		}
	}

	memcpy( imgGrayOrg[0], imgGray[0], nPicWidth * nPicHeight );

#ifdef SAVE_DEBUGINFO_FILES
	m_pImageDis->Create( nPicWidth, nPicHeight, 24 );
	m_pImageDis->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
	m_strCurDebugDir = m_strDebugDir + _T("0_ReduceFrame");
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
	m_strCurDebugFile.Format("%s%s.jpg",m_strCurDebugDir,fname);
	m_pImageDis->Save(m_strCurDebugFile);
#endif

	GetPreNumRects( imgGrayOrg, m_RectArray );
	
#ifdef SAVE_DEBUGINFO_FILES//Save Color Image with Plate Position
	m_strCurDebugDir = m_strDebugDir + "PlatePos";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
	if( m_RectArray.GetSize() > 0 )
	{
		CString strSubDir = m_strCurDebugDir + _T("Got");
		CreateDirectory( strSubDir, NULL );
		strSubDir += _T("\\");
		DrawObjRectArray( m_pImageDis, m_RectArray, 255, 0, 0 );
		m_strCurDebugFile.Format("%s%s%s",strSubDir,fname,CString(_T(".jpg")));
		m_pImageDis->Save(m_strCurDebugFile);
	}
	else
	{
		CString strSubDir = m_strCurDebugDir + _T("NotGet");
		CreateDirectory( strSubDir, NULL );
		strSubDir += _T("\\");
		m_strCurDebugFile.Format("%s%s%s",strSubDir,fname,CString(_T(".jpg")));
		m_pImageDis->Save(m_strCurDebugFile);
	}
#endif

	SortRect_T2B(m_RectArray);

	int nPreRcs = m_RectArray.GetSize();
	int nTimes = min(3,nPreRcs);
	for( i = 0; i < nTimes; i++ )
	{
		CRect rcNumRgn = m_RectArray.GetAt(i);
#ifdef SAVE_DEBUGINFO_FILES
		m_nCurProcID = i;
#endif
		if( CutForTopViewChars( rcNumRgn, imgGray, FALSE ) )
			break;
		else if( CutForTopViewChars( rcNumRgn, imgGray, TRUE ) )
		{
			m_bBlack = TRUE;
			break;
 		}
	}
	


REC:
	nNums = m_NumCharArray.GetSize();
	nABCs = m_ABCCharArray.GetSize();
	if( nNums > 0 && nABCs > 0 )
	{
		if( m_bBlack )
		{
			ImgReverse( imgGray, imgGray );
		}
		CString strNumSeq;
		RecCharArrayForTopView( imgGrayOrg, m_NumCharArray, strNumSeq, TRUE);
		m_strNumSeg = strNumSeq;
		CString strABCSeq;
		RecCharArrayForTopView( imgGrayOrg, m_ABCCharArray, strABCSeq, FALSE);
		m_strABCSeg = strABCSeq;

		int l,r;
		int t = nPicHeight - 1;
		int b = 0;		
		for( int ii = 0 ; ii < nNums; ii++ )
		{
			CRect rcCur = m_NumCharArray.GetAt(ii);
			if( ii == 0 ) l = rcCur.left;
			if( ii == nNums-1 ) r = rcCur.right;
			if( rcCur.top < t ) t = rcCur.top;
			if( rcCur.bottom > b ) b = rcCur.bottom;
		}
		m_rgnNumSeg = CRect( l, t, r, b );
		m_rgnNumSeg.InflateRect( 2, 2 );
		RegulateRect( m_rgnNumSeg, 0, 0, nPicWidth-1, nPicHeight-1 );

		t = nPicHeight - 1;
		b = 0;
		for( ii = 0 ; ii < nABCs; ii++ )
		{
			CRect rcCur = m_ABCCharArray.GetAt(ii);
			if( ii == 0 ) l = rcCur.left;
			if( ii == nABCs-1 ) r = rcCur.right;
			if( rcCur.top < t ) t = rcCur.top;
			if( rcCur.bottom > b ) b = rcCur.bottom;	
		}
		m_rgnABCSeg = CRect( l, t, r, b );
		m_rgnABCSeg.InflateRect( 2, 2 );
		RegulateRect( m_rgnABCSeg, 0, 0, nPicWidth-1, nPicHeight-1 );
	}

OVER:
	//ImageFree( imgVer);
	ImageFree( imgGray );
	ImageFree( imgGrayOrg );


//////////////////////结果输出//////////////////////////////
#ifdef SAVE_DEBUGINFO_FILES
	m_pImageDis->GetImage( rcPic , imgR[0] , imgG[0] , imgB[0] );
	m_pImage->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
#endif
	ImageFree( imgR );
	ImageFree( imgG );
	ImageFree( imgB );

	if( m_NumCharArray.GetSize() > 0 && m_ABCCharArray.GetSize() > 0 )
	{
		strRes = m_strABCSeg + _T("-") + m_strNumSeg;
	}


#ifdef CHECK_MEM_LEAK
	ASSERT( GetLeakCnt() == 0 );
	ASSERT( GetImgMemAlloced() == 0 );
	ASSERT( GetRunLenAlloced() == 0 );

	int nCurNewTimes = GetNewTimes();
	ASSERT( nCurNewTimes == nOrgNewTimes );
#endif

	return strRes;
}

CString CConNumProc::RecNum()
{
#ifdef TIME_LIMITED
	CTime TimeCur;
	TimeCur = CTime::GetCurrentTime();
	CTime TimeExpire( 2008, 9, 16, 0, 0, 0 );
	if( TimeCur >= TimeExpire )
	{
		CString strExpired = "error";
		return strExpired;
 	}
#endif

#ifdef CHECK_DOG
//	m_bCheckDog = CheckDog( 0x08 );//8 Cells
//	m_bCheckDog = CheckDog( 0x10 );//8 Cells
//	m_bCheckDog = CheckDog( 0x0A );//6 Cells
//	m_bCheckDog = CheckDog( 0x0E );//10 Cells
//	m_bCheckDog = CheckDog( 0x0C );//6 Cells
//	m_bCheckDog = CheckDog( 0x12 );//12 Cells
// 	m_bCheckDog = CheckDog( 0x16 );//8 Cells
	if( !m_bCheckDog )
	{
		CString strExpired = "error";
		return strExpired;
	}
#endif

#ifdef SAVE_DEBUGINFO_FILES//Save Debug Info File
	CString strDirInfo = m_strDebugDir + "Info";
	CreateDirectory( strDirInfo, NULL );
	strDirInfo += _T("\\");
	CString strIFilename;
	strIFilename.Format("%s%s%s",strDirInfo,fname,CString(_T(".txt")));
	fstream fInfo;
	fInfo.open( strIFilename, ios::out | ios::trunc );
	fInfo << m_strWholePath << _T(":") << endl;
#endif

#ifdef CHECK_MEM_LEAK
	int nOrgNewTimes = GetNewTimes();
#endif

	InitParas();
	int nNums = m_NumCharArray.GetSize();
	int nABCs = m_NumCharArray.GetSize();
	int nPreRcs = 0;
	ASSERT( nNums == 0 );
	ASSERT( nABCs == 0 );

	BOOL bCutBingle = FALSE;
	
	CString strRes = "";

	int i = 0;
	int nPicWidth = 0;
	int nPicHeight = 0;

	nPicWidth = m_pImage->GetWidth();
	nPicHeight = m_pImage->GetHeight();

	IMAGE imgR = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgG = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgB = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgGray = ImageAlloc( nPicWidth, nPicHeight );

	CRect rcPic;
	m_pImage->GetRect(rcPic);
	m_pImage->GetImage( rcPic , imgR[0] , imgG[0] , imgB[0] );

#ifdef SAVE_DEBUGINFO_FILES
	m_pImageDis->Create( nPicWidth, nPicHeight, 24 );
	m_pImageDis->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
#endif

	for( int jj = 0; jj < nPicHeight; jj++ )
	{
		for( int ii = 0; ii < nPicWidth; ii++ )
		{
			imgGray[jj][ii] = (imgR[jj][ii] + imgG[jj][ii] + imgB[jj][ii]) / 3;
		}
	}

	IMAGE imgGrayOrg = ImageAlloc( nPicWidth, nPicHeight );
	memcpy( imgGrayOrg[0], imgGray[0], nPicWidth * nPicHeight );

//*//Try Cut Vertically -- Try 2 times
	ObjRectArray rcVerArray;

	int t = 0;
#ifdef TEST_HORALIGN_ONLY
	t = 2;
#endif
	while( !bCutBingle && t < 2 )
	{
		t++;
		rcVerArray.RemoveAll();

#ifdef SAVE_DEBUGINFO_FILES
		//m_strSubDebugDir = m_strDebugDir + "AlignVertically_Try\\";
		m_strSubDebugDir.Format( "%sAlignVertically_%dTry\\", m_strDebugDir, t );
		CreateDirectory( m_strSubDebugDir, NULL );
#endif
		
		//GetPreVerNumRects( imgGrayOrg, rcVerArray );
		TryGetPreVerRgns( imgGrayOrg, rcVerArray, t+1 );
		
#ifdef SAVE_PLATEPOS_INFO//Save Color Image with Plate Position
		m_strCurDebugDir = m_strSubDebugDir + "VerPlatePos";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		m_pImageDis->PutImage(rcPic, imgR[0], imgG[0], imgB[0]);
		if( rcVerArray.GetSize() > 0 )
		{
			CString strGotVPosDir;
			strGotVPosDir.Format( "%s\\Got\\", m_strCurDebugDir );
			CreateDirectory( strGotVPosDir, NULL );
			DrawObjRectArray( m_pImageDis, rcVerArray );
			m_strCurDebugFile.Format("%s%s%s",strGotVPosDir,fname,CString(_T(".jpg")));
			m_pImageDis->Save(m_strCurDebugFile);
		}
		else
		{
			CString strNGotVPosDir;
			strNGotVPosDir.Format( "%s\\NGot\\", m_strCurDebugDir );
			CreateDirectory( strNGotVPosDir, NULL );
			m_strCurDebugFile.Format("%s%s%s",strNGotVPosDir,fname,CString(_T(".jpg")));
			m_pImageDis->Save(m_strCurDebugFile);
		}
#endif
		
#ifdef TEST_VERPLATEPOS_ONLY
		rcVerArray.RemoveAll();
#endif
		
		int nPreVerRcs = rcVerArray.GetSize();
		if( nPreVerRcs > 0 )
		{
#ifdef SAVE_DEBUGINFO_FILES
			m_nCurProcID = 0;
#endif
			for( i = 0; i < nPreVerRcs; i++ )
			{
				CRect rcCurRgn = rcVerArray.GetAt( i );
				
				rcCurRgn.InflateRect( 5 , 20 );
				RegulateRect( rcCurRgn, 0, 0, nPicWidth-1, nPicHeight-1 );
				if( (nPicHeight - 1 - rcCurRgn.bottom) < 15 )
				{
					rcCurRgn.bottom = nPicHeight - 1;
				}
				rcVerArray.SetAt(i,rcCurRgn);
				ObjRectArray rcCharArray;
				
				
				if( !CutForVerRgn( rcCurRgn, imgGrayOrg, rcCharArray ) )
				{
					rcCharArray.RemoveAll();
					if( CutForVerRgn( rcCurRgn, imgGrayOrg, rcCharArray, TRUE ) )
					{
						m_bBlack = TRUE;
					}
				}
				
#ifdef SAVE_CUTCHAR_INFO//Save Color Image with Plate Position
				m_strCurDebugDir = m_strSubDebugDir + "VerSeqCutRes";
				CreateDirectory( m_strCurDebugDir, NULL );
				m_strCurDebugDir += _T("\\");
				m_pImageDis->PutImage(rcPic, imgR[0], imgG[0], imgB[0]);
				DrawRect( m_pImageDis, rcCurRgn, 255, 0, 0 );
				DrawObjRectArray( m_pImageDis, rcCharArray, 255, 0, 0 );
				m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,i,CString(_T(".jpg")));
				m_pImageDis->Save(m_strCurDebugFile);
#endif
				
				if( rcCharArray.GetSize() == 11 )
				{
					m_rgnNumSeg = rcCurRgn;
					for( int nId = 0; nId < 4; nId++ )
					{
						CRect rcCur = rcCharArray.GetAt(nId);
						m_ABCCharArray.Add( rcCur );
					}
					for( nId = 0; nId < 7; nId++ )
					{
						CRect rcCur = rcCharArray.GetAt( nId + 4 );
						m_NumCharArray.Add( rcCur );
					}
					
					//Michael Add 080418 -- ReDraw the Check Char
					ASSERT( m_NumCharArray.GetSize() == 7 );
					CRect rcCheck = m_NumCharArray.GetAt(6);
					int nCharW = 0;
					int nCharH = 0;
					GetAverRcWH( m_NumCharArray, nCharW, nCharH );
					if( GetTheCheckRect( rcCheck, imgGrayOrg, nCharH, nCharW ) )
					{
						m_NumCharArray.SetAt( 6, rcCheck );
					}
					//End -- ReDraw the Check Char
					
					
					bCutBingle = TRUE;
					
#ifdef CHECK_DOG
					if( nPreRcs <= 2 )
					{
						m_bCheckDog = CheckDog( 0x0C );//6 Cells
					}
#endif
					
					break;
				}
			}
			
		}
	}

//*/

#ifdef TEST_VERALIGN_ONLY
	tt = 2;
#endif

	int nThHorPosTry = 0;
#ifdef TEST_HORALIGNxTH_ONLY
	//	0 : For Red Chars |  1 : For 4331 Format | 2 : HorPosTry1 | 3 : HorPosTry2
	int xTH = 2;
	nThHorPosTry = xTH;
#endif

	while( !bCutBingle && nThHorPosTry < 4 )
	{
#ifdef TEST_HORALIGNxTH_ONLY
		if( nThHorPosTry != xTH )
			break;
#endif
		BOOL bCutFor3LsOnly = FALSE;
		if( nThHorPosTry == 0 || bCutFor3LsOnly == 1 )
		{
			bCutFor3LsOnly = TRUE;
		}
		
		int tt = 0;
		if( nThHorPosTry == 0 )//red chars
		{
			tt = 1;
		}
		for( tt; tt < 2; tt++ )
		{
			if( bCutBingle ) break;
			
			BOOL bBlack = FALSE;
			if( tt == 1 )
			{
				bBlack = TRUE;
			}
			
#ifdef SAVE_DEBUGINFO_FILES
			//m_strSubDebugDir = m_strDebugDir + "AlignHorizontally_Try1th";
			m_strSubDebugDir.Format( "%sAlignHorizontally_Try%dth", m_strDebugDir, nThHorPosTry );
			CreateDirectory( m_strSubDebugDir, NULL );
			m_strSubDebugDir += "\\";
#endif
			m_RectArray.RemoveAll();
			
#ifdef SAVE_DEBUGINFO_FILES
			m_nCurProcID = tt;
#endif
			//GetPreNumRects( imgGrayOrg, m_RectArray, bBlack );
			GetPrePosStrategy( imgGrayOrg, m_RectArray, bBlack,  nThHorPosTry );
			
#ifdef SAVE_PLATEPOS_INFO//Save Color Image with Plate Position
			m_strCurDebugDir = m_strSubDebugDir + "PlatePos";
			CreateDirectory( m_strCurDebugDir, NULL );
			m_strCurDebugDir += _T("\\");
			m_pImageDis->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
			if( m_RectArray.GetSize() > 0 )
			{
				CString strDirGetPos = m_strCurDebugDir + _T("Got");
				CreateDirectory( strDirGetPos, NULL );
				strDirGetPos += _T("\\");
				//DrawObjRectArray( m_pImageDis, m_RectArray, 255, 0, 0 );
				DrawObjRectArray( m_pImageDis, m_RectArray);
				CString strSaveFile;
				//strSaveFile.Format("%s%s%s",strDirGetPos,fname,CString(_T("_1stTry.jpg")));
				strSaveFile.Format("%s%s_%d.jpg",strDirGetPos,fname,tt);
				m_pImageDis->Save(strSaveFile);
			}
			else
			{
				CString strDirGetPos = m_strCurDebugDir + _T("NotGet");
				CreateDirectory( strDirGetPos, NULL );
				strDirGetPos += _T("\\");
				CString strSaveFile;
				//strSaveFile.Format("%s%s%s",strDirGetPos,fname,CString(_T("_1stTry.jpg")));
				strSaveFile.Format("%s%s_%d.jpg",strDirGetPos,fname,tt);
				m_pImageDis->Save(strSaveFile);
			}
#endif
			
#ifdef TEST_PLATEPOS_ONLY
			m_RectArray.RemoveAll();
#endif
			
#ifdef SAVE_DEBUGINFO_FILES
			m_nCurProcID = nThHorPosTry;
#endif
			
			bCutBingle = CutForHorRgn( imgGrayOrg, bBlack, bCutFor3LsOnly );
		}
		
		nThHorPosTry++;
	}






	int tt = 0;//Try times
/*
	tt = 0;
#ifdef TEST_VERALIGN_ONLY
	tt = 2;
#endif
	for( tt; tt < 2; tt++ )
	{
		if( bCutBingle ) break;

		BOOL bBlack = FALSE;
		if( tt == 1 )
		{
			bBlack = TRUE;
		}

#ifdef SAVE_DEBUGINFO_FILES
		m_strSubDebugDir = m_strDebugDir + "AlignHorizontally_Try1th";
		CreateDirectory( m_strSubDebugDir, NULL );
		m_strSubDebugDir += "\\";
#endif
		m_RectArray.RemoveAll();

#ifdef SAVE_DEBUGINFO_FILES
		m_nCurProcID = tt;
#endif
		GetPreNumRects( imgGrayOrg, m_RectArray, bBlack );
		
#ifdef SAVE_PLATEPOS_INFO//Save Color Image with Plate Position
		m_strCurDebugDir = m_strSubDebugDir + "PlatePos";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		m_pImageDis->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
		if( m_RectArray.GetSize() > 0 )
		{
			CString strDirGetPos = m_strCurDebugDir + _T("Got");
			CreateDirectory( strDirGetPos, NULL );
			strDirGetPos += _T("\\");
			//DrawObjRectArray( m_pImageDis, m_RectArray, 255, 0, 0 );
			DrawObjRectArray( m_pImageDis, m_RectArray);
			CString strSaveFile;
			//strSaveFile.Format("%s%s%s",strDirGetPos,fname,CString(_T("_1stTry.jpg")));
			strSaveFile.Format("%s%s_%d.jpg",strDirGetPos,fname,tt);
			m_pImageDis->Save(strSaveFile);
		}
		else
		{
			CString strDirGetPos = m_strCurDebugDir + _T("NotGet");
			CreateDirectory( strDirGetPos, NULL );
			strDirGetPos += _T("\\");
			CString strSaveFile;
			//strSaveFile.Format("%s%s%s",strDirGetPos,fname,CString(_T("_1stTry.jpg")));
			strSaveFile.Format("%s%s_%d.jpg",strDirGetPos,fname,tt);
			m_pImageDis->Save(strSaveFile);
		}
#endif

#ifdef TEST_PLATEPOS_ONLY
		m_RectArray.RemoveAll();
#endif

#ifdef SAVE_DEBUGINFO_FILES
		m_nCurProcID = 1;
#endif
		bCutBingle = CutForHorRgn( imgGrayOrg, bBlack );
	}
//*/


/*
	tt = 0;
#ifdef TEST_HORALIGN1TH_ONLY
	tt = 2;
#endif
#ifdef TEST_VERALIGN_ONLY
	tt = 2;
#endif
	for( tt; tt < 2; tt++ )
	{
		if( bCutBingle ) break;

		BOOL bBlack = FALSE;
		if( tt == 1 )
		{
			bBlack = TRUE;
		}

#ifdef SAVE_DEBUGINFO_FILES
		m_strSubDebugDir = m_strDebugDir + "AlignHorizontally_Try2th";
		CreateDirectory( m_strSubDebugDir, NULL );
		m_strSubDebugDir += "\\";
#endif
		m_RectArray.RemoveAll();

#ifdef SAVE_DEBUGINFO_FILES
		m_nCurProcID = tt;
#endif
		GetPreJPNumRects( imgGrayOrg, m_RectArray, bBlack );
		
#ifdef SAVE_PLATEPOS_INFO//Save Color Image with Plate Position
		m_strCurDebugDir = m_strSubDebugDir + "PlatePos";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		m_pImageDis->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
		if( m_RectArray.GetSize() > 0 )
		{
			CString strDirGetPos = m_strCurDebugDir + _T("Got");
			CreateDirectory( strDirGetPos, NULL );
			strDirGetPos += _T("\\");
			//DrawObjRectArray( m_pImageDis, m_RectArray, 255, 0, 0 );
			DrawObjRectArray( m_pImageDis, m_RectArray);
			CString strSaveFile;
			//strSaveFile.Format("%s%s%s",strDirGetPos,fname,CString(_T("_1stTry.jpg")));
			strSaveFile.Format("%s%s_%d.jpg",strDirGetPos,fname,tt);
			m_pImageDis->Save(strSaveFile);
		}
		else
		{
			CString strDirGetPos = m_strCurDebugDir + _T("NotGet");
			CreateDirectory( strDirGetPos, NULL );
			strDirGetPos += _T("\\");
			CString strSaveFile;
			//strSaveFile.Format("%s%s%s",strDirGetPos,fname,CString(_T("_1stTry.jpg")));
			strSaveFile.Format("%s%s_%d.jpg",strDirGetPos,fname,tt);
			m_pImageDis->Save(strSaveFile);
		}
#endif

#ifdef TEST_PLATEPOS_ONLY
		m_RectArray.RemoveAll();
#endif

#ifdef SAVE_DEBUGINFO_FILES
		m_nCurProcID = 2;
#endif
		bCutBingle = CutForHorRgn( imgGrayOrg, bBlack );
	}
//*/

REC:
	nNums = m_NumCharArray.GetSize();
	nABCs = m_ABCCharArray.GetSize();

	BOOL bTryByConf = FALSE;
#ifdef TEST_CUT_CONF
	if( nNums == 0 && nABCs == 0 && m_fCutConf > 0.5f )
	{
		bTryByConf = TRUE;
		m_NumCharArray.Copy(m_NumCharArray_ByConf);
		m_ABCCharArray.Copy(m_ABCCharArray_ByConf);

		nNums = m_NumCharArray.GetSize();
		nABCs = m_ABCCharArray.GetSize();

#ifdef CHECK_ERROR
		ASSERT( nNums == 7 && nABCs == 4 );
#endif
	}
#endif
	
#ifdef TEST_HMMCONF
	if( nNums == 7 && nABCs == 4 )
	{
	}
#endif

	if( nNums == 7 && nABCs ==4 )
	{
		CRect rcCheck = m_NumCharArray.GetAt(6);
		if( rcCheck != CRect(0,0,0,0) )
		{
			int nCharW = 0;
			int nCharH = 0;
			ObjRectArray rcArrayAna;
			for( int nId = 0; nId < 7; nId++ )
			{
				CRect rcCur = m_NumCharArray.GetAt(nId);
				if( rcCur != CRect(0,0,0,0) )
				{
					rcArrayAna.Add( rcCur );
				}
			}
 			GetAverRcWH( rcArrayAna, nCharW, nCharH );
			if( GetTheCheckRect( rcCheck, imgGrayOrg, nCharH, nCharW ) )
			{
				m_NumCharArray.SetAt( 6, rcCheck );
			}
		}
		GetSubRgns( m_NumCharArray, m_ABCCharArray, m_rgnNumSeg, m_rgnABCSeg );
	}
	else
	{
		m_NumCharArray.RemoveAll();
		m_ABCCharArray.RemoveAll();
		nNums = 0;
		nABCs = 0;
	}

	if( nNums > 0 && nABCs > 0 )
	{
		if( m_bBlack )
		{
			ImgReverse( imgGrayOrg, imgGrayOrg );
		}
		CString strNumSeq;
		RecCharArray( imgGrayOrg, m_NumCharArray, strNumSeq, TRUE);
		m_strNumSeg = strNumSeq;
		CString strABCSeq;
		RecCharArray( imgGrayOrg, m_ABCCharArray, strABCSeq, FALSE);
		m_strABCSeg = strABCSeq;

		CString strSeq = strABCSeq + strNumSeq;

		if( !bTryByConf )
		{
			if( !JudgeResString( strSeq ) )
			{
				strNumSeq = "";
				strABCSeq = "";
				strSeq = "";
				RecCharArray( imgGrayOrg, m_NumCharArray, strNumSeq, TRUE, 1 );
				RecCharArray( imgGrayOrg, m_ABCCharArray, strABCSeq, FALSE, 1 );
				strSeq = strABCSeq + strNumSeq;
			}

			if( !JudgeResString(strSeq) )
			{
				strNumSeq = "";
				strABCSeq = "";
				strSeq = "";
				RecCharArray( imgGrayOrg, m_NumCharArray, strNumSeq, TRUE, 2 );
				RecCharArray( imgGrayOrg, m_ABCCharArray, strABCSeq, FALSE, 2 );
				strSeq = strABCSeq + strNumSeq;
			}

			if( JudgeResString(strSeq) )
			{
				m_strNumSeg = strNumSeq;
				m_strABCSeg = strABCSeq;
			}
		}
//		int l,r;
//		int t = nPicHeight - 1;
//		int b = 0;		
//		for( int ii = 0 ; ii < nNums; ii++ )
//		{
//			CRect rcCur = m_NumCharArray.GetAt(ii);
//			if( ii == 0 ) l = rcCur.left;
//			if( ii == nNums-1 ) r = rcCur.right;
//			if( rcCur.top < t ) t = rcCur.top;
//			if( rcCur.bottom > b ) b = rcCur.bottom;
//		}
//		m_rgnNumSeg = CRect( l, t, r, b );
//		m_rgnNumSeg.InflateRect( 2, 2 );
//		RegulateRect( m_rgnNumSeg, 0, 0, nPicWidth-1, nPicHeight-1 );
//
//		t = nPicHeight - 1;
//		b = 0;
//		for( ii = 0 ; ii < nABCs; ii++ )
//		{
//			CRect rcCur = m_ABCCharArray.GetAt(ii);
//			if( ii == 0 ) l = rcCur.left;
//			if( ii == nABCs-1 ) r = rcCur.right;
//			if( rcCur.top < t ) t = rcCur.top;
//			if( rcCur.bottom > b ) b = rcCur.bottom;	
//		}
//		m_rgnABCSeg = CRect( l, t, r, b );
//		m_rgnABCSeg.InflateRect( 2, 2 );
//		RegulateRect( m_rgnABCSeg, 0, 0, nPicWidth-1, nPicHeight-1 );
	}

OVER:
#ifdef SHOW_CHARPOS
	m_strCurDebugDir = m_strDebugDir + _T("CharPos\\");
	CreateDirectory( m_strCurDebugDir, NULL );
	m_pImageDis->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
	DrawObjRectArray( m_pImageDis, m_ABCCharArray, 255, 0, 0 );
	DrawObjRectArray( m_pImageDis, m_NumCharArray, 255, 0, 0 );
	m_strCurDebugFile.Format( "%s%s.jpg", m_strCurDebugDir, fname );
	m_pImageDis->Save( m_strCurDebugFile );
#endif

	//ImageFree( imgVer);
	ImageFree( imgGray );
	ImageFree( imgGrayOrg );


//////////////////////结果输出//////////////////////////////
#ifdef SAVE_DEBUGINFO_FILES//Save Debug Info File
	fInfo.close();
#endif

#ifdef SAVE_DEBUGINFO_FILES
	m_pImageDis->PutImage( rcPic , imgR[0] , imgG[0] , imgB[0] );
	m_pImage->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
#endif
	ImageFree( imgR );
	ImageFree( imgG );
	ImageFree( imgB );

	if( m_NumCharArray.GetSize() > 0 && m_ABCCharArray.GetSize() > 0 )
	{
		//strRes = m_strABCSeg + _T("-") + m_strNumSeg;
		strRes = m_strABCSeg + m_strNumSeg;
	}


#ifdef CHECK_MEM_LEAK
	ASSERT( GetLeakCnt() == 0 );
	ASSERT( GetImgMemAlloced() == 0 );
	ASSERT( GetRunLenAlloced() == 0 );

	int nCurNewTimes = GetNewTimes();
	ASSERT( nCurNewTimes == nOrgNewTimes );

#endif


	return strRes;
}

BOOL CConNumProc::CutForHorRgn( IMAGE imgGrayOrg, BOOL bBlack, BOOL bCutFor3LsOnly )
{
#ifdef SAVE_DEBUGINFO_FILES
	int nInputProcId = m_nCurProcID;
	CString strInputDebugDir = m_strDebugDir;
	m_strDebugDir.Format( "%sAlignHorizontally_Try%dth\\", strInputDebugDir, nInputProcId );
	CreateDirectory( m_strDebugDir, NULL );
#endif

	BOOL bCutBingle = FALSE;

	int nPreRcs = m_RectArray.GetSize();
#ifdef CHECK_DOG
	if( nPreRcs <= 2 )
	{
		m_bCheckDog = CheckDog( 0x0A );//6 Cells
	}
#endif

	if( !bCutBingle )
	{
#ifdef TEST_4331_FORMAT
		//if( nPreRcs >= 3 && !bBlack )
		if( nPreRcs >= 3 )
		{
#ifdef SAVE_DEBUGINFO_FILES
			m_strSubDebugDir = m_strDebugDir + "MulSeqTry";
			CreateDirectory( m_strSubDebugDir, NULL );
			m_strSubDebugDir += "\\";
#endif

			if( bBlack )
			{
				ImgReverse( imgGrayOrg, imgGrayOrg );
			}
			bCutBingle = CutForMulRgnChars( imgGrayOrg );
			if( bBlack )
			{
				ImgReverse( imgGrayOrg, imgGrayOrg );
			}
		}
#endif
	}
//*/

//*
	if( !bCutBingle && !bCutFor3LsOnly )
	{
#ifdef SAVE_CUTCHAR_INFO
		m_strSubDebugDir = m_strDebugDir + "\\NormTry";
		CreateDirectory( m_strSubDebugDir, NULL );
		m_strSubDebugDir += "\\";
#endif

		SortRectByLength(m_RectArray);
//		for( i = 0; i < nPreRcs; i++ )
//		{
//			CRect rcCur = m_RectArray.GetAt(i);
//			if( rcCur.Width() > rcCur.Height() * 9 )
//			{
//				m_RectArray.RemoveAt(i);
//				i--;
//				nPreRcs--;
//			}
// 		}
//		nPreRcs = m_RectArray.GetSize();
		int nTimes = min(5,nPreRcs);
		for(int i = 0; i < nTimes; i++ )
		{
			CRect rcNumRgn = m_RectArray.GetAt(i);
#ifdef SAVE_DEBUGINFO_FILES
			m_nCurProcID = i;
#endif
			if( CutForChars( rcNumRgn, imgGrayOrg, bBlack ) )
			{
				break;
			}
		}
				
		int nNums = m_NumCharArray.GetSize();
		int nABCs = m_ABCCharArray.GetSize();
		if( m_NumCharArray.GetSize() > 0 && m_ABCCharArray.GetSize() > 0 )
		{
			bCutBingle = TRUE;
		}
	}

	if( bCutBingle )
	{
		m_bBlack = bBlack;
	}

#ifdef SAVE_DEBUGINFO_FILES
	m_nCurProcID = nInputProcId;
	m_strDebugDir = strInputDebugDir;
#endif
	return bCutBingle;
}

BOOL CConNumProc::CutForMulRgnChars( IMAGE imgGrayOrg )
{
	int nPicWidth = 0;
	int nPicHeight = 0;
#ifdef SAVE_DEBUGINFO_FILES
	nPicWidth = m_pImageDis->GetWidth();
	nPicHeight = m_pImageDis->GetHeight();

	IMAGE imgR = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgG = ImageAlloc( nPicWidth , nPicHeight );
	IMAGE imgB = ImageAlloc( nPicWidth , nPicHeight );

	CRect rcPic;
	m_pImageDis->GetRect(rcPic);
	m_pImageDis->GetImage( rcPic, imgR[0], imgG[0], imgB[0] );
#endif
	
	nPicWidth = ImageWidth(imgGrayOrg);
	nPicHeight = ImageHeight(imgGrayOrg);

	BOOL bCutBingle = FALSE;
	SortRectByCols( m_RectArray );
#ifdef SAVE_PLATEPOS_INFO
	m_strCurDebugDir = m_strSubDebugDir + "MulSeqSearchInfo";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
	CString strMSearchInfoFilename;
	strMSearchInfoFilename.Format("%s%s_MulSearch%s",m_strCurDebugDir,fname,CString(_T(".txt")));
	fstream fMulSearchInfo;
	fMulSearchInfo.open( strMSearchInfoFilename, ios::out | ios::trunc );
	fMulSearchInfo << m_strWholePath << _T(":") << endl;
	WriteRcArray2Txt( fMulSearchInfo, m_RectArray );
	fMulSearchInfo.close();
	m_strCurDebugFile.Format( "%s%s_MulSearch%s",m_strCurDebugDir,fname,CString(_T(".jpg")));
	m_pImageDis->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );
	DrawObjRectArray( m_pImageDis, m_RectArray );
	m_pImageDis->Save(m_strCurDebugFile);
#endif
	
	ObjRectArray rcArray3Seqs;
	Get3SeqArray( rcArray3Seqs, m_RectArray );
	
	if( rcArray3Seqs.GetSize() == 3 )
	{
#ifdef SAVE_PLATEPOS_INFO//Save Color Image with Plate Position
		m_strCurDebugDir = m_strSubDebugDir + "3SeqPlatePos";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		m_pImageDis->PutImage(rcPic, imgR[0], imgG[0], imgB[0]);
		DrawObjRectArray( m_pImageDis, rcArray3Seqs, 255, 255, 0 );
		m_strCurDebugFile.Format("%s%s_TryJPRes%s",m_strCurDebugDir,fname,CString(_T(".jpg")));
		m_pImageDis->Save(m_strCurDebugFile);
#endif

		ObjRectArray rcArrayRes;
		CutForMulRgns( rcArray3Seqs, imgGrayOrg, rcArrayRes );
		
#ifdef SAVE_CUTCHAR_INFO//Save Color Image with Plate Position
		m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutRes";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		m_pImageDis->PutImage(rcPic, imgR[0], imgG[0], imgB[0]);
		DrawObjRectArray( m_pImageDis, rcArrayRes, 255, 0, 0 );
		m_strCurDebugFile.Format("%s%s_ByDajin%s",m_strCurDebugDir,fname,CString(_T(".jpg")));
		m_pImageDis->Save(m_strCurDebugFile);
#endif
		
		if( rcArrayRes.GetSize() != 11 )
		{
			rcArrayRes.RemoveAll();
			CutForMulRgnsByLLT( rcArray3Seqs, imgGrayOrg, rcArrayRes );
#ifdef SAVE_CUTCHAR_INFO//Save Color Image with Plate Position
			m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutRes";
			CreateDirectory( m_strCurDebugDir, NULL );
			m_strCurDebugDir += _T("\\");
			m_pImageDis->PutImage(rcPic, imgR[0], imgG[0], imgB[0]);
			DrawObjRectArray( m_pImageDis, rcArrayRes, 255, 0, 0 );
			m_strCurDebugFile.Format("%s%s_ByLLT%s",m_strCurDebugDir,fname,CString(_T(".jpg")));
			m_pImageDis->Save(m_strCurDebugFile);
#endif
		}
		
		if( rcArrayRes.GetSize() == 11 )
		{
			m_rgnNumSeg = rcArrayRes.GetAt(0);
			for( int nId = 0; nId < 4; nId++ )
			{
				CRect rcCur = rcArrayRes.GetAt(nId);
				m_ABCCharArray.Add( rcCur );

//Michael Delete 080705///////////////////////////				
//				m_rgnNumSeg.top = min(m_rgnNumSeg.top,rcCur.top);
//				m_rgnNumSeg.bottom = max(m_rgnNumSeg.bottom,rcCur.bottom);
//				m_rgnNumSeg.left = min(m_rgnNumSeg.left,rcCur.left);
// 				m_rgnNumSeg.right = max(m_rgnNumSeg.right,rcCur.right);
/////////////////////////////////////////////////
			}
			for( nId = 0; nId < 7; nId++ )
			{
				CRect rcCur = rcArrayRes.GetAt( nId + 4 );
				m_NumCharArray.Add( rcCur );
//Michael Delete 080705////////////////////////
//				m_rgnNumSeg.top = min(m_rgnNumSeg.top,rcCur.top);
//				m_rgnNumSeg.bottom = max(m_rgnNumSeg.bottom,rcCur.bottom);
//				m_rgnNumSeg.left = min(m_rgnNumSeg.left,rcCur.left);
// 				m_rgnNumSeg.right = max(m_rgnNumSeg.right,rcCur.right);
//////////////////////////////////////////////
			}
			
//Michael Delete 080705//////////////////////
//			m_rgnNumSeg.InflateRect(2,2);
// 			RegulateRect( m_rgnNumSeg, 0, 0, nPicWidth-1, nPicHeight-1 );
/////////////////////////////////////////////
			bCutBingle = TRUE;
		}
	}

#ifdef SAVE_DEBUGINFO_FILES
	ImageFree(imgR);
	ImageFree(imgG);
	ImageFree(imgB);
#endif

	return bCutBingle;
}

BOOL CConNumProc::GetSubRgns( ObjRectArray& NumCharArray, ObjRectArray& ABCCharArray, CRect& rcNumRgn, CRect& rcABCRgn )
{
	rcNumRgn = CRect(0,0,0,0);
	rcABCRgn = CRect(0,0,0,0);

	int nNumCharCnt = NumCharArray.GetSize();
	int nABCCharCnt = ABCCharArray.GetSize();

#ifdef CHECK_ERROR
	ASSERT( (nNumCharCnt == 7) && (nABCCharCnt == 4 ) );
#endif

	if( nNumCharCnt != 7 || nABCCharCnt != 4 )
	{
		return FALSE;
	}

	int i = 0;

	for( i = 0; i < 7; i++ )
	{
		CRect rcCur = NumCharArray.GetAt(i);
		if( rcCur == CRect(0,0,0,0) ) continue;
		if( rcNumRgn == CRect(0,0,0,0) )
		{
			rcNumRgn = rcCur;
			continue;
		}

		if( rcCur.left < rcNumRgn.left ) rcNumRgn.left = rcCur.left;
		if( rcCur.right > rcNumRgn.right ) rcNumRgn.right = rcCur.right;
		if( rcCur.top < rcNumRgn.top ) rcNumRgn.top = rcCur.top;
		if( rcCur.bottom > rcNumRgn.bottom ) rcNumRgn.bottom = rcCur.bottom;
	}

	for( i = 0; i < 4; i++ )
	{
		CRect rcCur = ABCCharArray.GetAt(i);
		if( rcCur == CRect(0,0,0,0) ) continue;
		if( rcABCRgn == CRect(0,0,0,0) )
		{
			rcABCRgn = rcCur;
			continue;
		}

		if( rcCur.left < rcABCRgn.left ) rcABCRgn.left = rcCur.left;
		if( rcCur.right > rcABCRgn.right ) rcABCRgn.right = rcCur.right;
		if( rcCur.top < rcABCRgn.top ) rcABCRgn.top = rcCur.top;
		if( rcCur.bottom > rcABCRgn.bottom ) rcABCRgn.bottom = rcCur.bottom;
	}

	return TRUE;
}

BOOL CConNumProc::Get3SeqArray( ObjRectArray& rcArray3Seqs, ObjRectArray& allRcArray )
{

	int nPreRcs = allRcArray.GetSize();
	if( nPreRcs < 3 ) return FALSE;
	
	BOOL bGot = FALSE;
	rcArray3Seqs.Add( allRcArray.GetAt(0) );
	for(int i = 0; i < nPreRcs - 1; i++ )
	{
		CRect rcCur = allRcArray.GetAt(i);
		CRect rcNext = allRcArray.GetAt(i+1);
		
		int nDisInY = rcNext.top - rcCur.bottom;
		int nDisInX = min( abs(rcCur.left-rcNext.left), abs(rcCur.right-rcNext.right) );
		int nDifInH = abs( rcCur.Height()-rcNext.Height() );
		int nDifInHLimit = max( rcCur.Height() * 0.25f, 15 );
		nDifInHLimit = min( nDifInHLimit, rcCur.Height() * 0.5f );
		if(
			//nDifInH < rcCur.Height() * 0.25f
			nDifInH < nDifInHLimit
			&& nDisInY < rcCur.Height() * 1.4f//080612 Michael Change from 1.2f to 1.4f
			//&& nDisInX < max( rcCur.Width(), rcNext.Width() ) / 8

			&& rcNext.top > rcCur.top
			&& rcNext.left < rcCur.right
			)
		{
			rcArray3Seqs.Add(rcNext);
			int nRcs = rcArray3Seqs.GetSize();
			if( nRcs == 3 )
				break;
		}
		else
		{
			rcArray3Seqs.RemoveAll();
			rcArray3Seqs.Add(rcNext);
		}
	}

	return bGot;
}

BOOL CConNumProc::CutForTopViewChars( CRect rcNumRgn, IMAGE imgGray, BOOL bBlack )
{
	BOOL bSuc = FALSE;

	m_NumCharArray.RemoveAll();
	m_ABCCharArray.RemoveAll();
	m_rgnABCSeg = CRect(0,0,0,0);
	m_rgnNumSeg = CRect(0,0,0,0);

	rcNumRgn.InflateRect(20,10);

	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );
	RegulateRect( rcNumRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

	IMAGE imgNumSeq_Gray = ImageAlloc( rcNumRgn.Width() + 1 , rcNumRgn.Height() + 1 );
	IMAGE imgNumSeq = ImageAlloc( rcNumRgn.Width() + 1 , rcNumRgn.Height() + 1 );

	GetImageRectPart( imgGray, imgNumSeq_Gray, rcNumRgn );
	if( bBlack )
	{
		ImgReverse( imgNumSeq_Gray, imgNumSeq_Gray );
	}
	
	CRect rcTmp( 0 , 0 , rcNumRgn.Width() , rcNumRgn.Height() );
	DajinBin( imgNumSeq_Gray , imgNumSeq , rcTmp );
#ifdef SAVE_DEBUGINFO_FILES
	m_strCurDebugDir = m_strDebugDir + "2_CutForTopChars_DajinBin";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += "\\";
	m_strCurDebugFile.Format( "%s%s_%d.jpg", m_strCurDebugDir,fname,m_nCurProcID);
	//ImageSave( imgNumSeq, m_strCurDebugFile );
#endif

	ObjRectArray charArray;
	ObjRectArray allRcsArray;
	GetObjRectForTopView( imgNumSeq, charArray, allRcsArray );
	SortRect( charArray );
#ifdef SAVE_DEBUGINFO_FILES
	m_strCurDebugDir = m_strDebugDir + "2_CutForTopChars_DajinBin";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += "\\";
	m_strCurDebugFile.Format( "%s%s_%d.jpg", m_strCurDebugDir,fname,m_nCurProcID);
	DrawObjRectArray( imgNumSeq, charArray );
	ImageSave( imgNumSeq, m_strCurDebugFile );
#endif

	int nCharW = 0;
	int nCharH = 0;
 	GetAverRcWHForTopView( charArray, nCharW, nCharH );

	TackleCongluatinationForTopView( charArray, imgNumSeq_Gray, nCharH, nCharW );

	int nRcDis = 0;
	GetRcDisForTopView( charArray, nRcDis );

#ifdef SAVE_DEBUGINFO_FILES
	fstream fInfo;
	m_strCurDebugDir = m_strDebugDir + "2_CutForTopChars_DajinBin";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += "\\";
	m_strCurDebugFile.Format( "%s%s_%d.txt", m_strCurDebugDir,fname,m_nCurProcID);
	fInfo.open( m_strCurDebugFile, ios::out|ios::trunc );
#endif
	CArray<int,int> intArray;
 	if( RectsAnalyzeForTopView( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray ) )
	{
#ifdef SAVE_DEBUGINFO_FILES
		fInfo << _T("1st Try RectsAnalyzeForTopView : ") << endl;
		WriteRect2Txt( fInfo, rcNumRgn );
		fInfo << _T(" : ") << endl;
		WriteRcArray2Txt( fInfo, charArray );
		fInfo << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
		fInfo << " ST RES -- RcDis : " << nRcDis << endl;
		int nSeqCnt = intArray.GetSize();
		for( int ii = 0; ii < nSeqCnt; ii++ )
		{
			fInfo << intArray.GetAt(ii);
			if( ii != nSeqCnt-1 )
				fInfo << " - ";
		}
		fInfo << endl;
#endif

		int nType = GetNumSeqTypeForTopView(intArray);
		

//*
		if( nType != 2 )
		{
			MergeApartRcs( charArray, allRcsArray, nCharW, nCharH, nRcDis );
#ifdef SAVE_DEBUGINFO_FILES
			fInfo << _T("Try MergeApartRcs : ") << endl;
			WriteRect2Txt( fInfo, rcNumRgn );
			fInfo << _T(" : ") << endl;
			WriteRcArray2Txt( fInfo, charArray );
			fInfo << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
			fInfo << " ST RES -- RcDis : " << nRcDis << endl;
#endif
			if( RectsAnalyzeForTopView( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray ) )
			{
#ifdef SAVE_DEBUGINFO_FILES
				fInfo << _T("2nd Try RectsAnalyzeForTopView : ") << endl;
				WriteRect2Txt( fInfo, rcNumRgn );
				fInfo << _T(" : ") << endl;
				WriteRcArray2Txt( fInfo, charArray );
				fInfo << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
				fInfo << " ST RES -- RcDis : " << nRcDis << endl;
				int nSeqCnt = intArray.GetSize();
				for( int ii = 0; ii < nSeqCnt; ii++ )
				{
					fInfo << intArray.GetAt(ii);
					if( ii != nSeqCnt-1 )
						fInfo << " - ";
				}
				fInfo << endl;
#endif
				nType = GetNumSeqTypeForTopView(intArray);
			}
			else
			{
#ifdef SAVE_DEBUGINFO_FILES
				fInfo << _T("2nd Try RectsAnalyzeForTopView : ") << endl;
				WriteRect2Txt( fInfo, rcNumRgn );
				fInfo << _T(" : ") << endl;
				WriteRcArray2Txt( fInfo, charArray );
				fInfo << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
				fInfo << " ST RES -- RcDis : " << nRcDis << endl;
				fInfo << "ERROR!" << endl;
				fInfo << endl;
#endif
			}
		}
//*/
		
		if( nType == 2 )
		{
			bSuc = TRUE;
			TransferRcArrayFromL2W( charArray, rcNumRgn );

			ASSERT( charArray.GetSize() >= 11 );
			for( int ii = 0 ; ii < 4 ; ii++ )
			{
				CRect rcCur = charArray.GetAt(ii);
				m_ABCCharArray.Add( rcCur );
			}
			m_rgnABCSeg = rcNumRgn;
			for( ii = 0 ; ii < 7 ; ii++ )
			{
				CRect rcCur = charArray.GetAt(ii+4);
				m_NumCharArray.Add( rcCur );
			}
			m_rgnNumSeg = rcNumRgn;
		}

	}
	else
	{
#ifdef SAVE_DEBUGINFO_FILES
		fstream fInfo;
		m_strCurDebugDir = m_strDebugDir + "2_CutForTopChars_DajinBin";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += "\\";
		m_strCurDebugFile.Format( "%s%s_%d.txt", m_strCurDebugDir,fname,m_nCurProcID);
		fInfo.open( m_strCurDebugFile, ios::out|ios::trunc );
		//fInfo << i << _T(" : ");
		WriteRect2Txt( fInfo, rcNumRgn );
		fInfo << _T(" : ") << endl;
		WriteRcArray2Txt( fInfo, charArray );
		fInfo << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
		fInfo << " ST RES -- RcDis : " << nRcDis << endl;
		fInfo << "ERROR!" << endl;
		fInfo << endl;
		DrawObjRectArray( imgNumSeq, charArray );
		m_pImageDis->PutImage( rcNumRgn, imgNumSeq[0], imgNumSeq[0], imgNumSeq[0] );
		fInfo.close();
#endif

		
	}

#ifdef SAVE_DEBUGINFO_FILES
	fInfo.close();
#endif

	ImageFree( imgNumSeq_Gray );
	ImageFree( imgNumSeq );

	if( m_NumCharArray.GetSize() > 0 && m_ABCCharArray.GetSize() > 0 )
	{
		bSuc = TRUE;
	}

	if( bSuc )
	{
		ASSERT( m_NumCharArray.GetSize() == 7 );
		CRect rcCheck = m_NumCharArray.GetAt(6);
		if( GetTheCheckRectForTopView( rcCheck, imgGray, nCharH, nCharW ) )
		{
			m_NumCharArray.SetAt( 6, rcCheck );
		}
	}

	return bSuc;
}



//BOOL CConNumProc::CutForChars( CRect rcNumRgn, IMAGE imgGrayOrg, BOOL bBlack )
//{
//	m_NumCharArray.RemoveAll();
//	m_ABCCharArray.RemoveAll();
//	m_rgnABCSeg = CRect(0,0,0,0);
//	m_rgnNumSeg = CRect(0,0,0,0);
//	
//	int nInfW = 20;
//	int nInfH = min( 20, rcNumRgn.Height() * 0.6 );
//	nInfH = max( 10, nInfH );
//	rcNumRgn.InflateRect( nInfW , nInfH );
//
//	int nPicWidth = ImageWidth(imgGrayOrg);
//	int nPicHeight = ImageHeight(imgGrayOrg);
//	RegulateRect( rcNumRgn, 0, 0, nPicWidth-1, nPicHeight-1 );
//
//	if( rcNumRgn.left < rcNumRgn.Width() * 0.1 )
//	{
//		rcNumRgn.left = 0;
//	}
//
//#ifdef SAVE_CUTCHAR_INFO//Save Cut NumSequence Region
//	m_strCurDebugDir = m_strSubDebugDir + "CutCharsByDJ";
//	CreateDirectory( m_strCurDebugDir, NULL );
//	m_strCurDebugDir += _T("\\");
//	if( !bBlack )
//		m_strCurDebugFile.Format("%s%s_%d_CUTPIC%s",m_strCurDebugDir,fname,m_nCurProcID*2,CString(_T(".jpg")));
//	else
//		m_strCurDebugFile.Format("%s%s_%d_CUTPIC%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,CString(_T(".jpg")));
// 	SavePICPart( m_pImage, rcNumRgn, m_strCurDebugFile);
//#endif
//
//	IMAGE imgNumSeq_Gray = ImageAlloc( rcNumRgn.Width() + 1 , rcNumRgn.Height() + 1 );
//	IMAGE imgNumSeq = ImageAlloc( rcNumRgn.Width() + 1 , rcNumRgn.Height() + 1 );
//
//	GetImageRectPart( imgGrayOrg, imgNumSeq_Gray, rcNumRgn );
//	if( bBlack )
//	{
//		ImgReverse( imgNumSeq_Gray, imgNumSeq_Gray );
//	}
//	CRect rcTmp( 0 , 0 , rcNumRgn.Width() , rcNumRgn.Height() );
//
//	DajinBin( imgNumSeq_Gray , imgNumSeq , rcTmp );
//	
//	ObjRectArray charArray;
//	ObjRectArray allRcsArray;
//	GetObjRect( imgNumSeq, charArray, allRcsArray );
//	//int nAllCnt = allRcsArray.GetSize();
//	SortRect( charArray );
//
//
//#ifdef SAVE_CUTCHAR_INFO
//	if( !bBlack )
//		m_strCurDebugFile.Format("%s%s_%d_DJBIN_RcsAna%s",m_strCurDebugDir,fname,m_nCurProcID*2,CString(_T(".txt")));
//	else
//		m_strCurDebugFile.Format("%s%s_%d_DJBIN_RcsAna%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,CString(_T(".txt")));
//	m_CurInfoFile.open( m_strCurDebugFile, ios::out|ios::trunc);
//#endif
//
//	int nCharW = 0;
//	int nCharH = 0;
// 	GetAverRcWH( charArray, nCharW, nCharH );
//
//#ifdef SAVE_TC_INFO//Tackle Congluatination
//	CString strDebugDir = m_strCurDebugDir + _T("TC\\");
//	CreateDirectory( strDebugDir, NULL );
//	CString strDebugFname = "";
//	if( !bBlack )
//		strDebugFname.Format( "%s_%d", fname , i * 2 );
//	else
//		strDebugFname.Format( "%s_%d", fname , i * 2 + 1 );
//	SetCutCharsDebugInfoPath( strDebugDir, strDebugFname );
//#endif
//
//	if( nCharW >= nCharH )//Michael Add 20080620, For fear of the congluatination
//	{
//		nCharW = nCharH * 1 / 2;
//	}
//
//	TackleCongluatination( charArray, allRcsArray, imgNumSeq_Gray, nCharH, nCharW );
//
//	int nRcDis = 0;
//	GetRcDis( charArray, nRcDis );
//	//int nCntTest = charArray.GetSize();
//
//	RectsReDraw( imgNumSeq_Gray, charArray, nCharH, nCharW, nRcDis );
//
//	CArray<int,int> intArray;
//	BOOL bCut = FALSE;
//	BOOL bGot = FALSE;
//	int nType = -1;
//#ifdef TEST_CUT_CONF
//	int nType_Conf = -1;
//#endif
//	
//	//Michael Add 2008-04-16 -- Tackle Splitted Chars
//	MergeSplittedChars( charArray, allRcsArray, nCharW, nCharH, nRcDis );
//	//End -- Tackle Splitted Chars
//#ifdef SAVE_CUTCHAR_INFO//Save Cut NumSequence Region
//	if( !bBlack )
//		m_strCurDebugFile.Format("%s%s_%d_DJBIN%s",m_strCurDebugDir,fname,m_nCurProcID*2,CString(_T(".jpg")));
//	else
//		m_strCurDebugFile.Format("%s%s_%d_DJBIN%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,CString(_T(".jpg")));
// 	DrawObjRectArray( imgNumSeq, charArray );
//	ImageSave( imgNumSeq, m_strCurDebugFile );
//#endif
// 	if( RectsAnalyze( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray ) )
//	{
//#ifdef SAVE_CUTCHAR_INFO
//		m_CurInfoFile << _T(" RectsAnalyze Suc : ") << endl;
//		WriteRect2Txt( m_CurInfoFile, rcNumRgn );
//		m_CurInfoFile << _T(" : ") << endl;
//		WriteRcArray2Txt( m_CurInfoFile, charArray );
//		m_CurInfoFile << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
//		m_CurInfoFile << " ST RES -- RcDis : " << nRcDis << endl;
//		int nSeqCnt = intArray.GetSize();
//		for( int ii = 0; ii < nSeqCnt; ii++ )
//		{
//			m_CurInfoFile << intArray.GetAt(ii);
//			if( ii != nSeqCnt-1 ) 
//				m_CurInfoFile << " - ";
//		}
//		m_CurInfoFile << endl;
//#endif
//		bGot = TRUE;
//		TransferRcArrayFromL2W( charArray, rcNumRgn );
//
//#ifdef TEST_CUT_CONF
//		ObjRectArray charArrayAna;
//		charArrayAna.Copy( charArray );
//#endif
//
//		bCut = PatternAnalyze( intArray, nType, charArray );
//
//#ifdef TEST_CUT_CONF
//		if( !bCut )
//		{
//			PatternAnalyze_ByConf(intArray, nType_Conf, charArray);
//		}
//		else
//		{
//			if( nType < 2 && charArray.GetSize() >= 9 )
//			{
//				PatternAnalyze_ByConf( intArray, nType_Conf, charArrayAna );
//			}
//		}
//#endif
//	}
//	else
//	{
//#ifdef SAVE_CUTCHAR_INFO
//		m_CurInfoFile << _T(" RectsAnalyze Fail : ") << endl;
//		WriteRect2Txt( m_CurInfoFile, rcNumRgn );
//		m_CurInfoFile << _T(" : ") << endl;
//		WriteRcArray2Txt( m_CurInfoFile, charArray );
//		m_CurInfoFile << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
//		m_CurInfoFile << " ST RES -- RcDis : " << nRcDis << endl;	
//		m_CurInfoFile << "ERROR!" << endl;
//		m_CurInfoFile << endl;
//#endif	
//		
//	}
//
//	int nTCharCnt = charArray.GetSize();
//	//if( bGot && !bCut )//Original Condition
//	if( bGot && ( !bCut || ( nTCharCnt == 9 || nTCharCnt == 10 )) )//Michael Modify 2008-04-17 For the Pattern 7-2-1( which should be 10-1)
//	{
//		TransferRcArrayFromL2W( allRcsArray, rcNumRgn );
//		RectsAnalyzeByScan( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray );
//		int nNewCnt = charArray.GetSize();
//		if( RectsAnalyze( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray ) )
//		{
//#ifdef SAVE_CUTCHAR_INFO
//			m_CurInfoFile << _T("2nd RectsAnalyze Suc : ") << endl;
//			WriteRect2Txt( m_CurInfoFile, rcNumRgn );
//			m_CurInfoFile << _T(" : ") << endl;
//			WriteRcArray2Txt( m_CurInfoFile, charArray );
//			m_CurInfoFile << "2nd ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
//			m_CurInfoFile << "2nd ST RES -- RcDis : " << nRcDis << endl;	
//			int nSeqCnt = intArray.GetSize();
//			for( int ii = 0; ii < nSeqCnt; ii++ )
//			{
//				m_CurInfoFile << intArray.GetAt(ii);
//				if( ii != nSeqCnt-1 ) 
//					m_CurInfoFile << " - ";
//			}
//			m_CurInfoFile << endl;
//#endif
//
//#ifdef TEST_CUT_CONF
//			ObjRectArray charArrayAna;
//			charArrayAna.Copy( charArray );
//#endif
//			bCut = PatternAnalyze( intArray, nType, charArray );
//#ifdef TEST_CUT_CONF
//			if( !bCut )
//			{
//				PatternAnalyze_ByConf(intArray, nType_Conf, charArray);
//			}
//			else
//			{
//				if( nType < 2 && charArray.GetSize() >= 9 )
//				{
//					PatternAnalyze_ByConf( intArray, nType_Conf, charArrayAna );
//				}
//			}
//#endif
//		}
//
//	}
//
//	if( bCut )
//	{
//#ifdef CHECK_ERROR
//		ASSERT( nType == 2 || nType == 0 );
//#endif
//		bCut = CopyCharPos( charArray, rcNumRgn, nType, imgGrayOrg, bBlack );
//	}
//
//#ifdef SAVE_CUTCHAR_INFO
//	m_CurInfoFile.close();
//#endif
//
//
//	if( !bCut )
//	{
//		charArray.RemoveAll();
//		allRcsArray.RemoveAll();
//		intArray.RemoveAll();
//#ifdef SAVE_CUTCHAR_INFO		
//		m_strCurDebugDir = m_strSubDebugDir + "CutCharsByLLT";
//		CreateDirectory( m_strCurDebugDir, NULL );
//		m_strCurDebugDir += _T("\\");
//#endif
//
//#ifdef SAVE_CUTCHAR_INFO
//		if( !bBlack )
//			m_strCurDebugFile.Format("%s%s_%d_LLTBIN_RcsAna%s",m_strCurDebugDir,fname,m_nCurProcID*2,CString(_T(".txt")));
//		else
//			m_strCurDebugFile.Format("%s%s_%d_LLTBIN_RcsAna%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,CString(_T(".txt")));
//		m_CurInfoFile.open( m_strCurDebugFile, ios::out|ios::trunc);
//#endif
//		if( CutForCharsByLLT( rcNumRgn, imgGrayOrg, bBlack, charArray, allRcsArray, intArray ) )
//		{
//			GetAverRcWH(charArray,nCharW,nCharH);
//			bGot = TRUE;
//#ifdef TEST_CUT_CONF
//			ObjRectArray charArrayAna;
//			charArrayAna.Copy( charArray );
//#endif
//			bCut = PatternAnalyze( intArray, nType, charArray );
//#ifdef TEST_CUT_CONF
//			if( !bCut )
//			{
//				PatternAnalyze_ByConf(intArray, nType_Conf, charArray);
//			}
//			else
//			{
//				if( nType < 2 && charArray.GetSize() >= 9 )
//				{
//					PatternAnalyze_ByConf( intArray, nType_Conf, charArrayAna );
//				}
//			}
//#endif
//		}
//
//#ifdef SAVE_CUTCHAR_INFO
//		m_CurInfoFile << m_nCurProcID << _T(" : ");
//		WriteRect2Txt( m_CurInfoFile, rcNumRgn );
//		m_CurInfoFile << _T(" : ") << endl;
//		WriteRcArray2Txt( m_CurInfoFile, charArray );
//		m_CurInfoFile << "1st LLT ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
//		m_CurInfoFile << "1st LLT ST RES -- RcDis : " << nRcDis << endl;
//		if( bGot )
//		{
//			int nSeqCnt = intArray.GetSize();
//			for( int ii = 0; ii < nSeqCnt; ii++ )
//			{
//				m_CurInfoFile << intArray.GetAt(ii);
//				if( ii != nSeqCnt-1 ) 
//					m_CurInfoFile << " - ";
//			}
//		}
//		else
//		{
//			m_CurInfoFile << "ERROR!" ;
//		}
//
//		m_CurInfoFile << endl;
//#endif
//
//		if( bGot && !bCut )
//		{
//			nCharW = 0;
// 			nCharH = 0;
// 			GetAverRcWH( charArray, nCharW, nCharH );
//
//			nRcDis = 0;
//			GetRcDis( charArray, nRcDis );
//			RectsAnalyzeByScan( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray );
//			int nNewCnt = charArray.GetSize();
//			bGot = FALSE;
//			if( RectsAnalyze( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray ) )
//			{
//				bGot = TRUE;
//#ifdef TEST_CUT_CONF
//				ObjRectArray charArrayAna;
//				charArrayAna.Copy( charArray );
//#endif
//				bCut = PatternAnalyze( intArray, nType, charArray );
//#ifdef TEST_CUT_CONF
//				if( !bCut )
//				{
//					PatternAnalyze_ByConf(intArray, nType_Conf, charArray);
//				}
//				else
//				{
//					if( nType < 2 && charArray.GetSize() >= 9 )
//					{
//						PatternAnalyze_ByConf( intArray, nType_Conf, charArrayAna );
//					}
//				}
//#endif
//			}
//		}
//#ifdef SAVE_CUTCHAR_INFO
//		m_CurInfoFile << _T("RectsAnalyzeByScan Try : ") << endl;
//		WriteRect2Txt( m_CurInfoFile, rcNumRgn );
//		m_CurInfoFile << _T(" : ") << endl;
//		WriteRcArray2Txt( m_CurInfoFile, charArray );
//		m_CurInfoFile << "ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
//		m_CurInfoFile << "ST RES -- RcDis : " << nRcDis << endl;	
//		int nSeqCnt = intArray.GetSize();
//		if( bGot )
//		{
//			int nSeqCnt = intArray.GetSize();
//			for( int ii = 0; ii < nSeqCnt; ii++ )
//			{
//				m_CurInfoFile << intArray.GetAt(ii);
//				if( ii != nSeqCnt-1 ) 
//					m_CurInfoFile << " - ";
//			}
//		}
//		else
//		{
//			m_CurInfoFile << "ERROR!" ;
//		}
//		m_CurInfoFile << endl;
//#endif
//
//#ifdef SAVE_CUTCHAR_INFO
//		m_CurInfoFile.close();
//#endif
//	
//		if( bCut )
//		{
//#ifdef CHECK_ERROR
//			ASSERT( nType == 2 || nType == 0 );
//#endif
//			bCut = CopyCharPos( charArray, rcNumRgn, nType, imgGrayOrg, bBlack );
//		}		
//	}
//	
//
//	
//	if( bCut )
//	{
//		ASSERT( m_NumCharArray.GetSize() == 7 );
//		CRect rcCheck = m_NumCharArray.GetAt(6);
//		m_bBlack = bBlack;
//		if( GetTheCheckRect( rcCheck, imgGrayOrg, nCharH, nCharW ) )
//		{
//			m_NumCharArray.SetAt( 6, rcCheck );
//		}
//	}
//
//#ifdef SAVE_CUTCHAR_INFO
//	{
//		m_strCurDebugDir = m_strSubDebugDir + "CharPos";
//		CreateDirectory( m_strCurDebugDir, NULL );
//		m_strCurDebugDir += _T("\\");
//		int nCnt = 0;
//		do {
//			nCnt++;
//			m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,nCnt,CString(_T(".jpg")));
//		} while( _access(m_strCurDebugFile,0) != -1);
//		
//		m_pImageDis->PutImage( rcNumRgn, imgNumSeq_Gray[0], imgNumSeq_Gray[0], imgNumSeq_Gray[0] );
//		DrawObjRectArray( m_pImageDis, m_NumCharArray, 255, 0, 0 );
//		DrawObjRectArray( m_pImageDis, m_ABCCharArray, 255, 0, 0 );
//		m_pImageDis->Save( m_strCurDebugFile );
//	}
//#endif
//
//
//	
//	ImageFree( imgNumSeq_Gray );
//	ImageFree( imgNumSeq );
//
//	bCut = m_NumCharArray.GetSize() > 0 && m_ABCCharArray.GetSize() > 0;
//
//	return bCut;
//}
//Michael Delete 20080707, Try to regulate this function

BOOL CConNumProc::HorRgnCutStrategy( IMAGE imgNumSeq_Gray, IMAGE imgNumSeq, CRect rcNumRgn, ObjRectArray& resArray, int &nType, BOOL bBlack )
{
#ifdef SAVE_CUTCHAR_INFO
	int nNumSeqWidth = ImageWidth( imgNumSeq );
	int nNumSeqHeight = ImageHeight( imgNumSeq );
	IMAGE imgNumSeq_Show = ImageAlloc( nNumSeqWidth, nNumSeqHeight );
#endif
	BOOL bSuc = FALSE;

	ObjRectArray charArray;
	ObjRectArray allRcsArray;
	GetObjRect( imgNumSeq, charArray, allRcsArray );
#ifdef TEST_CUT_CONF_V3
	ObjRectArray rcAllArray;
	rcAllArray.Copy( allRcsArray );
#endif// End -- TEST_CUT_CONF_V3

	SortRect( charArray );
	
#ifdef SAVE_CUTCHAR_INFO//Save Cut NumSequence Region
	int nCurID = 0;
#endif

#ifdef SAVE_CUTCHAR_INFO//_withAllRcsArray
	if( !bBlack )
		m_strCurDebugFile.Format("%s%s_%d_BIN_%d_withAllRcsArray%s",m_strCurDebugDir,fname,m_nCurProcID*2,nCurID++,CString(_T(".jpg")));
	else
		m_strCurDebugFile.Format("%s%s_%d_BIN_%d_withAllRcsArray%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,nCurID++,CString(_T(".jpg")));
	memcpy( imgNumSeq_Show[0], imgNumSeq[0], nNumSeqHeight * nNumSeqWidth );
	DrawObjRectArray( imgNumSeq_Show, allRcsArray );
	ImageSave( imgNumSeq_Show, m_strCurDebugFile );
#endif
	
#ifdef SAVE_CUTCHAR_INFO//_withCharArray
	if( !bBlack )
		m_strCurDebugFile.Format("%s%s_%d_BIN_%d_withCharArray%s",m_strCurDebugDir,fname,m_nCurProcID*2,nCurID++,CString(_T(".jpg")));
	else
		m_strCurDebugFile.Format("%s%s_%d_BIN_%d_withCharArray%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,nCurID++,CString(_T(".jpg")));
 	memcpy( imgNumSeq_Show[0], imgNumSeq[0], nNumSeqHeight * nNumSeqWidth );
	DrawObjRectArray( imgNumSeq_Show, charArray );
	ImageSave( imgNumSeq_Show, m_strCurDebugFile );
#endif

#ifdef SAVE_TC_INFO//Tackle Congluatination
	CString strDebugDir = m_strCurDebugDir + _T("TC\\");
	CreateDirectory( strDebugDir, NULL );
	CString strDebugFname = "";
	if( !bBlack )
		strDebugFname.Format( "%s_%d", fname , m_nCurProcID * 2 );
	else
		strDebugFname.Format( "%s_%d", fname , m_nCurProcID * 2 + 1 );
 	SetCutCharsDebugInfoPath( strDebugDir, strDebugFname );
#endif

	int nCharW = 0;
	int nCharH = 0;
 	GetAverRcWH( charArray, nCharW, nCharH );

	if( nCharW >= nCharH )//Michael Add 20080620, For fear of the congluatination
	{
		nCharW = nCharH * 1 / 2;
	}

	TackleCongluatination( charArray, allRcsArray, imgNumSeq_Gray, nCharH, nCharW );

	
#ifdef SAVE_CUTCHAR_INFO
	if( !bBlack )
		m_strCurDebugFile.Format("%s%s_%d_BIN_%d_withTackCong%s",m_strCurDebugDir,fname,m_nCurProcID*2,nCurID++,CString(_T(".jpg")));
	else
		m_strCurDebugFile.Format("%s%s_%d_BIN_%d_withTackCong%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,nCurID++,CString(_T(".jpg")));
 	memcpy( imgNumSeq_Show[0], imgNumSeq[0], nNumSeqHeight * nNumSeqWidth );
	DrawObjRectArray( imgNumSeq_Show, charArray );
	ImageSave( imgNumSeq_Show, m_strCurDebugFile );
#endif

	int nRcDis = 0;
	GetRcDis( charArray, nRcDis );
	//int nCntTest = charArray.GetSize();

	RectsReDraw( imgNumSeq_Gray, charArray, nCharH, nCharW, nRcDis );


	
	//Michael Add 2008-04-16 -- Tackle Splitted Chars
	MergeSplittedChars( charArray, allRcsArray, nCharW, nCharH, nRcDis );
	//End -- Tackle Splitted Chars
#ifdef SAVE_CUTCHAR_INFO
	if( !bBlack )
		m_strCurDebugFile.Format("%s%s_%d_BIN_%d_withMerge%s",m_strCurDebugDir,fname,m_nCurProcID*2,nCurID++,CString(_T(".jpg")));
	else
		m_strCurDebugFile.Format("%s%s_%d_BIN_%d_withMerge%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,nCurID++,CString(_T(".jpg")));
 	memcpy( imgNumSeq_Show[0], imgNumSeq[0], nNumSeqHeight * nNumSeqWidth );
	DrawObjRectArray( imgNumSeq_Show, charArray );
	ImageSave( imgNumSeq_Show, m_strCurDebugFile );
#endif

	bSuc = HorRgnRcsAnaStrategy( charArray, allRcsArray, resArray, nType, nCharW, nCharH, nRcDis, rcNumRgn, bBlack );

#ifdef TEST_CUT_CONF_V3
	if( !bSuc )
	{
		ObjRectArray rcMainline;
		ObjRectArray rcRemain;
		DigOutSeq( rcAllArray, rcMainline, rcRemain );

		int nh = 0;
		int nw = 0;
		int nd = 0;
		GetAverRcWH( rcMainline, nw, nh );
#ifdef SAVE_TC_INFO//Tackle Congluatination
		CString strDebugDir = m_strCurDebugDir + _T("TC_FORALL\\");
		CreateDirectory( strDebugDir, NULL );
		CString strDebugFname = "";
		if( !bBlack )
			strDebugFname.Format( "%s_%d", fname , m_nCurProcID * 2 );
		else
			strDebugFname.Format( "%s_%d", fname , m_nCurProcID * 2 + 1 );
 		SetCutCharsDebugInfoPath( strDebugDir, strDebugFname );
#endif
		TackleCongluatination( rcMainline, rcAllArray, imgNumSeq_Gray, nh, nh / 2 );
		GetAverRcWH( rcMainline, nw, nh );
		GetRcDis( rcMainline, nd );
		if( nd == 0 ) nd = 1;

		bSuc = HorRgnRcsAnaStrategy( rcMainline, rcAllArray, resArray, nType, nw, nh, nd, rcNumRgn, bBlack );
	}
#endif//End -- TEST_CUT_CONF_V3

#ifdef SAVE_CUTCHAR_INFO
	ImageFree( imgNumSeq_Show );
#endif

	return bSuc;
}


BOOL CConNumProc::HorRgnRcsAnaStrategy( ObjRectArray& charArray, ObjRectArray& allRcsArray, ObjRectArray& resArray, int& nType, int nCharW, int nCharH, int nRcDis, CRect rcNumRgn, BOOL bBlack )
{

	BOOL bSuc = FALSE;

#ifdef SAVE_CUTCHAR_INFO
	if( !bBlack )
		m_strCurDebugFile.Format("%s%s_%d_RcsAna%s",m_strCurDebugDir,fname,m_nCurProcID*2,CString(_T(".txt")));
	else
		m_strCurDebugFile.Format("%s%s_%d_RcsAna%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,CString(_T(".txt")));
	m_CurInfoFile.open( m_strCurDebugFile, ios::out|ios::trunc);
#endif

	CArray<int,int> intArray;
	BOOL bCut = FALSE;
	BOOL bGot = FALSE;
	nType = -1;
#ifdef TEST_CUT_CONF
	int nType_Conf = -1;
#endif

 	if( RectsAnalyze( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray ) )
	{
#ifdef SAVE_CUTCHAR_INFO
		m_CurInfoFile << _T(" RectsAnalyze Suc : ") << endl;
		WriteRect2Txt( m_CurInfoFile, rcNumRgn );
		m_CurInfoFile << _T(" : ") << endl;
		WriteRcArray2Txt( m_CurInfoFile, charArray );
		m_CurInfoFile << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
		m_CurInfoFile << " ST RES -- RcDis : " << nRcDis << endl;
		int nSeqCnt = intArray.GetSize();
		for( int ii = 0; ii < nSeqCnt; ii++ )
		{
			m_CurInfoFile << intArray.GetAt(ii);
			if( ii != nSeqCnt-1 ) 
				m_CurInfoFile << " - ";
		}
		m_CurInfoFile << endl;
#endif
		bGot = TRUE;
		TransferRcArrayFromL2W( charArray, rcNumRgn );

#ifdef TEST_CUT_CONF
		ObjRectArray charArrayAna;
		charArrayAna.Copy( charArray );
#endif

		bCut = PatternAnalyze( intArray, nType, charArray );

#ifdef TEST_CUT_CONF
		if( !bCut )
		{
			PatternAnalyze_ByConf(intArray, nType_Conf, charArray);
		}
		else
		{
			if( nType < 2 && charArray.GetSize() >= 9 )
			{
				PatternAnalyze_ByConf( intArray, nType_Conf, charArrayAna );
			}
		}
#endif
	}
	else
	{
#ifdef SAVE_CUTCHAR_INFO
		m_CurInfoFile << _T(" RectsAnalyze Fail : ") << endl;
		WriteRect2Txt( m_CurInfoFile, rcNumRgn );
		m_CurInfoFile << _T(" : ") << endl;
		WriteRcArray2Txt( m_CurInfoFile, charArray );
		m_CurInfoFile << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
		m_CurInfoFile << " ST RES -- RcDis : " << nRcDis << endl;
		m_CurInfoFile << "ERROR!" << endl;
		m_CurInfoFile << endl;
#endif			
	}

	int nTCharCnt = charArray.GetSize();
	//if( bGot && !bCut )//Original Condition
	if( bGot && ( !bCut || ( nTCharCnt == 9 || nTCharCnt == 10 )) )//Michael Modify 2008-04-17 For the Pattern 7-2-1( which should be 10-1)
	{
		TransferRcArrayFromL2W( allRcsArray, rcNumRgn );
		RectsAnalyzeByScan( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray );
		int nNewCnt = charArray.GetSize();
		if( RectsAnalyze( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray ) )
		{
#ifdef SAVE_CUTCHAR_INFO
			m_CurInfoFile << _T("2nd RectsAnalyze Suc : ") << endl;
			WriteRect2Txt( m_CurInfoFile, rcNumRgn );
			m_CurInfoFile << _T(" : ") << endl;
			WriteRcArray2Txt( m_CurInfoFile, charArray );
			m_CurInfoFile << "2nd ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
			m_CurInfoFile << "2nd ST RES -- RcDis : " << nRcDis << endl;	
			int nSeqCnt = intArray.GetSize();
			for( int ii = 0; ii < nSeqCnt; ii++ )
			{
				m_CurInfoFile << intArray.GetAt(ii);
				if( ii != nSeqCnt-1 ) 
					m_CurInfoFile << " - ";
			}
			m_CurInfoFile << endl;
#endif

#ifdef TEST_CUT_CONF
			ObjRectArray charArrayAna;
			charArrayAna.Copy( charArray );
#endif
			bCut = PatternAnalyze( intArray, nType, charArray );
#ifdef TEST_CUT_CONF
			if( !bCut )
			{
				PatternAnalyze_ByConf(intArray, nType_Conf, charArray);
			}
			else
			{
				if( nType < 2 && charArray.GetSize() >= 9 )
				{
					PatternAnalyze_ByConf( intArray, nType_Conf, charArrayAna );
				}
			}
#endif
		}

	}

#ifdef TEST_CUT_CONF_V2//Michael Add 20080711
	if( !bCut )
	{
		CArray<int,int> intSeqCharsArray;
		CArray<int,int> intSeqDisArray;
		int nCharH = 0;
		int nCharW = 0;
		GetAverRcWH( charArray, nCharW, nCharH );
		GetRcDis( charArray, nRcDis );
		if( nRcDis < 2 )
		{
			nRcDis = 2;
		}
		RectsAnalyze_ByProb( charArray, allRcsArray, nCharW, nCharH, nRcDis, intSeqCharsArray, intSeqDisArray );
		
#ifdef TEST_HMM
		{
			int nType_HMM = -1;
			CArray<int,int> intCharsArray;
			intCharsArray.RemoveAll();
			intCharsArray.Copy( intSeqCharsArray );
			CArray<int,int> intDistsArray;
			intDistsArray.RemoveAll();
			intDistsArray.Copy( intSeqDisArray );
			ObjRectArray objsArray;
			objsArray.RemoveAll();
			objsArray.Copy( charArray );
#ifdef SAVE_HMM_MID_RES
			fstream fHmmInputInfo;
			CString strHmmInputFile = m_strHMMDebugDir + _T("HMM_INPUT.txt");
			fHmmInputInfo.open( strHmmInputFile, ios::out );
			WriteRectsAnaRes( fHmmInputInfo, CString( _T("HMM TEST MID RES") ), rcNumRgn, objsArray , nCharW, nCharH, nRcDis
				 , &intCharsArray, &intDistsArray );
			fHmmInputInfo.close();
#endif
			PatternAnalyze_ByHMM( intCharsArray, intDistsArray, nType_HMM, objsArray );
		}

#endif

#ifdef SAVE_CUTCHAR_INFO
		m_CurInfoFile << _T(" Rects Posibility Analyze : ") << endl;
		WriteRect2Txt( m_CurInfoFile, rcNumRgn );
		m_CurInfoFile << _T( " : " ) << endl;
		WriteRcArray2Txt( m_CurInfoFile, charArray );
		m_CurInfoFile << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
		m_CurInfoFile << " ST RES -- RcDis : " << nRcDis << endl;
		int nSeqCnt = intSeqCharsArray.GetSize();
		for( int ii = 0; ii < nSeqCnt; ii++ )
		{
			m_CurInfoFile << intSeqCharsArray.GetAt(ii);
			if( ii != nSeqCnt-1 ) 
				m_CurInfoFile << " - ";
		}
		m_CurInfoFile << endl;
		nSeqCnt = intSeqDisArray.GetSize();
		for( ii = 0; ii < nSeqCnt; ii++ )
		{
			m_CurInfoFile << intSeqDisArray.GetAt(ii);
			if( ii != nSeqCnt - 1 )
			{
				m_CurInfoFile << " - ";
			}
		}
		m_CurInfoFile << endl;
#endif//End -- SAVE_CUTCHAR_INFO

		int nType_Conf = -1;
		PatternAnalyze_ByConf( intSeqCharsArray, intSeqDisArray, nType_Conf, charArray );
 	}
#endif//End -- TEST_CUT_CONF_V2

	if( bCut && bBlack )
	{
		ObjRectArray RcsForAna;
		RcsForAna.Copy(charArray);
		int nTNested = AnaNestedRcs( RcsForAna, allRcsArray );
		if( nTNested >= 2 ) bCut = FALSE;
	}

	if( bCut )
	{
#ifdef CHECK_ERROR
		ASSERT( nType == 2 || nType == 0 );
#endif
		resArray.Copy( charArray );
		bSuc = TRUE;
		//bCut = CopyCharPos( charArray, rcNumRgn, nType, imgGrayOrg, bBlack );
	}

#ifdef SAVE_CUTCHAR_INFO
	m_CurInfoFile.close();
#endif

	return bSuc;
}

#ifdef SAVE_DEBUGINFO_FILES
void CConNumProc::WriteRectsAnaRes( fstream& curInfoFile, CString& strTitle, CRect rcRgn, ObjRectArray& charArray, int nCharW, int nCharH, int nRcDis
								    , CArray<int, int>* pnSeqCharsArray , CArray<int, int>* pnSeqDisArray )//Michael 20081119
{
	curInfoFile << strTitle << endl;
	WriteRect2Txt( curInfoFile, rcRgn );
	curInfoFile << _T( " : " ) << endl;
	WriteRcArray2Txt( curInfoFile, charArray );
	curInfoFile << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
	curInfoFile << " ST RES -- RcDis : " << nRcDis << endl;

	if( pnSeqCharsArray != NULL )
	{
//		int nSeqCnt = pnSeqCharsArray->GetSize();
//		curInfoFile << "SEQ Chars : ";
//		for( int ii = 0; ii < nSeqCnt; ii++ )
//		{
//			curInfoFile << pnSeqCharsArray->GetAt(ii);
//			if( ii != nSeqCnt-1 ) 
//				curInfoFile << " - ";
//		}
// 		curInfoFile << endl;
		WriteIntArray( curInfoFile, CString(_T("SEQ Chars : ")), pnSeqCharsArray );
	}

	if( pnSeqDisArray != NULL )
	{
//		int nSeqCnt = pnSeqDisArray->GetSize();
//		curInfoFile << "SEQ Dis  : ";
//		for( int ii = 0; ii < nSeqCnt; ii++ )
//		{
//			curInfoFile << pnSeqDisArray->GetAt(ii);
//			if( ii != nSeqCnt - 1 )
//			{
//				curInfoFile << " - ";
//			}
//		}
// 		curInfoFile << endl;
		WriteIntArray( curInfoFile, CString(_T("SEQ Dists : ")), pnSeqDisArray );
	}

}

void CConNumProc::WriteIntArray( fstream& curInfoFile, CString& strTitle, CArray<int, int>* pnArray )//Michael 20081119
{
	if( pnArray != NULL )
	{
		int nSeqCnt = pnArray->GetSize();
		curInfoFile << strTitle;
		for( int ii = 0; ii < nSeqCnt; ii++ )
		{
			curInfoFile << pnArray->GetAt(ii);
			if( ii != nSeqCnt - 1 )
			{
				curInfoFile << " - ";
			}
		}
		curInfoFile << endl;
	}
}
#endif


BOOL CConNumProc::CutForChars( CRect rcNumRgn, IMAGE imgGrayOrg, BOOL bBlack )
{
	BOOL bCut = FALSE;

	m_NumCharArray.RemoveAll();
	m_ABCCharArray.RemoveAll();
	m_rgnABCSeg = CRect(0,0,0,0);
	m_rgnNumSeg = CRect(0,0,0,0);

	int nPicWidth = ImageWidth(imgGrayOrg);
	int nPicHeight = ImageHeight(imgGrayOrg);
	
	int tt = 0;//Try times
	for( tt; tt < 2; tt++ )
	{
		if( bCut ) break;

		int nw = 3;//LLT Bin parameter
		int nInfW = 0;
		int nInfH = 0;

		switch( tt )
		{
		case 0 :
			nInfW = 20;
			nInfH = min( 20, rcNumRgn.Height() * 0.6 );
			nInfH = max( 10, nInfH );
			rcNumRgn.InflateRect( nInfW , nInfH );			
			RegulateRect( rcNumRgn, 0, 0, nPicWidth-1, nPicHeight-1 );			
			if( rcNumRgn.left < rcNumRgn.Width() * 0.1 )
			{
				rcNumRgn.left = 0;
			}

			//Michael Test 20080708 -- Change the sequence of Dajin and LLT
			rcNumRgn.InflateRect( nw, nw );
			RegulateRect( rcNumRgn, 0, 0, nPicWidth-1, nPicHeight-1 );
			//End -- Michael Test 20080708 -- Change the sequence of Dajin and LLT
			break;
			
		case 1:
			//rcNumRgn.InflateRect( nw, nw );//Michael Delete 20080708 -- Change the sequence of Dajin and LLT
			rcNumRgn.DeflateRect( nw, nw );//Michael Modify 20080708 -- Change the sequence of Dajin and LLT
			RegulateRect( rcNumRgn, 0, 0, nPicWidth-1, nPicHeight-1 );
			break;
		}

#ifdef SAVE_CUTCHAR_INFO//Save Cut NumSequence Region
		//m_strCurDebugDir = m_strSubDebugDir + "CutCharsByDJ";
		m_strCurDebugDir.Format( "%sCutCharsBy%dthBinMethod", m_strSubDebugDir, tt );
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		if( !bBlack )
			m_strCurDebugFile.Format("%s%s_%d_ACUTPIC%s",m_strCurDebugDir,fname,m_nCurProcID*2,CString(_T(".jpg")));
		else
			m_strCurDebugFile.Format("%s%s_%d_ACUTPIC%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,CString(_T(".jpg")));
 		SavePICPart( m_pImage, rcNumRgn, m_strCurDebugFile);
#endif

		IMAGE imgNumSeq_Gray = ImageAlloc( rcNumRgn.Width() + 1 , rcNumRgn.Height() + 1 );
		IMAGE imgNumSeq = ImageAlloc( rcNumRgn.Width() + 1 , rcNumRgn.Height() + 1 );

		GetImageRectPart( imgGrayOrg, imgNumSeq_Gray, rcNumRgn );
		if( bBlack )
		{
			ImgReverse( imgNumSeq_Gray, imgNumSeq_Gray );
		}
		CRect rcTmp( 0 , 0 , rcNumRgn.Width() , rcNumRgn.Height() );

		switch( tt )
		{
		//case 0:
		case 1:
			DajinBin( imgNumSeq_Gray , imgNumSeq , rcTmp );
			break;
		//case 1:
		case 0:
			ImgReverse( imgNumSeq_Gray, imgNumSeq_Gray );
			LLTFast(imgNumSeq_Gray, imgNumSeq, 3 , 8);
			ImgReverse( imgNumSeq_Gray, imgNumSeq_Gray );
			break;
		}
		
		int nType = -1;
		ObjRectArray charArray;
		bCut = HorRgnCutStrategy( imgNumSeq_Gray, imgNumSeq, rcNumRgn, charArray, nType, bBlack );
		
		if( bCut )
		{
#ifdef CHECK_ERROR
			ASSERT( nType == 2 || nType == 0 );
#endif
			bCut = CopyCharPos( charArray, rcNumRgn, nType, imgGrayOrg, bBlack );
		}
		
		ImageFree( imgNumSeq_Gray );
		ImageFree( imgNumSeq );
	}



	
	if( bCut )
	{
		ASSERT( m_NumCharArray.GetSize() == 7 );
		CRect rcCheck = m_NumCharArray.GetAt(6);
		m_bBlack = bBlack;
		int nCharH = 0;
		int nCharW = 0;
		GetAverRcWH( m_NumCharArray, nCharW, nCharH );
		if( GetTheCheckRect( rcCheck, imgGrayOrg, nCharH, nCharW ) )
		{
			m_NumCharArray.SetAt( 6, rcCheck );
		}
	}

#ifdef SAVE_CUTCHAR_INFO
	{
		m_strCurDebugDir = m_strSubDebugDir + "CharPos";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		int nCnt = 0;
		do {
			nCnt++;
			m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,nCnt,CString(_T(".jpg")));
		} while( _access(m_strCurDebugFile,0) != -1);
		
		DrawObjRectArray( m_pImageDis, m_NumCharArray, 255, 0, 0 );
		DrawObjRectArray( m_pImageDis, m_ABCCharArray, 255, 0, 0 );
		m_pImageDis->Save( m_strCurDebugFile );
	}
#endif

	bCut = m_NumCharArray.GetSize() > 0 && m_ABCCharArray.GetSize() > 0;

	return bCut;
}

BOOL CConNumProc::CopyCharPos( ObjRectArray& charArray, CRect rcNumRgn, int nType, IMAGE imgGrayOrg, BOOL bBlack )
{
	m_NumCharArray.RemoveAll();
	m_ABCCharArray.RemoveAll();
	m_rgnABCSeg = CRect(0,0,0,0);
	m_rgnNumSeg = CRect(0,0,0,0);


	if( nType == 2)
	{
		ASSERT( charArray.GetSize() >= 11 );
		for( int ii = 0 ; ii < 4 ; ii++ )
		{
			CRect rcCur = charArray.GetAt(ii);
			m_ABCCharArray.Add( rcCur );
//#ifdef SAVE_CUTCHAR_INFO				
//				m_pImageDis->PutImage( rcNumRgn, imgNumSeq_Gray[0], imgNumSeq_Gray[0], imgNumSeq_Gray[0] );
//				DrawObjRectArray( m_pImageDis, m_ABCCharArray, 255, 0, 0 );
//#endif
		}
		m_rgnABCSeg = rcNumRgn;
		for( ii = 0 ; ii < 7 ; ii++ )
		{
			CRect rcCur = charArray.GetAt(ii+4);
			m_NumCharArray.Add( rcCur );
		}
		m_rgnNumSeg = rcNumRgn;
	}
	else if( nType == 0 )
	{
		ASSERT( charArray.GetSize() >= 7 );
		for( int nId = 0 ; nId < 7 ; nId++ )
		{
			CRect rcCur = charArray.GetAt(nId);
			m_NumCharArray.Add( rcCur );
		}
		m_rgnNumSeg = rcNumRgn;

			
		SearchABCChars( imgGrayOrg, bBlack );

	}
	else
	{
//#ifdef SAVE_CUTCHAR_INFO
//			//m_pImageDis->PutImage( rcNumRgn, imgNumSeq_Gray[0], imgNumSeq_Gray[0], imgNumSeq_Gray[0] );
//			DrawObjRectArray(m_pImageDis,charArray,255,255,255);
//#endif
		return FALSE;
	}

	BOOL bCut = m_NumCharArray.GetSize() > 0 && m_ABCCharArray.GetSize() > 0;
	return bCut;
	
}

BOOL CConNumProc::SearchABCChars( IMAGE imgGrayOrg, BOOL bBlack )
{
#ifdef CHECK_ERROR
	ASSERT( m_NumCharArray.GetSize() == 7 );
#endif
	if( m_NumCharArray.GetSize() != 7 ) return FALSE;

	int nPicWidth = ImageWidth( imgGrayOrg );
	int nPicHeight = ImageHeight( imgGrayOrg );

	int nCharW = 0;
	int nCharH = 0;
	int nRcDis = 0;
	GetAverRcWH( m_NumCharArray, nCharW, nCharH );
	GetRcDis( m_NumCharArray, nRcDis );

	CRect rcABCRgn = CRect(0,0,0,0);
	if( GetABCSeqRgn(rcABCRgn, CRect(0,0,nPicWidth-1,nPicHeight-1), m_NumCharArray, nPicWidth, nPicHeight ) )
	{
#ifdef SAVE_CUTCHAR_INFO//Save Cut ABCSequence Region
		m_strCurDebugDir = m_strSubDebugDir + "ABCRgnCutChars";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		int nCnt = 0;
		do
		{
			nCnt++;
			m_strCurDebugFile.Format("%s%s_%d_ABCTRGN%s",m_strCurDebugDir,fname,nCnt,CString(_T(".jpg")));
		}while( _access(m_strCurDebugFile, 0) != -1 );
 		SavePICPart( m_pImage, rcABCRgn, m_strCurDebugFile);
#endif

		//CutABCRgn( imgGrayOrg, rcABCRgn, bBlack );
		CutABCRgn( imgGrayOrg, rcABCRgn, bBlack, nCharW, nCharH, nRcDis );

		if( m_ABCCharArray.GetSize() == 0 )
		{
#ifdef SAVE_CUTCHAR_INFO
			m_strCurDebugDir = m_strSubDebugDir + "ABCRgnCutCharsByLLT\\";
			CreateDirectory( m_strCurDebugDir, NULL );
#endif
			//CutABCRgnByLLT( imgGrayOrg, rcABCRgn, bBlack );
			CutABCRgnByLLT( imgGrayOrg, rcABCRgn, bBlack, nCharW, nCharH, nRcDis );
		}

	}
	
	if( m_ABCCharArray.GetSize() == 0 )
	{
		if( GetLABCSeqRgn(rcABCRgn, CRect(0,0,nPicWidth-1,nPicHeight-1), m_NumCharArray, nPicWidth, nPicHeight ) )
		{
#ifdef SAVE_CUTCHAR_INFO//Save Cut ABCSequence Region
			m_strCurDebugDir = m_strSubDebugDir + "ABCRgnCutChars";
			CreateDirectory( m_strCurDebugDir, NULL );
			m_strCurDebugDir += _T("\\");
			int nCnt = 0;
			do
			{
				nCnt++;
				m_strCurDebugFile.Format("%s%s_%d_ABCLRGN%s",m_strCurDebugDir,fname,nCnt,CString(_T(".jpg")));
			}while( _access(m_strCurDebugFile, 0) != -1 );
 			SavePICPart( m_pImage, rcABCRgn, m_strCurDebugFile);
#endif

			//CutABCRgn( imgGrayOrg, rcABCRgn, bBlack );
			CutABCRgn( imgGrayOrg, rcABCRgn, bBlack, nCharW, nCharH, nRcDis );

			if( m_ABCCharArray.GetSize() == 0 )
			{
#ifdef SAVE_CUTCHAR_INFO
				m_strCurDebugDir = m_strSubDebugDir + "ABCRgnCutCharsByLLT\\";
				CreateDirectory( m_strCurDebugDir, NULL );
#endif
				//CutABCRgnByLLT( imgGrayOrg, rcABCRgn, bBlack );
				CutABCRgnByLLT( imgGrayOrg, rcABCRgn, bBlack, nCharW, nCharH, nRcDis );
			}

		}
	}

	if( m_ABCCharArray.GetSize() == 4 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConNumProc::PatternAnalyze( CArray<int,int>& intArray, int& nType, ObjRectArray& charArray )
{
	ObjRectArray charArrayBak;
	charArrayBak.Copy(charArray);

	BOOL bCut = FALSE;

	//nType = GetNumSeqType( intArray );
	int nDrop = 0;
	CArray<int,int> intAnaArray;
	intAnaArray.Copy( intArray );
	nType = GetNumSeqType( intAnaArray, nDrop );

	//Michael Add 2008-4-15 -- For RZDU Pattern Analyze
	//Process for the "1-1-1-1-6-1" case
	//if( nType != -1 && nDrop >= 4 )//Original Judgement
	if( nType != -1 && nType < 2 && nDrop >= 4 )//Michael Change This Judgement 2008-4-21
	{
		int nTAnaCnt = intArray.GetSize();
		int nDropSeqs = 0;// nDropSeqs is not equal to nDrop;
		int nDropSeqLen = 0;
		for( nDropSeqs = 0; nDropSeqs < nTAnaCnt; nDropSeqs++ )
		{
			nDropSeqLen += intArray.GetAt(nDropSeqs);
			if( nDropSeqLen >= nDrop )
			{
#ifdef CHECK_ERROR
				ASSERT( nDropSeqLen == nDrop );
#endif
				break;
			}
		}
		nDropSeqs++;

#ifdef CHECK_ERROR
		ASSERT( nDropSeqs < nTAnaCnt );
#endif
		
		int nn = 0;
		if( nDropSeqs >= 4 )
		{
			for( nn = nDropSeqs - 1; nn >= nDropSeqs - 4; nn-- )
			{
				if( nn >= nTAnaCnt )
				{
				#ifdef CHECK_ERROR
					ASSERT( nn < nTAnaCnt );
				#endif
					break;
				}
				int nCurSeqLen = intArray.GetAt(nn);
				if( nCurSeqLen != 1 ) break;
			}

			if( nn < nDropSeqs - 4 )//Maybe the Pattern For "RZDU"
			{
				nType = nType + 2;
				nDrop = nDrop - 4;
			}
		}

	}
	//End -- For RZDU Pattern Analyze

	if( nType != -1 )
	{
		for( int nn =0; nn < nDrop; nn++ )
		{
			charArray.RemoveAt(0);
		}
	}
	
	if( nType == -1 )
	{
		bCut = FALSE;
	}
	else
	{
		bCut = TRUE;
	}

	if( nType == 1 || nType == 3 )
	{
		int nCnt = charArray.GetSize();
		if( (nType == 1) && nCnt > 8 )
		{
			for( int nn = nCnt - 1; nn >= 8; nn-- )
			{
				charArray.RemoveAt(nn);
			}
			nCnt = 8;
			ASSERT( nCnt == charArray.GetSize() );
		}
		if( (nType == 3) && nCnt > 12 )
		{
//BUG FOUND 2008-04-17 -- Call CArray RemoveAt Bug
//			for( int nn = 8; nn < nCnt; nn++ )
//			{
//				charArray.RemoveAt(nn);
// 			}
//End -- Call CArray RemoveAt Bug
//Michael Modify 2008-04-17 -- Remove Call CArray RemoveAt Bug
			for( int nn = nCnt - 1; nn >= 12; nn-- )
			{
				charArray.RemoveAt(nn);
			}
//End -- Remove Call CArray RemoveAt Bug
			nCnt = 12;
			ASSERT( nCnt == charArray.GetSize() );
		}
		CRect rcBefore = charArray.GetAt( nCnt - 3 );
		CRect rcPre = charArray.GetAt( nCnt - 2 );
		CRect rcLast = charArray.GetAt( nCnt - 1 );
		
		int nDis = rcLast.left - rcPre.right;
		int nDisPre = rcPre.left - rcBefore.right; 
		int nRemoved = 0;
		if( nDis <= 1 || nDisPre >= 3 * nDis )
		{
			if( (float)rcPre.Height() / (float)rcPre.Width() > 4.5f )
			{
				charArray.RemoveAt( nCnt-2 );
				nCnt--;
				nRemoved++;
			}
			if( (float)rcLast.Height() / (float)rcLast.Width() > 4.5f )
			{
				charArray.RemoveAt( nCnt-1 );
				nCnt--;
				nRemoved++;
			}
		}
		if( nDis < 0 && nRemoved == 0 )
		{
			CRect rcMerge;
			rcMerge.left = rcPre.left;
			rcMerge.right = rcLast.right;
			rcMerge.top = min( rcPre.top, rcLast.top );
			rcMerge.bottom = max( rcPre.bottom, rcPre.bottom );
			charArray.RemoveAt(nCnt-1);
			nCnt--;
			nRemoved++;
			charArray.SetAt(nCnt-1,rcMerge);
		}
		if( nRemoved == 1 )
		{
			nType--;
		}
		else
		{
			bCut = FALSE;
		}
	}

	if( !bCut )
	{
		charArray.RemoveAll();
		charArray.Copy(charArrayBak);
	}

	return bCut;
}

#ifdef TEST_HMM
BOOL CConNumProc::PatternAnalyze_ByHMM( CArray<int,int>& intArray, CArray<int,int>& disArray, int& nType, ObjRectArray& charArray )//Belief Propagation Analyze
{
	int i = 0;
	int j = 0;
	int k = 0;
	
	int nCharsCnt = charArray.GetSize();
	int nInCharSeqsCnt = intArray.GetSize();
	int nInDisCnt = disArray.GetSize();

	if( nCharsCnt >= 11 ) return FALSE;
	if( nCharsCnt <= 7 ) return FALSE;
 	if( nInCharSeqsCnt != nInDisCnt + 1 ) return FALSE;
	for( i = 0; i < nInDisCnt; i++ )//Michael Add 20081209 -- May meet value 9999, which'll cause large time cost;
	{
		if( disArray.GetAt(i) >= 5 )
		{
			return FALSE;
		}
	}
	
	int nMisChars = 11 - nCharsCnt;//20081127, refer to GetNumSeqType_ByConf(3 paras), only tackle 11 chars' situation
	
	CRect rcMis = CRect(0,0,0,0);

	BOOL bPos = FALSE;

	ObjRectArray charArrayAna;
	charArrayAna.Copy(charArray);

	int nDrop = 0;
	CArray<int,int> intAnaArray;
	intAnaArray.Copy( intArray );

	CArray<int,int> disAnaArray;
	disAnaArray.Copy( disArray );

	
	int nCharH = 0;
	int nCharW = 0;
	int nRcDis = 0;
	GetAverRcWH( charArrayAna, nCharW, nCharH );
	GetRcDis( charArrayAna, nRcDis );
	
	int* nTopBorders = new int[nCharsCnt];
	int* nBtmBorders = new int[nCharsCnt];
	int* nLeftBorders = new int[nCharsCnt];
	int* nRightBorders = new int[nCharsCnt];
	memset( nTopBorders, 0, nCharsCnt * sizeof(int) );
	memset( nBtmBorders, 0, nCharsCnt * sizeof(int) );
	memset( nLeftBorders, 0, nCharsCnt * sizeof(int) );
	memset( nRightBorders, 0, nCharsCnt * sizeof(int) );
	for( i = 0; i < nCharsCnt; i++ )
	{
		CRect rcCur = charArrayAna.GetAt(i);
		nTopBorders[ i ] = rcCur.top;
		nBtmBorders[ i ] = rcCur.bottom;
		nLeftBorders[ i ] = rcCur.left;
		nRightBorders[ i ] = rcCur.right;
	}
	
	objFrames objInfo;
	objInfo.nChars = nCharsCnt;
	objInfo.averDis = nRcDis;
	objInfo.averWidth = nCharW;
	objInfo.averHeight = nCharH;
	objInfo.inputCharsTopBorder = nTopBorders;
	objInfo.inputCharsBtmBorder = nBtmBorders;
	objInfo.inputCharsLeftBorder = nLeftBorders;
	objInfo.inputCharsRightBorder = nRightBorders;
	
	int nNodes = 0;
	int nPosModes = 0;
	ObNode* obNodes = NULL;
	HiNode* hiNodes = NULL;
	int* insertModes = NULL;
	int nSeqs = 0;
	ExNodesFea( obNodes, hiNodes, nNodes, nPosModes, insertModes, nSeqs, objInfo, intAnaArray, disAnaArray );//Write in nodes info
	CBfPropa bf;
#ifdef SAVE_HMM_MID_RES
	ofstream fHmmInfo;
	CString strHMMInfoFile = m_strHMMDebugDir + _T( "HmmInfo.txt" );
	fHmmInfo.open( strHMMInfoFile, ios::out | ios::trunc );
#endif
	int nSelModeID = -1;
	float fBestConf = -0.0f;
	for( i = 0; i < nPosModes; i++ )
	{
		ObNode* pCurObNodes = obNodes + i * nNodes;
		ASSERT( pCurObNodes == &( obNodes[i * nNodes] ) );
		HiNode* pCurHiNodes = hiNodes + i * nNodes;
		ASSERT( pCurHiNodes == &( hiNodes[i * nNodes] ) );
		bf.SetStates( pCurObNodes,  pCurHiNodes, nNodes );
		bf.beliefPropagation();
		
		int * res = NULL;
		int nlen = 0;
		float fConf = 0.0f;
		bf.GetResult( res, nlen, fConf );
		
		if( i == 0 )
		{
			nSelModeID = i;
			fBestConf = fConf;
		}
		else if( fConf < fBestConf )
		{
			nSelModeID = i;
			fBestConf = fConf;
		}
		
#ifdef SAVE_HMM_MID_RES
		fHmmInfo << i << "th mode's sels\t:" << endl;
		for( j = 0; j < nSeqs; j++ )
		{
			fHmmInfo << insertModes[ i * nSeqs + j];
			if( j != nSeqs - 1 )
				fHmmInfo << " - ";
		}
		fHmmInfo << endl;
		for( j = 0; j < nlen; j++ )
		{
			fHmmInfo << "( ";
			int nFeaLen = hiNodes[ i * nNodes + j ].nFeaLen;
			for( k = 0; k < nFeaLen; k++ )
			{
				fHmmInfo << hiNodes[ i * nNodes + j].pfFea[ res[j] * nFeaLen + k ] << "; ";
			}
			fHmmInfo << ")";
			fHmmInfo << endl;
		}
		
		fHmmInfo << i << "th mode's conf\t:\t" << fConf << endl;
		fHmmInfo << endl;
#endif
		
		delete[] res;
	}
	
	if( nPosModes > 0 )
	{
		ASSERT( nSelModeID >= 0 );		
	}
#ifdef SAVE_HMM_MID_RES
	fHmmInfo.close();
#endif
	
	
	int nValChars = 0;
	int nTChars = 0;
	ObjRectArray ABCCharArray_ByConf;
	ObjRectArray NumCharArray_ByConf;
	
	int nIdOff = nSelModeID * nSeqs;
	for( i = 0; i < nSeqs; i++ )
	{
		int nCurSeqChars = insertModes[ nIdOff + i ];
		if( i % 2 == 0 )
		{
			for( j = 0; j < nCurSeqChars; j++ )
			{
				if( nTChars < 4 )
				{
					ABCCharArray_ByConf.Add( rcMis );
				}
				else
				{
					NumCharArray_ByConf.Add( rcMis );
				}
				nTChars++;
			}
		}
		else
		{
			for( j = 0; j < nCurSeqChars; j++ )
			{
				CRect rcCurValChar = charArrayAna.GetAt( nValChars );
				if( nTChars < 4 )
				{
					ABCCharArray_ByConf.Add( rcCurValChar );
				}
				else
				{
					NumCharArray_ByConf.Add( rcCurValChar );
				}
				nTChars++;
				nValChars++;
			}
		}
		
	}
	
	float fCutConf = 0.8f - ( nMisChars / (float)11 ) * 0.8;//Output the confidence anyway??????No ristrict conditions?????
	
	if( fCutConf > m_fCutConf )
	{
		m_NumCharArray_ByConf.RemoveAll();
		m_ABCCharArray_ByConf.RemoveAll();
		m_NumCharArray_ByConf.Copy( NumCharArray_ByConf );
		m_ABCCharArray_ByConf.Copy( ABCCharArray_ByConf );
		m_fCutConf = fCutConf;

#ifdef SAVE_HMM_MID_RES
		CString strHMMInfoFile_MaxProbBak = m_strHMMDebugDir + _T( "HmmInfo_MaxProb.txt" );
		CopyFile( strHMMInfoFile, strHMMInfoFile_MaxProbBak, FALSE );
		CString strHmmInputFile = m_strHMMDebugDir + _T("HMM_INPUT.txt");
		CString strHmmInputFile_MaxProbBak = m_strHMMDebugDir + _T("HMM_INPUT_MaxProb.txt");
		CopyFile( strHmmInputFile, strHmmInputFile_MaxProbBak, FALSE );
#endif
	}
		
	for( i = 0; i < nNodes * nPosModes; i++ )
	{
		freeObNode( obNodes[i] );
		freeHiNode( hiNodes[i] );
	}
	delete[] insertModes;
	insertModes = NULL;
	delete[] obNodes;
	obNodes = NULL;
	delete[] hiNodes;
	hiNodes = NULL;
	
	delete[] nTopBorders;
	nTopBorders = NULL;
	delete[] nBtmBorders;
	nBtmBorders = NULL;
	delete[] nLeftBorders;
	nLeftBorders = NULL;
	delete[] nRightBorders;
	nRightBorders = NULL;

	

	return bPos;	
}
#endif

#ifdef TEST_CUT_CONF
BOOL CConNumProc::PatternAnalyze_ByConf( CArray<int,int>& intArray, CArray<int,int>& disArray, int& nType, ObjRectArray& charArray )
{
	CRect rcMis = CRect(0,0,0,0);

	BOOL bPos = FALSE;

	ObjRectArray charArrayAna;
	charArrayAna.Copy(charArray);

	int nDrop = 0;
	CArray<int,int> intAnaArray;
	intAnaArray.Copy( intArray );
	
	nType = GetNumSeqType_ByConf( intAnaArray, disArray, nDrop );
	
	BOOL bFind = FALSE;
	int nId = 0;
	if( nType > 0 )
	{
		for( nId =0; nId < nDrop; nId++ )
		{
			charArrayAna.RemoveAt(0);
		}

		int nCnt = charArrayAna.GetSize();

		int nCnt1 = intAnaArray.GetSize();
		int nCnt2 = disArray.GetSize();
#ifdef ERROR_NEED_MEND
		ASSERT( nCnt2 == nCnt1 - 1 );
#endif
		
		if( nCnt2 == nCnt1 - 1 )
		{
			int nValChars = 0;
			int nTChars = 0;
			int nMisChars = 0;
			ObjRectArray ABCCharArray_ByConf;
			ObjRectArray NumCharArray_ByConf;
			float fCutConf = 0.0f;
			for( nId = 0; nId < nCnt1; nId++ )
			{
				int nNum = intAnaArray.GetAt(nId);

				for( int nn = 0; nn < nNum; nn++ )
				{
					CRect rcCur = charArrayAna.GetAt( nValChars );
					if( nTChars < 4 )
					{
						ABCCharArray_ByConf.Add( rcCur );
					}
					else
					{
						NumCharArray_ByConf.Add( rcCur );
					}
					nValChars++;
					nTChars++;
				}

				if( nId != nCnt1 - 1 )
				{
					nNum = 0;
					if( nId < nCnt2 )
					{
						nNum = disArray.GetAt(nId);
					}
					for( int nn = 0; nn < nNum; nn++ )
					{
						CRect rcCur = charArrayAna.GetAt( nValChars );
						if( nTChars < 4 )
						{
							ABCCharArray_ByConf.Add( rcMis );
						}
						else
						{
							NumCharArray_ByConf.Add( rcMis );
						}
						nMisChars++;
						nTChars++;
					}
				}
				
			}//End -- for( nId = 0; nId < nCnt1; nId++ )

			if( nTChars < 11 )
			{
				for( int nn = 11; nn > nTChars; nn-- )
				{
					if( nn >= 4 )
					{
						NumCharArray_ByConf.Add( rcMis );
					}
					else
					{
						ABCCharArray_ByConf.Add( rcMis );
					}
					nMisChars++;
				}
				nTChars = 11;
			}

			fCutConf = 0.8f - ( nMisChars / (float)11 ) * 0.8;

			if( fCutConf > m_fCutConf )
			{
				m_NumCharArray_ByConf.RemoveAll();
				m_ABCCharArray_ByConf.RemoveAll();
				m_NumCharArray_ByConf.Copy( NumCharArray_ByConf );
				m_ABCCharArray_ByConf.Copy( ABCCharArray_ByConf );
				m_fCutConf = fCutConf;
			}
		}//End -- if( nCnt2 == nCnt1 - 1 )

		


//		if( nType == 3 )
//		{
//#ifdef CHECK_ERROR
//			ASSERT( nCnt >= 10 );			
//#endif
//			if( nCnt >= 10 )
//			{
//				if( nCnt > 10 )
//				{
//					for( nId = nCnt - 1; nId > 10; nId-- )
//					{
//						charArrayAna.RemoveAt(nId);
//					}
//				}
//
//				for( nId = 0; nId < 9; nId++ )//Check Abnormality
//				{
//					CRect rcCur = charArrayAna.GetAt(nId);
//					CRect rcNext = charArrayAna.GetAt(nId+1);
//
//					int h1 = rcCur.Height();
//					int w1 = rcCur.Width();
//					int h2 = rcCur.Height();
//					int w2 = rcCur.Width();
//
//					float fDifHRatio = abs( h1 - h2 ) / (float)max( h1, h2 );
//					float fDifWRatio = abs( w1 - w2 ) / (float)max( w1, w2 );
//					if( fDifHRatio > 0.4 )
//					{
//						break;
//					}
//				}
//				
//				if( nId == 9 )//No Abnormality
//				{
//					int nMissPos = -1;
//					int nSeqCnt = intAnaArray.GetSize();
//					int nCurSeqRcsCnt = intAnaArray[0];
//					if( nCurSeqRcsCnt > 4 || nCurSeqRcsCnt < 3 )
//					{
//						nMissPos = nCurSeqRcsCnt;
//					}
//
//					if( nCurSeqRcsCnt == 4 )
//					{
//#ifdef CHECK_ERROR
//						ASSERT( nSeqCnt > 1 );
//#endif
//						nCurSeqRcsCnt = intAnaArray[1];
//						nMissPos = 4 + nCurSeqRcsCnt;
//					}
//
//					if( nCurSeqRcsCnt == 3 )
//					{
//						nMissPos = 0;
//					}
//
//					if( nMissPos >= 0 && m_fCutConf < 0.6)
//					{
//						int nCurID = 0;
//						m_ABCCharArray_ByConf.RemoveAll();
//						m_NumCharArray_ByConf.RemoveAll();
//						for( nId = 0; nId < 10; nId++ )
//						{
//							if( nId == nMissPos )
//							{
//								if( nCurID < 4 )
//								{
//									m_ABCCharArray_ByConf.Add( rcMis );
//								}
//
//								if( nCurID >= 4 )
//								{
//									m_NumCharArray_ByConf.Add( rcMis );
//								}
//
//								nId--;
//								nMissPos = -1;
//							}
//							else
//							{
//								CRect rcCur = charArrayAna.GetAt(nId);
//								if( nCurID < 4 )
//								{
//									m_ABCCharArray_ByConf.Add( rcCur );
//								}
//
//								if( nCurID >= 4 )
//								{
//									m_NumCharArray_ByConf.Add( rcCur );
//								}
//							}
//							nCurID++;
//						}
//
//						if( nMissPos == 10 )
//						{
//#ifdef CHECK_ERROR
//							ASSERT(nCurID==10);
//#endif
//							m_NumCharArray_ByConf.Add(rcMis);
//						}
//					
//						m_fCutConf = 0.6;
//					}
//
//				}
//			}
// 		}//End -- if( nType == 3 )


	}

	return bPos;
}

BOOL CConNumProc::PatternAnalyze_ByConf( CArray<int,int>& intArray, int& nType, ObjRectArray& charArray )
{
	CRect rcMis = CRect(0,0,0,0);

	BOOL bPos = FALSE;

	ObjRectArray charArrayAna;
	charArrayAna.Copy(charArray);

	int nDrop = 0;
	CArray<int,int> intAnaArray;
	intAnaArray.Copy( intArray );
	nType = GetNumSeqType_ByConf( intAnaArray, nDrop );
	
	BOOL bFind = FALSE;
	int nId = 0;
	if( nType > 0 )
	{
		for( nId =0; nId < nDrop; nId++ )
		{
			charArrayAna.RemoveAt(0);
		}

		int nCnt = charArrayAna.GetSize();

		if( nType == 3 )
		{
#ifdef CHECK_ERROR
			ASSERT( nCnt >= 10 );			
#endif
			if( nCnt >= 10 )
			{
				if( nCnt > 10 )
				{
					for( nId = nCnt - 1; nId > 10; nId-- )
					{
						charArrayAna.RemoveAt(nId);
					}
				}

				for( nId = 0; nId < 9; nId++ )//Check Abnormality
				{
					CRect rcCur = charArrayAna.GetAt(nId);
					CRect rcNext = charArrayAna.GetAt(nId+1);

					int h1 = rcCur.Height();
					int w1 = rcCur.Width();
					int h2 = rcCur.Height();
					int w2 = rcCur.Width();

					float fDifHRatio = abs( h1 - h2 ) / (float)max( h1, h2 );
					float fDifWRatio = abs( w1 - w2 ) / (float)max( w1, w2 );
					if( fDifHRatio > 0.4 )
					{
						break;
					}
				}
				
				if( nId == 9 )//No Abnormality
				{
					int nMissPos = -1;
					int nSeqCnt = intAnaArray.GetSize();
					int nCurSeqRcsCnt = intAnaArray[0];
					if( nCurSeqRcsCnt > 4 || nCurSeqRcsCnt < 3 )
					{
						nMissPos = nCurSeqRcsCnt;
					}

					if( nCurSeqRcsCnt == 4 )
					{
#ifdef CHECK_ERROR
						ASSERT( nSeqCnt > 1 );
#endif
						nCurSeqRcsCnt = intAnaArray[1];
						nMissPos = 4 + nCurSeqRcsCnt;
					}

					if( nCurSeqRcsCnt == 3 )
					{
						nMissPos = 0;
					}

					if( nMissPos >= 0 && m_fCutConf < 0.6)
					{
						int nCurID = 0;
						m_ABCCharArray_ByConf.RemoveAll();
						m_NumCharArray_ByConf.RemoveAll();
						for( nId = 0; nId < 10; nId++ )
						{
							if( nId == nMissPos )
							{
								if( nCurID < 4 )
								{
									m_ABCCharArray_ByConf.Add( rcMis );
								}

								if( nCurID >= 4 )
								{
									m_NumCharArray_ByConf.Add( rcMis );
								}

								nId--;
								nMissPos = -1;
							}
							else
							{
								CRect rcCur = charArrayAna.GetAt(nId);
								if( nCurID < 4 )
								{
									m_ABCCharArray_ByConf.Add( rcCur );
								}

								if( nCurID >= 4 )
								{
									m_NumCharArray_ByConf.Add( rcCur );
								}
							}
							nCurID++;
						}

						if( nMissPos == 10 )
						{
#ifdef CHECK_ERROR
							ASSERT(nCurID==10);
#endif
							m_NumCharArray_ByConf.Add(rcMis);
						}
					
						m_fCutConf = 0.6;
					}

				}
			}
		}
	}

	return bPos;
}
#endif

BOOL CConNumProc::CutForCharsByLLT( CRect rcNumRgn, IMAGE imgGrayOrg, BOOL bBlack, ObjRectArray& charArrayRes, ObjRectArray& allRcsArray, CArray<int,int>& intArray )
{
	BOOL bCut = FALSE;

	int nPicWidth = ImageWidth(imgGrayOrg);
	int nPicHeight = ImageHeight(imgGrayOrg);

	int nw = 5;
	rcNumRgn.InflateRect( nw, nw );
	RegulateRect( rcNumRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

#ifdef SAVE_CUTCHAR_INFO//Save Cut NumSequence Region
	if( !bBlack )
		m_strCurDebugFile.Format("%s%s_%d_CUTPIC%s",m_strCurDebugDir,fname,m_nCurProcID*2,CString(_T(".jpg")));
	else
		m_strCurDebugFile.Format("%s%s_%d_CUTPIC%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,CString(_T(".jpg")));
 	SavePICPart( m_pImage, rcNumRgn, m_strCurDebugFile);
#endif

	IMAGE imgNumSeq_Gray = ImageAlloc( rcNumRgn.Width() + 1 , rcNumRgn.Height() + 1 );
	IMAGE imgNumSeq = ImageAlloc( rcNumRgn.Width() + 1 , rcNumRgn.Height() + 1 );

	GetImageRectPart( imgGrayOrg, imgNumSeq_Gray, rcNumRgn );
	if( bBlack )
	{
		ImgReverse( imgNumSeq_Gray, imgNumSeq_Gray );
	}

	CRect rcTmp( 0 , 0 , rcNumRgn.Width() , rcNumRgn.Height() );
	
	ImgReverse( imgNumSeq_Gray, imgNumSeq_Gray );
	LLTFast(imgNumSeq_Gray, imgNumSeq, 3 , 8);
	//LLTFast(imgNumSeq_Gray, imgNumSeq, 6 , 8);
	ImgReverse( imgNumSeq_Gray, imgNumSeq_Gray );

	ObjRectArray charArray;
	allRcsArray.RemoveAll();
	GetObjRect( imgNumSeq, charArray, allRcsArray );
	SortRect( charArray );

	int nCharW = 0;
 	int nCharH = 0;
 	GetAverRcWH( charArray, nCharW, nCharH );

	int nRcDis = 0;
	GetRcDis( charArray, nRcDis );

	//MergeApartRcs( charArray, allRcsArray, nCharW, nCharH, nRcDis );

	BOOL bGot = FALSE;
 	if( RectsAnalyze( charArray, allRcsArray, nCharW, nCharH, nRcDis, intArray ) )
	{
		charArrayRes.Copy(charArray);
		TransferRcArrayFromL2W( charArrayRes, rcNumRgn );
		bGot = TRUE;
	}
	TransferRcArrayFromL2W( allRcsArray, rcNumRgn );

#ifdef SAVE_CUTCHAR_INFO//Save Cut NumSequence Region
	if( !bBlack )
		m_strCurDebugFile.Format("%s%s_%d_LLTBIN%s",m_strCurDebugDir,fname,m_nCurProcID*2,CString(_T(".jpg")));
	else
		m_strCurDebugFile.Format("%s%s_%d_LLTBIN%s",m_strCurDebugDir,fname,m_nCurProcID*2+1,CString(_T(".jpg")));
 	DrawObjRectArray( imgNumSeq, charArray );
	ImageSave( imgNumSeq, m_strCurDebugFile );
#endif

	if( bGot )
	{
		bCut = TRUE;
	}

	if( bCut && bBlack )
	{
		ObjRectArray RcsForAna;
		RcsForAna.Copy(charArrayRes);
		int nTNested = AnaNestedRcs( RcsForAna, allRcsArray );
		if( nTNested >= 2 ) bCut = FALSE;
	}

	ImageFree( imgNumSeq_Gray );
	ImageFree( imgNumSeq );

	return bCut;
}

int CConNumProc::AnaNestedRcs( ObjRectArray& charArray, ObjRectArray& allRcArray )
{
	int i = 0;
	int j = 0;

	int nTNested = 0;
	
	SortRect( charArray );
	SortRect( allRcArray );

	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcArray.GetSize();

	for( i = 0; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt(i);

		int w = rcCur.Width();
		int h = rcCur.Height();

		if( w < 5 ) continue;
		if( h < 5 ) continue;
		
		rcCur.InflateRect( -1, -1 );

		for( j = 0; j < nCnt2; j++ )
		{
			CRect rcAna = allRcArray.GetAt(j);
			int w2 = rcAna.Width()+1;
			int h2 = rcAna.Height()+1;
			if( w2 * h2 > 20 && RcInRgn( rcAna, rcCur ) )
			{
				break;
			}
		}

		if( j != nCnt2 ) nTNested++;
	}

	return nTNested;
}

BOOL CConNumProc::CutForMulRgnsPolicy( ObjRectArray &rcRgns, IMAGE imgGray, ObjRectArray& rcArrayRes, int nthBinMed, BOOL bBlack )
{
	if( nthBinMed != 0 && nthBinMed != 1 ) return FALSE;

	int nCnt = rcRgns.GetSize();
	if( nCnt < 2 ) return FALSE;

	if( bBlack )
		ImgReverse( imgGray, imgGray );

	BOOL bCut = FALSE;
	
	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);
	
	ObjRectArray rcArrayRes_All;
	for( int i = 0; i < nCnt; i++ )
	{
		int nw = 0;
		//if( nthBinMed == 1 ) nw = 3;
		if( nthBinMed == 1 ) nw = 5;
		CRect rcCurRgn = rcRgns.GetAt(i);
		rcCurRgn.InflateRect( 20 + nw , 10 + nw );
		RegulateRect( rcCurRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

		ASSERT( rcCurRgn.Width() > 25 );
		ASSERT( rcCurRgn.Height() > 5 );

		IMAGE imgCurRgnGray = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
		IMAGE imgCurRgnBin = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
#ifdef SAVE_CUTCHAR_INFO
		IMAGE imgCurRgnShow = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
#endif

		GetImageRectPart( imgGray, imgCurRgnGray, rcCurRgn );
		CRect rcTmp(0, 0, rcCurRgn.Width(), rcCurRgn.Height() );

		switch( nthBinMed )
		{
		case 0:
			DajinBin( imgCurRgnGray, imgCurRgnBin, rcTmp );
			break;
		case 1:
			ImgReverse( imgCurRgnGray, imgCurRgnGray );
			//LLTFast(imgCurRgnGray, imgCurRgnBin, nw , 8);
			LLTFast(imgCurRgnGray, imgCurRgnBin, nw , 5);
			ImgReverse( imgCurRgnGray, imgCurRgnGray );
		}
		

		ObjRectArray charArray;
		ObjRectArray allRcsArray;
		GetObjRect( imgCurRgnBin, charArray, allRcsArray );

#ifdef SAVE_CUTCHAR_INFO
		m_strCurDebugDir.Format("%sMulSeqCutChar_Method%d", m_strSubDebugDir, nthBinMed );
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		memcpy( imgCurRgnShow[0], imgCurRgnBin[0], (rcCurRgn.Width()+1) * (rcCurRgn.Height()+1) );
		DrawObjRectArray( imgCurRgnShow, charArray );
		if( !bBlack)
			m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,i * 2,CString(_T(".jpg")));
		else
			m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,i * 2 + 1,CString(_T(".jpg")));
	 	ImageSave( imgCurRgnShow, m_strCurDebugFile );
#endif

//Michael Add 20080724 -- For Remove ABC Rgn Noise
		if( i == 0 )
		{
			RemoveABCRgnNoise( charArray, allRcsArray );
		}
//End -- Michael Add 20080724 -- For Remove ABC Rgn Noise

		TransferRcArrayFromL2W( charArray, rcCurRgn );
		TransferRcArrayFromL2W( allRcsArray, rcCurRgn );
		rcArrayRes.Append( charArray );
		rcArrayRes_All.Append( allRcsArray );
		
#ifdef  SAVE_CUTCHAR_INFO
		ImageFree( imgCurRgnShow );
#endif
		ImageFree( imgCurRgnGray );
		ImageFree( imgCurRgnBin );
	}

#ifdef SAVE_CUTCHAR_INFO
	CString strMCutInfoFilename;
	strMCutInfoFilename.Format("%s%s_MulCut%s",m_strCurDebugDir,fname,CString(_T(".txt")));
	fstream fMulCutInfo;
	fMulCutInfo.open( strMCutInfoFilename, ios::out | ios::trunc );
	fMulCutInfo << m_strWholePath << _T(":") << endl;
	WriteRcArray2Txt( fMulCutInfo, rcArrayRes );
	fMulCutInfo.close();
#endif

	int nCharW, nCharH;
	GetAverRcWH( rcArrayRes, nCharW, nCharH );
	TackleCongluatination( rcArrayRes, imgGray, nCharH, nCharW );

	ObjRectArray rcArrayAna;

	CArray<int,int> intArray;
	RectsMulAnalyze( rcArrayRes, rcArrayAna, nCharW, nCharH, nCharH, intArray );
	int nType = GetMulNumSeqType(intArray);

	if( nType < 0 )//Try Merge
	{
		rcArrayRes.RemoveAll();
		rcArrayRes.Copy( rcArrayAna );
		RectsAnalyzeByScan( rcArrayRes, rcArrayRes_All, nCharW, nCharH, nCharH, intArray );
		intArray.RemoveAll();
		RectsMulAnalyze( rcArrayRes, rcArrayAna, nCharW, nCharH, nCharH, intArray );
		nType = GetMulNumSeqType(intArray);
	}

	if( nType < 0 )
	{
		rcArrayRes.RemoveAll();
	}
	
	BOOL bEstiCheckPos = FALSE;
	CRect rcLastRgn = CRect( 0, 0, 0, 0 );
	if( nType == 1 )
	{
		bEstiCheckPos = TRUE;
		ASSERT( rcArrayRes.GetSize() == 10 );
		CRect rc1 = rcArrayRes.GetAt(7);
		CRect rc2 = rcArrayRes.GetAt(9);
		rcLastRgn.left = rc1.right;
		rcLastRgn.right = rc2.left;
		ASSERT( rcLastRgn.left < rcLastRgn.right );
		rcLastRgn.top = min(rc1.top,rc2.top);
		rcLastRgn.bottom = max(rc1.bottom,rc2.bottom);
		ASSERT( rcLastRgn.top < rcLastRgn.bottom );

	}
	
	if( bEstiCheckPos )
	{
		CRect rcEstiCheck = CRect( rcLastRgn.left, rcLastRgn.bottom, rcLastRgn.right, rcLastRgn.bottom + 2 * rcLastRgn.Height() );
		RegulateRect( rcEstiCheck, 0, 0, nPicWidth-1, nPicHeight-1 );

		IMAGE imgCheckGray = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
		IMAGE imgCheckBin = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
#ifdef SAVE_DEBUGINFO_FILES
		IMAGE imgCheckShow = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
#endif
		GetImageRectPart( imgGray, imgCheckGray, rcEstiCheck );
		CRect rcTmp(0, 0, rcEstiCheck.Width(), rcEstiCheck.Height() );
		//DajinBin( imgCheckGray, imgCheckBin, rcTmp );
		switch( nthBinMed )
		{
		case 0:
			DajinBin( imgCheckGray, imgCheckBin, rcTmp );
			break;
		case 1:
			ImgReverse( imgCheckGray, imgCheckGray );
			LLTFast(imgCheckGray, imgCheckBin, 3 , 8);
			ImgReverse( imgCheckGray, imgCheckGray );
		}

		ObjRectArray charArray;
		ObjRectArray allRcsArray;
		GetObjRect( imgCheckBin, charArray, allRcsArray );

#ifdef SAVE_DEBUGINFO_FILES
		m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutChar_EsCheck";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		DrawObjRectArray( imgCheckBin, charArray );
		m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,i,CString(_T(".jpg")));
 	 	ImageSave( imgCheckBin, m_strCurDebugFile );
#endif

		CRect rcCheck  = CRect(0,0,0,0);
		for( int nId = 0; nId < charArray.GetSize(); nId++ )
		{
			CRect rcCur = charArray.GetAt(nId);
			if( rcCur.Height() > nCharH * 0.9 )
			{
				rcCheck = rcCur;
				break;
			}
		}
		
		if( nId != charArray.GetSize() )
		{
			TransferRectFromL2W( rcCheck, rcEstiCheck );
			rcArrayRes.Add( rcCheck );
		}
		
		ImageFree( imgCheckGray );
		ImageFree( imgCheckBin );
#ifdef SAVE_DEBUGINFO_FILES
		ImageFree(imgCheckShow);
#endif
	}

	if( rcArrayRes.GetSize() == 11 )
	{
		bCut = TRUE;
	}

	if( bBlack )
		ImgReverse( imgGray, imgGray );

	return bCut;
}

BOOL CConNumProc::CutForMulRgns( ObjRectArray &rcRgns, IMAGE imgGray, ObjRectArray& rcArrayRes )
{
	BOOL bCut = FALSE;
	bCut = CutForMulRgnsPolicy( rcRgns, imgGray, rcArrayRes, 0 );
//Michael delete 20080724 -- imgGray has been set color
//	if( !bCut )
//	{
//		bCut = CutForMulRgnsPolicy( rcRgns, imgGray, rcArrayRes, 0, TRUE );
//		if( bCut ) m_bBlack = TRUE;
// 	}
	return bCut;
/*
	int nCnt = rcRgns.GetSize();
	if( nCnt < 2 ) return FALSE;

	BOOL bCut = FALSE;
	
	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);

	ObjRectArray rcArrayRes_All;
	for( int i = 0; i < nCnt; i++ )
	{
		CRect rcCurRgn = rcRgns.GetAt(i);
		rcCurRgn.InflateRect( 20 , 10 );
		RegulateRect( rcCurRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

		ASSERT( rcCurRgn.Width() > 25 );
		ASSERT( rcCurRgn.Height() > 5 );

		IMAGE imgCurRgnGray = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
		IMAGE imgCurRgnBin = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
#ifdef SAVE_CUTCHAR_INFO
		IMAGE imgCurRgnShow = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
#endif

		GetImageRectPart( imgGray, imgCurRgnGray, rcCurRgn );
		CRect rcTmp(0, 0, rcCurRgn.Width(), rcCurRgn.Height() );
		DajinBin( imgCurRgnGray, imgCurRgnBin, rcTmp );

		ObjRectArray charArray;
		ObjRectArray allRcsArray;
		GetObjRect( imgCurRgnBin, charArray, allRcsArray );

#ifdef SAVE_CUTCHAR_INFO
		m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutChar";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		memcpy( imgCurRgnShow[0], imgCurRgnBin[0], (rcCurRgn.Width()+1) * (rcCurRgn.Height()+1) );
		DrawObjRectArray( imgCurRgnShow, charArray );
		m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,i,CString(_T(".jpg")));
	 	ImageSave( imgCurRgnShow, m_strCurDebugFile );
#endif
		TransferRcArrayFromL2W( charArray, rcCurRgn );
		TransferRcArrayFromL2W( allRcsArray, rcCurRgn );
		rcArrayRes.Append( charArray );
		rcArrayRes_All.Append( allRcsArray );
		
#ifdef  SAVE_CUTCHAR_INFO
		ImageFree( imgCurRgnShow );
#endif
		ImageFree( imgCurRgnGray );
		ImageFree( imgCurRgnBin );
	}

#ifdef SAVE_CUTCHAR_INFO
//	m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutInfo";
//	CreateDirectory( m_strCurDebugDir, NULL );
// 	m_strCurDebugDir += _T("\\");
	CString strMCutInfoFilename;
	strMCutInfoFilename.Format("%s%s_MulCut%s",m_strCurDebugDir,fname,CString(_T(".txt")));
	fstream fMulCutInfo;
	fMulCutInfo.open( strMCutInfoFilename, ios::out | ios::trunc );
	fMulCutInfo << m_strWholePath << _T(":") << endl;
	WriteRcArray2Txt( fMulCutInfo, rcArrayRes );
	fMulCutInfo.close();
#endif

	int nCharW, nCharH;
	GetAverRcWH( rcArrayRes, nCharW, nCharH );
	TackleCongluatination( rcArrayRes, imgGray, nCharH, nCharW );

	ObjRectArray rcArrayAna;

	CArray<int,int> intArray;
	RectsMulAnalyze( rcArrayRes, rcArrayAna, nCharW, nCharH, nCharH, intArray );
	int nType = GetMulNumSeqType(intArray);

	if( nType < 0 )//Try Merge
	{
		rcArrayRes.RemoveAll();
		rcArrayRes.Copy( rcArrayAna );
		RectsAnalyzeByScan( rcArrayRes, rcArrayRes_All, nCharW, nCharH, nCharH, intArray );
		intArray.RemoveAll();
		RectsMulAnalyze( rcArrayRes, rcArrayAna, nCharW, nCharH, nCharH, intArray );
		nType = GetMulNumSeqType(intArray);
	}

	if( nType < 0 )
	{
		rcArrayRes.RemoveAll();
	}
	
	BOOL bEstiCheckPos = FALSE;
	CRect rcLastRgn = CRect( 0, 0, 0, 0 );
	if( nType == 1 )
	{
		bEstiCheckPos = TRUE;
		ASSERT( rcArrayRes.GetSize() == 10 );
		CRect rc1 = rcArrayRes.GetAt(7);
		CRect rc2 = rcArrayRes.GetAt(9);
		rcLastRgn.left = rc1.right;
		rcLastRgn.right = rc2.left;
		ASSERT( rcLastRgn.left < rcLastRgn.right );
		rcLastRgn.top = min(rc1.top,rc2.top);
		rcLastRgn.bottom = max(rc1.bottom,rc2.bottom);
		ASSERT( rcLastRgn.top < rcLastRgn.bottom );

	}
	
	if( bEstiCheckPos )
	{
		CRect rcEstiCheck = CRect( rcLastRgn.left, rcLastRgn.bottom, rcLastRgn.right, rcLastRgn.bottom + 2 * rcLastRgn.Height() );
		RegulateRect( rcEstiCheck, 0, 0, nPicWidth-1, nPicHeight-1 );

		IMAGE imgCheckGray = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
		IMAGE imgCheckBin = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
#ifdef SAVE_DEBUGINFO_FILES
		IMAGE imgCheckShow = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
#endif
		GetImageRectPart( imgGray, imgCheckGray, rcEstiCheck );
		CRect rcTmp(0, 0, rcEstiCheck.Width(), rcEstiCheck.Height() );
		DajinBin( imgCheckGray, imgCheckBin, rcTmp );

		ObjRectArray charArray;
		ObjRectArray allRcsArray;
		GetObjRect( imgCheckBin, charArray, allRcsArray );

#ifdef SAVE_DEBUGINFO_FILES
		m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutChar_EsCheck";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		DrawObjRectArray( imgCheckBin, charArray );
		m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,i,CString(_T(".jpg")));
 	 	ImageSave( imgCheckBin, m_strCurDebugFile );
#endif

		CRect rcCheck  = CRect(0,0,0,0);
		for( int nId = 0; nId < charArray.GetSize(); nId++ )
		{
			CRect rcCur = charArray.GetAt(nId);
			if( rcCur.Height() > nCharH * 0.9 )
			{
				rcCheck = rcCur;
				break;
			}
		}
		
		if( nId != charArray.GetSize() )
		{
			TransferRectFromL2W( rcCheck, rcEstiCheck );
			rcArrayRes.Add( rcCheck );
		}
		
		ImageFree( imgCheckGray );
		ImageFree( imgCheckBin );
#ifdef SAVE_DEBUGINFO_FILES
		ImageFree(imgCheckShow);
#endif
	}

	if( rcArrayRes.GetSize() == 11 )
	{
		bCut = TRUE;
	}

	return bCut;
//*/
}

BOOL CConNumProc::CutForMulRgnsByLLT( ObjRectArray &rcRgns, IMAGE imgGray, ObjRectArray& rcArrayRes )
{
	BOOL bCut = FALSE;
	bCut = CutForMulRgnsPolicy( rcRgns, imgGray, rcArrayRes, 1 );
	if( !bCut )
	{
		bCut = CutForMulRgnsPolicy( rcRgns, imgGray, rcArrayRes, 1, TRUE );
		if( bCut ) m_bBlack = TRUE;
	}
	return bCut;
/*	
	int nCnt = rcRgns.GetSize();
	if( nCnt < 2 ) return FALSE;

	BOOL bCut = FALSE;
	
	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);

	for( int i = 0; i < nCnt; i++ )
	{
		int nw = 3;
		CRect rcCurRgn = rcRgns.GetAt(i);
		rcCurRgn.InflateRect( 20 + nw, 10 + nw);
		RegulateRect( rcCurRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

		ASSERT( rcCurRgn.Width() > 25 );
		ASSERT( rcCurRgn.Height() > 5 );
		
		IMAGE imgCurRgnGray = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
		IMAGE imgCurRgnBin = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
#ifdef SAVE_CUTCHAR_INFO
		IMAGE imgCurRgnShow = ImageAlloc( rcCurRgn.Width() + 1, rcCurRgn.Height() + 1 );
#endif

		GetImageRectPart( imgGray, imgCurRgnGray, rcCurRgn );
		CRect rcTmp(0, 0, rcCurRgn.Width(), rcCurRgn.Height() );
		//DajinBin( imgCurRgnGray, imgCurRgnBin, rcTmp );
		ImgReverse( imgCurRgnGray, imgCurRgnGray );
		LLTFast(imgCurRgnGray, imgCurRgnBin, 3 , 8);
		//LLTFast(imgNumSeq_Gray, imgNumSeq, 6 , 8);
		ImgReverse( imgCurRgnGray, imgCurRgnGray );

		ObjRectArray charArray;
		ObjRectArray allRcsArray;
		GetObjRect( imgCurRgnBin, charArray, allRcsArray );

#ifdef SAVE_CUTCHAR_INFO
		m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutCharByLLT";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		memcpy( imgCurRgnShow[0], imgCurRgnBin[0], (rcCurRgn.Width()+1) * (rcCurRgn.Height()+1) );
		DrawObjRectArray( imgCurRgnShow, charArray );
		m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,i,CString(_T(".jpg")));
	 	ImageSave( imgCurRgnShow, m_strCurDebugFile );
#endif
		TransferRcArrayFromL2W( charArray, rcCurRgn );
		rcArrayRes.Append( charArray );
		
#ifdef  SAVE_CUTCHAR_INFO
		ImageFree( imgCurRgnShow );
#endif
		ImageFree( imgCurRgnGray );
		ImageFree( imgCurRgnBin );
	}

#ifdef SAVE_CUTCHAR_INFO
//	m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutInfo";
//	CreateDirectory( m_strCurDebugDir, NULL );
// 	m_strCurDebugDir += _T("\\");
	CString strMCutInfoFilename;
	strMCutInfoFilename.Format("%s%s_MulCut%s",m_strCurDebugDir,fname,CString(_T(".txt")));
	fstream fMulCutInfo;
	fMulCutInfo.open( strMCutInfoFilename, ios::out | ios::trunc );
	fMulCutInfo << m_strWholePath << _T(":") << endl;
	WriteRcArray2Txt( fMulCutInfo, rcArrayRes );
	fMulCutInfo.close();
#endif

	int nCharW, nCharH;
	GetAverRcWH( rcArrayRes, nCharW, nCharH );
	TackleCongluatination( rcArrayRes, imgGray, nCharH, nCharW );

	ObjRectArray rcArrayAna;

	CArray<int,int> intArray;
	RectsMulAnalyze( rcArrayRes, rcArrayAna, nCharW, nCharH, nCharH, intArray );

	int nType = GetMulNumSeqType(intArray);
	if( nType < 0 )
	{
		rcArrayRes.RemoveAll();
	}
	
	BOOL bEstiCheckPos = FALSE;
	CRect rcLastRgn = CRect( 0, 0, 0, 0 );
	if( nType == 1 )
	{
		bEstiCheckPos = TRUE;
		ASSERT( rcArrayRes.GetSize() == 10 );
		CRect rc1 = rcArrayRes.GetAt(7);
		CRect rc2 = rcArrayRes.GetAt(9);
		rcLastRgn.left = rc1.right;
		rcLastRgn.right = rc2.left;
		ASSERT( rcLastRgn.left < rcLastRgn.right );
		rcLastRgn.top = min(rc1.top,rc2.top);
		rcLastRgn.bottom = max(rc1.bottom,rc2.bottom);
		ASSERT( rcLastRgn.top < rcLastRgn.bottom );

	}
	
	if( bEstiCheckPos )
	{
		CRect rcEstiCheck = CRect( rcLastRgn.left, rcLastRgn.bottom, rcLastRgn.right, rcLastRgn.bottom + 2 * rcLastRgn.Height() );
		RegulateRect( rcEstiCheck, 0, 0, nPicWidth-1, nPicHeight-1 );

		IMAGE imgCheckGray = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
		IMAGE imgCheckBin = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
#ifdef SAVE_DEBUGINFO_FILES
		IMAGE imgCheckShow = ImageAlloc( rcEstiCheck.Width()+1, rcEstiCheck.Height()+1 );
#endif
		GetImageRectPart( imgGray, imgCheckGray, rcEstiCheck );
		CRect rcTmp(0, 0, rcEstiCheck.Width(), rcEstiCheck.Height() );
		DajinBin( imgCheckGray, imgCheckBin, rcTmp );

		ObjRectArray charArray;
		ObjRectArray allRcsArray;
		GetObjRect( imgCheckBin, charArray, allRcsArray );

#ifdef SAVE_DEBUGINFO_FILES
		m_strCurDebugDir = m_strSubDebugDir + "MulSeqCutChar_EsCheck";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
		DrawObjRectArray( imgCheckBin, charArray );
		m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,i,CString(_T(".jpg")));
 	 	ImageSave( imgCheckBin, m_strCurDebugFile );
#endif

		CRect rcCheck  = CRect(0,0,0,0);
		for( int nId = 0; nId < charArray.GetSize(); nId++ )
		{
			CRect rcCur = charArray.GetAt(nId);
			if( rcCur.Height() > nCharH * 0.9 )
			{
				rcCheck = rcCur;
				break;
			}
		}

//		if( nId != charArray.GetSize() )
// 			GetTheCheckRect( rcCheck, imgCheckGray, nCharH * 0.9, nCharW * 0.9 );
		
		if( nId != charArray.GetSize() )
		{
			TransferRectFromL2W( rcCheck, rcEstiCheck );
			rcArrayRes.Add( rcCheck );
		}
		
		ImageFree( imgCheckGray );
		ImageFree( imgCheckBin );
#ifdef SAVE_DEBUGINFO_FILES
		ImageFree(imgCheckShow);
#endif
	}

	if( rcArrayRes.GetSize() == 11 )
	{
		bCut = TRUE;
	}

	return bCut;
//*/
}

BOOL CConNumProc::CutForVerRgn( CRect rcNumRgn, IMAGE imgGray, ObjRectArray& rcArrayRes, BOOL bBlack )
{
	BOOL bCut = FALSE;
	if( bBlack )
	{
		ImgReverse( imgGray, imgGray );
	}

	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);

	IMAGE imgCurRgnGray = ImageAlloc( rcNumRgn.Width() + 1, rcNumRgn.Height() + 1 );
	IMAGE imgCurRgnBin = ImageAlloc( rcNumRgn.Width() + 1, rcNumRgn.Height() + 1 );

	GetImageRectPart( imgGray, imgCurRgnGray, rcNumRgn );

	CRect rcTmp(0, 0, rcNumRgn.Width(), rcNumRgn.Height() );
//	DajinBin( imgCurRgnGray, imgCurRgnBin, rcTmp );

	ObjRectArray charArray;

	DajinBin( imgCurRgnGray, imgCurRgnBin, rcTmp );
	//if( GetCharsInVerRgn( imgCurRgnGray, imgCurRgnBin, charArray, rcNumRgn ) )
	if( GetCharsInVerRgn( imgGray, imgCurRgnBin, charArray, rcNumRgn ) )
	{
		//TransferRcArrayFromL2W( charArray, rcNumRgn );
		rcArrayRes.Copy( charArray );
		bCut = TRUE;
	}
	
	if( !bCut )
	{
		DajinBin( imgCurRgnGray, imgCurRgnBin, rcTmp, 0.2f );
		//if( GetCharsInVerRgn( imgCurRgnGray, imgCurRgnBin, charArray, rcNumRgn ) )
		if( GetCharsInVerRgn( imgGray, imgCurRgnBin, charArray, rcNumRgn ) )
		{
			//TransferRcArrayFromL2W( charArray, rcNumRgn );
			rcArrayRes.Copy( charArray );
			bCut = TRUE;
		}
 	}

	if( !bCut )
	{
		DajinBin( imgCurRgnGray, imgCurRgnBin, rcTmp, 0.6f );
		//if( GetCharsInVerRgn( imgCurRgnGray, imgCurRgnBin, charArray, rcNumRgn ) )
		if( GetCharsInVerRgn( imgGray, imgCurRgnBin, charArray, rcNumRgn ) )
		{
			//TransferRcArrayFromL2W( charArray, rcNumRgn );
			rcArrayRes.Copy( charArray );
			bCut = TRUE;
		}
 	}

	//Michael Add 2008-04-16 -- LLT Try in CutForVerRgn
	if( !bCut )
	{
		ImgReverse( imgCurRgnGray, imgCurRgnGray );
		LLTFast(imgCurRgnGray, imgCurRgnBin, 3 , 8);
		ImgReverse( imgCurRgnGray, imgCurRgnGray );
		//if( GetCharsInVerRgn( imgCurRgnGray, imgCurRgnBin, charArray, rcNumRgn ) )
		if( GetCharsInVerRgn( imgGray, imgCurRgnBin, charArray, rcNumRgn ) )
		{
			//TransferRcArrayFromL2W( charArray, rcNumRgn );
			rcArrayRes.Copy( charArray );
			bCut = TRUE;
		}		
	}
	//End -- LLT Try in CutForVerRgn

	ImageFree( imgCurRgnGray );
	ImageFree( imgCurRgnBin );

	if( bBlack )
	{
		ImgReverse( imgGray, imgGray );
	}
	
	return ( bCut );
}

BOOL CConNumProc::GetCharsInVerRgn( IMAGE imgGray, IMAGE imgCurRgnBin, ObjRectArray& charArray, CRect rcRgn )
{
	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);
#ifdef CHECK_ERROR
	ASSERT( rcRgn.top >= 0 );
	ASSERT( rcRgn.left >= 0 );
	ASSERT( rcRgn.right < nPicWidth );
	ASSERT( rcRgn.bottom < nPicHeight );
#endif
	if( rcRgn.top < 0 
		|| rcRgn.left < 0 
		|| rcRgn.right >= nPicWidth
		|| rcRgn.bottom >= nPicHeight )
	{
		charArray.RemoveAll();
		return FALSE;
	}

	int nRgnWidth = rcRgn.Width() + 1;
	int nRgnHeight = rcRgn.Height() + 1;
	int nRgnWidthRef = ImageWidth( imgCurRgnBin );
	int nRgnHeightRef = ImageHeight( imgCurRgnBin );
#ifdef CHECK_ERROR
	ASSERT( (nRgnWidth == nRgnWidthRef) && (nRgnHeight == nRgnHeightRef) );
#endif
	if( nRgnWidth != nRgnWidthRef 
		|| nRgnHeight != nRgnHeightRef )
	{
		charArray.RemoveAll();
		return FALSE;
	}


	IMAGE imgCurRgnGray = ImageAlloc( nRgnWidth, nRgnHeight );
	GetImageRectPart( imgGray, imgCurRgnGray, rcRgn );	


	BOOL bSuc = FALSE;

#ifdef SAVE_CUTCHAR_INFO
	IMAGE imgCurRgnShow = ImageAlloc( nRgnWidth, nRgnHeight );
#endif

	charArray.RemoveAll();
	ObjRectArray allRcsArray;
	GetObjRect( imgCurRgnBin, charArray, allRcsArray );
	SortRect_T2B( charArray );

#ifdef SAVE_CUTCHAR_INFO
	m_strCurDebugDir = m_strSubDebugDir + "VerSeqCutChar";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
	memcpy( imgCurRgnShow[0], imgCurRgnBin[0], nRgnWidth * nRgnHeight );
	DrawObjRectArray( imgCurRgnShow, charArray );
	m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,++m_nCurProcID,CString(_T(".jpg")));
	ImageSave( imgCurRgnShow, m_strCurDebugFile );
#endif

	int nCharW = 0;
	int nCharH = 0;
 	GetAverRcWH( charArray, nCharW, nCharH );
#ifdef SAVE_TC_INFO//Tackle Congluatination
	CString strDebugDir = m_strCurDebugDir + _T("TC\\");
	CreateDirectory( strDebugDir, NULL );
	CString strDebugFname = "";
	strDebugFname.Format( "%s_NumSeq_%d", fname , m_nCurProcID );
	SetCutCharsDebugInfoPath( strDebugDir, strDebugFname );
#endif
	TackleVerCongluatination( charArray, imgCurRgnGray, nCharH, nCharW );

	MerVerApartRcs( charArray, allRcsArray, nCharW, nCharH );

	CArray<int,int> intRelaArray;
	RectsVerAnalyze( charArray, allRcsArray, nCharW, nCharH, 9, intRelaArray);

#ifdef SAVE_CUTCHAR_INFO
	fstream fVerCutInfo;
	m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,m_nCurProcID,CString(_T(".txt")));
	fVerCutInfo.open( m_strCurDebugFile, ios::out | ios::trunc );
	fVerCutInfo << _T("The Res of RectsVerAnalyze : ") << endl;
	WriteRect2Txt( fVerCutInfo, CRect(0,0,nRgnWidth-1,nRgnHeight-1) );
	fVerCutInfo << _T(" : ") << endl;
	WriteRcArray2Txt( fVerCutInfo, charArray );
	fVerCutInfo << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
	int nSeqCnt = intRelaArray.GetSize();
	for( int ii = 0; ii < nSeqCnt; ii++ )
	{
		fVerCutInfo << intRelaArray.GetAt(ii);
		if( ii != nSeqCnt-1 )
			fVerCutInfo << " - ";
	}
	fVerCutInfo << endl;
#endif

	int nTotalNum = 0;
	for( int nId = 0; nId < intRelaArray.GetSize(); nId++ )
	{
		int num = intRelaArray.GetAt(nId);
		nTotalNum += num;
	}

	//Michael Add 20080701 -- Extract 11 ver chars
	if( nTotalNum > 11 )
	{
		int nDrop = 0;
		CArray<int,int> intAnaArray;
		intAnaArray.Copy( intRelaArray );
		int nResType = GetNumSeqType( intAnaArray, nDrop );
		if( nResType == 2 )
		{
			int nn = 0;
			for( nn = 0; nn < nDrop; nn++ )
			{
				charArray.RemoveAt(nn);
			}

			nTotalNum = charArray.GetSize();

			if( nTotalNum > 11 )
			{
				for( nn = nTotalNum - 1; nn >= 11; nn-- )
				{
					charArray.RemoveAt(nn);
				}
			}
		}
	}
	//End -- Extract 11 ver chars
	
	if( nTotalNum == 11 )
	{
		bSuc = TRUE;
		TransferRcArrayFromL2W( charArray, rcRgn );
	}
#ifdef TEST_CUT_CONF
	else
	{
		CRect rcMis = CRect(0,0,0,0);
		if( nTotalNum == 10 && charArray.GetSize() == 10 )
		{
			ObjRectArray charArrayAna;
			charArrayAna.Copy( charArray );
			CArray<int,int> intAnaArray;
			intAnaArray.Copy( intRelaArray );
			
			BOOL bNorm = CheckAbnorm( charArrayAna );
				
			if( bNorm )//No Abnormality
			{
				int nMissPos = -1;
				float fCutConf = 0.6f;
				GetMisPosForWhole( intAnaArray, nMissPos, fCutConf );

				if( nMissPos >= 0 && m_fCutConf < fCutConf)
				{
					TransferRcArrayFromL2W( charArrayAna, rcRgn );
					SetMemCharArrayByConf( charArrayAna, nMissPos );
					
					m_fCutConf = fCutConf;
				}

			}

		}
	}
#endif

#ifdef TEST_VER_FORMAT2
	if( nTotalNum == 7 && charArray.GetSize() == 7 )//The Condition for VerAlign Format 2
	{
		ObjRectArray rcNumArray;
		rcNumArray.Copy( charArray );
		charArray.RemoveAll();
		TransferRcArrayFromL2W( rcNumArray, rcRgn );

#ifdef SAVE_CUTCHAR_INFO
		int nCharW = 0;
		int nCharH = 0;
		GetAverRcWH( rcNumArray, nCharW, nCharH );		
		fVerCutInfo << _T("VerAlign Format 2 Analyze : ") << endl;
		fVerCutInfo << " NumChars' W&H -- w : " << nCharW << ", h : " << nCharH << endl;
#endif

		CRect rcABCRgn = CRect(0,0,0,0);
 		if( GetABCSeqRgnForV2( rcABCRgn, rcNumArray, nPicWidth, nPicHeight) )
		{
#ifdef CHECK_ERROR
			ASSERT( rcABCRgn != CRect(0,0,0,0) );
#endif
			int nABCRgnWidth = rcABCRgn.Width() + 1;
			int nABCRgnHeight = rcABCRgn.Height() + 1;
			IMAGE imgABCRgn = ImageAlloc( nABCRgnWidth, nABCRgnHeight );
			GetImageRectPart( imgGray, imgABCRgn, rcABCRgn );
#ifdef SAVE_CUTCHAR_INFO
			m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,++m_nCurProcID,CString(_T(".jpg")));
			ImageSave( imgABCRgn, m_strCurDebugFile );			
#endif
			ObjRectArray rcABCArray;			
			BOOL bSucInABCRgn = CutForV2ABCRgn( imgGray, rcABCRgn, rcABCArray );
			if( bSucInABCRgn )
			{
				charArray.RemoveAll();
				charArray.Append( rcABCArray );
				charArray.Append( rcNumArray );
				bSuc = TRUE;
			}

			ImageFree( imgABCRgn );
		}

	}
#endif

#ifdef  SAVE_CUTCHAR_INFO
	ImageFree( imgCurRgnShow );
	fVerCutInfo.close();
#endif	

	ImageFree( imgCurRgnGray );

	return bSuc;
}

#ifdef TEST_VER_FORMAT2
BOOL CConNumProc::CutForV2ABCRgn( IMAGE imgGray, CRect rcABCRgn, ObjRectArray& charArray )
{
	charArray.RemoveAll();

	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );

	BOOL bValRgn = CheckRcInImg( rcABCRgn, nPicWidth, nPicHeight );
#ifdef CHECK_ERROR
	ASSERT( bValRgn );
#endif
	if( !bValRgn ) return FALSE;

	BOOL bSuc = FALSE;
	int nthTry = 0;
	while( !bSuc && nthTry < 2 )
	{
		charArray.RemoveAll();
		bSuc = V2ABCRgnCutStrategy( imgGray, rcABCRgn, charArray, nthTry );
		nthTry++;
	}
	
	return bSuc;
}

BOOL CConNumProc::V2ABCRgnCutStrategy( IMAGE imgGray, CRect rcABCRgn, ObjRectArray& charArray, int nthTry )
{
	charArray.RemoveAll();
	if( nthTry > 2 ) return FALSE;
	
	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );
	BOOL bValRgn = CheckRcInImg( rcABCRgn, nPicWidth, nPicHeight );
#ifdef CHECK_ERROR
	ASSERT( bValRgn );
#endif
	if( !bValRgn ) return FALSE;

	BOOL bSuc = FALSE;

	CRect rcRgn = CRect( 0, 0, 0, 0 );
	int nw = 3;//For LLT
	switch( nthTry )
	{
	case 0:
		rcABCRgn.InflateRect( nw, nw );
		RegulateRect( rcABCRgn, 0, 0, nPicWidth-1, nPicHeight-1 );
		rcRgn = rcABCRgn;
		break;
	default:
		rcRgn = rcABCRgn;
	}
	int nRgnWidth = rcRgn.Width() + 1;
	int nRgnHeight = rcRgn.Height() + 1;

#ifdef ERROR_NEED_MEND
	ASSERT( nRgnWidth >= 8 && nRgnHeight >= 50 );
#endif
	if( nRgnWidth < 8 || nRgnHeight < 50 )
	{
		return FALSE;
	}

	IMAGE imgCurRgnGray = ImageAlloc( nRgnWidth, nRgnHeight );
	IMAGE imgCurRgnBin = ImageAlloc( nRgnWidth, nRgnHeight );

	GetImageRectPart( imgGray, imgCurRgnGray, rcRgn );
#ifdef SAVE_CUTCHAR_INFO
	IMAGE imgCurRgnShow = ImageAlloc( nRgnWidth, nRgnHeight );
#endif


	CRect rcTmp(0, 0, rcRgn.Width(), rcRgn.Height() );
	switch( nthTry )
	{
	case 0:
		ImgReverse( imgCurRgnGray, imgCurRgnGray );
		LLTFast(imgCurRgnGray, imgCurRgnBin, 3 , 8);
		ImgReverse( imgCurRgnGray, imgCurRgnGray );
		break;
	case 1:
		DajinBin( imgCurRgnGray, imgCurRgnBin, rcTmp, 0.2f);
		break;
	}



#ifdef SAVE_CUTCHAR_INFO
	m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,++m_nCurProcID,CString(_T(".jpg")));
	ImageSave( imgCurRgnBin, m_strCurDebugFile );
#endif

	ObjRectArray allRcsArray;
	GetObjRect( imgCurRgnBin, charArray, allRcsArray );
	SortRect_T2B( charArray );

#ifdef SAVE_CUTCHAR_INFO
	memcpy( imgCurRgnShow[0], imgCurRgnBin[0], nRgnWidth * nRgnHeight );
	DrawObjRectArray( imgCurRgnShow, charArray );
	m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,++m_nCurProcID,CString(_T(".jpg")));
	ImageSave( imgCurRgnShow, m_strCurDebugFile );
#endif

	int nCharW = 0;
	int nCharH = 0;
 	GetAverRcWH( charArray, nCharW, nCharH );
#ifdef SAVE_TC_INFO//Tackle Congluatination
	CString strDebugDir = m_strCurDebugDir + _T("TC\\");
	CreateDirectory( strDebugDir, NULL );
	CString strDebugFname = "";
	strDebugFname.Format( "%s_ABCSeq_%d", fname , m_nCurProcID );
	SetCutCharsDebugInfoPath( strDebugDir, strDebugFname );
#endif
	TackleVerCongluatination( charArray, imgCurRgnGray, nCharH, nCharW );

	MerVerApartRcs( charArray, allRcsArray, nCharW, nCharH );

	CArray<int,int> intRelaArray;
	RectsVerAnalyze( charArray, allRcsArray, nCharW, nCharH, 9, intRelaArray);

#ifdef SAVE_CUTCHAR_INFO
	memcpy( imgCurRgnShow[0], imgCurRgnBin[0], nRgnWidth * nRgnHeight );
	DrawObjRectArray( imgCurRgnShow, charArray );
	m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,++m_nCurProcID,CString(_T(".jpg")));
	ImageSave( imgCurRgnShow, m_strCurDebugFile );
#endif

#ifdef SAVE_CUTCHAR_INFO
	fstream fVerCutInfo;
	m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,m_nCurProcID,CString(_T(".txt")));
	fVerCutInfo.open( m_strCurDebugFile, ios::out | ios::trunc );
	fVerCutInfo << _T("The Res of RectsVerAnalyze : ") << endl;
	WriteRect2Txt( fVerCutInfo, CRect(0,0,nRgnWidth-1,nRgnHeight-1) );
	fVerCutInfo << _T(" : ") << endl;
	WriteRcArray2Txt( fVerCutInfo, charArray );
	fVerCutInfo << " ST RES -- w : " << nCharW << ", h : " << nCharH << endl;
	int nSeqCnt = intRelaArray.GetSize();
	for( int ii = 0; ii < nSeqCnt; ii++ )
	{
		fVerCutInfo << intRelaArray.GetAt(ii);
		if( ii != nSeqCnt-1 )
			fVerCutInfo << " - ";
	}
	fVerCutInfo << endl;
#endif

	if( intRelaArray.GetSize() == 1 && charArray.GetSize() == 4 )
	{
		TransferRcArrayFromL2W( charArray, rcABCRgn );
		bSuc = TRUE;
	}

	ImageFree( imgCurRgnGray );
	ImageFree( imgCurRgnBin );
#ifdef SAVE_CUTCHAR_INFO
	ImageFree( imgCurRgnShow );
#endif

	return bSuc;
}
#endif//End -- TEST_VER_FORMAT2

BOOL CConNumProc::CheckAbnorm( ObjRectArray& charArrayAna )
{
	int nCnt = charArrayAna.GetSize();
	if( nCnt < 5 ) return FALSE;

	for(int nId = 0; nId < nCnt - 1; nId++ )//Check Abnormality
	{
		CRect rcCur = charArrayAna.GetAt(nId);
		CRect rcNext = charArrayAna.GetAt(nId+1);

		int h1 = rcCur.Height();
		int w1 = rcCur.Width();
		int h2 = rcCur.Height();
		int w2 = rcCur.Width();

		float fDifHRatio = abs( h1 - h2 ) / (float)max( h1, h2 );
		float fDifWRatio = abs( w1 - w2 ) / (float)max( w1, w2 );
		if( fDifHRatio > 0.4 )
		{
			break;
		}
	}

	return (nId == nCnt-1 );
}

BOOL CConNumProc::GetMisPosForWhole( CArray<int,int>& intAnaArray, int &nMissPos, float &fConf )
{
	BOOL bSuc = FALSE;
	nMissPos = -1;
	fConf = 0.0f;

	int nCnt = intAnaArray.GetSize();
	int nTotal = 0;
	for( int i = 0; i < nCnt; i++ )
	{
		nTotal += intAnaArray.GetAt(i);
	}
	if( nTotal < 10 )
	{
		return FALSE;
	}

	
	int nSeqCnt = intAnaArray.GetSize();
	int nCurSeqRcsCnt = intAnaArray[0];
	if( nCurSeqRcsCnt > 4 || nCurSeqRcsCnt < 3 )
	{
		nMissPos = nCurSeqRcsCnt;
	}

	if( nCurSeqRcsCnt == 4 )
	{
#ifdef CHECK_ERROR
		ASSERT( nSeqCnt > 1 );
#endif
		nCurSeqRcsCnt = intAnaArray[1];
		nMissPos = 4 + nCurSeqRcsCnt;
	}

	if( nCurSeqRcsCnt == 3 )
	{
		nMissPos = 0;
	}

	if( nMissPos >= 0 )
	{
		fConf = 0.6f;
	}

	return bSuc;
}

BOOL CConNumProc::SetMemCharArrayByConf( ObjRectArray& charArrayAna, int nMissPos )
{
	CRect rcMis = CRect(0,0,0,0);

	int nCnt = charArrayAna.GetSize();
	if( (nCnt + 1) != 11 ) return FALSE;
	
	int nCurID = 0;
	m_ABCCharArray_ByConf.RemoveAll();
	m_NumCharArray_ByConf.RemoveAll();
	for(int nId = 0; nId < nCnt; nId++ )
	{
		if( nId == nMissPos )
		{
			if( nCurID < 4 )
			{
				m_ABCCharArray_ByConf.Add( rcMis );
			}

			if( nCurID >= 4 )
			{
				m_NumCharArray_ByConf.Add( rcMis );
			}

			nId--;
			nMissPos = -1;
		}
		else
		{
			CRect rcCur = charArrayAna.GetAt(nId);
			if( nCurID < 4 )
			{
				m_ABCCharArray_ByConf.Add( rcCur );
			}

			if( nCurID >= 4 )
			{
				m_NumCharArray_ByConf.Add( rcCur );
			}
		}
		nCurID++;
	}

	if( nMissPos == nCnt )
	{
#ifdef CHECK_ERROR
		ASSERT(nCurID==nCnt);
#endif
		m_NumCharArray_ByConf.Add(rcMis);
	}

	return TRUE;
}

BOOL CConNumProc::CutABCRgn( IMAGE imgGrayOrg, CRect rcABCRgn, BOOL bBlack )
{
	IMAGE imgABCSeq_Gray = ImageAlloc( rcABCRgn.Width() + 1 , rcABCRgn.Height() + 1 );
	IMAGE imgABCSeq = ImageAlloc( rcABCRgn.Width() + 1 , rcABCRgn.Height() + 1 );

	GetImageRectPart( imgGrayOrg, imgABCSeq_Gray, rcABCRgn );
	if( bBlack )
	{
		ImgReverse( imgABCSeq_Gray, imgABCSeq_Gray );
	}

	CRect rcTmp( 0 , 0 , rcABCRgn.Width() , rcABCRgn.Height() );
	DajinBin( imgABCSeq_Gray , imgABCSeq , rcTmp );
	ObjRectArray charArray;
	ObjRectArray allRcsArray;
	GetObjRect( imgABCSeq, charArray, allRcsArray );
	SortRect( charArray );

	//ImageSave( imgABCSeq, "C:\\bin.bmp");

	int nTryTimes =0;
	//while( charArray.GetSize() != 4 )
	while( !TopRectsAnalyze(charArray) )
	{
		nTryTimes++;
		DajinBin( imgABCSeq_Gray, imgABCSeq, rcTmp, 0.1f * nTryTimes );
		charArray.RemoveAll();
		allRcsArray.RemoveAll();
		GetObjRect( imgABCSeq, charArray, allRcsArray );
		SortRect( charArray );
		if( nTryTimes >= 3 ) break;
	}

#ifdef SAVE_DEBUGINFO_FILES
	{
		DrawObjRectArray( imgABCSeq, charArray );
		m_pImageDis->PutImage( rcABCRgn, imgABCSeq[0], imgABCSeq[0], imgABCSeq[0] );
	}
#endif

	ImageFree(imgABCSeq_Gray);
	ImageFree(imgABCSeq);


	if( charArray.GetSize() == 4 )
	{
		m_ABCCharArray.Copy( charArray );
		TransferRcArrayFromL2W( m_ABCCharArray, rcABCRgn);
		m_rgnABCSeg = rcABCRgn;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConNumProc::CutABCRgn( IMAGE imgGrayOrg, CRect rcABCRgn, BOOL bBlack, int nCharW, int nCharH, int nCharDis )
{
	BOOL bSuc = FALSE;	
	
	IMAGE imgABCSeq_Gray = ImageAlloc( rcABCRgn.Width() + 1 , rcABCRgn.Height() + 1 );
	IMAGE imgABCSeq = ImageAlloc( rcABCRgn.Width() + 1 , rcABCRgn.Height() + 1 );

	GetImageRectPart( imgGrayOrg, imgABCSeq_Gray, rcABCRgn );
	if( bBlack )
	{
		ImgReverse( imgABCSeq_Gray, imgABCSeq_Gray );
	}

	CRect rcTmp( 0 , 0 , rcABCRgn.Width() , rcABCRgn.Height() );
	DajinBin( imgABCSeq_Gray , imgABCSeq , rcTmp );
	ObjRectArray charArray;
	ObjRectArray allRcsArray;
	GetObjRect( imgABCSeq, charArray, allRcsArray );

	bSuc = TopRectsAnalyze(charArray);

	if( !bSuc )
	{
		MergeApartRcsByScan( allRcsArray, charArray, nCharW, nCharH );
		SortRect( charArray );
		bSuc = TopRectsAnalyze(charArray);
	}

#ifdef TEST_CUT_CONF
	if( !bSuc && charArray.GetSize() == 3 )
	{
		TransferRcArrayFromL2W( charArray, rcABCRgn );
		ABCRgnAna_ByConf( charArray );
	}
#endif
	
	int nTryTimes =0;
	while( !bSuc )
	{
		nTryTimes++;
		DajinBin( imgABCSeq_Gray, imgABCSeq, rcTmp, 0.1f * nTryTimes );
		charArray.RemoveAll();
		allRcsArray.RemoveAll();
		GetObjRect( imgABCSeq, charArray, allRcsArray );
		MergeApartRcsByScan( allRcsArray, charArray, nCharW, nCharH );
		bSuc = TopRectsAnalyze(charArray);

		if( !bSuc )
		{
			MergeApartRcsByScan( allRcsArray, charArray, nCharW, nCharH );
			SortRect( charArray );
			bSuc = TopRectsAnalyze(charArray);
		}
#ifdef TEST_CUT_CONF
		if( !bSuc && charArray.GetSize() == 3 )
		{
			TransferRcArrayFromL2W( charArray, rcABCRgn );
			ABCRgnAna_ByConf( charArray );
		}
#endif

		if( nTryTimes >= 3 ) break;
	}

#ifdef SAVE_DEBUGINFO_FILES
//	{
//		DrawObjRectArray( imgABCSeq, charArray );
//		m_pImageDis->PutImage( rcABCRgn, imgABCSeq[0], imgABCSeq[0], imgABCSeq[0] );
// 	}
#endif

	ImageFree(imgABCSeq_Gray);
	ImageFree(imgABCSeq);

	if( charArray.GetSize() == 4 )
	{
		m_ABCCharArray.Copy( charArray );
		TransferRcArrayFromL2W( m_ABCCharArray, rcABCRgn);
		m_rgnABCSeg = rcABCRgn;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#ifdef TEST_CUT_CONF
BOOL CConNumProc::ABCRgnAna_ByConf( ObjRectArray& charArray )
{
	BOOL bSuc = FALSE;

	int i = 0;

	BOOL bAbNormal = FALSE;
	for( i = 0; i < 2; i++ )
	{
		CRect rcCur = charArray.GetAt(i);
		CRect rcNext = charArray.GetAt(i+1);

		int h1 = rcCur.Height();
		int h2 = rcNext.Height();

		if( abs(h1-h2) > min(h1,h2) * 0.4 )
		{
			bAbNormal = TRUE;
			break;
		}

		if( !RcsInSameLine( rcCur, rcNext ) )
		{
			bAbNormal = TRUE;
			break;
		}
	}

	if( !bAbNormal )
	{
		int nMissPos = -1;
		
		CRect rcTopLast = charArray.GetAt(2);
		CRect rcBotFirst;
		for( i = 0; i < 7; i++ )
		{
			CRect rcCur = m_NumCharArray.GetAt(i);
			if( rcCur != CRect(0,0,0,0) )
			{
				rcBotFirst = rcCur;
				break;
			}
		}

		if( rcTopLast.right > rcBotFirst.left )
		{
			for( i = 0; i < 3; i++ )
			{
				CRect rcCur = charArray.GetAt(i);
				CRect rcRef = m_NumCharArray.GetAt(i);

				if( rcCur.left > rcRef.right )
				{
					nMissPos = i;
					break;
				}
			}

			if( i == 3 ) nMissPos = 3;
		}
		else
		{
			int nDis0 = charArray.GetAt(1).left - charArray.GetAt(0).right;
			if( nDis0 < 0 ) nDis0 = 0;
			int nDis1 = charArray.GetAt(2).left - charArray.GetAt(1).right;
			if( nDis1 < 0 ) nDis1 = 0;
			
			if( abs(nDis0 - nDis1) > rcTopLast.Height() * 0.5 )
			{
				if( nDis0 > nDis1 )
				{
					nMissPos = 1;
				}
				if( nDis1 > nDis0 )
				{
					nMissPos = 2;
				}
			}
			else
			{
				nMissPos = 0;
			}
		}

		if( nMissPos >= 0 )
		{
			bSuc = TRUE;
			if( m_fCutConf < 0.6f )
			{
				m_fCutConf = 0.6f;
				m_NumCharArray_ByConf.RemoveAll();
				m_NumCharArray_ByConf.Copy( m_NumCharArray );
				m_ABCCharArray_ByConf.RemoveAll();

				CRect rcMis = CRect(0,0,0,0);
				int nCurID = 0;
				for( i = 0; i < 3; i++ )
				{
					CRect rcCur = charArray.GetAt(i);
					if( nCurID == nMissPos )
					{
						m_ABCCharArray_ByConf.Add( rcMis );
						nCurID++;
					}
					m_ABCCharArray_ByConf.Add(rcCur);
					nCurID++;
				}
				if( nMissPos == 3 )
				{
					m_ABCCharArray_ByConf.Add(rcMis);
				}
			}
		}
	}

	return bSuc;
}
#endif

BOOL CConNumProc::CutABCRgnByLLT( IMAGE imgGrayOrg, CRect rcABCRgn, BOOL bBlack, int nCharW, int nCharH, int nCharDis  )
{
	BOOL bCut = FALSE;

	int nPicWidth = ImageWidth( imgGrayOrg );
	int nPicHeight = ImageHeight( imgGrayOrg );

	int nw = 5;
	rcABCRgn.InflateRect( nw, nw );
	RegulateRect( rcABCRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

	IMAGE imgABCSeq_Gray = ImageAlloc( rcABCRgn.Width() + 1 , rcABCRgn.Height() + 1 );
	IMAGE imgABCSeq = ImageAlloc( rcABCRgn.Width() + 1 , rcABCRgn.Height() + 1 );

	GetImageRectPart( imgGrayOrg, imgABCSeq_Gray, rcABCRgn );
	if( bBlack )
	{
		ImgReverse( imgABCSeq_Gray, imgABCSeq_Gray );
	}

	ImgReverse( imgABCSeq_Gray, imgABCSeq_Gray );
	LLTFast(imgABCSeq_Gray, imgABCSeq, 3 , 8);
	//LLTFast(imgABCSeq_Gray, imgABCSeq, 6 , 8);
	ImgReverse( imgABCSeq_Gray, imgABCSeq_Gray );

	ObjRectArray charArray;
	ObjRectArray allRcsArray;
	GetObjRect( imgABCSeq, charArray, allRcsArray );
	SortRect( charArray );
	//TopRectsAnalyze( charArray );

	bCut = TopRectsAnalyze(charArray);

	if( !bCut )
	{
		MergeApartRcsByScan( allRcsArray, charArray, nCharW, nCharH );
		SortRect( charArray );
		bCut = TopRectsAnalyze(charArray);
	}

	if( !bCut )
	{
		MergeApartRcsByScan( allRcsArray, charArray, nCharW * 1.2, nCharH );
		SortRect(charArray);
		bCut = TopRectsAnalyze(charArray);
	}

#ifdef SAVE_CUTCHAR_INFO
	CString strCutPicFilename;
	int nCnt = 0;
	do
	{
		nCnt++;
		strCutPicFilename.Format("%s%s_%d%s",m_strCurDebugDir,fname,nCnt,CString(_T(".jpg")));
	}while( _access(strCutPicFilename, 0) != -1 );
	DrawObjRectArray( imgABCSeq, charArray );
 	ImageSave( imgABCSeq, strCutPicFilename);	
#endif

	ImageFree( imgABCSeq_Gray );
	ImageFree( imgABCSeq );

	//if( charArray.GetSize() == 4 )
	if( bCut )
	{
		m_ABCCharArray.Copy( charArray );
		TransferRcArrayFromL2W( m_ABCCharArray, rcABCRgn);
		m_rgnABCSeg = rcABCRgn;
		bCut = TRUE;
	}

	return bCut;
}

BOOL CConNumProc::CutABCRgnByLLT( IMAGE imgGrayOrg, CRect rcABCRgn, BOOL bBlack  )
{
	BOOL bCut = FALSE;

	int nPicWidth = ImageWidth( imgGrayOrg );
	int nPicHeight = ImageHeight( imgGrayOrg );

	int nw = 5;
	rcABCRgn.InflateRect( nw, nw );
	RegulateRect( rcABCRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

	IMAGE imgABCSeq_Gray = ImageAlloc( rcABCRgn.Width() + 1 , rcABCRgn.Height() + 1 );
	IMAGE imgABCSeq = ImageAlloc( rcABCRgn.Width() + 1 , rcABCRgn.Height() + 1 );

	GetImageRectPart( imgGrayOrg, imgABCSeq_Gray, rcABCRgn );
	if( bBlack )
	{
		ImgReverse( imgABCSeq_Gray, imgABCSeq_Gray );
	}

	ImgReverse( imgABCSeq_Gray, imgABCSeq_Gray );
	LLTFast(imgABCSeq_Gray, imgABCSeq, 3 , 8);
	//LLTFast(imgABCSeq_Gray, imgABCSeq, 6 , 8);
	ImgReverse( imgABCSeq_Gray, imgABCSeq_Gray );

	ObjRectArray charArray;
	ObjRectArray allRcsArray;
	GetObjRect( imgABCSeq, charArray, allRcsArray );
	SortRect( charArray );
	TopRectsAnalyze( charArray );
#ifdef SAVE_CUTCHAR_INFO
	m_strCurDebugDir = m_strSubDebugDir + "ABCRgnCutCharsByLLT\\";
	CreateDirectory( m_strCurDebugDir, NULL );
	CString strCutPicFilename;
	int nCnt = 0;
	do
	{
		nCnt++;
		strCutPicFilename.Format("%s%s_%d%s",m_strCurDebugDir,fname,nCnt,CString(_T(".jpg")));
	}while( _access(strCutPicFilename, 0) != -1 );
	DrawObjRectArray( imgABCSeq, charArray );
 	ImageSave( imgABCSeq, strCutPicFilename);	
#endif

	ImageFree( imgABCSeq_Gray );
	ImageFree( imgABCSeq );

	if( charArray.GetSize() == 4 )
	{
		m_ABCCharArray.Copy( charArray );
		TransferRcArrayFromL2W( m_ABCCharArray, rcABCRgn);
		m_rgnABCSeg = rcABCRgn;
		bCut = TRUE;
	}

	return bCut;
}

int CConNumProc::GetNumSeqType( CArray<int,int>& intArray )
{
	// 0 : 正常切分出NumSeq
	// 1 : NumSeq末字符可能存在边框影响
	// 2 : 正常切分出ABCSeq+NumSeq
	// 3 : 判定为ABCSeq+NumSeq，

	int nCnt = intArray.GetSize();

	if( nCnt >= 1 )
	{
		int nNum = intArray.GetAt(0);
		if( nNum == 7 )
		{
			return 0;
		}
		if( nNum == 8 )
		{
			return 1;
		}
		if( nNum == 11 )
		{
			return 2;
		}
	}
	if( nCnt >= 2 )
	{
		int nNum0 = intArray.GetAt(0);
		int nNum1 = intArray.GetAt(1);
		if( nNum0 == 6 && nNum1 == 1 )
		{
			return 0;
		}
		if( nNum0 == 6 && nNum1 == 2 )
		{
			return 1;
		}
		if( nNum0 == 4 && nNum1 == 7 )
		{
			return 2;
		}
		if( nNum0 == 4 && nNum1 == 8 )
		{
			return 3;
		}
	}
	if( nCnt >= 3 )
	{
		int nNum0 = intArray.GetAt(0);
		int nNum1 = intArray.GetAt(1);
		int nNum2 = intArray.GetAt(2);
		if( nNum0 == 4 && nNum1 == 6 && nNum2 == 1 )
		{
			return 2;
		}
		if( nNum0 == 4 && nNum1 == 6 && nNum2 == 2 )
		{
			return 3;
		}
	}
	return -1;

}

int CConNumProc::GetNumSeqType( CArray<int,int>& intArray, int &nDrop )
{
	// 0 : 正常切分出NumSeq
	// 1 : NumSeq末字符可能存在边框影响
	// 2 : 正常切分出ABCSeq+NumSeq
	// 3 : 判定为ABCSeq+NumSeq，

	nDrop = 0;
	int nCnt = intArray.GetSize();
	while( nCnt != 0 )
	{
		if( nCnt >= 1 )
		{
			int nNum = intArray.GetAt(0);
			if( nNum == 7 )
			{
				return 0;
			}
			if( nNum == 8 )
			{
				return 1;
			}
			if( nNum == 11 )
			{
				return 2;
			}
		}
		if( nCnt >= 2 )
		{
			int nNum0 = intArray.GetAt(0);
			int nNum1 = intArray.GetAt(1);
			if( nNum0 == 6 && nNum1 == 1 )
			{
				return 0;
			}
			if( nNum0 == 6 && nNum1 == 2 )
			{
				return 1;
			}
			if( nNum0 == 4 && nNum1 == 7 )
			{
				return 2;
			}
			if( nNum0 == 10 && nNum1 == 1 )//Michael Add 2008-04-17
			{
				return 2;
			}
			if( nNum0 == 4 && nNum1 == 8 )
			{
				return 3;
			}
		}
		if( nCnt >= 3 )
		{
			int nNum0 = intArray.GetAt(0);
			int nNum1 = intArray.GetAt(1);
			int nNum2 = intArray.GetAt(2);
			if( nNum0 == 4 && nNum1 == 6 && nNum2 == 1 )
			{
				return 2;
			}
			if( nNum0 == 4 && nNum1 == 6 && nNum2 == 2 )
			{
				return 3;
			}
		}

		nDrop += intArray.GetAt(0);
		intArray.RemoveAt(0);
		nCnt--;
	}

	return -1;
}

#ifdef TEST_CUT_CONF
int CConNumProc::GetNumSeqType_ByConf( CArray<int,int>& intArray, CArray<int,int>& disArray, int &nDrop )
{
	// 0 : 正常切分出NumSeq
	// 1 : NumSeq末字符可能存在边框影响
	// 2 : 正常切分出ABCSeq+NumSeq
	// 3 : 判定为ABCSeq+NumSeq，

//	nDrop = 0;
//	int i = 0;
//	int nCnt = intArray.GetSize();
//	while( nCnt != 0 )
//	{
//		int nTCnt = 0;
//		int nRes = 0;
//		
//		for( i = 0; i < nCnt; i++ )
//		{
//			int nNum = intArray.GetAt(i);
//			nTCnt += nNum;
//
//			if( nNum > 12 ) break;
//
//			if( nTCnt == 6 ) nRes = 1;
//			if( nTCnt == 10 ) nRes = 3;
//		}
//
//		if( nRes > 0 ) return nRes;
//		
//		nDrop += intArray.GetAt(0);
//		intArray.RemoveAt(0);
//		nCnt--;
// 	}

	nDrop = 0;
	int i = 0;
	int nCnt1 = intArray.GetSize();
	int nCnt2 = disArray.GetSize();

	if( nCnt1 == 0 )
	{
#ifdef CHECK_ERROR
		ASSERT( nCnt2 == 0 );
#endif
		intArray.RemoveAll();
		disArray.RemoveAll();
		return -1;
	}

#ifdef ERROR_NEED_MEND
	ASSERT( nCnt2 == nCnt1 - 1 );
#endif
	if( nCnt2 != nCnt1 - 1 )//Abnormal 
	{
		intArray.RemoveAll();
		disArray.RemoveAll();
		return -1;
	}

	int nTCnt = 0;
	int nRes = -1;
	CArray<int,int> misCharsArray;
	for( i = 0; i < nCnt1; i++ )
	{
		int nNum0 = intArray.GetAt(i);
		
		nTCnt += nNum0;
		if( nTCnt == 4 )
		{
			misCharsArray.Add(0);
			continue;//May find the 4 ABCs' chars
		}
		if( nTCnt == 10 )
		{
			misCharsArray.Add(0);
			continue;
		}
		if( nTCnt == 11 )
		{
			nRes = 2;
			break;
		}
		if( i != nCnt1 - 1 )
		{
			int nDis = disArray.GetAt(i);
			nTCnt += nDis;
			misCharsArray.Add(nDis);
		}

	}

	if( nRes != -1 )
	{
#ifdef CHECK_ERROR
		ASSERT( i < nCnt1 );
#endif
		if( i < nCnt1 - 1 )
		{
			for( int ii = nCnt1 - 1; ii > i; ii-- )
			{
				intArray.RemoveAt(ii);
			}
		}
		disArray.RemoveAll();
		disArray.Copy( misCharsArray );
	}


	return nRes;

}

int CConNumProc::GetNumSeqType_ByConf( CArray<int,int>& intArray, int &nDrop )
{
	// 0 : 正常切分出NumSeq
	// 1 : NumSeq末字符可能存在边框影响
	// 2 : 正常切分出ABCSeq+NumSeq
	// 3 : 判定为ABCSeq+NumSeq，

	nDrop = 0;
	int i = 0;
	int nCnt = intArray.GetSize();
	while( nCnt != 0 )
	{
		int nTCnt = 0;
		int nRes = 0;
		
		for( i = 0; i < nCnt; i++ )
		{
			int nNum = intArray.GetAt(i);
			nTCnt += nNum;

			if( nNum > 12 ) break;

			if( nTCnt == 6 ) nRes = 1;
			if( nTCnt == 10 ) nRes = 3;
		}

		if( nRes > 0 ) return nRes;
		
		nDrop += intArray.GetAt(0);
		intArray.RemoveAt(0);
		nCnt--;
	}

	return -1;

}
#endif

int CConNumProc::GetNumSeqTypeForTopView( CArray<int,int>& intArray )
{
	// 0 : 正常切分出NumSeq
	// 1 : NumSeq末字符可能存在边框影响
	// 2 : 正常切分出ABCSeq+NumSeq
	// 3 : 判定为ABCSeq+NumSeq

	int nCnt = intArray.GetSize();

	if( nCnt >= 1 )
	{
		int nNum0 = intArray.GetAt(0);
		if( nNum0 == 11 )
		{
			return 2;
		}
	}

	if( nCnt >= 2 )
	{
		int nNum0 = intArray.GetAt(0);
		int nNum1 = intArray.GetAt(1);
		if( (nNum0 == 10 && nNum1 == 1) 
			|| (nNum0 == 4 && nNum1 == 7) )
		{
			return 2;
		}
	}

	if( nCnt >= 3 )
	{
		int nNum0 = intArray.GetAt(0);
		int nNum1 = intArray.GetAt(1);
		int nNum2 = intArray.GetAt(2);
		if( nNum0 == 4 && nNum1 == 6 && nNum2 == 1 )
		{
			return 2;
		}
	}

	return -1;
}

int CConNumProc::GetMulNumSeqType( CArray<int,int>& intArray )
{
	// 0 : 正常切分出NumSeq
	// 1 : 少一个字符
	int nCnt = intArray.GetSize();

	if( nCnt == 3 )
	{
		int nNum0 = intArray.GetAt(0);
		int nNum1 = intArray.GetAt(1);
		int nNum2 = intArray.GetAt(2);
		if( nNum0 == 4 && nNum1 == 3 && nNum2 == 3 )
		{
			return 1;
		}
		if( nNum0 == 4 && nNum1 == 3 && nNum2 == 4 )
		{
			return 0;
		}
	}
	return -1;

}

BOOL CConNumProc::GetTheCheckRectForTopView( CRect& rcCheck, IMAGE imgGray, int nCharH, int nCharW )
{
	ASSERT( nCharW != 0 );
	ASSERT( nCharH != 0 );
#ifdef SAVE_DEBUGINFO_FILES//SAVE FOR CHECK CHARS
	CString strDirCheckChars = m_strDebugDir + "Checks";
	CreateDirectory( strDirCheckChars, NULL );
	CString strSubDir1 = strDirCheckChars + _T("\\WithFrame");
	CreateDirectory( strSubDir1, NULL );
	strSubDir1 += _T("\\");
	CString strSubDir2 = strDirCheckChars + _T("\\WithoutFrame");
	CreateDirectory( strSubDir2, NULL );
	strSubDir2 += _T("\\");
	CString strSubDir3 = strDirCheckChars + _T("\\NewChars");
	CreateDirectory( strSubDir3, NULL );
	strSubDir3 += _T("\\");
	CString strSubDir4 = strDirCheckChars + _T("\\Bins");
	CreateDirectory( strSubDir4, NULL );
	strSubDir4 += _T("\\");
	CString strDes = "";
#endif
	int nh = rcCheck.Height();
	int nw = rcCheck.Width();

	BOOL bReDraw = FALSE;

	if( nh > nCharH * 1.1 || nw > nCharW * 1.1 )
	{
		IMAGE imgReCut_Gray;
		IMAGE imgReCut_Bin;
		int nImgW = rcCheck.Width() + 1;
		int nImgH = rcCheck.Height() + 1;
		imgReCut_Gray = ImageAlloc( nImgW, nImgH );
		imgReCut_Bin = ImageAlloc( nImgW, nImgH );
		GetImageRectPart(imgGray, imgReCut_Gray, rcCheck);
		DajinBin( imgReCut_Gray, imgReCut_Bin, CRect(0,0,rcCheck.Width(),rcCheck.Height()));

#ifdef SAVE_DEBUGINFO_FILES//SAVE FOR CHECK CHARS
		int nNum = 0;
		do
		{
			nNum++;
			strDes.Format( "%s\\%s_%02d.bmp" , strSubDir4, fname, nNum );
 		}while( _access(strDes, 0) != -1 );
		ImageSave( imgReCut_Bin, strDes );
#endif

		EliminCheckCharFrame(imgReCut_Bin);

		ObjRectArray arrayObj;
		ObjRectArray allObj;
		GetObjRectForTopView( imgReCut_Bin, arrayObj, allObj );

		int nCntTmp = arrayObj.GetSize();

		if( nCntTmp > 0 )
		{
			int nCenRefX = nImgW / 2;
			for( int ii = 0; ii < nCntTmp; ii++ )
			{
				CRect rcTmp = arrayObj.GetAt(ii);
				int nCenX = (rcTmp.left + rcTmp.right) / 2;
				if( rcTmp.Height() <= nCharH * 1.1 && rcTmp.Height() >= nCharH * 0.85 
					&& rcTmp.Width() <= nCharW * 1.1 && nImgW / (abs(nCenRefX-nCenX)+1) > 5 )
				{
					rcTmp.top += rcCheck.top;
					rcTmp.bottom += rcCheck.top;
					rcTmp.left += rcCheck.left;
					rcTmp.right += rcCheck.left;
					rcCheck = rcTmp;
					break;
				}
			}

			if( ii != nCntTmp )
			{
#ifdef SAVE_DEBUGINFO_FILES//SAVE FOR CHECK CHARS
				CString strDes = "";
				int nNum = 0;
				do
				{
					nNum++;
					strDes.Format( "%s\\%s_%02d.bmp" , strSubDir3, fname, nNum );
 				}while( _access(strDes, 0) != -1 );
				ImagePartSave( imgGray, rcCheck, strDes );		
#endif
				bReDraw = TRUE;
			}
			else
			{
#ifdef SAVE_DEBUGINFO_FILES//SAVE FOR CHECK CHARS
				CString strDes = "";
				int nNum = 0;
				do
				{
					nNum++;
					strDes.Format( "%s\\%s_%02d.bmp" , strSubDir1, fname, nNum );			
 				}while( _access(strDes, 0) != -1 );
				ImagePartSave( imgGray, rcCheck, strDes );
#endif
			}
		}
		
		ImageFree( imgReCut_Gray );
		ImageFree( imgReCut_Bin );
	}
	else
	{
#ifdef SAVE_DEBUGINFO_FILES//SAVE FOR CHECK CHARS
		CString strDes = "";
		int nNum = 0;
		do
		{
			nNum++;
			strDes.Format( "%s\\%s_%02d.bmp" , strSubDir2, fname, nNum );
 		}while( _access(strDes, 0) != -1 );
		ImagePartSave( imgGray, rcCheck, strDes );
#endif
	}

	return bReDraw;
}

BOOL CConNumProc::GetTheCheckRect( CRect& rcCheck, IMAGE imgGray, int nCharH, int nCharW )
{
	//ASSERT( nCharW != 0 );
	//ASSERT( nCharH != 0 );
	if( nCharH <= 6 || nCharW <= 6 ) return FALSE;
	int nh = rcCheck.Height();
	int nw = rcCheck.Width();

	//if( nh < nCharH * 0.7 ) return FALSE;
	if( nw < nCharW * 0.7 ) return FALSE;
	
#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
	CString strDirCheckChars = m_strDebugDir + "Checks";
	CreateDirectory( strDirCheckChars, NULL );
	CString strSubDir1 = strDirCheckChars + _T("\\WithFrame");
	CreateDirectory( strSubDir1, NULL );
	strSubDir1 += _T("\\");
	CString strSubDir2 = strDirCheckChars + _T("\\WithoutFrame");
	CreateDirectory( strSubDir2, NULL );
	strSubDir2 += _T("\\");
	CString strSubDir3 = strDirCheckChars + _T("\\NewChars");
	CreateDirectory( strSubDir3, NULL );
	strSubDir3 += _T("\\");
	CString strSubDir4 = strDirCheckChars + _T("\\Bins");
	CreateDirectory( strSubDir4, NULL );
	strSubDir4 += _T("\\");
	CString strDes = "";
#endif

	BOOL bReDraw = FALSE;

	if( nh > nCharH * 1.1 || nw > nCharW * 1.1 )
	//if( nh > nCharH * 1.0 || nw > nCharW * 1.0 )
	{
		IMAGE imgReCut_Gray;
		IMAGE imgReCut_Bin;
		int nImgW = rcCheck.Width() + 1;
		int nImgH = rcCheck.Height() + 1;
		imgReCut_Gray = ImageAlloc( nImgW, nImgH );
		imgReCut_Bin = ImageAlloc( nImgW, nImgH );
		GetImageRectPart(imgGray, imgReCut_Gray, rcCheck);
		if( m_bBlack )
		{
			ImgReverse( imgReCut_Gray, imgReCut_Gray );
		}
		DajinBin( imgReCut_Gray, imgReCut_Bin, CRect(0,0,rcCheck.Width(),rcCheck.Height()));

#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
		int nNum = 0;
		do
		{
			nNum++;
			strDes.Format( "%s\\%s_%02d.bmp" , strSubDir4, fname, nNum );
 		}while( _access(strDes, 0) != -1 );
		ImageSave( imgReCut_Bin, strDes );
#endif

		ObjRectArray arrayObj;
		ObjRectArray allObj;
		GetObjRect( imgReCut_Bin, arrayObj, allObj );
		int nCntTmp = arrayObj.GetSize();
		if( nCntTmp > 0 )
		{
			int nCenRefX = nImgW / 2;
			for( int ii = 0; ii < nCntTmp; ii++ )
			{
				CRect rcTmp = arrayObj.GetAt(ii);
				int nCenX = (rcTmp.left + rcTmp.right) / 2;
				if( rcTmp.Height() <= nCharH * 1.1 && rcTmp.Height() >= nCharH * 0.85 
					&& rcTmp.Width() <= nCharW * 1.1 && nImgW / (abs(nCenRefX-nCenX)+1) > 5 )
				{
					rcTmp.top += rcCheck.top;
					rcTmp.bottom += rcCheck.top;
					rcTmp.left += rcCheck.left;
					rcTmp.right += rcCheck.left;
					rcCheck = rcTmp;
					bReDraw = TRUE;
					break;
				}
			}

#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
			CString strDes = "";
			int nNum = 0;
			do
			{
				nNum++;
				strDes.Format( "%s\\%s_%02d.bmp" , strSubDir3, fname, nNum );
 			}while( _access(strDes, 0) != -1 );
			ImagePartSave( imgGray, rcCheck, strDes );		
#endif

		}

		if( !bReDraw )
		{
			EliminCheckCharFrame(imgReCut_Bin);

			ObjRectArray arrayObj;
			ObjRectArray allObj;
			GetObjRect( imgReCut_Bin, arrayObj, allObj );

			nCntTmp = arrayObj.GetSize();

			if( nCntTmp > 0 )
			{
				int nCenRefX = nImgW / 2;
				for( int ii = 0; ii < nCntTmp; ii++ )
				{
					CRect rcTmp = arrayObj.GetAt(ii);
					int nCenX = (rcTmp.left + rcTmp.right) / 2;
					if( 1
						//&& rcTmp.Height() <= nCharH * 1.1 
						&& rcTmp.Height() >= nCharH * 0.85 
						//&& rcTmp.Width() <= nCharW * 1.2 
						&& nImgW / (abs(nCenRefX-nCenX)+1) > 5 )
					{
						rcTmp.top += rcCheck.top;
						rcTmp.bottom += rcCheck.top;
						rcTmp.left += rcCheck.left;
						rcTmp.right += rcCheck.left;
						rcCheck = rcTmp;
						break;
					}
				}

				if( ii != nCntTmp )
				{
	#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
					CString strDes = "";
					int nNum = 0;
					do
					{
						nNum++;
						strDes.Format( "%s\\%s_%02d.bmp" , strSubDir3, fname, nNum );
 					}while( _access(strDes, 0) != -1 );
					ImagePartSave( imgGray, rcCheck, strDes );		
	#endif
					bReDraw = TRUE;
				}
				else
				{
	#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
					CString strDes = "";
					int nNum = 0;
					do
					{
						nNum++;
						strDes.Format( "%s\\%s_%02d.bmp" , strSubDir1, fname, nNum );			
 					}while( _access(strDes, 0) != -1 );
					ImagePartSave( imgGray, rcCheck, strDes );
	#endif
				}
			}
		}		
		ImageFree( imgReCut_Gray );
		ImageFree( imgReCut_Bin );
	}
	else
	{
#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
		CString strDes = "";
		int nNum = 0;
		do
		{
			nNum++;
			strDes.Format( "%s\\%s_%02d.bmp" , strSubDir2, fname, nNum );
 		}while( _access(strDes, 0) != -1 );
		ImagePartSave( imgGray, rcCheck, strDes );
#endif
	}

	return bReDraw;
}

BOOL CConNumProc::EliminCheckCharFrame(IMAGE imgReCut_Bin)
{
#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
	CString strDirCheckChars = m_strDebugDir + "Checks";
	CreateDirectory( strDirCheckChars, NULL );
	CString strSubDir1 = strDirCheckChars + _T("\\WithFrame");
	CreateDirectory( strSubDir1, NULL );
	strSubDir1 += _T("\\");
	CString strSubDir2 = strDirCheckChars + _T("\\WithoutFrame");
	CreateDirectory( strSubDir2, NULL );
	strSubDir2 += _T("\\");
	CString strSubDir3 = strDirCheckChars + _T("\\NewChars");
	CreateDirectory( strSubDir3, NULL );
	strSubDir3 += _T("\\");
	CString strSubDir4 = strDirCheckChars + _T("\\Bins");
	CreateDirectory( strSubDir4, NULL );
	strSubDir4 += _T("\\");
	CString strDes = "";
	int nNum = 0;
#endif	

	int nImgW = ImageWidth(imgReCut_Bin);
	int nImgH = ImageHeight(imgReCut_Bin);

	int *pTFL = new int[ nImgW * 2 ];//Top Frame Line
	int *pBFL = new int[ nImgW * 2 ];//Bottom Frame Line
	int *pLFL = new int[ nImgH * 2 ];//Left Frame Line
	int *pRFL = new int[ nImgH * 2 ];//Right Frame Line
#ifdef CHECK_MEM_LEAK
	nNewTimes += 4;
#endif

	memset( pTFL, 0, nImgW * 2 * sizeof(int) );
	memset( pBFL, 0, nImgW * 2 * sizeof(int) );
	memset( pLFL, 0, nImgH * 2 * sizeof(int) );
	memset( pRFL, 0, nImgH * 2 * sizeof(int) );

	int ii = 0;
	int jj = 0;
	BOOL bFind1st = FALSE;
	//Top Frame Line
	for( ii = 0, bFind1st = FALSE; ii < nImgW; ii++ )
	{
		for( jj = 0; jj < nImgH; jj++ )
		{
			if( imgReCut_Bin[jj][ii] && !bFind1st )
			{
				bFind1st = TRUE;
				pTFL[ii*2] = jj;
			}

			if( !imgReCut_Bin[jj][ii] && bFind1st )
			{
				bFind1st = FALSE;
				pTFL[ii*2+1] = jj;
				break;
			}
		}

		if( bFind1st && jj == nImgH )
		{
			pTFL[ii*2+1] = jj;
			bFind1st = FALSE;
		}
	}

	int nMaxFrameW = 3;
	int nThreLine = nMaxFrameW * 3;
	int nFrameWidth = 0;
	int nPtCnt = 0;
	for( ii = 0, nFrameWidth = 0; ii < nImgW; ii++ )
	{
		int nId1 = pTFL[ii*2];
		int nId2 = pTFL[ii*2+1];
		if( nId1 < nThreLine )
		{
			int nw = nId2 - nId1;
			ASSERT( nw >= 0 );
			if( nw > nFrameWidth && nw <= nMaxFrameW )
			{
				nFrameWidth = nw;
			}
			if( nw <= nFrameWidth )
			{
				nPtCnt++;
			}
		}
	}

	if( nFrameWidth > 0 && nPtCnt >= 4)
	for( ii = 0; ii < nImgW; ii++ )
	{
		int nId = pTFL[ii*2];
		if( nId < nThreLine )
		{
			for( jj = nId; jj < nId + nFrameWidth; jj++ )
			{
				//ASSERT( jj < nImgH );
				if( jj >= nImgH ) break;
				imgReCut_Bin[jj][ii] = 0;
			}
		}
	}

#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
	do
	{
		nNum++;
		strDes.Format( "%s\\%s_%02d.bmp" , strSubDir4, fname, nNum );
 	}while( _access(strDes, 0) != -1 );
	ImageSave( imgReCut_Bin, strDes );
#endif

	//Bottom Frame Line
	for( ii = 0, bFind1st = FALSE; ii < nImgW; ii++ )
	{
		for( jj = nImgH-1; jj >= 0; jj-- )
		{
			if( imgReCut_Bin[jj][ii] && !bFind1st )
			{
				bFind1st = TRUE;
				pBFL[ii*2] = jj;
			}

			if( !imgReCut_Bin[jj][ii] && bFind1st )
			{
				bFind1st = FALSE;
				pBFL[ii*2+1] = jj;
				break;
			}
		}

		if( bFind1st && jj == -1 )
		{
			pBFL[ii*2+1] = jj;
			bFind1st = FALSE;
		}
	}

	nPtCnt = 0;
	for( ii = 0, nFrameWidth = 0; ii < nImgW; ii++ )
	{
		int nId2 = pBFL[ii*2];
		int nId1 = pBFL[ii*2+1];
		if( nId1 > nImgH - nThreLine )
		{
			int nw = nId2 - nId1;
			ASSERT( nw >= 0 );
			if( nw > nFrameWidth && nw <= nMaxFrameW )
			{
				nFrameWidth = nw;
			}
			if( nw <= nFrameWidth )
			{
				nPtCnt++;
			}
		}
	}

	if( nFrameWidth > 0 && nPtCnt >= 4)
	for( ii = 0; ii < nImgW; ii++ )
	{
		int nId = pBFL[ii*2];
		if( nId > nImgH - nThreLine )
		{
			for( jj = nId; jj > nId - nFrameWidth; jj-- )
			{
				//ASSERT( jj < nImgH );
				if( jj >= nImgH ) break;
				imgReCut_Bin[jj][ii] = 0;
			}
		}
	}
#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
	do
	{
		nNum++;
		strDes.Format( "%s\\%s_%02d.bmp" , strSubDir4, fname, nNum );
 	}while( _access(strDes, 0) != -1 );
	ImageSave( imgReCut_Bin, strDes );
#endif

	//Left Frame Line
	for( ii = 0, bFind1st = FALSE; ii < nImgH; ii++ )
	{
		for( jj = 0; jj < nImgW; jj++ )
		{
			if( imgReCut_Bin[ii][jj] && !bFind1st )
			{
				bFind1st = TRUE;
				pLFL[ii*2] = jj;
			}

			if( !imgReCut_Bin[ii][jj] && bFind1st )
			{
				bFind1st = FALSE;
				pLFL[ii*2+1] = jj;
				break;
			}
		}

		if( bFind1st && jj == nImgW )
		{
			pLFL[ii*2+1] = jj;
			bFind1st = FALSE;
		}
	}

	nMaxFrameW = 3;
	nThreLine = nMaxFrameW * 2;
	nFrameWidth = 0;
	nPtCnt = 0;
	for( ii = 0, nFrameWidth = 0; ii < nImgH; ii++ )
	{
		int nId1 = pLFL[ii*2];
		int nId2 = pLFL[ii*2+1];
		if( nId1 < nThreLine )
		{
			int nw = nId2 - nId1;
			ASSERT( nw >= 0 );
			if( nw > nFrameWidth && nw <= nMaxFrameW )
			{
				nFrameWidth = nw;
			}
			if( nw <= nFrameWidth )
			{
				nPtCnt++;
			}
		}
	}

	if( nFrameWidth > 0 && nPtCnt >= 5 )
	for( ii = 0; ii < nImgH; ii++ )
	{
		int nId = pLFL[ii*2];
		if( nId < nThreLine )
		{
			for( jj = nId; jj < nId + nFrameWidth; jj++ )
			{
				//ASSERT( jj < nImgW );
				if( jj >= nImgW ) break;
				imgReCut_Bin[ii][jj] = 0;
			}
		}
	}

#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
	do
	{
		nNum++;
		strDes.Format( "%s\\%s_%02d.bmp" , strSubDir4, fname, nNum );
 	}while( _access(strDes, 0) != -1 );
	ImageSave( imgReCut_Bin, strDes );
#endif

	//Right Frame Line
	for( ii = 0, bFind1st = FALSE; ii < nImgH; ii++ )
	{
		for( jj = nImgW-1; jj >= 0; jj-- )
		{
			if( imgReCut_Bin[ii][jj] && !bFind1st )
			{
				bFind1st = TRUE;
				pRFL[ii*2] = jj;
			}

			if( !imgReCut_Bin[ii][jj] && bFind1st )
			{
				bFind1st = FALSE;
				pRFL[ii*2+1] = jj;
				break;
			}
		}

		if( bFind1st && jj == -1 )
		{
			pRFL[ii*2+1] = jj;
			bFind1st = FALSE;
		}
	}

	nPtCnt = 0;
	for( ii = 0, nFrameWidth = 0; ii < nImgH; ii++ )
	{
		int nId2 = pRFL[ii*2];
		int nId1 = pRFL[ii*2+1];
		if( nId1 > nImgW - nThreLine )
		{
			int nw = nId2 - nId1;
			ASSERT( nw >= 0 );
			if( nw > nFrameWidth && nw <= nMaxFrameW )
			{
				nFrameWidth = nw;
			}
			if( nw <= nFrameWidth )
			{
				nPtCnt++;
			}
		}
	}

	if( nFrameWidth > 0 && nPtCnt >= 5 )
	for( ii = 0; ii < nImgH; ii++ )
	{
		int nId = pRFL[ii*2];
		if( nId > nImgW - nThreLine )
		{
			for( jj = nId; jj > nId - nFrameWidth; jj-- )
			{
				//ASSERT( jj < nImgW );
				if( jj >= nImgW ) break;
				imgReCut_Bin[ii][jj] = 0;
			}
		}
	}

#ifdef SAVE_CHECKCHAR_INFO//SAVE FOR CHECK CHARS
	do
	{
		nNum++;
		strDes.Format( "%s\\%s_%02d.bmp" , strSubDir4, fname, nNum );
 	}while( _access(strDes, 0) != -1 );
	ImageSave( imgReCut_Bin, strDes );
#endif

	delete[] pTFL;
	delete[] pBFL;
	delete[] pLFL;
	delete[] pRFL;
#ifdef CHECK_MEM_LEAK
	nNewTimes -= 4;
#endif

	return TRUE;
}

BOOL CConNumProc::GetABCSeqRgn(CRect &rcABCRgn, CRect rcNumRgn, ObjRectArray& NumCharArray, int nPicWidth, int nPicHeight )
{
	int nNumChars = NumCharArray.GetSize();
	if( nNumChars < 6 )
	{
		rcABCRgn = CRect(0,0,0,0);
		return FALSE;
	}
	int NumSeqTop = nPicHeight - 1;
	int NumSeqBottom = 0;
	for( int ii = 0; ii < nNumChars; ii++ )
	{
		CRect rcCur = NumCharArray.GetAt(ii);

		if( rcCur.top < NumSeqTop )
		{
			NumSeqTop = rcCur.top;
		}

		if( rcCur.bottom > NumSeqBottom )
		{
			NumSeqBottom = rcCur.bottom;
		}
	}

	int l = rcNumRgn.left + NumCharArray.GetAt(0).left;
	int r = rcNumRgn.left + NumCharArray.GetAt(5).right;
	int t = rcNumRgn.top + NumSeqTop - (NumSeqBottom-NumSeqTop) * 1.5;
	int b = rcNumRgn.top + NumSeqTop - (NumSeqBottom-NumSeqTop) * 0.2;
	rcABCRgn = CRect(l,t,r,b);

	rcABCRgn.InflateRect( 20 , 10 );
	RegulateRect( rcABCRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

	if( (rcABCRgn.bottom-rcABCRgn.top) > (NumSeqBottom-NumSeqTop) && (rcABCRgn.right-rcABCRgn.left) > 25)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConNumProc::GetLABCSeqRgn(CRect &rcABCRgn, CRect rcNumRgn, ObjRectArray& NumCharArray, int nPicWidth, int nPicHeight )
{
	int nNumChars = NumCharArray.GetSize();
	if( nNumChars < 6 )
	{
		rcABCRgn = CRect(0,0,0,0);
		return FALSE;
	}
	int NumSeqTop = nPicHeight - 1;
	int NumSeqBottom = 0;
	for( int ii = 0; ii < nNumChars; ii++ )
	{
		CRect rcCur = NumCharArray.GetAt(ii);

		if( rcCur.top < NumSeqTop )
		{
			NumSeqTop = rcCur.top;
		}

		if( rcCur.bottom > NumSeqBottom )
		{
			NumSeqBottom = rcCur.bottom;
		}
	}

	int l = rcNumRgn.left + 2 * NumCharArray.GetAt(0).left - NumCharArray.GetAt(nNumChars-1).right;
	int r = rcNumRgn.left + NumCharArray.GetAt(0).left - 20;
	int t = rcNumRgn.top + NumSeqTop;
	int b = rcNumRgn.top + NumSeqBottom;
	rcABCRgn = CRect(l,t,r,b);

	rcABCRgn.InflateRect( 20 , 10 );
	RegulateRect( rcABCRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

	if( rcABCRgn.Height() > (NumSeqBottom-NumSeqTop) && (rcABCRgn.right-rcABCRgn.left) > 25 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConNumProc::GetABCSeqRgnForV2(CRect &rcABCRgn, ObjRectArray& NumCharArray, int nPicWidth, int nPicHeight )//For Vertically Aligned Format 2
{
	int nNumChars = NumCharArray.GetSize();
	if( nNumChars < 6 )
	{
		rcABCRgn = CRect(0,0,0,0);
		return FALSE;
	}

	BOOL bAbInPara = FALSE;
	for( int i = 0; i < nNumChars - 1; i++ )
	{
		CRect rcCur = NumCharArray.GetAt( i );
		CRect rcNext = NumCharArray.GetAt( i + 1 );

		if( (rcCur.top > rcNext.top) || (rcCur.bottom > rcNext.bottom) )
		{
			bAbInPara = TRUE;
			break;
		}
	}
#ifdef ERROR_NEED_MEND
	ASSERT( !bAbInPara );
#endif
	if( bAbInPara )
	{
		rcABCRgn = CRect(0,0,0,0);
		return FALSE;
	}

	int nCharW = 0;
	int nCharH = 0;
	GetAverRcWH( NumCharArray, nCharW, nCharH );
	if( nCharW == 0 || nCharH == 0 )
	{
		rcABCRgn = CRect(0,0,0,0);
		return FALSE;
	}

	CRect rcTop = NumCharArray.GetAt( 0 );
	int NumSeqRefTop = rcTop.top;
	CRect rcBot = NumCharArray.GetAt(2);
	int NumSeqRefBot = rcBot.bottom;

	int nRefL = 0;
	for( i = 0; i < 2; i++ )
	{
		CRect rcCur = NumCharArray.GetAt(i);

		if( rcCur.left > nRefL )
		{
			nRefL = rcCur.left;
		}
	}

	int l = nRefL - 2 * nCharW;
	int r = nRefL - nCharW / 4;
	int t = NumSeqRefTop - nCharH;
	int b = NumSeqRefBot + nCharH / 2;
	rcABCRgn = CRect(l,t,r,b);

	int nInflW = max( nCharW / 4, 4 );
	int nInflH = max( nCharH / 4, 8 );
	rcABCRgn.InflateRect( nInflW , nInflH );
	RegulateRect( rcABCRgn, 0, 0, nPicWidth-1, nPicHeight-1 );

	int nABCRgnW = rcABCRgn.Width() + 1;
	int nABCRgnH = rcABCRgn.Height() + 1;
	if( nABCRgnH > NumSeqRefBot - NumSeqRefTop + 1 && nABCRgnW > max( nCharW, nCharH * 0.5 ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConNumProc::RectsReDraw(IMAGE imgGray, ObjRectArray& charArray, int nCharH, int nCharW, int nRcDis )
{
	//主要针对画小的框
	int nCnt = charArray.GetSize();

	for( int ii = 0; ii < nCnt; ii++ )
	{
		CRect rcCur = charArray.GetAt(ii);
		int h1 = rcCur.Height();
		int w1 = rcCur.Width();

		BOOL bNeedMod = FALSE;
		float fRatio = (nCharH - h1) / (float)max(h1,nCharH);
		float fHWRatio = float( h1 ) / float( w1 );
		if( fRatio > 0.10f && fHWRatio < 4.5f)
		{
			bNeedMod = TRUE;
		}

		if( bNeedMod )
		{
			int t = 0;
			int b = ImageHeight(imgGray)-1;
			int l = 0;
			if( ii != 0 )
			{
				int nId = ii-1;
				while( nId >= 0 )
				{
					CRect rcPre = charArray.GetAt( nId );
					if( rcPre.right < rcCur.left )
					{
						l = rcPre.right;
						break;
					}
					nId--;
				}
			}
			int r = ImageWidth(imgGray)-1;
			if( ii != nCnt - 1 )
			{
				int nId = ii + 1;
				while( nId != nCnt - 1 )
				{
					CRect rcNext = charArray.GetAt( ii+1 );
					if( rcNext.left > rcCur.right )
					{
						r = rcNext.left;
						break;
					}
					nId++;
				}			
			}
			CRect rcBinRgn = CRect(l,t,r,b);

			IMAGE imgReCut_Gray;
			IMAGE imgReCut_Bin;
			imgReCut_Gray = ImageAlloc(rcBinRgn.Width()+1, rcBinRgn.Height()+1);
			imgReCut_Bin = ImageAlloc(rcBinRgn.Width()+1, rcBinRgn.Height()+1);
			GetImageRectPart(imgGray, imgReCut_Gray, rcBinRgn);
			DajinBin( imgReCut_Gray, imgReCut_Bin, CRect(0,0,rcBinRgn.Width(),rcBinRgn.Height()));

			//ImageSave(imgReCut_Bin, CString("C:\\a.bmp"));

			ObjRectArray arrayObj;
			ObjRectArray allObj;
			GetObjRect( imgReCut_Bin, arrayObj, allObj );

			int nCntTmp = arrayObj.GetSize();

			if( nCntTmp >= 1 )
			{
				SortRectByArea( arrayObj );
				CRect rcNew = arrayObj.GetAt(0);
				int hNew = rcNew.Height();
				int wNew = rcNew.Width();
				float fHDifRatio = abs(h1-nCharH) / (float)(nCharH);
				float fWDifRatio = (wNew - nCharW) / (float)(nCharW);
				if( fHDifRatio < 0.2f || fWDifRatio < 0.2f )
				{
					rcNew.left += l;
					rcNew.right += l;
					rcNew.top += t;
					rcNew.bottom += t;						
					charArray.SetAt( ii, rcNew );
				}
			}
			

			ImageFree(imgReCut_Gray);
			ImageFree(imgReCut_Bin);
		}

	}


	return TRUE;
}

BOOL CConNumProc::ReDrawRect( IMAGE imgGray, CRect& rcChar )
{
	BOOL bSuc = FALSE;
	
	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );

	CRect rcOrg = rcChar;
	rcChar.InflateRect(1,1);
	RegulateRect( rcChar, 0, 0, nPicWidth-1, nPicHeight-1 );

	int nCharH = rcChar.Height() + 1;
	int nCharW = rcChar.Width() + 1;

	IMAGE imgReCut_Bin;
	imgReCut_Bin = ImageAlloc( nCharW, nCharH );
	DajinBin( imgGray, imgReCut_Bin, rcChar );

	int i = 0;
	int j = 0;
/*
	int t = 0;
	int nTLimit = nCharH * 0.2f;
	for( i = 0; i < nTLimit; i++ )
	{
		for( j = 0; j < nCharW; j++ )
		{
			if( imgReCut_Bin[i][j] == 255 )
			{
				break;
			}
		}
		if( j != nCharW )
		{
			break;
		}
	}
	if( i != nTLimit )
	{
		t = i;
	}

	int l = 0;
	int nLLimit = nCharW * 0.3f;
	for( i = 0; i < nLLimit; i++ )
	{
		for( j = 0; j < nCharH; j++ )
		{
			if( imgReCut_Bin[j][i] == 255 )
			{
				break;
			}
		}
		if( j != nCharH )
		{
			break;
		}
	}
	if( i != nLLimit )
	{
		l = i;
	}

	int b = nCharH-1;
	for( i = 0; i < nTLimit; i++ )
	{
		for( j = 0; j < nCharW; j++ )
		{
			if( imgReCut_Bin[nCharH-1-i][j] == 255 )
			{
				break;
			}
		}
		if( j != nCharW )
		{
			break;
		}
	}
	if( i != nTLimit )
	{
		b = nCharH-1-i; 
	}

	int r = nCharW-1;
	for( i = 0; i < nLLimit; i++ )
	{
		for( j = 0; j < nCharH; j++ )
		{
			if( imgReCut_Bin[j][nCharW-1-i] == 255 )
			{
				break;
			}
		}
		if( j != nCharH )
		{
			break;
		}
	}
	if( i != nLLimit )
	{
		r = nCharW - 1 - i;
	}

	CRect rcNew = CRect( l, t, r, b );
	int hOrg = rcChar.Height();
	int wOrg = rcChar.Width();
	int hNew = rcNew.Height();
	int wNew = rcNew.Width();
	
	if( hNew >= hOrg * 0.8f && wNew >= wOrg * 0.8f )
	{
		int nOff = rcChar.left;
		rcNew.left += nOff;
		rcNew.right += nOff;
		nOff = rcChar.top;
		rcNew.top += nOff;
		rcNew.bottom += nOff;

		rcChar = rcNew;
		bSuc = TRUE;
	}
//*/

	ObjRectArray arrayObj;
	ObjRectArray allObj;
	GetObjRect( imgReCut_Bin, arrayObj, allObj );
	int nCntTmp = arrayObj.GetSize();
	if( nCntTmp >= 1 )
	{
		SortRectByArea( arrayObj );
		CRect rcNew = arrayObj.GetAt(0);

		int hOrg = rcChar.Height();
		int wOrg = rcChar.Width();
		int hNew = rcNew.Height();
		int wNew = rcNew.Width();
		
		if( hNew >= hOrg * 0.8f && wNew >= wOrg * 0.8f )
		{
			int nOff = rcChar.left;
			rcNew.left += nOff;
			rcNew.right += nOff;
			nOff = rcChar.top;
			rcNew.top += nOff;
			rcNew.bottom += nOff;

			rcChar = rcNew;
			bSuc = TRUE;
		}
 	}

	if( !bSuc )
	{
		rcChar = rcOrg;
	}

	ImageFree(imgReCut_Bin);

	return bSuc;
}

BOOL CConNumProc::ReDrawRectByTopValueBin( IMAGE imgGray, CRect& rcChar, float fTRatio )
{
	BOOL bSuc = FALSE;
	
	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );

	CRect rcOrg = rcChar;
	rcChar.InflateRect(1,1);
	RegulateRect( rcChar, 0, 0, nPicWidth-1, nPicHeight-1 );

	int nCharH = rcChar.Height() + 1;
	int nCharW = rcChar.Width() + 1;

	IMAGE imgReCut_Bin;
	imgReCut_Bin = ImageAlloc( nCharW, nCharH );
	TopValueBin( imgGray, imgReCut_Bin, rcChar, fTRatio );

	int i = 0;
	int j = 0;

	ObjRectArray arrayObj;
	ObjRectArray allObj;
	GetObjRect( imgReCut_Bin, arrayObj, allObj );
	int nCntTmp = arrayObj.GetSize();
	if( nCntTmp >= 1 )
	{
		SortRectByArea( arrayObj );
		CRect rcNew = arrayObj.GetAt(0);

		int hOrg = rcChar.Height();
		int wOrg = rcChar.Width();
		int hNew = rcNew.Height();
		int wNew = rcNew.Width();
		
		if( hNew >= hOrg * 0.8f && wNew >= wOrg * 0.8f )
		{
			int nOff = rcChar.left;
			rcNew.left += nOff;
			rcNew.right += nOff;
			nOff = rcChar.top;
			rcNew.top += nOff;
			rcNew.bottom += nOff;

			rcChar = rcNew;
			bSuc = TRUE;
		}
 	}

	if( !bSuc )
	{
		rcChar = rcOrg;
	}

	ImageFree(imgReCut_Bin);

	return bSuc;
}

BOOL CConNumProc::TopValueBin( IMAGE imgGray, IMAGE imgBin, CRect rcChar, float fTopRatio )
{
	int i = 0;
	int j = 0;
	int w1 = rcChar.Width() + 1;
	int h1 = rcChar.Height() + 1;

	int w2 = ImageWidth(imgBin);
	int h2 = ImageHeight(imgBin);

	if( (h2 < h1) || (w2 < w1) )
	{
		return FALSE;
	}

	IMAGE img;
	img = ImageAlloc(rcChar.Width()+1, rcChar.Height()+1);
	GetImageRectPart(imgGray, img, rcChar);

	long* histo = new long[256];
	memset( histo, 0, sizeof(long) );
#ifdef CHECK_MEM_LEAK
	nNewTimes += 1;
#endif
	ImgHisto( img, histo );
	int nTotPixs = 0;
	int nThre = 0;
	for( i = 255; i >= 0; i-- )
	{
#ifdef CHECK_ERROR
		ASSERT( i <= 255 );
#endif
		nTotPixs += histo[i];
		if( nTotPixs >= w1 * h1 * fTopRatio )
		{
			nThre = i;
			break;
		}
	}
	delete[] histo;
#ifdef CHECK_MEM_LEAK
	nNewTimes--;
#endif
	ImgSingleSegment(img, imgBin, nThre);
	ImageFree( img );

	if( nThre == 0 ) return FALSE;

	return TRUE;
}

BOOL CConNumProc::RemoveABCRgnNoise( ObjRectArray &charArray, ObjRectArray &allRcsArray )
{
	ObjRectArray charAnaArray;
	charAnaArray.Copy( charArray );

	int nCnt1 = charAnaArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();

	if( nCnt1 <= 4 )
	{
		return FALSE;
	}

	int w = 0;
	int h = 0;
	int nRcDis = 0;

	GetAverRcWH( charAnaArray, w, h );
	GetRcDis( charAnaArray, nRcDis );
	if( w == 0 || h == 0 || nRcDis == 0 )
	{
		return FALSE;
	}

	BOOL bSuc = FALSE;

	for( int i = 0; i < nCnt1; i++ )
	{
		CRect rcCur = charAnaArray.GetAt( i );
		int h1 = rcCur.Height();
		int w1 = rcCur.Width();

		if( w1 <= 2 )
		{
			charAnaArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		if( w1 > w * 1.25f )
		{
			charAnaArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		float fRatio = abs(h1-h) / (float)(h);
		if( fRatio > 0.2f )
		{
			charAnaArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}
	}

	//SortRect_T2B( charAnaArray );
	//SortRect( charAnaArray );

	ObjRectArray subSeqArray;
	ObjRectArray leftCharArray;
	DigOutSeq( charAnaArray, subSeqArray, leftCharArray );
	charAnaArray.RemoveAll();
	charAnaArray.Copy( subSeqArray );
	nCnt1 = charAnaArray.GetSize();
	SortRect( charAnaArray );

	int nResCnt = 0;
	ObjRectArray charResArray;
	for( i = 0 ; i < nCnt1; i++ )
	{
		CRect rcCur = charAnaArray.GetAt( i );
		int h1 = rcCur.Height();
		int w1 = rcCur.Width();
		charResArray.Add( rcCur );

		if( i != nCnt1 - 1 )
		{
			CRect rcNext =  charAnaArray.GetAt( i + 1 );

			if( !RcsInSameLine(rcCur,rcNext) )//上下分层
			{
				nResCnt = charResArray.GetSize();
				if( nResCnt < 4 )
				{
					charResArray.RemoveAll();
					continue;
				}
				else
				{
					break;
				}
			}

			int h2 = rcNext.Height();
			int w2 = rcNext.Width();
			int nDis = rcNext.left - rcCur.right;

			if( nDis > nRcDis * 1.5f )
			{
				nResCnt = charResArray.GetSize();
				if( nResCnt < 4 )
				{
					charResArray.RemoveAll();
					continue;
				}
				else
				{
					break;
				}
			}
		}
	}

	nResCnt = charResArray.GetSize();
	if( nResCnt >= 4 )
	{
		charArray.RemoveAll();
		charArray.Copy( charResArray );
		bSuc = TRUE;
	}

	return bSuc;

}

BOOL CConNumProc::DigOutSeq( ObjRectArray &charArray, ObjRectArray &subSeqArray, ObjRectArray &leftCharArray )
{
	ObjRectArray charAnaArray;
	charAnaArray.Copy( charArray );
	SortRect( charAnaArray );
	int nCnt = charAnaArray.GetSize();
	if( nCnt == 0 ) return FALSE;

	BOOL bSuc = FALSE;
	ObjRectArray rcCurSeqArray;
	int nPreSeqCnt = 0;
	int nCurSeqCnt = 0;

	int i = 0;
	while( nCnt > 0 )
	{
		ObjRectArray rcCurSeqArray;
		CRect rcPre = charAnaArray.GetAt( 0 );
		rcCurSeqArray.Add( rcPre );
		charAnaArray.RemoveAt( 0 );
		nCnt--;
		
		for( i = 0; i < nCnt; i++ )
		{
			CRect rcCur = charAnaArray.GetAt( i );
			if( RcsInSameLine( rcPre, rcCur ) )
			{
				charAnaArray.RemoveAt( i );
				nCnt--;
				i--;
				rcCurSeqArray.Add(rcCur);
				rcPre = rcCur;
			}
		}

		nCurSeqCnt = rcCurSeqArray.GetSize();
		if( nCurSeqCnt > nPreSeqCnt )
		{
			leftCharArray.Append( subSeqArray );
			subSeqArray.RemoveAll();
			subSeqArray.Copy(rcCurSeqArray);
			nPreSeqCnt = nCurSeqCnt;
		}
		else
		{
			leftCharArray.Append( rcCurSeqArray );
		}
	}

#ifdef CHECK_ERROR
	int nCntT = charArray.GetSize();
	int nCnt1 = subSeqArray.GetSize();
	int nCnt2 = leftCharArray.GetSize();
	ASSERT( nCntT == nCnt1 + nCnt2 );
	ASSERT( nCnt1 == nPreSeqCnt );
#endif

	int nResCnt = subSeqArray.GetSize();
	bSuc = ( nResCnt > 0 );

	return bSuc;
}

BOOL CConNumProc::RectsMulAnalyze( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray )
{
	intArray.RemoveAll();

	if( w == 0 || h == 0 )
	{
		return FALSE;
	}

	BOOL bSuc = FALSE;

	SortRect_T2B( charArray );
	SortRect_T2B( allRcsArray );

	int i;
	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();

	int nCntInSeq = 0;
	int nCurSeqLen = 0;

	for( i = 0; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height();
		int w1 = rcCur.Width();

		if( w1 <= 2 )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		float fRatio = abs(h1-h) / (float)(h);
		float fRatioShort = (h - h1) / (float)(h); 
		
		if( fRatio > 0.2f  || fRatioShort > 0.2f )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

//		fRatio = abs(w1-w) / (float)max(w1,w);
//		if( fRatio > 0.3f )
//		{
//			charArray.RemoveAt(i);
//			i--;
//			nCnt1--;
//			continue;
// 		}

	}

	allRcsArray.RemoveAll();
	allRcsArray.Copy( charArray );

	for( i = 0 ; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height();
		int w1 = rcCur.Width();

		if( i != nCnt1 - 1 )
		{
			nCurSeqLen++;
			CRect rcNext =  charArray.GetAt( i + 1 );

			if( !RcsInSameLine(rcCur,rcNext) )//上下分层
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
				continue;
			}

			int h2 = rcNext.Height();
			int w2 = rcNext.Width();

			int nDis = rcNext.left - rcCur.right;
			if( nDis < 0 )
			{
				if( rcNext.right < rcCur.right )//嵌套
				{
					charArray.RemoveAt(i+1);
					i--;
					nCnt1--;
					nCurSeqLen--;
					continue;
				}
			}

			if( h1 / w1 >= 3 )
			{
				rcCur.left -= ( w - w1 ) * 0.8;
				rcCur.right += ( w - w1 ) * 0.8;
			}

			if( h2 / w2 >= 3 )
			{
				rcNext.left -= ( w - w2 ) * 0.8;
				rcNext.right += ( w - w2 ) * 0.8;
			}

			nDis = rcNext.left - rcCur.right;

			if( nRcDis < 5 ) nRcDis++;//平均字符距离太小时，避免fRatio条件太容易满足
			float fRatio = (float)nDis / (float)nRcDis;
			if( fRatio > 2.0f )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
			}

		}
		
	}

	if( nCurSeqLen >= 0 )
		intArray.Add(nCurSeqLen+1);

//	int nSeqs = intArray.GetSize();
//	for( i = 0; i < nSeqs; i++ )
//	{
//		int nCurNums = intArray.GetAt(i);
// 	}
	int nRemId = 0;
	int nSeqsCnt = intArray.GetSize();
	for( i = 0; i < nSeqsCnt; i++ )
	{
		int nSeqRcsCnt = intArray.GetAt(i);

		if( nSeqRcsCnt < 3 )
		{
			intArray.RemoveAt( i );
			i--;
			nSeqsCnt--;
			for( int n = 0; n < nSeqRcsCnt; n++ )
			{
				ASSERT( nRemId < charArray.GetSize() );
				charArray.RemoveAt(nRemId);
			}

			continue;
		}

		nRemId += nSeqRcsCnt;
	}


	if( charArray.GetSize() >= 10 )
	{
		bSuc = TRUE;
	}

	return bSuc;
}

BOOL CConNumProc::RectsAnalyzeForTopView( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray )
{
	intArray.RemoveAll();

	if( nRcDis == 0 ) nRcDis = h * 0.8;

	if( w==0 || h==0 || nRcDis == 0 )
	{
		return FALSE;
	}
	
	SortRect_T2B( charArray );
	SortRect_T2B( allRcsArray );

	BOOL bSuc = FALSE;
	
	int i;
	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();

	int nCntInSeq = 0;
	int nCurSeqLen = 0;

	int n1s = 0;
	int n1s_2 = 0;
	int nDisAb = 0;
	int nWHRatioAb = 0;

	for( i = 0; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height() + 1;
		int w1 = rcCur.Width() + 1;
		if( w1 <= 2 )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		if( (float)h1 / (float)w1 > 4.0f )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		if( w1 >= h1 * 2.0f )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		float fRatio = abs(h1-h) / (float)(h);
		float fRatioShort = (h - h1) / (float)(h); 
		if( fRatio > 0.5f  || fRatioShort > 0.2f )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

	}

	for( i = 0 ; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height() + 1;
		int w1 = rcCur.Width() + 1;



		//float fRatio = abs(h1-h) / (float)max(h1,h);
//		float fRatio = abs(h1-h) / (float)(h);
//		float fRatioShort = (h - h1) / (float)(h); 
//		
//		if( fRatio > 0.5f  || fRatioShort > 0.2f )
//		{
//			charArray.RemoveAt(i);
//			i--;
//			nCnt1--;
//			if( i == nCnt1 - 1 )
//				nCurSeqLen--;
//			continue;
// 		}
		
		if( (float)h1 / (float)w1 > 2.0f )
		{
			n1s++;
		}
		if( (float)h1 / (float)w1 > 2.5f )
		{
			n1s_2++;
		}
		
//		fRatio = abs(w1-w) / (float)max(w1,w);
//		if( fRatio > 0.3f )
//		{
//			charArray.RemoveAt(i);
//			i--;
//			nCnt1--;
//			continue;
// 		}

		if( i != nCnt1 - 1 )
		{
			nCurSeqLen++;
			CRect rcNext =  charArray.GetAt( i + 1 );

			//if( rcCur.bottom < rcNext.top )//上下分层
			if( !RcsInSameLine(rcCur,rcNext) )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
				continue;
			}

			int nVDis = abs( rcNext.top - rcCur.top );
			int nhmin = min(rcNext.Height(),rcCur.Height());
			if( nVDis > nhmin * 0.4f )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
				continue;				
			}

			int h2 = rcNext.Height();
			int w2 = rcNext.Width();

			int nDis = rcNext.left - rcCur.right;
			if( nDis < 0 )
			{
				CRect rcOL;
				if( rcOL.IntersectRect(&rcCur,&rcNext) )
				{
					int Area1 = h1 * w1;
					int Area2 = h2 * w2;
					int Area3 = rcOL.Width() * rcOL.Height();
					if( Area3 > min(Area1,Area2) / 4 )
					{
						CRect rcMerge = MergeRects( rcCur, rcNext );
						charArray.SetAt(i,rcMerge);
						charArray.RemoveAt(i+1);
						i--;
						nCnt1--;
						nCurSeqLen--;
						continue;
					}
				}
//				if( rcNext.right < rcCur.right )//嵌套
//				{
//					charArray.RemoveAt(i+1);
//					i--;
//					nCnt1--;
//					nCurSeqLen--;
//					continue;
// 				}
			}

			if( (float)h1 / (float)w1 >= 1.5f )
			{
				rcCur.left -= ( w - w1 ) * 0.8;
				rcCur.right += ( w - w1 ) * 0.8;
			}

			if( (float)h2 / (float)w2 >= 1.5f )
			{
				rcNext.left -= ( w - w2 ) * 0.8;
				rcNext.right += ( w - w2 ) * 0.8;
			}

			nDis = rcNext.left - rcCur.right;

			if( nRcDis < 5 ) nRcDis=5;//平均字符距离太小时，避免fRatio条件太容易满足
			float fRatio = (float)nDis / (float)nRcDis;
			if( fRatio > 2.0f )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
			}

//			if( (nRcDis > min( h, 25 ) ) && ( nDis < nRcDis * 0.25f) )
//			{
//				nDisAb++;
// 			}

		}
		
	}

	if( nCurSeqLen >= 0 )
		intArray.Add(nCurSeqLen+1);

	nCnt1 = charArray.GetSize();
	if( nCnt1 >= 6 )
	{
		bSuc = TRUE;
	}

	if( n1s >= 5 )
	{
		bSuc = FALSE;
	}
	if( n1s_2 >= 3 )
	{
		bSuc = FALSE;
	}
//	if( nDisAb > 1 )
//	{
//		bSuc = FALSE;
// 	}

	return bSuc;
}

BOOL CConNumProc::RectsAnalyze( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray)
{
	intArray.RemoveAll();

	if( w==0 || h==0 || nRcDis == 0 )
	{
		return FALSE;
	}
	
//	SortRect(charArray);
// 	SortRect(allRcsArray);
	SortRect_T2B( charArray );
	SortRect_T2B( allRcsArray );

	BOOL bSuc = FALSE;
	
	int i;
	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();

	int nCntInSeq = 0;
	int nCurSeqLen = 0;

	int n1s = 0;
	int n1s_2 = 0;
	int nDisAb = 0;
	int nWHRatioAb = 0;

	for( i = 0; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height() + 1;
		int w1 = rcCur.Width() + 1;
		if( w1 <= 2 )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		//if( (float)h1 / (float)w1 > 8.0f )//Original Condition
		if( (float)h1 / (float)w1 > 8.5f )//Michael Modify 2008-04-17 -- For Char 'I' (h / w = 25 / 3)
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		if( w1 >= h1)
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		//float fRatio = abs(h1-h) / (float)max(h1,h);
		float fRatio = abs(h1-h) / (float)(h);
		float fRatioShort = (h - h1) / (float)(h);
		
		//if( fRatio > 0.3f  || fRatioShort > 0.2f )
		if( fRatio > 0.4f  || fRatioShort > 0.2f )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

	}

	for( i = 0 ; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height() + 1;
		int w1 = rcCur.Width() + 1;

		if( (float)h1 / (float)w1 > 4.0f )
		{
			n1s++;
		}
		if( (float)h1 / (float)w1 > 7.0f )
		{
			n1s_2++;
		}

		//float fRatio = abs(h1-h) / (float)max(h1,h);
		float fRatio = abs(h1-h) / (float)(h);
		float fRatioShort = (h - h1) / (float)(h);

		if( i != nCnt1 - 1 )
		{
			nCurSeqLen++;
			CRect rcNext =  charArray.GetAt( i + 1 );

			//if( rcCur.bottom < rcNext.top )//上下分层
			if( !RcsInSameLine(rcCur,rcNext) )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
				
				if( (nCnt1 - i) <= 5)
				{
					nCurSeqLen = -1;//Mark quit
					break;
				}
				
				continue;
			}

			int nVDis = abs( rcNext.top - rcCur.top );
			int nhmin = min(rcNext.Height(),rcCur.Height());
			if( nVDis > nhmin * 0.4f )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
				continue;				
			}

			int h2 = rcNext.Height();
			int w2 = rcNext.Width();

			int nDis = rcNext.left - rcCur.right;
			if( nDis < 0 )
			{
				CRect rcOL;
				if( rcOL.IntersectRect(&rcCur,&rcNext) )
				{
					int Area1 = h1 * w1;
					int Area2 = h2 * w2;
					int Area3 = rcOL.Width() * rcOL.Height();
					if( Area3 > min(Area1,Area2) / 4 )
					{
						CRect rcMerge = MergeRects( rcCur, rcNext );
						charArray.SetAt(i,rcMerge);
						charArray.RemoveAt(i+1);
						i--;
						nCnt1--;
						nCurSeqLen--;
						continue;
					}
				}
//				if( rcNext.right < rcCur.right )//嵌套
//				{
//					charArray.RemoveAt(i+1);
//					i--;
//					nCnt1--;
//					nCurSeqLen--;
//					continue;
// 				}
			}

			if( h1 / w1 >= 3 )
			{
				rcCur.left -= ( w - w1 ) * 0.8;
				rcCur.right += ( w - w1 ) * 0.8;
			}

			if( h2 / w2 >= 3 )
			{
				rcNext.left -= ( w - w2 ) * 0.8;
				rcNext.right += ( w - w2 ) * 0.8;
			}

			nDis = rcNext.left - rcCur.right;

			if( nRcDis < 5 ) nRcDis++;//平均字符距离太小时，避免fRatio条件太容易满足
			fRatio = (float)nDis / (float)nRcDis;
			if( fRatio > 2.0f )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
			}

			if( (nRcDis > min( h, 25 ) ) && ( nDis < nRcDis * 0.25f) )
			{
				nDisAb++;
			}

		}
		
	}

	if( nCurSeqLen >= 0 )
		intArray.Add(nCurSeqLen+1);

	nCnt1 = charArray.GetSize();
	if( nCnt1 >= 5 )
	{
		bSuc = TRUE;
	}

	if( n1s >= 5 )
	{
		bSuc = FALSE;
	}
	if( n1s_2 >= 3 )
	{
		bSuc = FALSE;
	}
	if( nDisAb > 1 )
	{
		bSuc = FALSE;
	}

	return bSuc;
}

#ifdef TEST_CUT_CONF
BOOL CConNumProc::RectsAnalyze_ByProb( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray, CArray<int,int>& disArray)
{
	//PAY ATTENTION TO disArray

	intArray.RemoveAll();

	if( w==0 || h==0 || nRcDis == 0 )
	{
		return FALSE;
	}

	if( nRcDis < 5 ) nRcDis++;//平均字符距离太小时，避免fRatio条件太容易满足
	
//	SortRect(charArray);
// 	SortRect(allRcsArray);
	SortRect_T2B( charArray );
	SortRect_T2B( allRcsArray );

	BOOL bSuc = FALSE;

	int nApartVer = 9999;
	
	int i;
	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();

	int nCntInSeq = 0;
	int nCurSeqLen = 0;

	int n1s = 0;
	int n1s_2 = 0;
	int nDisAb = 0;
	int nWHRatioAb = 0;

	for( i = 0; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height() + 1;
		int w1 = rcCur.Width() + 1;
		if( w1 <= 2 )//too slim
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		if( (float)h1 / (float)w1 > 8.5f )//Michael Modify 2008-04-17 -- For Char 'I' (h / w = 25 / 3)
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		if( w1 >= h1)//Abnormal H/W Ratio
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

		float fRatio = abs(h1-h) / (float)(h);
		float fRatioShort = (h - h1) / (float)(h);
		
		if( fRatio > 0.4f  || fRatioShort > 0.2f )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			continue;
		}

	}

	for( i = 0 ; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height() + 1;
		int w1 = rcCur.Width() + 1;

		if( (float)h1 / (float)w1 > 4.0f )
		{
			n1s++;
		}
		if( (float)h1 / (float)w1 > 7.0f )
		{
			n1s_2++;
		}

		//float fRatio = abs(h1-h) / (float)max(h1,h);
		float fRatio = abs(h1-h) / (float)(h);
		float fRatioShort = (h - h1) / (float)(h);

		if( i != nCnt1 - 1 )
		{
			nCurSeqLen++;
			CRect rcNext =  charArray.GetAt( i + 1 );
	
			if( !RcsInSameLine(rcCur,rcNext) )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;

				disArray.Add( nApartVer );
				
				if( (nCnt1 - i) <= 5)
				{
					nCurSeqLen = -1;//Mark quit
					break;
				}
				
				continue;
			}

			int nVDis = abs( rcNext.top - rcCur.top );
			int nhmin = min(rcNext.Height(),rcCur.Height());
			if( nVDis > nhmin * 0.4f )//??????????????????????
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;

				disArray.Add( nApartVer );//??????????????????

				continue;				
			}

			int h2 = rcNext.Height();
			int w2 = rcNext.Width();

			int nDis = rcNext.left - rcCur.right;
			if( nDis < 0 )
			{
				CRect rcOL;
				if( rcOL.IntersectRect(&rcCur,&rcNext) )
				{
					int Area1 = h1 * w1;
					int Area2 = h2 * w2;
					int Area3 = rcOL.Width() * rcOL.Height();
					if( Area3 > min(Area1,Area2) / 4 )
					{
						CRect rcMerge = MergeRects( rcCur, rcNext );
						charArray.SetAt(i,rcMerge);
						charArray.RemoveAt(i+1);
						i--;
						nCnt1--;
						nCurSeqLen--;
						continue;
					}
				}
//				if( rcNext.right < rcCur.right )//嵌套
//				{
//					charArray.RemoveAt(i+1);
//					i--;
//					nCnt1--;
//					nCurSeqLen--;
//					continue;
// 				}
			}

			if( h1 / w1 >= 3 )
			{
				rcCur.left -= ( w - w1 ) * 0.8;
				rcCur.right += ( w - w1 ) * 0.8;
			}

			if( h2 / w2 >= 3 )
			{
				rcNext.left -= ( w - w2 ) * 0.8;
				rcNext.right += ( w - w2 ) * 0.8;
			}

			nDis = rcNext.left - rcCur.right;

			fRatio = (float)nDis / (float)nRcDis;
			if( fRatio > 2.0f )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;


				int nPosMisChars = 0;
				//nPosMisChars = int( (float)nDis / (float)( w + nRcDis ) + 0.5 );//Michael Update This Calculation 20081119(ORG)
				nPosMisChars = int( (float)( nDis - nRcDis ) / (float)( w + nRcDis ) + 0.5 );//Updated 20081119 -- Michael(MOD)

				disArray.Add( nPosMisChars );

			}

			if( (nRcDis > min( h, 25 ) ) && ( nDis < nRcDis * 0.25f) )
			{
				nDisAb++;
			}

		}
		
	}

	if( nCurSeqLen >= 0 )
		intArray.Add(nCurSeqLen+1);

	nCnt1 = charArray.GetSize();
	if( nCnt1 >= 5 )
	{
		bSuc = TRUE;
	}

	if( n1s >= 5 )
	{
		bSuc = FALSE;
	}
	if( n1s_2 >= 3 )
	{
		bSuc = FALSE;
	}
	if( nDisAb > 1 )
	{
		bSuc = FALSE;
	}

	return bSuc;
}
#endif

BOOL CConNumProc::MergeApartRcsByScan( ObjRectArray& allRcsArray, ObjRectArray& resArray, int w, int h )
{
	resArray.RemoveAll();

	if( w == 0 || h == 0 )
	{
		return FALSE;
	}

	BOOL bSuc = FALSE;
	SortRect( allRcsArray );
	int nCnt = allRcsArray.GetSize();

	for( int i = 0; i < nCnt; i++ )
	{
		CRect rcCur = allRcsArray.GetAt(i);

		int h1 = rcCur.Height() + 1;
		int w1 = rcCur.Width() + 1;

		float fHDifRatio = ( h1 - h ) / (float)h;
		float fWDifRatio = ( w1 - w ) / (float)w;
		if( fHDifRatio > 0.15f || fWDifRatio > 1.0f )
		{
			continue;
		}
		if( fabs(fHDifRatio) <= 0.15f && fWDifRatio > 0.0f && fabs(fWDifRatio) <= 0.5f )//Do not lower 0.5f, because of "M"
		{
			resArray.Add(rcCur);
			continue;
		}
		if( fabs(fHDifRatio) <= 0.15f && fWDifRatio <= 0.8f && fWDifRatio >= 0.0f )
		{
			resArray.Add(rcCur);
			continue;
		}
		
		BOOL bMergeSuc = FALSE;
		CRect rcMerge = rcCur;
		for( int j = i+1; j < nCnt; j++ )
		{
			rcCur = rcMerge;
			CRect rcNext = allRcsArray.GetAt(j);
			rcMerge = MergeRects( rcMerge, rcNext );

			int h2 = rcMerge.Height() + 1;
			int w2 = rcMerge.Width() + 1;

			float fHDifRatio2 = (h2 - h) / (float)h;
			float fWDifRatio2 = (w2 - w) / (float)w;
			if( fHDifRatio2 > 0.25f || fWDifRatio2 > 0.5f ) break;
			if( fabs(fHDifRatio2) <= 0.25f && fWDifRatio2 > 0.0f && fabs(fWDifRatio2) <= 0.5f )
			{
				resArray.Add(rcMerge);
				i = j;
				bMergeSuc = TRUE;
				break;
			}
		}
		if( !bMergeSuc )
		{
			h1 = rcCur.Height() + 1;
			w1 = rcCur.Width() + 1;
			fHDifRatio = ( h1 - h ) / (float)h;
			fWDifRatio = ( w1 - w ) / (float)w;
			if( fabs(fHDifRatio) <= 0.25f && w1 < w * 1.5f && w1 >= h1 * 0.125f )
			{
				resArray.Add(rcCur);
				continue;
			}
		}
	}

	return bSuc;
}

BOOL CConNumProc::MerVerApartRcs( ObjRectArray& charArray, ObjRectArray& allRcsArray, int w, int h )//Merge Split Chars in Ver-Aligned Container Number
{
	if( w == 0 || h == 0 )
	{
		return FALSE;
	}
	
	BOOL bMerge = FALSE;

	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();

#ifndef CHECK_ERROR
	if( nCnt1 <= 2 ) return FALSE;
	if( nCnt2 <= 2 ) return FALSE;
#endif

	//SortRectByCols( charArray );
	//SortRectByCols( allRcsArray );
	VSortRect( charArray );
	VSortRect( allRcsArray );

	int i = 0;
	int j = 0;
	ObjRectArray FinalResArray;
	while( charArray.GetSize() > 0 )
	{
		ObjRectArray CurLineRcs;
//////////////////Get Current Col/////////////////////
		CRect rcCur = charArray.GetAt(0);
		charArray.RemoveAt(0);
		CurLineRcs.Add( rcCur );
		nCnt1 = charArray.GetSize();

		for( i = 0; i < nCnt1; i++ )
		{
			CRect rcNext = charArray.GetAt(i);
			if( RcsInSameCol( rcCur, rcNext ) )
			{
				CurLineRcs.Add( rcNext );
				//rcNext = rcCur;//Rects Propagation
				rcCur = MergeRects( rcCur, rcNext );//Region Update
				charArray.RemoveAt(i);
				i--;
				nCnt1--;
			}
		}
///////////////End--Get Current Col///////////////////

		int nCurCnt = CurLineRcs.GetSize();
		VSortRect( CurLineRcs );
		for( i = 0; i < nCurCnt-1; i++ )//if all the split parts are in charArray
		{
			CRect rcCur = CurLineRcs.GetAt(i);
			CRect rcNext = CurLineRcs.GetAt(i+1);
			if( max(rcCur.bottom,rcNext.bottom) - min(rcCur.top,rcNext.top) < h * 1.2f )
			{
				CRect rcMerge = MergeRects( rcCur, rcNext );
				if( rcMerge.Width() < w * 1.2f )
				{
					CurLineRcs.SetAt( i, rcMerge );
					CurLineRcs.RemoveAt( i + 1 );
					nCurCnt--;
					i--;
					bMerge = TRUE;
				}
			}
		}

		CRect rcSearchRgn = CRect(0,0,0,0);
		float fWExR = 1.2f;
		float fHExR = 1.2f;
		for( i = 0; i < nCurCnt; i++ )//if some of the split parts are in charArray
		{
			CRect rcCur = CurLineRcs.GetAt(i);

			if( rcCur.Height() < h * 0.8f || rcCur.Width() < w * 0.8f )
			{
				rcSearchRgn = CRect( rcCur.right - w * fWExR,
									 rcCur.bottom - h * fHExR,
									 rcCur.left + w * fWExR,
									 rcCur.top + h * fHExR );

				ObjRectArray rcAnaRcsArray;
				for( j = 0; j < nCnt2; j++ )
				{
					CRect rcTmp = allRcsArray.GetAt(j);
					if( RcInRgn( rcTmp, rcSearchRgn ) )
					{
						rcAnaRcsArray.Add( rcTmp );
					}
					//Considering Quik Quit
				}
				
				CRect rcMerge;
				if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
				{
#ifdef CHECK_ERROR
					ASSERT( rcAnaRcsArray.GetSize() == 0 );
					ASSERT( rcMerge != CRect(0,0,0,0));
#endif				
					CurLineRcs.SetAt( i, rcMerge );
					bMerge = TRUE;
				}				
			}
		}

		for( i = 0; i < nCurCnt-1; i++ )//none of the split parts are in charArray 
		{
			CRect rcCur = CurLineRcs.GetAt(i);
			CRect rcNext = CurLineRcs.GetAt(i+1);

			if( (rcNext.top - rcCur.bottom) > h * 1.1f )
			{
				rcSearchRgn = CRect( min( rcCur.left, rcNext.left ) - w * (fWExR - 1.0f),
												 rcCur.bottom,
												 max( rcCur.right, rcNext.right ) + w * (fWExR -1.0f),
												 rcNext.top
												);
				ObjRectArray rcAnaRcsArray;
				for( j = 0; j < nCnt2; j++ )
				{
					CRect rcTmp = allRcsArray.GetAt(j);
					if( RcInRgn( rcTmp, rcSearchRgn ) )
					{
						rcAnaRcsArray.Add( rcTmp );
					}
					//Considering Quik Quit
				}
				
				CRect rcMerge;
				if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
				{
#ifdef CHECK_ERROR
					ASSERT( rcAnaRcsArray.GetSize() == 0 );
					ASSERT( rcMerge != CRect(0,0,0,0));
#endif				
					CurLineRcs.InsertAt( i+1, rcMerge );
					bMerge = TRUE;
					nCurCnt++;
				}

			}
		}

		FinalResArray.Append( CurLineRcs );

	}

	charArray.Append( FinalResArray );

	return bMerge;
}

BOOL CConNumProc::MergeApartRcs( ObjRectArray& charArray, ObjRectArray& allRcsArray, int w, int h, int nRcDis )
{
	if( w == 0 || h == 0 )
	{
		return FALSE;
	}

	if( nRcDis <= 0 ) return FALSE;
	
	BOOL bSuc = FALSE;

	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();
	if( nCnt1 == 0 ) return FALSE;
	if( nCnt2 == 0 ) return FALSE;

	SortRect( charArray );
	SortRect( allRcsArray );

	int i = 0;
	int j = 0;

	ObjRectArray FinalResArray;
	while( charArray.GetSize() > 0 )
	{
		ObjRectArray CurLineRcs;
		CRect rcCur = charArray.GetAt(0);
		charArray.RemoveAt(0);
		CurLineRcs.Add( rcCur );
		nCnt1 = charArray.GetSize();
		for( i = 0; i < nCnt1; i++ )
		{
			CRect rcNext = charArray.GetAt(i);
			if( RcsInSameLine( rcCur, rcNext ) )
			{
				CurLineRcs.Add( rcNext );
				rcNext = rcCur;
				charArray.RemoveAt(i);
				i--;
				nCnt1--;
			}
//			else
//			{
//				int nMergeHeight = max(rcCur.bottom,rcNext.bottom) - min(rcCur.top,rcNext.top);
//				if( nMergeHeight < h * 1.2f )
//				{
//					CurLineRcs.Add( rcNext );
//					rcNext = rcCur;
//					charArray.RemoveAt(i);
//					i--;
//					nCnt1--;
//				}
// 			}
		}
		
		int nCurCnt = CurLineRcs.GetSize();

		CRect rcSearchRgn = CRect(0,0,0,0);
		CRect rcRef = CurLineRcs.GetAt( 0 );
		rcSearchRgn = CRect( rcRef.left - (w + nRcDis) * 1.2f ,
								rcRef.top - h * 0.2f,
								rcRef.left,
								rcRef.bottom + h * 0.2f );
		ObjRectArray rcAnaRcsArray;
		for( j = 0; j < nCnt2; j++ )
		{
			CRect rcCur = allRcsArray.GetAt(j);
			if( RcInRgn( rcCur, rcSearchRgn ) )
			{
				rcAnaRcsArray.Add( rcCur );
			}
			//考虑快速退出代码
		}

		CRect rcMerge;
		if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
		{
			ASSERT( rcAnaRcsArray.GetSize() == 0 );
			ASSERT( rcMerge != CRect(0,0,0,0));
			FinalResArray.Add( rcMerge );
			bSuc = TRUE;
		}
		FinalResArray.Add( rcRef );
		CRect rcPre = CRect(0,0,0,0);
		for( i = 1; i < nCurCnt; i++ )
		{
			rcPre = rcRef;			
			rcRef = CurLineRcs.GetAt(i);
			FinalResArray.Add( rcRef );

			int nDis = rcRef.left - rcPre.right;
			if( ( nDis > w + nRcDis)
				&& nDis < (w + nRcDis) * 2
				)
			{
				rcSearchRgn = CRect( rcPre.right,
										min( rcRef.top, rcPre.top ),
										rcRef.left,
										max( rcRef.bottom, rcPre.bottom )										
										);

				//rcAnaRcsArray.RemoveAll();//EsRect函数包含此功能
				for( j = 0; j < nCnt2; j++ )
				{
					CRect rcCur = allRcsArray.GetAt(j);
					if( RcInRgn( rcCur, rcSearchRgn ) )
					{
						rcAnaRcsArray.Add( rcCur );
					}
					//考虑快速退出代码
				}

				if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
				{
					ASSERT( rcAnaRcsArray.GetSize() == 0 );
					ASSERT( rcMerge != CRect(0,0,0,0));
					FinalResArray.Add( rcMerge );
					bSuc = TRUE;


				}
				
			}

			if( nDis > (w + nRcDis) * 2 )
			{
				rcSearchRgn = CRect( rcPre.right - w * 0.1 ,
										min( rcRef.top, rcPre.top ),
										rcPre.right + (w + nRcDis) * 1.2f,
										max( rcRef.bottom, rcPre.bottom )										
										);

				//rcAnaRcsArray.RemoveAll();//EsRect函数包含此功能
				for( j = 0; j < nCnt2; j++ )
				{
					CRect rcCur = allRcsArray.GetAt(j);
					if( RcInRgn( rcCur, rcSearchRgn ) )
					{
						rcAnaRcsArray.Add( rcCur );
					}
					//考虑快速退出代码
				}

				if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
				{
					ASSERT( rcAnaRcsArray.GetSize() == 0 );
					ASSERT( rcMerge != CRect(0,0,0,0));
					//FinalResArray.Add( rcMerge );
					bSuc = TRUE;
					CurLineRcs.SetAt(i,rcMerge);
					i--;
				}
			}
			
		}

		//rcRef = CurLineRcs.GetAt( nCnt1 - 1 );
		rcSearchRgn = CRect( rcRef.right  ,
								rcRef.top - h * 0.2f,
								rcRef.right + (w + nRcDis) * 1.2f,
								rcRef.bottom + h * 0.2f );

		for( j = 0; j < nCnt2; j++ )
		{
			CRect rcCur = allRcsArray.GetAt(j);
			if( RcInRgn( rcCur, rcSearchRgn ) )
			{
				rcAnaRcsArray.Add( rcCur );
			}
			//考虑快速退出代码
		}

		if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
		{
			ASSERT( rcAnaRcsArray.GetSize() == 0 );
			ASSERT( rcMerge != CRect(0,0,0,0));
			FinalResArray.Add( rcMerge );
			bSuc = TRUE;
		}
		//FinalResArray.Add( rcRef );
		

	}
	

	charArray.Copy( FinalResArray );

	return bSuc;
}

//Michael Add 2008-04-16
BOOL CConNumProc::MergeSplittedChars( ObjRectArray& charArray, ObjRectArray& allRcsArray, int w, int h, int nRcDis )
{
	BOOL bSuc = FALSE;
	
	int nCnt1 = charArray.GetSize();
	if( nCnt1 <= 2 ) return FALSE;

	int nCnt2 = allRcsArray.GetSize();
	if( nCnt2 <= 2 ) return FALSE;

	SortRect( charArray );
	SortRect( allRcsArray );

	int i = 0;
	int j = 0;

	ObjRectArray CurLineRcs;
	ObjRectArray rcsAnaArray;
	ObjRectArray FinalRcs;
	while( charArray.GetSize() > 0 )
	{
		CurLineRcs.RemoveAll();
		CRect rcCur = charArray.GetAt(0);
		charArray.RemoveAt(0);
		CurLineRcs.Add( rcCur );
		nCnt1 = charArray.GetSize();
		for( i = 0; i < nCnt1; i++ )
		{
			CRect rcNext = charArray.GetAt(i);
			if( RcsInSameLine( rcCur, rcNext ) )
			{
				CurLineRcs.Add( rcNext );
				//rcNext = rcCur;//Bug?
				rcCur = rcNext;//Michael Modify 20080612
				charArray.RemoveAt(i);
				i--;
				nCnt1--;
			}
		}

		int nCurCnt = CurLineRcs.GetSize();
		for( i = 0; i < nCurCnt; i++ )
		{
			CRect rcCur = CurLineRcs.GetAt(i);
			
			int nh1 = rcCur.Height();
			int nw1 = rcCur.Width();

			if( nw1 < w * 0.8f )
			{
				if( i != nCurCnt-1 )
				{
					CRect rcNext = CurLineRcs.GetAt(i+1);
					if( (rcNext.right - rcCur.left) < w * 1.1f )
					{
						CRect rcMerge = MergeRects( rcCur, rcNext );
						CurLineRcs.SetAt( i, rcMerge );
						CurLineRcs.RemoveAt(i+1);
						nCurCnt--;
						bSuc = TRUE;
						continue;
					}
				}
			}

			if( nh1 < h * 0.9 )
			{
				CRect rcSearchRgn = CRect( 
											min( rcCur.right - w * 1.1f, rcCur.left - 1 ),
											rcCur.bottom - h * 1.1f,
											max( rcCur.left + w * 1.1f, rcCur.right + 1),
											rcCur.top + h * 1.1f
											);

#ifdef CHECK_ERROR
				ASSERT( rcsAnaArray.GetSize() == 0 );
#endif
				rcsAnaArray.RemoveAll();
				for( j = 0; j < nCnt2; j++ )
				{
					CRect rcTry = allRcsArray.GetAt(j);
					if( RcInRgn( rcTry, rcSearchRgn ) )
					{
						rcsAnaArray.Add( rcTry );
					}
				}

				CRect rcMerge;
				if( EsRect( rcsAnaArray, w, h, rcMerge ) )
				{
#ifdef CHECK_ERROR
					ASSERT( rcsAnaArray.GetSize() == 0 );
					ASSERT( rcMerge != CRect(0,0,0,0));
#endif				
					CurLineRcs.SetAt( i, rcMerge );
					bSuc = TRUE;
					continue;
				}
			}
		}

		//FinalRcs.Copy( CurLineRcs );//BUG?
		FinalRcs.Append( CurLineRcs );//Michael Modify 2008-04-17 -- In Case of FinalRcs be covered
	}

	charArray.Copy(FinalRcs);

	return bSuc;
}

BOOL CConNumProc::RectsAnalyzeByScan( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray)
{
	intArray.RemoveAll();

	if( w==0 || h==0 || nRcDis == 0 )
	{
		return FALSE;
	}

	BOOL bSuc = FALSE;
	
	SortRect( charArray );
	SortRect( allRcsArray );

	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();

	if( nCnt1 == 0 ) return FALSE;
	if( nCnt2 == 0 ) return FALSE;

	int i = 0;
	int j = 0;

	ObjRectArray FinalResArray;
	ObjRectArray rcAnaRcsArray;
	while( charArray.GetSize() > 0 )
	{
		ObjRectArray CurLineRcs;
		CRect rcCur = charArray.GetAt(0);
		charArray.RemoveAt(0);
		CurLineRcs.Add( rcCur );
		nCnt1 = charArray.GetSize();
		for( i = 0; i < nCnt1; i++ )
		{
			CRect rcNext = charArray.GetAt(i);
			if( RcsInSameLine( rcCur, rcNext ) )
			{
				CurLineRcs.Add( rcNext );
				//rcNext = rcCur;//Bug?
				rcCur = rcNext;//Michael Modify 20080612
				charArray.RemoveAt(i);
				i--;
				nCnt1--;
			}
		}
		
		int nCurCnt = CurLineRcs.GetSize();

////////////////////////////////////Michael Add 2008-05-07 For char "M" In LLT Cut//////////////////////////////////////
		for( i = 0; i < nCurCnt-1; i++ )//if all the split parts are in charArray
		{
			CRect rcCur = CurLineRcs.GetAt(i);
			CRect rcNext = CurLineRcs.GetAt(i+1);
			if( rcNext.left - rcCur.right < nRcDis * 0.2f )
			{
				CRect rcMerge = MergeRects( rcCur, rcNext );
				if( rcMerge.Width() < w * 1.8f )
				{
					CurLineRcs.SetAt( i, rcMerge );
					CurLineRcs.RemoveAt( i + 1 );
					nCurCnt--;
					i--;
				}
			}
 		}
/////////////////////////////////End -- Michael Add 2008-05-07 For char "M" In LLT Cut////////////////////////////////

		CRect rcSearchRgn = CRect(0,0,0,0);
		CRect rcRef = CurLineRcs.GetAt( 0 );
		rcSearchRgn = CRect( rcRef.left - (w + nRcDis) * 1.2f ,
								rcRef.top - h * 0.2f,
								rcRef.left,
								rcRef.bottom + h * 0.2f );
		//rcAnaRcsArray.RemoveAll();
		ASSERT( rcAnaRcsArray.GetSize() == 0 );
		for( j = 0; j < nCnt2; j++ )
		{
			CRect rcCur = allRcsArray.GetAt(j);
			if( RcInRgn( rcCur, rcSearchRgn ) )
			{
				rcAnaRcsArray.Add( rcCur );
			}
			//考虑快速退出代码
		}

		CRect rcMerge;
		if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
		{
			ASSERT( rcAnaRcsArray.GetSize() == 0 );
			ASSERT( rcMerge != CRect(0,0,0,0));
			FinalResArray.Add( rcMerge );
			bSuc = TRUE;
		}
		FinalResArray.Add( rcRef );
		CRect rcPre = CRect(0,0,0,0);
		for( i = 1; i < nCurCnt; i++ )
		{
			rcPre = rcRef;
			rcRef = CurLineRcs.GetAt(i);
			FinalResArray.Add( rcRef );

			int nDis = rcRef.left - rcPre.right;
			if( ( nDis > w + nRcDis)
				&& nDis < (w + nRcDis) * 2
				)
			{
				rcSearchRgn = CRect( rcPre.right,
										//min( rcRef.top, rcPre.top ),//Original Condition
										min( rcRef.top, rcPre.top ) - min( h * 0.15, 3),//Michael Modify 2008-04-17 -- rcSearchRgn
										rcRef.left,
										//max( rcRef.bottom, rcPre.bottom )//Original Condition
										max( rcRef.bottom, rcPre.bottom ) + min( h * 0.15, 3)//Michael Modify 2008-04-17 -- rcSearchRgn
										);

				//rcAnaRcsArray.RemoveAll();//EsRect函数包含此功能
				for( j = 0; j < nCnt2; j++ )
				{
					CRect rcCur = allRcsArray.GetAt(j);
					if( RcInRgn( rcCur, rcSearchRgn ) )
					{
						rcAnaRcsArray.Add( rcCur );
					}
					//考虑快速退出代码
				}

				if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
				{
					ASSERT( rcAnaRcsArray.GetSize() == 0 );
					ASSERT( rcMerge != CRect(0,0,0,0));
					FinalResArray.Add( rcMerge );
					bSuc = TRUE;
				}
				
			}
			else if( nDis >= (w + nRcDis) * 2 )//Try to merge the split char in a distance
			{
				CRect rcCur = CRect(0,0,0,0);
				for( j = 0; j < nCnt2; j++ )
				{
					rcCur = allRcsArray.GetAt(j);
					if( rcCur.left > rcPre.right )
					{
						break;
					}
				}

				if( j != nCnt2 )
				{
					if( rcCur.right < rcRef.left )
					{
						rcSearchRgn = CRect( rcCur.left - 1,
							rcRef.top - h * 0.2f,
							rcCur.left + w * 1.2f,
							rcRef.bottom + h * 0.2f );
						
						ASSERT( rcAnaRcsArray.GetSize() == 0 );
						for( int jj = j; jj < nCnt2; jj++ )
						{
							rcCur = allRcsArray.GetAt(jj);
							if( RcInRgn( rcCur, rcSearchRgn ) )
							{
								rcAnaRcsArray.Add( rcCur );
							}
						}
						
						CRect rcMerge = CRect(0,0,0,0);
						if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
						{
							ASSERT( rcAnaRcsArray.GetSize() == 0 );
							ASSERT( rcMerge != CRect(0,0,0,0));
							FinalResArray.Add( rcMerge );
							bSuc = TRUE;
							rcRef = rcMerge;
							i--;
						}						
					}
				}
			}



			
			
		}

		//rcRef = CurLineRcs.GetAt( nCnt1 - 1 );
		rcSearchRgn = CRect( rcRef.right,
								rcRef.top - h * 0.2f,
								rcRef.right + (w + nRcDis) * 1.2f,
								rcRef.bottom + h * 0.2f );

		for( j = 0; j < nCnt2; j++ )
		{
			CRect rcCur = allRcsArray.GetAt(j);
			if( RcInRgn( rcCur, rcSearchRgn ) )
			{
				rcAnaRcsArray.Add( rcCur );
			}
			//考虑快速退出代码
		}

		if( EsRect( rcAnaRcsArray, w, h, rcMerge ) )
		{
			ASSERT( rcAnaRcsArray.GetSize() == 0 );
			ASSERT( rcMerge != CRect(0,0,0,0));
			FinalResArray.Add( rcMerge );
			bSuc = TRUE;
		}
		//FinalResArray.Add( rcRef );
		

	}

	//Try to merge the last rect
	//*
	nCnt1 = FinalResArray.GetSize();
	if( nCnt1 >= 6 )//Try to find the check rect
	{
		CRect rcRef = FinalResArray.GetAt(nCnt1-1);

		CRect rcNext = CRect(0,0,0,0);
		for( i = 0; i < nCnt2; i++ )
		{
			rcNext = allRcsArray.GetAt(i);
			if( rcNext.left > rcRef.right && RcsInSameLine( rcRef, rcNext ) )
			{
				break;
			}
		}

		if( i != nCnt2 )
		{
			ASSERT( rcNext.left > rcRef.right );
			
			CRect rcSearchRgn = CRect( rcNext.left - 1,
									rcRef.top - h * 0.2f,
									//rcNext.left + w * 1.5f,//Original Condition
									rcNext.left + w * 2.0f,//Michael Change 20080419
									rcRef.bottom + h * 0.2f );

			ASSERT( rcAnaRcsArray.GetSize() == 0 );
			for( j = i; j < nCnt2; j++ )
			{
				CRect rcCur = allRcsArray.GetAt(j);
				if( RcInRgn( rcCur, rcSearchRgn ) )
				{
					rcAnaRcsArray.Add( rcCur );
				}
			}

			CRect rcMerge = CRect(0,0,0,0);
			//if( EsRect( rcAnaRcsArray, w*1.2, h, rcMerge ) )
			if( EsRect(rcAnaRcsArray, w*1.4, h, rcMerge))//Michael Change 20080419
			{
				ASSERT( rcAnaRcsArray.GetSize() == 0 );
				ASSERT( rcMerge != CRect(0,0,0,0));
				FinalResArray.Add( rcMerge );
				bSuc = TRUE;
			}
		}
	}
	//*/

	charArray.Copy( FinalResArray );



	return bSuc;
}

//BOOL CConNumProc::EsRect( ObjRectArray& rcAnaRcsArray, int w, int h, CRect& rcMerge )
//{
//	rcMerge = CRect(0,0,0,0);
//	int nAnaRcsCnt = rcAnaRcsArray.GetSize();
//	if( nAnaRcsCnt <= 1 )
//	{
//		rcAnaRcsArray.RemoveAll();
//		return FALSE;
//	}
//
//	SortRect( rcAnaRcsArray );
//// Original Merge Method
////	rcMerge = rcAnaRcsArray.GetAt(0);
////	for( int jj = 1; jj < nAnaRcsCnt; jj++ )
////	{
////		CRect rcCur = rcAnaRcsArray.GetAt(jj);
////		rcMerge = MergeRects( rcCur, rcMerge );
////	}
//// End -- Original Merge Method
//
////Michael Modify 2008-04-17 -- Merge & Judge
//	CRect rcRef = rcAnaRcsArray.GetAt(0);
//	for( int jj = 1; jj < nAnaRcsCnt; jj++ )
//	{
//		CRect rcCur = rcAnaRcsArray.GetAt(jj);
//		rcRef = MergeRects( rcCur, rcRef );
//
//		int hRef = rcRef.Height() + 1;
//		int wRef = rcRef.Width() + 1;
//
//		if( hRef < h * 1.3f && wRef < w * 1.5f )
//		{
//			rcMerge = rcRef;
//		}
//		else
//		{
//			rcRef = rcMerge;
//		}
//	}
////End -- Merge & Judge
//
//	rcAnaRcsArray.RemoveAll();
//	int h1 = rcMerge.Height() + 1;
//	int w1 = rcMerge.Width() + 1;
//
//	float fRatio = abs(h1-h) / (float)(h);
//	float fRatioShort = (h - h1) / (float)(h); 
//		
//	if( fRatio > 0.3f  || fRatioShort > 0.2f || w1 >= w * 1.5f )
//	{
//		rcMerge = CRect(0,0,0,0);
//		return FALSE;
//	}
//
//	return TRUE;
//}

BOOL CConNumProc::TopRectsAnalyze( ObjRectArray &charArray )
{
	int i = 0;
	int nCnt = charArray.GetSize();

	if( nCnt < 4 ) return FALSE;

	BOOL bSuc = FALSE;

	int nCharW = 0;
	int nCharH = 0;
	GetAverRcWH( charArray, nCharW, nCharH );

	if( nCharW == 0 || nCharH == 0 )
	{
		return FALSE;
	}

	CRect rcCur = CRect(0,0,0,0);
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = charArray.GetAt(i);
		int nh = rcCur.Height();
		int nw = rcCur.Width();
		if( nh < nCharH * 0.8 || nh > nCharH * 1.2 )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt--;
			continue;
		}
		if( nw > nCharW * 1.8 )//Do not set lower, because of 'M'
		{
			charArray.RemoveAt(i);
			i--;
			nCnt--;
			continue;
		}
	}

	nCnt = charArray.GetSize();
	while( nCnt >= 4 )
	{
		SortRect(charArray);
		ObjRectArray rcArrayCurLine;

		CRect rcCur = charArray.GetAt(0);
		CRect rcNext = CRect(0,0,0,0);
		rcArrayCurLine.RemoveAll();
		rcArrayCurLine.Add(rcCur);
		charArray.RemoveAt(0);
		nCnt--;
		for( i = 0; i < nCnt; i++ )
		{
			rcNext = charArray.GetAt(i);

			if( !RcsInSameLine(rcCur,rcNext) )
			{
				continue;
			}

			int nDis = rcNext.left - rcCur.right;
			
			if( nDis >= rcCur.Height() * 2 )
			{
				break;
			}

			if( nDis < 0 )
			{
				CRect rcOL;
				if( rcOL.IntersectRect(&rcCur,&rcNext) )//when rects intersect.
				{
					int Area1 = rcCur.Width() * rcCur.Height();
					int Area2 = rcNext.Width() * rcNext.Height();
					int Area3 = rcOL.Width() * rcOL.Height();
					if( Area3 > min(Area1,Area2) / 4 )
					{
						charArray.RemoveAt(i);
						i--;
						nCnt--;
						continue;
					}
				}
			}

			rcArrayCurLine.Add(rcNext);
			charArray.RemoveAt(i);
			i--;
			nCnt--;
			rcCur = rcNext;
		}

		int nCurLineRcs = rcArrayCurLine.GetSize();
		if( nCurLineRcs >= 4 )
		{
			//bSuc = TRUE;
			charArray.RemoveAll();
			charArray.Copy( rcArrayCurLine );
			break;
		}
	}
	
	if( charArray.GetSize() == 4 )
	{
		int nCharAbCnt = 0;
		for( i = 0; i < 4; i++ )
		{
			CRect rcCur = charArray.GetAt(i);
			int h1 = rcCur.Height() + 1;
			int w1 = rcCur.Width() + 1;
			if( (float)h1 / (float)w1 > 3.0f )
			{
				nCharAbCnt++;
			}	
		}

		if( nCharAbCnt == 4 )
		{
			bSuc = FALSE;
		}
		else
		{
			bSuc = TRUE;
		}
	}

	return (bSuc);
}

BOOL CConNumProc::RectsVerAnalyze( ObjRectArray &charArray, ObjRectArray &allRcsArray, int w, int h, int nRcDis, CArray<int,int>& intArray)
{
	intArray.RemoveAll();

	if( w==0 || h==0 || nRcDis == 0 )
	{
		return FALSE;
	}
	
	SortRect_T2B( charArray );
	SortRect_T2B( allRcsArray );

	BOOL bSuc = FALSE;

	int i;
	int nCnt1 = charArray.GetSize();
	int nCnt2 = allRcsArray.GetSize();

	int nCntInSeq = 0;
	int nCurSeqLen = 0;

	for( i = 0 ; i < nCnt1; i++ )
	{
		CRect rcCur = charArray.GetAt( i );
		int h1 = rcCur.Height();
		int w1 = rcCur.Width();

		//if( w1 <= 2 )//original
		if( w < 2 )//20080411 adjust for the char "1"
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			if( i == nCnt1 - 1 )
				nCurSeqLen--;
			continue;
		}

		//float fRatio = abs(h1-h) / (float)max(h1,h);
		float fRatio = abs(h1-h) / (float)(h);
		float fRatioShort = (h - h1) / (float)(h); 

		if( fRatio > 0.3f  || fRatioShort > 0.2f )
		{
			charArray.RemoveAt(i);
			i--;
			nCnt1--;
			if( i == nCnt1 - 1 )
				nCurSeqLen--;
			continue;
		}

//		fRatio = abs(w1-w) / (float)max(w1,w);
//		if( fRatio > 0.3f )
//		{
//			charArray.RemoveAt(i);
//			i--;
//			nCnt1--;
//			continue;
// 		}

		if( i != nCnt1 - 1 )
		{
			nCurSeqLen++;
			CRect rcNext =  charArray.GetAt( i + 1 );

			int h2 = rcNext.Height();
			int w2 = rcNext.Width();

			int nDis = rcNext.top - rcCur.bottom;
			if( nDis > nRcDis + 1 )
			{
				intArray.Add( nCurSeqLen );
				nCurSeqLen = 0;
			}

//			if( nDis < 0 )
//			{
//				if( rcNext.right < rcCur.right )//嵌套
//				{
//					charArray.RemoveAt(i+1);
//					i--;
//					nCnt1--;
//					nCurSeqLen--;
//					continue;
//				}
// 			}

//			if( h1 / w1 >= 3 )
//			{
//				rcCur.left -= ( w - w1 ) * 0.8;
//				rcCur.right += ( w - w1 ) * 0.8;
//			}
//
//			if( h2 / w2 >= 3 )
//			{
//				rcNext.left -= ( w - w2 ) * 0.8;
//				rcNext.right += ( w - w2 ) * 0.8;
// 			}

//			nDis = rcNext.left - rcCur.right;

//			if( nRcDis < 5 ) nRcDis++;//平均字符距离太小时，避免fRatio条件太容易满足
//			fRatio = (float)nDis / (float)nRcDis;
//			if( fRatio > 2.0f )
//			{
//				intArray.Add( nCurSeqLen );
//				nCurSeqLen = 0;
// 			}

		}
		
	}

	if( nCurSeqLen >= 0 )
		intArray.Add(nCurSeqLen+1);

	if( charArray.GetSize() >= 10 )
	{
		bSuc = TRUE;
	}

	return bSuc;
}

BOOL CConNumProc::RecCharArrayForTopView( IMAGE ImgGray, ObjRectArray &charArray, CString& resStr, BOOL bNums)
{
	const int WIDTH = NORM_CHAR_WIDTH;
	const int HEIGHT = NORM_CHAR_HEIGHT;

	IMAGE TIMG_gray = ImageAlloc(WIDTH, HEIGHT);
	IMAGE TIMG_bin = ImageAlloc(WIDTH, HEIGHT);

	int nCnt = charArray.GetSize();

	for( int i = 0; i < nCnt; i++ )
	{
		CRect rcChar = charArray.GetAt(i);
		
		int nRcH = rcChar.Height();
		int nRcW = rcChar.Width();

		Norm_Image( ImgGray , TIMG_gray , rcChar );
		int nThre = ImgDaJinThreshold( TIMG_gray );
		ImgSingleSegment(TIMG_gray, TIMG_bin, nThre, 255, 0);	
		
		if( nRcH / (float)nRcW >= 1.5f )//判定为“1”的条件,需要判断除0错?
		{
			int nCnt = 0;
			if( nRcH / (float)nRcW >= 2.0f )
			{
				if( bNums )
					resStr += _T("1");
				else
					resStr += _T("I");

				continue;
 			}

			//Check for character "1", method 1
//			for( int jj = WIDTH / 3; jj < WIDTH; jj++ )
//			{
//				int nlen = 0;
//				for( int ii = 0; ii < HEIGHT; ii++ )
//				{
//					if( TIMG_bin[ii][jj] == 255)
//					{
//						nlen++;
//					}
//				}
//				if( nlen >= HEIGHT * 0.75)
//				{
//					nCnt++;
//				}
//			}
//			if( nCnt >= (WIDTH * 0.8) / 2  )
//			{
//				if( bNums )
//					resStr += _T("1");
//				else
//					resStr += _T("I");
//
//				continue;
//			}
			//method 1 end

			//Check for character "1", method 2
			int cValPt = 0;
			int nTotal = 0;
			for( int jj = 7; jj < WIDTH - 1; jj++ )
			{
				for( int ii = 0; ii < HEIGHT; ii++ )
				{
					if( TIMG_bin[ii][jj] == 255)
					{
						cValPt++;
					}
					nTotal++;
				}
			}
			if( cValPt >= nTotal * 0.7  )
			{
				if( bNums )
					resStr += _T("1");
				else
					resStr += _T("I");

				continue;
			}
			//method2 end
 		}

		IMAGE pimage = ImageAlloc( 17+9-1, 33+9-1 );
		for( int j = (9-1)/2; j < 33+(9-1)/2; j++ )
		{
			for( int i = (9-1)/2; i < 17+(9-1)/2; i++ )
 			{
				int ii = i - (9-1)/2;
				int jj = j - (9-1)/2;
				pimage[j][i] = TIMG_bin[jj][ii];
			}
		}
		float confidence = 0.0f;
		int nRes = -1;
 		nRes = m_ecoConChar.CharCongnize( pimage[0], &confidence, bNums );
		
		char cRes = (char)nRes;
		if( nRes < 10 ) cRes += 48;
		else cRes += 55;
		CString strChar;
		strChar.Format("%c",cRes);
		if( !bNums && i == 3 )
		{
			strChar.Format("U");
		}
 		resStr += strChar;
 		ImageFree( pimage );

#ifdef SAVE_CHARS
		CString strMDir = "字符信息";
		CreateDirectory( strMDir, NULL );
		if( bNums )
		{
			strMDir += "\\Nums";
			CreateDirectory( strMDir, NULL );
		}
		else
		{
			strMDir += "\\ABCs";
			CreateDirectory( strMDir, NULL );			
		}
		strMDir += "\\";
		CString strGDir = strMDir + _T("\\Gray");
		CreateDirectory( strGDir , NULL );
		CString strBDir = strMDir + _T("\\Bin");
		CreateDirectory( strBDir , NULL );
		CreateDirectory( strBDir, NULL );
		CString strDes;
		int nNum = i-1;
		do
		{
			nNum++;
			strDes.Format( "%s\\%s_%02d.bmp" , strGDir, fname, nNum );
 		}while( _access(strDes, 0) != -1 );
		ImageSave( TIMG_gray , strDes );
		strDes.Format( "%s\\%s_%02d.bmp", strBDir, fname, nNum);
		ImageSave( TIMG_bin , strDes );
#endif
	}

#ifdef SAVE_CHARS
	nCnt = charArray.GetSize();
	if( bNums  && nCnt == 7 )
	{
		CString strMDir = "字符信息";
		CreateDirectory( strMDir, NULL );
		strMDir += "\\CheckChars";
		CreateDirectory( strMDir, NULL );
		strMDir += "\\";
		CString strDes;
		int nNum = 0;
		do
		{
			nNum++;
			strDes.Format( "%s\\%s_%02d.bmp" , strMDir, fname, nNum );			
 		}while( _access(strDes, 0) != -1 );
		ImagePartSave( ImgGray, charArray.GetAt(6), strDes );	
	}
#endif


	ImageFree(TIMG_gray);
	ImageFree(TIMG_bin);

	return TRUE;
}

BOOL CConNumProc::RecCharArray( IMAGE ImgGray, ObjRectArray &charArray, CString& resStr, BOOL bNums, int nTrys )
{
// nTrys : 0 -- directly using charArray
//		 : 1 -- ReDraw Rects by Local Binary
//		 : 2 -- ReDraw Rects by 30% method
	const int WIDTH = NORM_CHAR_WIDTH;
	const int HEIGHT = NORM_CHAR_HEIGHT;

	IMAGE TIMG_gray = ImageAlloc(WIDTH, HEIGHT);
	IMAGE TIMG_bin = ImageAlloc(WIDTH, HEIGHT);

	int nCnt = charArray.GetSize();

	for( int i = 0; i < nCnt; i++ )
	{
		CRect rcChar = charArray.GetAt(i);

#ifdef TEST_CUT_CONF
		if( (rcChar.left == rcChar.right) && (rcChar.top==rcChar.bottom) )
		{
			resStr += "#";

			continue;
		}
#endif

		//Michael Add 2008-04-19 -- ReDraw Char Rect Before Rec
		if( nTrys == 1 )
		{
			ReDrawRect( ImgGray, rcChar );
		}
		//End -- ReDraw Char Rect Before Rec

		//Michael Add 2008-05-06 -- ReDraw Char Rect Using 30% top pixel value method
		float fTRatio = 0.5f;
		if( nTrys == 2 )
		{
			//ReDrawRectByTopValueBin( ImgGray, rcChar, fTRatio );
			ReDrawRect( ImgGray, rcChar );
		}
		//End -- ReDraw Char Rect Using 30% top pixel value method

		int nRcH = rcChar.Height();
		int nRcW = rcChar.Width();
		Norm_Image( ImgGray , TIMG_gray , rcChar );

		if( nTrys < 2 )
		{
			int nThre = ImgDaJinThreshold( TIMG_gray );
			ImgSingleSegment(TIMG_gray, TIMG_bin, nThre, 255, 0);
		}

		//Michael Add 2008-05-06 -- ReDraw Char Rect Using 30% top pixel value method
		if( nTrys == 2 )
		{
			TopValueBin( TIMG_gray, TIMG_bin, CRect( 0, 0, WIDTH-1, HEIGHT-1), fTRatio );
		}
		//End -- ReDraw Char Rect Using 30% top pixel value method

		
#ifdef CHECK_ERROR
		ASSERT( nRcW != 0 );
#endif
		if( nRcW == 0 )
		{
			nRcW = 1;
		}
		if( nRcH / (float)nRcW >= 3.1f )//判定为“1”的条件,需要判断除0错?
		{

#ifdef SAVE_CHARS
			CString strMDir = "字符信息";
			CreateDirectory( strMDir, NULL );
			strMDir += "\\1s\\";
			CreateDirectory( strMDir, NULL );
			CString strGDir = strMDir + _T("\\Gray");
			CreateDirectory( strGDir , NULL );
			CString strBDir = strMDir + _T("\\Bin");
			CreateDirectory( strBDir , NULL );
			CreateDirectory( strBDir, NULL );
			CString strDes;
			int nNum = i-1;
			do
			{
				nNum++;
				strDes.Format( "%s\\%s_%02d.bmp" , strGDir, fname, nNum );
 			}while( _access(strDes, 0) != -1 );
			ImageSave( TIMG_gray , strDes );
			strDes.Format( "%s\\%s_%02d.bmp", strBDir, fname, nNum);
			ImageSave( TIMG_bin , strDes );
#endif

			BOOL bCharI = FALSE;

			int nCnt = 0;
			if( nRcH / (float)nRcW >= 4.0f )
			{
				bCharI = TRUE;
 			}

			//Check for character "1", method 1
//			for( int jj = WIDTH / 3; jj < WIDTH; jj++ )
//			{
//				int nlen = 0;
//				for( int ii = 0; ii < HEIGHT; ii++ )
//				{
//					if( TIMG_bin[ii][jj] == 255)
//					{
//						nlen++;
//					}
//				}
//				if( nlen >= HEIGHT * 0.75)
//				{
//					nCnt++;
//				}
//			}
//			if( nCnt >= (WIDTH * 0.8) / 2  )
//			{
//				if( bNums )
//					resStr += _T("1");
//				else
//					resStr += _T("I");
//
//				continue;
//			}
			//method 1 end

			//Check for character "1", method 2
			if( !bCharI )
			{
				int cValPt = 0;
				int nTotal = 0;
				int ii = 0;
				int jj = 0;
				for( jj = 6; jj < WIDTH - 4; jj++ )
				{
					for( ii = 0; ii < HEIGHT; ii++ )
					{
						if( TIMG_bin[ii][jj] == 255)
						{
							cValPt++;
						}
						nTotal++;
					}
				}
				if( cValPt >= nTotal * 0.8  )
				{
					bCharI = TRUE;
				}
			}

			if( !bCharI )
			{
				int cValPt = 0;
				int nTotal = 0;
				for( int jj = 8; jj < WIDTH - 1; jj++ )
				{
					for( int ii = 0; ii < HEIGHT; ii++ )
					{
						if( TIMG_bin[ii][jj] == 255)
						{
							cValPt++;
						}
						nTotal++;
					}
				}
				if( cValPt >= nTotal * 0.85  )
				{
					bCharI = TRUE;
				}
			}
			//method2 end

			if( bCharI )
			{
				if( bNums )
					resStr += _T("1");
				else
				{
					if( i == 3 )
					{
						resStr += _T("U");
					}
					else
					{
						resStr += _T("I");
					}
				}
				
				continue;
			}
 		}

		IMAGE pimage = ImageAlloc( 17+9-1, 33+9-1 );
		for( int j = (9-1)/2; j < 33+(9-1)/2; j++ )
		{
			for( int i = (9-1)/2; i < 17+(9-1)/2; i++ )
 			{
				int ii = i - (9-1)/2;
				int jj = j - (9-1)/2;
				pimage[j][i] = TIMG_bin[jj][ii];
			}
		}
		float confidence = 0.0f;
		int nRes = -1;
 		nRes = m_ecoConChar.CharCongnize( pimage[0], &confidence, bNums );
		
		char cRes = (char)nRes;
		if( nRes < 10 ) cRes += 48;
		else cRes += 55;
		CString strChar;
		strChar.Format("%c",cRes);
		if( !bNums && i == 3 )
		{
			strChar.Format("U");
		}
 		resStr += strChar;
 		ImageFree( pimage );

#ifdef SAVE_CHARS
		CString strMDir = "字符信息";
		CreateDirectory( strMDir, NULL );
		if( bNums )
		{
			strMDir += "\\Nums";
			CreateDirectory( strMDir, NULL );
		}
		else
		{
			strMDir += "\\ABCs";
			CreateDirectory( strMDir, NULL );
		}
		strMDir += "\\";
		CString strGDir = strMDir + _T("\\Gray");
		CreateDirectory( strGDir , NULL );
		CString strBDir = strMDir + _T("\\Bin");
		CreateDirectory( strBDir , NULL );
		CreateDirectory( strBDir, NULL );
		CString strDes;
		int nNum = i-1;
		do
		{
			nNum++;
			strDes.Format( "%s\\%s_%02d.bmp" , strGDir, fname, nNum );
 		}while( _access(strDes, 0) != -1 );
		ImageSave( TIMG_gray , strDes );
		strDes.Format( "%s\\%s_%02d.bmp", strBDir, fname, nNum);
		ImageSave( TIMG_bin , strDes );
#endif
	}

#ifdef SAVE_CHARS
	nCnt = charArray.GetSize();
	if( bNums  && nCnt == 7 )
	{
		CString strMDir = "字符信息";
		CreateDirectory( strMDir, NULL );
		strMDir += "\\CheckChars";
		CreateDirectory( strMDir, NULL );
		strMDir += "\\";
		CString strDes;
		int nNum = 0;
		do
		{
			nNum++;
			strDes.Format( "%s\\%s_%02d.bmp" , strMDir, fname, nNum );			
 		}while( _access(strDes, 0) != -1 );
		ImagePartSave( ImgGray, charArray.GetAt(6), strDes );	
	}
#endif


	ImageFree(TIMG_gray);
	ImageFree(TIMG_bin);

	return TRUE;
}

BOOL CConNumProc::GetPrePosStrategy( IMAGE imgGray, ObjRectArray& rcArray, BOOL bBlack, int nthTry )
{
#ifdef SAVE_PLATEPOS_INFO
	int nCurDebugID = 0;
#endif

	if( bBlack )
	{
		ImgReverse( imgGray, imgGray );
 	}

	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);
	IMAGE imgVer = ImageAlloc( nPicWidth, nPicHeight );

#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
 	m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("gray.jpg")));
 	ImageSave( imgGray, m_strCurDebugFile );
#endif

	switch( nthTry )
	{
	case 0://For Red Chars
		VerEnhance_WholeAdjust( imgGray, imgVer, 10, 0.05f );
		break;
	case 1://For 4331 Format
		VerEnhance( imgGray, imgVer, 10, 0.15f );
		CheckVerEnhanceVal( imgGray, imgVer, 180 );
		break;
	case 2:
		VerEnhance( imgGray, imgVer, 10, 0.05f );
		break;
	case 3:
		VerEnhance( imgGray, imgVer, 16, 0.10f );
		break;
	}
		

#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
 	m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,++nCurDebugID,CString(_T("ver.jpg")));
 	ImageSave( imgVer, m_strCurDebugFile );
#endif

	CRect rcProcess(0,0,nPicWidth-1,nPicHeight-1);
	int nXOmit = 5;//int( nPicWidth * 0.05 );
	int nYOmit = 5;//int( nPicWidth * 0.10 );
	rcProcess.DeflateRect(nXOmit,nYOmit,nXOmit,nYOmit);

	rcArray.RemoveAll();
	BOOL bCheckAb = FALSE;
	switch( nthTry )
	{
	case 1:
		GetPerPlateRect( imgVer , rcProcess , rcArray );
		break;
	case 2:
		GetPerPlateRect( imgVer , rcProcess , rcArray );
		bCheckAb = TRUE;
		break;
	default:
		GetPerPlateRect( imgVer , rcProcess , rcArray, TRUE );
		break;
	}	
	MergeSplitPlateRcs( rcArray );
	
#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
// 	m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("line.jpg")));
// 	ImageSave( imgVer, m_strCurDebugFile );
	DrawObjRectArray( imgVer, rcArray );
	m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("_platepos.jpg")));
	ImageSave( imgVer, m_strCurDebugFile );
#endif
	

	//Michael Add 2008-05-03
	BOOL bAbNorm = FALSE;
	if( bCheckAb )
	{
		int nPosRgnCnt = rcArray.GetSize();
		for( int i = 0; i < nPosRgnCnt; i++ )
		{
			CRect rcAna = rcArray.GetAt(i);
			
			if( rcAna.Width() * rcAna.Height() > 640 * 480 / 2 )
			{
				bAbNorm = TRUE;
			}
		}
	}

	if( bAbNorm )
	{
		rcArray.RemoveAll();
		memset( imgVer[0], 0, nPicWidth * nPicHeight );
		VerEnhance( imgGray, imgVer, 10, 0.05f );
#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
		m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
 		m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,++nCurDebugID,CString(_T("ver_RN.jpg")));
 		ImageSave( imgVer, m_strCurDebugFile );
#endif

		GetPerPlateRect( imgVer, rcProcess, rcArray, TRUE );
		MergeSplitPlateRcs( rcArray );

#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
		m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
// 		m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("line.jpg")));
// 		ImageSave( imgVer, m_strCurDebugFile );
		DrawObjRectArray( imgVer, rcArray );
		m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("_platepos.jpg")));
		ImageSave( imgVer, m_strCurDebugFile );
#endif
	}
	//End - Michael Add 2008-05-03

	EreaseNoiseRect(rcArray);
  	SortRectFromBtoT(rcArray);
	
	ImageFree(imgVer);

	if( bBlack )
		ImgReverse( imgGray, imgGray );

	return rcArray.GetSize() > 0;
}


BOOL CConNumProc::GetPreJPNumRects( IMAGE imgGray, ObjRectArray& rcArray, BOOL bBlack )
{
#ifdef SAVE_PLATEPOS_INFO
	int nCurColorID = 0;
#endif
	
	if( bBlack )
	{
		ImgReverse( imgGray, imgGray );
#ifdef SAVE_PLATEPOS_INFO
		int nCurColorID = 1;
#endif
 	}

	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);
	IMAGE imgVer = ImageAlloc( nPicWidth, nPicHeight );
	//VerEnhance( imgGray, imgVer, 10, 0.20f );
	VerEnhance( imgGray, imgVer, 16, 0.10f );

#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	CString strDirVerEn = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( strDirVerEn, NULL );
	strDirVerEn += _T("\\");
	CString strVerEnFilename;
 	strVerEnFilename.Format("%s%s_%d%s",strDirVerEn,fname,nCurColorID,CString(_T(".jpg")));
 	ImageSave( imgVer, strVerEnFilename );
#endif

	CRect rcProcess(0,0,nPicWidth-1,nPicHeight-1);
	int nXOmit = 5;//int( nPicWidth * 0.05 );
	int nYOmit = 5;//int( nPicWidth * 0.10 );
	rcProcess.DeflateRect(nXOmit,nYOmit,nXOmit,nYOmit);

	rcArray.RemoveAll();
	//GetPerPlateRect( imgVer , rcProcess , rcArray );//Original Call
	GetPerPlateRect( imgVer , rcProcess , rcArray, TRUE );//Michael Change 2008-05-07
#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
 	m_strCurDebugFile.Format("%s%s_%d%s",m_strCurDebugDir,fname,nCurColorID,CString(_T("_line.jpg")));
 	ImageSave( imgVer, m_strCurDebugFile );
#endif
	MergeSplitPlateRcs( rcArray );
	EreaseNoiseRect(rcArray);
  	SortRectByCols(rcArray);
	
	ImageFree(imgVer);

	if( bBlack )
	{
		ImgReverse( imgGray, imgGray );
 	}

	return rcArray.GetSize() > 0;
}

BOOL CConNumProc::GetPreNumRects( IMAGE imgGray, ObjRectArray& rcArray, BOOL bBlack )
{
#ifdef SAVE_PLATEPOS_INFO
	int nCurDebugID = 0;
#endif

	if( bBlack )
	{
		ImgReverse( imgGray, imgGray );
 	}

	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);
	IMAGE imgVer = ImageAlloc( nPicWidth, nPicHeight );

#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
 	m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("gray.jpg")));
 	ImageSave( imgGray, m_strCurDebugFile );
#endif
	VerEnhance( imgGray, imgVer, 10, 0.05f );	

#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
 	m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,++nCurDebugID,CString(_T("ver.jpg")));
 	ImageSave( imgVer, m_strCurDebugFile );
#endif

	CRect rcProcess(0,0,nPicWidth-1,nPicHeight-1);
	int nXOmit = 5;//int( nPicWidth * 0.05 );
	int nYOmit = 5;//int( nPicWidth * 0.10 );
	rcProcess.DeflateRect(nXOmit,nYOmit,nXOmit,nYOmit);

	rcArray.RemoveAll();
	GetPerPlateRect( imgVer , rcProcess , rcArray );//划定区域,包括行程滤波算法，即把一些大小不对的行程填0
	MergeSplitPlateRcs( rcArray );
#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
 	m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("line.jpg")));
 	ImageSave( imgVer, m_strCurDebugFile );
	DrawObjRectArray( imgVer, rcArray );
	m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("_platepos.jpg")));
	ImageSave( imgVer, m_strCurDebugFile );
#endif
	

	//Michael Add 2008-05-03
	BOOL bAbNorm = FALSE;
	int nPosRgnCnt = rcArray.GetSize();
	for( int i = 0; i < nPosRgnCnt; i++ )
	{
		CRect rcAna = rcArray.GetAt(i);
		
		if( rcAna.Width() * rcAna.Height() > 640 * 480 / 2 )
		{
			bAbNorm = TRUE;
		}

	}

	if( bAbNorm )
	{
		rcArray.RemoveAll();
		memset( imgVer[0], 0, nPicWidth * nPicHeight );
		VerEnhance( imgGray, imgVer, 10, 0.05f );
#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
		m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
 		m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,++nCurDebugID,CString(_T("ver_RN.jpg")));
 		ImageSave( imgVer, m_strCurDebugFile );
#endif

		GetPerPlateRect( imgVer, rcProcess, rcArray, TRUE );
		MergeSplitPlateRcs( rcArray );

#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
		m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
		CreateDirectory( m_strCurDebugDir, NULL );
		m_strCurDebugDir += _T("\\");
 		m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("line.jpg")));
 		ImageSave( imgVer, m_strCurDebugFile );
		DrawObjRectArray( imgVer, rcArray );
		m_strCurDebugFile.Format("%s%s_%d_%d%s",m_strCurDebugDir,fname,m_nCurProcID,nCurDebugID,CString(_T("_platepos.jpg")));
		ImageSave( imgVer, m_strCurDebugFile );
#endif
	}
	//End - Michael Add 2008-05-03

	EreaseNoiseRect(rcArray);
  	SortRectFromBtoT(rcArray);
	
	ImageFree(imgVer);

	if( bBlack )
		ImgReverse( imgGray, imgGray );

	return rcArray.GetSize() > 0;
}

BOOL CConNumProc::GetPreVerNumRects( IMAGE imgGray, ObjRectArray& rcArray )
{
//	int nPicWidth = ImageWidth(imgGray);
//	int nPicHeight = ImageHeight(imgGray);
//	IMAGE imgVer = ImageAlloc( nPicWidth, nPicHeight );
//	HoriEnhance(imgGray, imgVer, 10, 0.10f );
// 	VerEnhance( imgGray, imgVer, 12, 0.10f );
//
////	HoriEnhance(imgGray, imgVer, 10, 0.20f );
//// 	VerEnhance( imgGray, imgVer, 12, 0.20f );
//
//#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
//	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
//	CreateDirectory( m_strCurDebugDir, NULL );
//	m_strCurDebugDir += _T("\\");
//	m_strCurDebugFile.Format("%s%s_norm%s",m_strCurDebugDir,fname,CString(_T(".jpg")));
// 	ImageSave( imgVer, m_strCurDebugFile );
//#endif
//
//	CRect rcProcess(0,0,nPicWidth-1,nPicHeight-1);
//	int nXOmit = 5;//int( nPicWidth * 0.05 );
//	int nYOmit = 5;//int( nPicWidth * 0.10 );
//	rcProcess.DeflateRect(nXOmit,nYOmit,nXOmit,nYOmit);
//
//	rcArray.RemoveAll();
//	BOOL bNorm = GetPerVerPlateRect( imgVer , rcProcess , rcArray );//划定区域,包括行程滤波算法，即把一些大小不对的行程填0
//#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
// 	m_strCurDebugFile.Format("%s%s_norm%s",m_strCurDebugDir,fname,CString(_T("_line.jpg")));
// 	ImageSave( imgVer, m_strCurDebugFile );
//	DrawObjRectArray( imgVer, rcArray );
//	m_strCurDebugFile.Format("%s%s_norm%s",m_strCurDebugDir,fname,CString(_T("_platepos.jpg")));
//	ImageSave( imgVer, m_strCurDebugFile );
//#endif
//	
//	//if( !bNorm && rcArray.GetSize() == 0 )
//	if( !bNorm )//Michael Add 2008-04-30
//	{
//		memset( imgVer[0], 0, nPicWidth * nPicHeight );
//
//		HoriEnhance(imgGray, imgVer, 10, 0.20f );
// 		VerEnhance( imgGray, imgVer, 12, 0.20f );
//#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
//		m_strCurDebugFile.Format("%s%s_abtry%s",m_strCurDebugDir,fname,CString(_T(".jpg")));
// 		ImageSave( imgVer, m_strCurDebugFile );
//#endif
//
//		GetPerVerPlateRect( imgVer, rcProcess, rcArray, TRUE );
//#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
// 		m_strCurDebugFile.Format("%s%s_abtry%s",m_strCurDebugDir,fname,CString(_T("_line.jpg")));
// 		ImageSave( imgVer, m_strCurDebugFile );
//		DrawObjRectArray( imgVer, rcArray );
//		m_strCurDebugFile.Format("%s%s_abtry%s",m_strCurDebugDir,fname,CString(_T("_platepos.jpg")));
//		ImageSave( imgVer, m_strCurDebugFile );
//#endif		
//	}
//
//	SortRect(rcArray);
//	
//	ImageFree(imgVer);
//
// 	return rcArray.GetSize() > 0;

	return TryGetPreVerRgns( imgGray, rcArray, 1 );
}

BOOL CConNumProc::TryGetPreVerRgns( IMAGE imgGray, ObjRectArray& rcArray, int nTimes )
{
	int nPicWidth = ImageWidth(imgGray);
	int nPicHeight = ImageHeight(imgGray);
	IMAGE imgVer = ImageAlloc( nPicWidth, nPicHeight );

	float fRatio = 0.1f;
	BOOL bEliminNoise = FALSE;
	switch( nTimes )
	{
	case 1:
		fRatio = 0.10f;
		bEliminNoise = FALSE;
		break;
	case 2:
		fRatio = 0.20f;
		bEliminNoise = TRUE;
		break;
	}
	HoriEnhance(imgGray, imgVer, 10, fRatio );
 	VerEnhance( imgGray, imgVer, 12, fRatio );

#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
	m_strCurDebugDir = m_strSubDebugDir + "VerEnhance";
	CreateDirectory( m_strCurDebugDir, NULL );
	m_strCurDebugDir += _T("\\");
	m_strCurDebugFile.Format("%s%s_%dthTry%s",m_strCurDebugDir,fname,nTimes,CString(_T(".jpg")));
 	ImageSave( imgVer, m_strCurDebugFile );
#endif

	CRect rcProcess(0,0,nPicWidth-1,nPicHeight-1);
	int nXOmit = 5;//int( nPicWidth * 0.05 );
	int nYOmit = 5;//int( nPicWidth * 0.10 );
	rcProcess.DeflateRect(nXOmit,nYOmit,nXOmit,nYOmit);

	rcArray.RemoveAll();
	GetPerVerPlateRect( imgVer , rcProcess , rcArray, bEliminNoise );//划定区域,包括行程滤波算法，即把一些大小不对的行程填0
#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
 	m_strCurDebugFile.Format("%s%s_%dthTry%s",m_strCurDebugDir,fname,nTimes,CString(_T("_line.jpg")));
 	ImageSave( imgVer, m_strCurDebugFile );
	DrawObjRectArray( imgVer, rcArray );
	m_strCurDebugFile.Format("%s%s_%dthTry%s",m_strCurDebugDir,fname,nTimes,CString(_T("_platepos.jpg")));
	ImageSave( imgVer, m_strCurDebugFile );
#endif
	
	//if( !bNorm && rcArray.GetSize() == 0 )
//	if( !bNorm )//Michael Add 2008-04-30
//	{
//		memset( imgVer[0], 0, nPicWidth * nPicHeight );
//
//		HoriEnhance(imgGray, imgVer, 10, 0.20f );
// 		VerEnhance( imgGray, imgVer, 12, 0.20f );
//#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
//		m_strCurDebugFile.Format("%s%s_abtry%s",m_strCurDebugDir,fname,CString(_T(".jpg")));
// 		ImageSave( imgVer, m_strCurDebugFile );
//#endif
//
//		GetPerVerPlateRect( imgVer, rcProcess, rcArray, TRUE );
//#ifdef SAVE_PLATEPOS_INFO//Save VerEnhanced Image
// 		m_strCurDebugFile.Format("%s%s_abtry%s",m_strCurDebugDir,fname,CString(_T("_line.jpg")));
// 		ImageSave( imgVer, m_strCurDebugFile );
//		DrawObjRectArray( imgVer, rcArray );
//		m_strCurDebugFile.Format("%s%s_abtry%s",m_strCurDebugDir,fname,CString(_T("_platepos.jpg")));
//		ImageSave( imgVer, m_strCurDebugFile );
//#endif		
// 	}

	SortRect(rcArray);
	
	ImageFree(imgVer);

	return rcArray.GetSize() > 0;
}