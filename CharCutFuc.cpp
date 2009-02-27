#include "StdAfx.h"
#include "CharCutFuc.h"

CString strDebugCCFucDir = "";
CString strDebugCCFucSubDir = "";
CString strDebugCCFucFname = "";
CString strCurCCFucDebugFile = "";
int nSubProcID = 0;
int nReCutTimes = 0;

BOOL SetCutCharsDebugInfoPath( CString& strDebugDir, CString& strDebugFname )
{
	strDebugCCFucDir = strDebugDir;
	strDebugCCFucFname = strDebugFname;

	return TRUE;
}

BOOL TackleCongluatination( ObjRectArray &rects, IMAGE imgGray, int nh, int nw )
{
	if( nh <= 0 || nw <= 0 ) return FALSE;

	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );
#ifdef SAVE_DEBUGINFO_FILES
	//ImageSave( imgGray, "C:\\a.jpg");
#endif

	nReCutTimes = 0;

	int nCnt = rects.GetSize();
	int i = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = rects.GetAt(i);
		int nCurH = rcCur.Height();
		int nCurW = rcCur.Width();

		BOOL bNeedTackle = FALSE;
		if(  nCurW >= 1.8f * nw  )
		{
			bNeedTackle = TRUE;
		}

		if( nCurH >= 1.5f * nh && nCurH >= nPicHeight * 0.8f )
		{
			bNeedTackle = TRUE;
		}

		if( bNeedTackle )
		{
			ObjRectArray rcChars;

			if( ReCutRgn( rcCur, imgGray, nh, nw, rcChars ) )
			{
				rects.RemoveAt(i);
				rects.InsertAt( i, &rcChars );
				i += rcChars.GetSize() - 1;
				nCnt = rects.GetSize();
			}
		}
	}

	return FALSE;
}

BOOL TackleCongluatinationForTopView( ObjRectArray &rects, IMAGE imgGray, int nh, int nw )
{
	if( nh <= 0 || nw <= 0 ) return FALSE;

	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );

	int nCnt = rects.GetSize();
	int i = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = rects.GetAt(i);
		int nCurH = rcCur.Height();
		int nCurW = rcCur.Width();

		BOOL bNeedTackle = FALSE;
		if(  nCurW >= 1.8f * nw  )
		{
			bNeedTackle = TRUE;
		}

		if( nCurH >= 1.5f * nh && nCurH >= nPicHeight * 0.8f )
		{
			bNeedTackle = TRUE;
		}

		if( bNeedTackle )
		{
			ObjRectArray rcChars;
			if( ReCutRgnForTopView( rcCur, imgGray, nh, nw, rcChars ) )
			{
				rects.RemoveAt(i);
				rects.InsertAt( i, &rcChars );
				i += rcChars.GetSize() - 1;
				nCnt = rects.GetSize();
			}
		}
	}

	return FALSE;
}

BOOL TackleCongluatination( ObjRectArray &rects, ObjRectArray &allrcs, IMAGE imgGray, int nh, int nw )
{
	if( nh <= 0 || nw <= 0 ) return FALSE;

	nReCutTimes = 0;

	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );
#ifdef SAVE_TC_INFO
	strCurCCFucDebugFile.Format( "%s%s.jpg", strDebugCCFucDir, strDebugCCFucFname );
	ImageSave( imgGray, strCurCCFucDebugFile);
#endif

	int nCnt = rects.GetSize();
	int i = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = rects.GetAt(i);
		int nCurH = rcCur.Height();
		int nCurW = rcCur.Width();

		BOOL bNeedTackle = FALSE;
		if(  nCurW >= 1.8f * nw  )
		{
			bNeedTackle = TRUE;
		}

		if( nCurH >= 1.5f * nh && nCurH >= nPicHeight * 0.8f )
		{
			bNeedTackle = TRUE;
		}

		if( bNeedTackle )
		{
			ObjRectArray rcChars;
#ifdef SAVE_TC_INFO
			nSubProcID = i;
#endif
			if( ReCutRgn( rcCur, imgGray, nh, nw, rcChars ) )
			{
				rects.RemoveAt(i);
				rects.InsertAt( i, &rcChars );
				i += rcChars.GetSize() - 1;
				nCnt = rects.GetSize();
			}
		}
	}

	int nAllCnt = allrcs.GetSize();
	for( i = 0; i < nAllCnt; i++ )
	{
		CRect rcCur = allrcs.GetAt( i );
		BOOL bNeedTackle = FALSE;
		int nCurH = rcCur.Height() + 1;
		
		if( nCurH >= 1.5f * nh && nCurH >= nPicHeight * 0.8f && nCurH >= MAX_CHAR_HEIGHT)//Tackle for chars which are overheight because of Congluatination
		{
			bNeedTackle = TRUE;
		}

		if( bNeedTackle )
		{
			ObjRectArray rcChars;
			if( ReCutRgn( rcCur, imgGray, nh, nw, rcChars ) )
			{
				rects.Append( rcChars );
			}
		}
	}

	return FALSE;
}

BOOL TackleCong_ByConf( ObjRectArray &rects, ObjRectArray &allrcs, IMAGE imgGray, int nh, int nw )
{
	if( nh <= 0 || nw <= 0 ) return FALSE;

	nReCutTimes = 0;

	int nPicWidth = ImageWidth( imgGray );
	int nPicHeight = ImageHeight( imgGray );
#ifdef SAVE_TC_INFO
	strCurCCFucDebugFile.Format( "%s%s.jpg", strDebugCCFucDir, strDebugCCFucFname );
	ImageSave( imgGray, strCurCCFucDebugFile);
#endif

	int nCnt = rects.GetSize();
	int i = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = rects.GetAt(i);
		int nCurH = rcCur.Height();
		int nCurW = rcCur.Width();

		BOOL bNeedTackle = FALSE;
		if(  nCurW >= 1.8f * nw  )
		{
			bNeedTackle = TRUE;
		}

		if( nCurH >= 1.5f * nh && nCurH >= nPicHeight * 0.8f )
		{
			bNeedTackle = TRUE;
		}

		if( bNeedTackle )
		{
			ObjRectArray rcChars;
#ifdef SAVE_TC_INFO
			nSubProcID = i;
#endif
			if( ReCutRgn( rcCur, imgGray, nh, nw, rcChars ) )
			{
				rects.RemoveAt(i);
				rects.InsertAt( i, &rcChars );
				i += rcChars.GetSize() - 1;
				nCnt = rects.GetSize();
			}
		}
	}

	int nAllCnt = allrcs.GetSize();
	for( i = 0; i < nAllCnt; i++ )
	{
		CRect rcCur = allrcs.GetAt( i );
		BOOL bNeedTackle = FALSE;
		int nCurH = rcCur.Height() + 1;
		
		if( nCurH >= 1.5f * nh && nCurH >= nPicHeight * 0.8f && nCurH >= MAX_CHAR_HEIGHT)//Tackle for chars which are overheight because of Congluatination
		{
			bNeedTackle = TRUE;
		}

		if( bNeedTackle )
		{
			ObjRectArray rcChars;
			if( ReCutRgn( rcCur, imgGray, nh, nw, rcChars ) )
			{
				rects.Append( rcChars );
			}
		}
	}

	return FALSE;
}

BOOL TackleVerCongluatination( ObjRectArray &rects, IMAGE imgGray, int nh, int nw )
{
	if( nh <= 0 || nw <= 0 ) return FALSE;

#ifdef SAVE_TC_INFO
	strCurCCFucDebugFile.Format( "%s%s.jpg", strDebugCCFucDir, strDebugCCFucFname );
	ImageSave( imgGray, strCurCCFucDebugFile);
#endif

	int nCnt = rects.GetSize();
	int i = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = rects.GetAt(i);

		if( rcCur.Height() >= 1.8f * nh )
		{
			ObjRectArray rcChars;
#ifdef SAVE_TC_INFO
			nSubProcID = i;
#endif
			if( ReCutVerRgn( rcCur, imgGray, nh, nw, rcChars ) )
			{
				rects.RemoveAt(i);
				rects.InsertAt( i, &rcChars );
				i += rcChars.GetSize() - 1;
				nCnt = rects.GetSize();
			}
		}
	}

	return FALSE;
}

BOOL SearchObjInRgn( IMAGE imgReCut_Bin, int nh, int nw, ObjRectArray &rcObj )
{
	BOOL bSuc = FALSE;
	ObjRectArray rcArrayUnk;
	bSuc = SearchObjInRgn( imgReCut_Bin, nh, nw, rcArrayUnk );

	return bSuc;
}

BOOL SearchObjInRgn( IMAGE imgReCut_Bin, int nh, int nw, ObjRectArray &rcObj, ObjRectArray &rcUnknow )//Michael Deleted 20080729
{
	rcUnknow.RemoveAll();

	BOOL bSuc = FALSE;
	ObjRectArray arrayObj;
	ObjRectArray allObj;
	GetObjRect( imgReCut_Bin, arrayObj, allObj );
	SortRect(arrayObj);

	int nCnt = arrayObj.GetSize();
	int i = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = arrayObj.GetAt(i);
		int nh1 = rcCur.Height();
		int nw1 = rcCur.Width();
		float fDifHRatio = (nh1-nh) / (float)nh;
		float fDifHShort = (nh - nh1) / (float)nh;
		float fDifWRatio = (nw1 - nw) / (float)nw;

		if( fDifWRatio > 0.2f )//May require Cut -- Michael Add 20080621
		{
			rcUnknow.Add( rcCur );
		}

		//if( abs(fDifHRatio) > 0.1f || fDifWRatio > 0.2f || nw1 < 5 )
		if( fDifHRatio > 0.15f || fDifHShort > 0.2f || fDifWRatio > 0.2f || nw1 < 2 )//Michael Change 2008-4-15 : For Merge Next
		{
			arrayObj.RemoveAt(i);
			i--;
			nCnt--;
		}
	}

	//Michael Add 2008-4-15 -- Try to Merge
	//Base on the suppose that all objs are on the same line!!!
	if( nCnt > 1 )
	{
		SortRect( arrayObj );

		for( i = 0; i < nCnt-1; i++ )
		{
			CRect rcCur = arrayObj.GetAt(i);
			CRect rcNext = arrayObj.GetAt(i+1);

			CRect rcMerge = MergeRects( rcCur, rcNext );//Merge
			int nh1 = rcMerge.Height();
			int nw1 = rcMerge.Width();
		
			float fDifHRatio = abs(nh1-nh) / (float)nh;
			float fDifWRatio = (nw1 - nw) / (float)nw;
			if( fDifHRatio <= 0.15f && fDifWRatio < 0.2f )//Michael Change 2008-4-15 : For Merge Next
			{
				arrayObj.SetAt(i,rcMerge);
				arrayObj.RemoveAt(i+1);
				nCnt--;
				i--;
			}			
		}
	}
	//End -- Try to Merge

	if( nCnt > 0 )
	{
		rcObj.Copy( arrayObj );
		bSuc = TRUE;
	}

	return bSuc;
}

//BOOL SearchObjInRgn( IMAGE imgReCut_Bin, int nh, int nw, ObjRectArray &rcObj, ObjRectArray &rcUnknow )//Michael Changed 200080729
//{
//	rcUnknow.RemoveAll();
//
//	BOOL bSuc = FALSE;
//	ObjRectArray arrayObj;
//	ObjRectArray allObj;
//	GetObjRect( imgReCut_Bin, arrayObj, allObj );
//	SortRect(arrayObj);
//
//	int nCnt = arrayObj.GetSize();
//	int i = 0;
//	for( i = 0; i < nCnt; i++ )
//	{
//		CRect rcCur = arrayObj.GetAt(i);
//		int nh1 = rcCur.Height();
//		int nw1 = rcCur.Width();
//		float fDifHRatio = (nh1-nh) / (float)nh;
//		float fDifHShort = (nh - nh1) / (float)nh;
//		float fDifWRatio = (nw1 - nw) / (float)nw;
//
//		//if( abs(fDifHRatio) > 0.1f || fDifWRatio > 0.2f || nw1 < 5 )
//		if( fDifHRatio > 0.15f || fDifHShort > 0.2f || fDifWRatio > 0.2f || nw1 < 2 )//Michael Change 2008-4-15 : For Merge Next
//		{
//			arrayObj.RemoveAt(i);
//			i--;
//			nCnt--;
//		}
//	}
//	
//	int nAllCnt = allObj.GetSize();
//	for( i = 0; i < nAllCnt; i++ )
//	{
//		CRect rcCur = allObj.GetAt(i);
//		int nh1 = rcCur.Height();
//		int nw1 = rcCur.Width();
//		float fDifHRatio = (nh1-nh) / (float)nh;
//		float fDifHShort = (nh - nh1) / (float)nh;
//		float fDifWRatio = (nw1 - nw) / (float)nw;
//
//		if( fDifWRatio > 0.2f && fDifHShort < 0.2f )//May require Cut -- Michael Add 20080621
//		{
//			rcUnknow.Add( rcCur );
//		}		
//	}
//
//	//Michael Add 2008-4-15 -- Try to Merge
//	//Base on the suppose that all objs are on the same line!!!
//	if( nCnt > 1 )
//	{
//		SortRect( arrayObj );
//
//		for( i = 0; i < nCnt-1; i++ )
//		{
//			CRect rcCur = arrayObj.GetAt(i);
//			CRect rcNext = arrayObj.GetAt(i+1);
//
//			CRect rcMerge = MergeRects( rcCur, rcNext );//Merge
//			int nh1 = rcMerge.Height();
//			int nw1 = rcMerge.Width();
//		
//			float fDifHRatio = abs(nh1-nh) / (float)nh;
//			float fDifWRatio = (nw1 - nw) / (float)nw;
//			if( fDifHRatio <= 0.15f && fDifWRatio < 0.2f )//Michael Change 2008-4-15 : For Merge Next
//			{
//				arrayObj.SetAt(i,rcMerge);
//				arrayObj.RemoveAt(i+1);
//				nCnt--;
//				i--;
//			}			
//		}
//	}
//	//End -- Try to Merge
//
//	if( nCnt > 0 )
//	{
//		rcObj.Copy( arrayObj );
//		bSuc = TRUE;
//	}
//
//	return bSuc;
//}

//BOOL ReCutRgn( CRect rcRgn, IMAGE imgGray, int nh, int nw, ObjRectArray &rcChars )//Michael Delete 20080621
//{
//	int i = 0;
//	
//	rcChars.RemoveAll();
//
//	IMAGE imgReCut_Gray;
//	IMAGE imgReCut_Bin;
//	imgReCut_Gray = ImageAlloc(rcRgn.Width()+1, rcRgn.Height()+1);
//	imgReCut_Bin = ImageAlloc(rcRgn.Width()+1, rcRgn.Height()+1);
//	GetImageRectPart(imgGray, imgReCut_Gray, rcRgn);
//#ifdef SAVE_TC_INFO
//	strCurCCFucDebugFile.Format( "%s%s_%d_0Gray.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID );
//	ImageSave( imgReCut_Gray, strCurCCFucDebugFile);
//#endif
//
//	DajinBin( imgReCut_Gray, imgReCut_Bin, CRect(0,0,rcRgn.Width(),rcRgn.Height()));
//#ifdef SAVE_TC_INFO	
//	strCurCCFucDebugFile.Format( "%s%s_%d_1Bin0.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID );
//	ImageSave( imgReCut_Bin, strCurCCFucDebugFile);
//#endif
//
//	ObjRectArray arrayObj;
//	SearchObjInRgn( imgReCut_Bin, nh, nw, arrayObj );
//	int nCnt = arrayObj.GetSize();
//
//	int nTryTimes = 0;
//	BOOL bDajinThre = TRUE;
//	while( nCnt <= 0 && bDajinThre )
//	{
//		nTryTimes++;
//		CRect rcBin = CRect(0,0,rcRgn.Width(),rcRgn.Height());
//		bDajinThre = DajinBin( imgReCut_Gray, imgReCut_Bin, rcBin, 0.1f * nTryTimes);
//#ifdef SAVE_TC_INFO
//		strCurCCFucDebugFile.Format( "%s%s_%d_1Bin%d.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID, nTryTimes );
//		ImageSave( imgReCut_Bin, strCurCCFucDebugFile);
//#endif
//		arrayObj.RemoveAll();
//		SearchObjInRgn( imgReCut_Bin, nh, nw, arrayObj );
//		nCnt = arrayObj.GetSize();
//
//		if( nTryTimes >= 4 )
//			break;
//	}
//	
//	ImageFree( imgReCut_Gray );
//	ImageFree( imgReCut_Bin );
//
//	nCnt = arrayObj.GetSize();
//	for( i = 0; i < nCnt; i++ )
//	{
//		CRect rcCur = arrayObj.GetAt(i);
//		rcCur.left += rcRgn.left;
//		rcCur.right += rcRgn.left;
//		rcCur.top += rcRgn.top;
//		rcCur.bottom += rcRgn.top;
//		rcChars.Add( rcCur );
//	}
//	
//	SortRect(rcChars);
//
//	if( rcChars.GetSize() > 0 ) return TRUE;
//	else 
//		return FALSE;
//}


BOOL ReCutRgn( CRect rcRgn, IMAGE imgGray, int nh, int nw, ObjRectArray &rcChars )//Michael Modify 20080621
{
	nReCutTimes++;
	if( nReCutTimes >= 4 ) return FALSE;

	int i = 0;
	
	rcChars.RemoveAll();

	IMAGE imgReCut_Gray;
	IMAGE imgReCut_Bin;
	imgReCut_Gray = ImageAlloc(rcRgn.Width()+1, rcRgn.Height()+1);
	imgReCut_Bin = ImageAlloc(rcRgn.Width()+1, rcRgn.Height()+1);
	GetImageRectPart(imgGray, imgReCut_Gray, rcRgn);
#ifdef SAVE_TC_INFO
	strCurCCFucDebugFile.Format( "%s%s_%d_0Gray.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID );
	ImageSave( imgReCut_Gray, strCurCCFucDebugFile);
#endif

	DajinBin( imgReCut_Gray, imgReCut_Bin, CRect(0,0,rcRgn.Width(),rcRgn.Height()));
#ifdef SAVE_TC_INFO	
	strCurCCFucDebugFile.Format( "%s%s_%d_1Bin0.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID );
	ImageSave( imgReCut_Bin, strCurCCFucDebugFile);
#endif

	ObjRectArray arrayObj;
	ObjRectArray arrayUnkonw;
	SearchObjInRgn( imgReCut_Bin, nh, nw, arrayObj, arrayUnkonw );
	int nCnt = arrayObj.GetSize();

	int nTryTimes = 0;
	BOOL bDajinThre = TRUE;
	while( nCnt <= 0 && bDajinThre )
	{
		nTryTimes++;
		CRect rcBin = CRect(0,0,rcRgn.Width(),rcRgn.Height());
		bDajinThre = DajinBin( imgReCut_Gray, imgReCut_Bin, rcBin, 0.1f * nTryTimes);
#ifdef SAVE_TC_INFO
		strCurCCFucDebugFile.Format( "%s%s_%d_1Bin%d.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID, nTryTimes );
		ImageSave( imgReCut_Bin, strCurCCFucDebugFile);
#endif
		arrayObj.RemoveAll();
		arrayUnkonw.RemoveAll();
		SearchObjInRgn( imgReCut_Bin, nh, nw, arrayObj, arrayUnkonw );
		nCnt = arrayObj.GetSize();

		if( nTryTimes >= 5 )
			break;
	}
	
	//Michael Add 20080621
	int nCnt1 = arrayObj.GetSize();
	int nCnt2 = arrayUnkonw.GetSize();
	if( nCnt1 > 0 && nCnt2 > 0 )
	{
		for( i = 0; i < nCnt2; i++ )
		{
			CRect rcSubRgn = arrayUnkonw.GetAt(i);
			ObjRectArray rcArrayChars;
			ReCutRgn( rcSubRgn, imgReCut_Gray, nh, nw, rcArrayChars );
			arrayObj.Append(rcArrayChars);
		}
	}
	//End -- Michael Add 20080621
	
	nCnt = arrayObj.GetSize();
	int nFinalCutChars = arrayObj.GetSize();

#ifdef TEST_PROJ	
	if( nFinalCutChars == 0 )
	{
		CRect rcBin = CRect(0,0,rcRgn.Width(),rcRgn.Height());
		bDajinThre = DajinBin( imgReCut_Gray, imgReCut_Bin, rcBin, 0.1f);

		int nRgnWidth = rcRgn.Width() + 1;
		int nRgnHeight = rcRgn.Height() + 1;
		int *pVerProj = new int[ nRgnWidth ];
		ImageVerProj( imgReCut_Bin, rcBin, pVerProj );
#ifdef SAVE_TC_INFO
		strCurCCFucDebugFile.Format( "%s%s_%d_2Proj.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID );
		SavePRes2Pic( pVerProj, nRgnWidth, strCurCCFucDebugFile, nRgnHeight, TRUE);
#endif

		int nPredictChars = int( nRgnWidth / (float)nw + 0.7 );

		ObjRectArray rcEsSubRgns;
		int l = 0;
		int r = 0;
		for( int ii = 0; ii < nPredictChars; ii++ )
		{
			//r = l + nRgnWidth / nPredictChars;
			r = l + ( nRgnWidth - 1 - l ) / ( nPredictChars - ii );

			float fTolRatio = 0.2f;
			int nOff1 = max( r - nw * fTolRatio, 0 );
			int nOff2 = min( r + nw * fTolRatio, nRgnWidth - 1);
			int nMinValPos = 0;
			int nMinVal = FindMinValPos( pVerProj + nOff1, nOff2 - nOff1 + 1, nMinValPos );
			r = nOff1 + nMinValPos;

#ifdef CHECK_ERROR
			ASSERT( r < nRgnWidth);
#endif
			if( r >= nRgnWidth ) r = nRgnWidth - 1;
			CRect rcCurSubRgn = CRect( l, 0, r, nRgnHeight - 1);
			int nCurSubRgnWidth = rcCurSubRgn.Width() + 1;
			int nCurSubRgnHeight = rcCurSubRgn.Height() + 1;

			IMAGE imgSubRgnBin = ImageAlloc( nCurSubRgnWidth, nCurSubRgnHeight );
			GetImageRectPart( imgReCut_Bin, imgSubRgnBin, rcCurSubRgn );
#ifdef SAVE_TC_INFO
			strCurCCFucDebugFile.Format( "%s%s_%d_2Proj_CutRes%d.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID, ii );
			ImageSave( imgSubRgnBin, strCurCCFucDebugFile );
#endif
			ObjRectArray arrayObjInSubRgn;
			ObjRectArray arrayUnkonwInSubRgn;
			SearchObjInRgn( imgSubRgnBin, nh, nw, arrayObjInSubRgn, arrayUnkonwInSubRgn );

			if( arrayObjInSubRgn.GetSize() == 1 )
			{
				CRect rcCur = arrayObjInSubRgn.GetAt(0);
				rcCur.left += l;
				rcCur.right += l;
				arrayObj.Add(rcCur);
			}

			ImageFree( imgSubRgnBin );

			l = r;

		}

		delete[] pVerProj;

	}

	nCnt = arrayObj.GetSize();
#endif//End -- TEST_PROJ

	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = arrayObj.GetAt(i);
		rcCur.left += rcRgn.left;
		rcCur.right += rcRgn.left;
		rcCur.top += rcRgn.top;
		rcCur.bottom += rcRgn.top;
		rcChars.Add( rcCur );
	}
	SortRect(rcChars);

	ImageFree( imgReCut_Gray );
	ImageFree( imgReCut_Bin );

	nReCutTimes--;
	nFinalCutChars = rcChars.GetSize();
	if( nFinalCutChars > 0 ) return TRUE;
	else 
		return FALSE;
}

BOOL ReCutVerRgn( CRect rcRgn, IMAGE imgGray, int nh, int nw, ObjRectArray &rcChars )
{
	rcChars.RemoveAll();

	IMAGE imgReCut_Gray;
	IMAGE imgReCut_Bin;
	imgReCut_Gray = ImageAlloc(rcRgn.Width()+1, rcRgn.Height()+1);
	imgReCut_Bin = ImageAlloc(rcRgn.Width()+1, rcRgn.Height()+1);
	GetImageRectPart(imgGray, imgReCut_Gray, rcRgn);
#ifdef SAVE_TC_INFO
	strCurCCFucDebugFile.Format( "%s%s_%d_0Gray.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID );
	ImageSave( imgReCut_Gray, strCurCCFucDebugFile);
#endif	
	DajinBin( imgReCut_Gray, imgReCut_Bin, CRect(0,0,rcRgn.Width(),rcRgn.Height()));
#ifdef SAVE_TC_INFO	
	strCurCCFucDebugFile.Format( "%s%s_%d_1Bin0.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID );
	ImageSave( imgReCut_Bin, strCurCCFucDebugFile);
#endif
	ObjRectArray arrayObj;
	ObjRectArray allObj;
	GetObjRect( imgReCut_Bin, arrayObj, allObj );
	SortRect(arrayObj);

	int nCnt = arrayObj.GetSize();
	int i = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = arrayObj.GetAt(i);
		int nh1 = rcCur.Height();
		int nw1 = rcCur.Width();
		float fDifHRatio = abs(nh1-nh) / (float)nh;
		float fDifWRatio = (nw1 - nw) / (float)nw;
		if( fDifHRatio > 0.1f || fDifWRatio > 0.3f || nw1 < 5 )
		{
			arrayObj.RemoveAt(i);
			i--;
			nCnt--;
		}
	}

	int nTryTimes = 0;
	while( nCnt <= 0 )
	{
		nTryTimes++;
		DajinBin( imgReCut_Gray, imgReCut_Bin, CRect(0,0,rcRgn.Width(),rcRgn.Height()), 0.1f * nTryTimes);
#ifdef SAVE_TC_INFO
		strCurCCFucDebugFile.Format( "%s%s_%d_1Bin%d.jpg", strDebugCCFucDir, strDebugCCFucFname, nSubProcID, nTryTimes );
		ImageSave( imgReCut_Bin, strCurCCFucDebugFile);
#endif
		arrayObj.RemoveAll();
		allObj.RemoveAll();
		GetObjRect( imgReCut_Bin, arrayObj, allObj );
		SortRect(arrayObj);

		nCnt = arrayObj.GetSize();
		for( i = 0; i < nCnt; i++ )
		{
			CRect rcCur = arrayObj.GetAt(i);
			int nh1 = rcCur.Height();
			int nw1 = rcCur.Width();
			float fDifHRatio = abs(nh1-nh) / (float)nh;
			float fDifWRatio = (nw1 - nw) / (float)nw;
			if( fDifHRatio > 0.1f || fDifWRatio > 0.3f || nw1 < 4 )
			{
				arrayObj.RemoveAt(i);
				i--;
				nCnt--;
			}
		}

		if( nTryTimes >= 4 )
			break;

		nCnt = arrayObj.GetSize();
	}
	
	ImageFree( imgReCut_Gray );
	ImageFree( imgReCut_Bin );

	nCnt = arrayObj.GetSize();
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = arrayObj.GetAt(i);
		rcCur.left += rcRgn.left;
		rcCur.right += rcRgn.left;
		rcCur.top += rcRgn.top;
		rcCur.bottom += rcRgn.top;
		rcChars.Add( rcCur );
	}
	
	SortRect(rcChars);

	if( rcChars.GetSize() > 0 ) return TRUE;
	else 
		return FALSE;
}

BOOL ReCutRgnForTopView( CRect rcRgn, IMAGE imgGray, int nh, int nw, ObjRectArray &rcChars )
{
	rcChars.RemoveAll();

	IMAGE imgReCut_Gray;
	IMAGE imgReCut_Bin;
	imgReCut_Gray = ImageAlloc(rcRgn.Width()+1, rcRgn.Height()+1);
	imgReCut_Bin = ImageAlloc(rcRgn.Width()+1, rcRgn.Height()+1);
	GetImageRectPart(imgGray, imgReCut_Gray, rcRgn);
	DajinBin( imgReCut_Gray, imgReCut_Bin, CRect(0,0,rcRgn.Width(),rcRgn.Height()));
#ifdef SAVE_DEBUGINFO_FILES	
	ImageSave(imgReCut_Bin, CString("C:\\b.jpg"));
#endif
	ObjRectArray arrayObj;
	ObjRectArray allObj;
	GetObjRectForTopView( imgReCut_Bin, arrayObj, allObj );
	SortRect(arrayObj);

	int nCnt = arrayObj.GetSize();
	int i = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = arrayObj.GetAt(i);
		int nh1 = rcCur.Height();
		int nw1 = rcCur.Width();
		float fDifHRatio = abs(nh1-nh) / (float)nh;
		float fDifWRatio = (nw1 - nw) / (float)nw;
		if( fDifHRatio > 0.1f || fDifWRatio > 0.2f || nw1 < 5 )
		{
			arrayObj.RemoveAt(i);
			i--;
			nCnt--;
		}
	}

	int nTryTimes = 0;
	BOOL bDajinThre = TRUE;
	while( nCnt <= 0 && bDajinThre )
	{
		nTryTimes++;
		CRect rcBin = CRect(0,0,rcRgn.Width(),rcRgn.Height());
		bDajinThre = DajinBin( imgReCut_Gray, imgReCut_Bin, rcBin, 0.1f * nTryTimes);
#ifdef SAVE_DEBUGINFO_FILES		
		ImageSave(imgReCut_Bin, CString("C:\\b.jpg"));
#endif
		arrayObj.RemoveAll();
		allObj.RemoveAll();
		GetObjRectForTopView( imgReCut_Bin, arrayObj, allObj );
		SortRect(arrayObj);

		nCnt = arrayObj.GetSize();
		for( i = 0; i < nCnt; i++ )
		{
			CRect rcCur = arrayObj.GetAt(i);
			int nh1 = rcCur.Height();
			int nw1 = rcCur.Width();
			float fDifHRatio = abs(nh1-nh) / (float)nh;
			float fDifWRatio = (nw1 - nw) / (float)nw;
			if( fDifHRatio > 0.1f || fDifWRatio > 0.2f || nw1 < 5 )
			{
				arrayObj.RemoveAt(i);
				i--;
				nCnt--;
			}
		}

		if( nTryTimes >= 4 )
			break;

		nCnt = arrayObj.GetSize();
	}
	
	ImageFree( imgReCut_Gray );
	ImageFree( imgReCut_Bin );

	nCnt = arrayObj.GetSize();
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcCur = arrayObj.GetAt(i);
		rcCur.left += rcRgn.left;
		rcCur.right += rcRgn.left;
		rcCur.top += rcRgn.top;
		rcCur.bottom += rcRgn.top;
		rcChars.Add( rcCur );
	}
	
	SortRect(rcChars);

	if( rcChars.GetSize() > 0 ) return TRUE;
	else 
		return FALSE;
}


BOOL MergePolicy( ObjRectArray& charArray, ObjRectArray& allRcsArray, int w, int h, int nRcDis )//MergeSplittedChars
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

BOOL EsRect( ObjRectArray& rcAnaRcsArray, int w, int h, CRect& rcMerge )
{
	rcMerge = CRect(0,0,0,0);
	int nAnaRcsCnt = rcAnaRcsArray.GetSize();
	if( nAnaRcsCnt <= 1 )
	{
		rcAnaRcsArray.RemoveAll();
		return FALSE;
	}

	SortRect( rcAnaRcsArray );
// Original Merge Method
//	rcMerge = rcAnaRcsArray.GetAt(0);
//	for( int jj = 1; jj < nAnaRcsCnt; jj++ )
//	{
//		CRect rcCur = rcAnaRcsArray.GetAt(jj);
//		rcMerge = MergeRects( rcCur, rcMerge );
//	}
// End -- Original Merge Method

//Michael Modify 2008-04-17 -- Merge & Judge
	CRect rcRef = rcAnaRcsArray.GetAt(0);
	for( int jj = 1; jj < nAnaRcsCnt; jj++ )
	{
		CRect rcCur = rcAnaRcsArray.GetAt(jj);
		rcRef = MergeRects( rcCur, rcRef );

		int hRef = rcRef.Height() + 1;
		int wRef = rcRef.Width() + 1;

		if( hRef < h * 1.3f && wRef < w * 1.5f )
		{
			rcMerge = rcRef;
		}
		else
		{
			rcRef = rcMerge;
		}
	}
//End -- Merge & Judge

	rcAnaRcsArray.RemoveAll();
	int h1 = rcMerge.Height() + 1;
	int w1 = rcMerge.Width() + 1;

	float fRatio = abs(h1-h) / (float)(h);
	float fRatioShort = (h - h1) / (float)(h); 
		
	if( fRatio > 0.3f  || fRatioShort > 0.2f || w1 >= w * 1.5f )
	{
		rcMerge = CRect(0,0,0,0);
		return FALSE;
	}

	return TRUE;
}
