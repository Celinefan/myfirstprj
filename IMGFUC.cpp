#include "stdafx.h"
#include "IMGFUC.h"

static int nAllocImgMem = 0;
static int nNewedMem = 0;

IMAGE IMGALLOC_MEMCHECK( int width, int height )
{
	nAllocImgMem++;

	return ImageAlloc( width, height );
}

void IMGFREE_MEMCHECK( IMAGE img )
{
	nAllocImgMem--;

	ImageFree(img);
}

int GetImgMemAlloced()
{
	return nAllocImgMem;
}

void ReverseColor( CImage* pPic )
{
	int nWidth = pPic->GetWidth();
	int nHeight = pPic->GetHeight();

	IMAGE imgR = IMGALLOC_MEMCHECK( nWidth, nHeight );
	IMAGE imgG = IMGALLOC_MEMCHECK( nWidth, nHeight );
	IMAGE imgB = IMGALLOC_MEMCHECK( nWidth, nHeight );

	CRect rcPic;
	pPic->GetRect( rcPic );
	pPic->GetImage( rcPic, imgR[0], imgG[0], imgB[0] );

	ImgReverse( imgR, imgR );
	ImgReverse( imgG, imgG );
	ImgReverse( imgB, imgB );

	pPic->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );

	IMGFREE_MEMCHECK( imgR );
	IMGFREE_MEMCHECK( imgG );
	IMGFREE_MEMCHECK( imgB );

}

void ReverseColor( CDIBitmap* pPic )
{
	int nWidth = pPic->GetWidth();
	int nHeight = pPic->GetHeight();

	IMAGE imgR = IMGALLOC_MEMCHECK( nWidth, nHeight );
	IMAGE imgG = IMGALLOC_MEMCHECK( nWidth, nHeight );
	IMAGE imgB = IMGALLOC_MEMCHECK( nWidth, nHeight );

	CRect rcPic;
	pPic->GetRect( rcPic );
	pPic->GetImage( rcPic, imgR[0], imgG[0], imgB[0] );

	ImgReverse( imgR, imgR );
	ImgReverse( imgG, imgG );
	ImgReverse( imgB, imgB );

	pPic->PutImage( rcPic, imgR[0], imgG[0], imgB[0] );

	IMGFREE_MEMCHECK( imgR );
	IMGFREE_MEMCHECK( imgG );
	IMGFREE_MEMCHECK( imgB );
}

void SplitFilePath( CString& filePath, CString& fileName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(filePath, drive, dir, fname, ext );
	fileName.Format("%s%s",fname,ext);
}

int GetPicturePathList( CString strDir, CStringList &strPathList) 
{
	strPathList.RemoveAll();

	int nFileFind = 0;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	CFileFind  filefinder;
	CStringList DirList;
	CString PathStr;
	CString CurPath;
	CurPath= strDir +_T("\\*.*");

	while(1)
	{
		int re;
		if(filefinder.FindFile(LPCTSTR(CurPath)))
		{
			do
			{
				re=filefinder.FindNextFile();
				if(filefinder.IsDots())
					continue;
				
				PathStr=filefinder.GetFilePath();
				
				if(filefinder.IsDirectory())
				{
					DirList.AddTail(PathStr);
				}
				else
				{
					CString str = filefinder.GetFileName();
					_splitpath(PathStr, drive, dir, fname, ext );
					CString str1 = ext;
					str1.MakeLower();
					if ( str1 == ".bmp" || str1 == ".jpg")
					{
						strPathList.AddTail(PathStr);
						++nFileFind;
					}
				}
			}
			while(re);
		}
		
		if(DirList.IsEmpty())
		{
			break;
		}
		else
		{
			CString tempStr = DirList.RemoveHead();
			CurPath= tempStr + _T("\\*.*");
		}
	}

	return nFileFind;
}

int GetPicturePathList( CString strDir, CStringList &strPathList, CStringList &strSubDirList )
{
	strPathList.RemoveAll();

	int nFileFind = 0;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	CFileFind  filefinder;
	CStringList DirList;
	CString PathStr;
	CString CurPath;
	CurPath= strDir +_T("\\*.*");

	CString strCurSubDir = "";

	while(1)
	{
		int re;
		if(filefinder.FindFile(LPCTSTR(CurPath)))
		{
			do
			{
				re=filefinder.FindNextFile();
				if(filefinder.IsDots())
					continue;
				
				PathStr=filefinder.GetFilePath();
				
				if(filefinder.IsDirectory())
				{
					DirList.AddTail(PathStr);
				}
				else
				{
					CString str = filefinder.GetFileName();
					_splitpath(PathStr, drive, dir, fname, ext );
					CString str1 = ext;
					str1.MakeLower();
					if ( str1 == ".bmp" || str1 == ".jpg")
					{
						strPathList.AddTail(PathStr);
						strSubDirList.AddTail(strCurSubDir);
						++nFileFind;
					}
				}
			}
			while(re);
		}
		
		if(DirList.IsEmpty())
		{
			break;
		}
		else
		{
			CString tempStr = DirList.RemoveHead();

			int n1 = strDir.GetLength();
			int n2 = tempStr.GetLength();
			strCurSubDir = tempStr.Right( n2 - n1 - 1 );

			CurPath= tempStr + _T("\\*.*");
		}
	}

	return nFileFind;
}

BOOL Norm_Image( IMAGE ImgOrg, IMAGE ImgNorm, CRect rcProc )
{
	int W = ImageWidth( ImgOrg );
	int H = ImageHeight( ImgOrg );

	int W1 = rcProc.Width() + 1;
	int H1 = rcProc.Height() + 1;

	int W2 = ImageWidth( ImgNorm );
	int H2 = ImageHeight( ImgNorm );

	float x_ratio, y_ratio;
	x_ratio = (float)W1/(float)W2;
	y_ratio = (float)H1/(float)H2;

	int i, j;

	float x, y;
	int x1, y1;
	int x2, y2;
	float rx1, rx2, ry1, ry2;

	for( j = 0 ; j < H2 ; j++ )
	{
		y = rcProc.top + j * y_ratio;
		if( j == H2 - 1 )
		{
			//y = float(H1 - 1);
			y = float(rcProc.bottom);
		}
		y1 = (int)y;
		//y2 = min( y1 + 1, H - 1 );
		y2 = min( y1 + 1, rcProc.bottom );

		ry1 = y - y1;
		ry2 = 1 - ry1;

		for( i = 0 ; i < W2 ; i++ )
		{
			x = rcProc.left + i * x_ratio;
			//if( x == W2 - 1 )
			if( i == W2 - 1 )
			{
				//x = float(W1 - 1);
				x = float(rcProc.right);
			}
			x1 = (int)x;
			//x2 = min( x1 + 1, W - 1);
			x2 = min( x1 + 1, rcProc.right);

			rx1 = x - x1;
			rx2 = 1 - rx1;

			//四点插值
			float nVal = ImgOrg[y1][x1] * rx2 * ry2 +
						ImgOrg[y1][x2] * rx1 * ry2 +
						ImgOrg[y2][x1] * rx2 * ry1 +
						ImgOrg[y2][x2] * rx1 * ry1 ; //四点插值
		
			ImgNorm[j][i] = int( nVal + 0.5 );
		}
	}

	return TRUE;
}

void DrawRect( CImage* pPic, CRect rect, int r, int g, int b )
{ 
	////画矩形的位置
	CPen pen;
	pen.CreatePen( PS_SOLID, 2, RGB(r,g,b));	
	CDC* pDC = pPic->GetImageDC();
	CPen *oldPen = pDC->SelectObject( &pen);
	
	pDC->MoveTo( rect.left,rect.top);
	pDC->LineTo( rect.right,rect.top);
	pDC->LineTo( rect.right,rect.bottom);
	pDC->LineTo( rect.left,rect.bottom);
	pDC->LineTo( rect.left,rect.top);
	pDC->SelectObject( oldPen);
	pen.DeleteObject();
	
	pPic->SetChangeFlag(TRUE);
}

void DrawRect( CDIBitmap* pPic, CRect rect, int r, int g, int b )
{ 
	////画矩形的位置
	CPen pen;
	pen.CreatePen( PS_SOLID, 2, RGB(r,g,b));	
	CDC* pDC = pPic->GetDC();
	CPen *oldPen = pDC->SelectObject( &pen);
	
	pDC->MoveTo( rect.left,rect.top);
	pDC->LineTo( rect.right,rect.top);
	pDC->LineTo( rect.right,rect.bottom);
	pDC->LineTo( rect.left,rect.bottom);
	pDC->LineTo( rect.left,rect.top);
	pDC->SelectObject( oldPen);
	pen.DeleteObject();

}

void DrawObjRectArray( CDIBitmap* pPic, ObjRectArray &rectArray, int r, int g, int b )
{
	int count = rectArray.GetSize();
	int i = 0;
	for( i = 0; i < count; i++ )
	{
		CRect rcDraw = rectArray.GetAt(i);
		DrawRect( pPic, rcDraw, r, g, b );
	}
}

void DrawObjRectArray( CImage* pPic, ObjRectArray &rectArray, int r, int g, int b )
{
	int count = rectArray.GetSize();
	int i = 0;
	for( i = 0; i < count; i++ )
	{
		CRect rcDraw = rectArray.GetAt(i);
		DrawRect( pPic, rcDraw, r, g, b );
	}
}

void DrawObjRectArray( CDIBitmap* pPic, ObjRectArray &rectArray )
{
	int count = rectArray.GetSize();
	int i = 0;
	for( i = 0; i < count; i++ )
	{
		CRect rcDraw = rectArray.GetAt(i);
		int r = 255 - (200 / count) * i;
		int g = 0;
		int b = 0;
		DrawRect( pPic, rcDraw, r, g, b );
	}
}

void DrawObjRectArray( CImage* pPic, ObjRectArray &rectArray )
{
	int count = rectArray.GetSize();
	int i = 0;
	for( i = 0; i < count; i++ )
	{
		CRect rcDraw = rectArray.GetAt(i);
		int r = 255 - (200 / count) * i;
		int g = 0;
		int b = 0;
		DrawRect( pPic, rcDraw, r, g, b );
	}
}

void DrawObjRectArray(IMAGE imgDis, ObjRectArray &RectArray )
{
	int count = RectArray.GetSize();

	int w = ImageWidth(imgDis);
	int h = ImageHeight(imgDis);

	int xMin,yMin,xMax,yMax;
	int i = 0;
	for( i = 0 ; i < count ; i++ )
	{
		CRect rcObj = RectArray.GetAt(i);
		xMin = rcObj.left;
		xMax =  rcObj.right;
		yMin =  rcObj.top;
		yMax =  rcObj.bottom;
		ASSERT( xMin >= 0 && xMin < w );
		ASSERT( xMax >= xMin && xMax < w );
		ASSERT( yMin >= 0 && yMin < h );
		ASSERT( yMax >= yMin && yMax < h );
		int ii = 0;
		int jj = 0;
		for( ii = xMin ; ii <= xMax ; ii++ )
		{
			imgDis[yMin][ii] = 255;
			imgDis[yMax][ii] = 255;
		}
		for( jj = yMin ; jj <= yMax ; jj++ )
		{
			imgDis[jj][xMin] = 255;
			imgDis[jj][xMax] = 255;
		}
	}
}



void FillImage(CImage* pPic, CRect rc, int R, int G, int B)
{
	COLORREF cl = RGB( R, G, B);
    CBrush brush(cl);
    CBrush* pBrushSave = pPic->GetImageDC()->SelectObject(&brush);
    pPic->GetImageDC()->Rectangle(rc);
    pPic->GetImageDC()->SelectObject(pBrushSave);
	pPic->SetChangeFlag(TRUE);
}

void SaveIMGPart( CImage *pPic, CRect rcSave, int nChannel, CString strFilePath )
{
	ASSERT( nChannel > 0 );
	ASSERT( nChannel <= 3 );
	CImage PicSCh;//CImage Object with Single Channel

	int w = pPic->GetWidth();
	int h = pPic->GetHeight();

	int rcW = rcSave.Width() + 1;
	int rcH = rcSave.Height() + 1;

	PicSCh.Create( rcW, rcH, 8 );

	IMAGE imgChannel = IMGALLOC_MEMCHECK( rcW, rcH );
	pPic->GetImage( rcSave, imgChannel[0], nChannel );
	PicSCh.GetRect(rcSave);
	PicSCh.PutImage( rcSave, imgChannel[0], 1 );
	IMGFREE_MEMCHECK( imgChannel );

	PicSCh.Save( strFilePath );
}

void SavePICPart( CImage *pPic, CRect rcSave, CString strFilePath )
{
	CImage PicSave;

	int w = pPic->GetWidth();
	int h = pPic->GetHeight();

	int rcW = rcSave.Width() + 1;
	int rcH = rcSave.Height() + 1;

	PicSave.Create( rcW, rcH, 24 );

	IMAGE imgR = IMGALLOC_MEMCHECK( rcW, rcH );
	IMAGE imgG = IMGALLOC_MEMCHECK( rcW, rcH );
	IMAGE imgB = IMGALLOC_MEMCHECK( rcW, rcH );
	pPic->GetImage( rcSave, imgR[0], imgG[0], imgB[0] );
	PicSave.GetRect(rcSave);
	PicSave.PutImage( rcSave, imgR[0], imgG[0], imgB[0] );
	IMGFREE_MEMCHECK( imgR );
	IMGFREE_MEMCHECK( imgG );
	IMGFREE_MEMCHECK( imgB );

	PicSave.Save( strFilePath );
}

BOOL GetPicRectPart( CImage* pPic, CImage *pPartPic, CRect &rect )
{
	int w1 = pPic->GetWidth();
	int h1 = pPic->GetHeight();

	int w2 = pPartPic->GetWidth();
	int h2 = pPartPic->GetHeight();

	int rcW = rect.Width() + 1;
	int rcH = rect.Height() + 1;

	ASSERT( w2 == rcW );
	ASSERT( h2 == rcH );
	ASSERT( rect.left >= 0 && rect.left < w1);
	ASSERT( rect.top >= 0 && rect.top < h1 );
	ASSERT( rect.right >= 0 && rect.right < w1 );
	ASSERT( rect.bottom >= 0 && rect.bottom < h1 );

	IMAGE imgR = IMGALLOC_MEMCHECK( rcW, rcH );
	IMAGE imgG = IMGALLOC_MEMCHECK( rcW, rcH );
	IMAGE imgB = IMGALLOC_MEMCHECK( rcW, rcH );
	pPic->GetImage( rect, imgR[0], imgG[0], imgB[0] );
	pPartPic->GetRect( rect );
	pPartPic->PutImage( rect, imgR[0], imgG[0], imgB[0] );
	IMGFREE_MEMCHECK( imgR );
	IMGFREE_MEMCHECK( imgG );
	IMGFREE_MEMCHECK( imgB );

	return TRUE;
}

BOOL GetPicRectPart( CDIBitmap* pPic, CDIBitmap *pPartPic, CRect &rect )
{
	int w1 = pPic->GetWidth();
	int h1 = pPic->GetHeight();

	int w2 = pPartPic->GetWidth();
	int h2 = pPartPic->GetHeight();

	int rcW = rect.Width() + 1;
	int rcH = rect.Height() + 1;

	ASSERT( w2 == rcW );
	ASSERT( h2 == rcH );
	ASSERT( rect.left >= 0 && rect.left < w1);
	ASSERT( rect.top >= 0 && rect.top < h1 );
	ASSERT( rect.right >= 0 && rect.right < w1 );
	ASSERT( rect.bottom >= 0 && rect.bottom < h1 );

	IMAGE imgR = IMGALLOC_MEMCHECK( rcW, rcH );
	IMAGE imgG = IMGALLOC_MEMCHECK( rcW, rcH );
	IMAGE imgB = IMGALLOC_MEMCHECK( rcW, rcH );
	pPic->GetImage( rect, imgR[0], imgG[0], imgB[0] );
	pPartPic->GetRect( rect );
	pPartPic->PutImage( rect, imgR[0], imgG[0], imgB[0] );
	IMGFREE_MEMCHECK( imgR );
	IMGFREE_MEMCHECK( imgG );
	IMGFREE_MEMCHECK( imgB );
	
	return TRUE;
}

void SavePICPart( CDIBitmap *pPic, CRect rcSave, CString strFilePath )
{
	CImage PicSave;

	int w = pPic->GetWidth();
	int h = pPic->GetHeight();

	int rcW = rcSave.Width() + 1;
	int rcH = rcSave.Height() + 1;

	PicSave.Create( rcW, rcH, 24 );

	IMAGE imgR = IMGALLOC_MEMCHECK( rcW, rcH );
	IMAGE imgG = IMGALLOC_MEMCHECK( rcW, rcH );
	IMAGE imgB = IMGALLOC_MEMCHECK( rcW, rcH );
	pPic->GetImage( rcSave, imgR[0], imgG[0], imgB[0] );
	PicSave.GetRect(rcSave);
	PicSave.PutImage( rcSave, imgR[0], imgG[0], imgB[0] );
	IMGFREE_MEMCHECK( imgR );
	IMGFREE_MEMCHECK( imgG );
	IMGFREE_MEMCHECK( imgB );

	PicSave.Save( strFilePath );
}

BOOL ImageSave( IMAGE img, CString strSavePath )
{
	int w = ImageWidth( img );
	int h = ImageHeight( img );

	CImage picSave;
	picSave.Create( w, h );
	CRect rc;
	picSave.GetRect(rc);
	picSave.PutImage( rc, img[0], 1 );
	
	picSave.Save( strSavePath );

	return TRUE;
}

BOOL ImagePartSave( IMAGE img, CRect rc, CString strSavePath )
{
	int w = ImageWidth( img );
	int h = ImageHeight( img );

	ASSERT( rc.left >= 0 && rc.left < w );
	ASSERT( rc.right >= 0 && rc.right < w );
	ASSERT( rc.top >= 0 && rc.top < h );
	ASSERT( rc.bottom >= 0 && rc.bottom < h);
	ASSERT( rc.left < rc.right );
	ASSERT( rc.top < rc.bottom );

	int w1 = rc.Width() + 1;
	int h1 = rc.Height() + 1;

	IMAGE imgCut = IMGALLOC_MEMCHECK( w1, h1 );
	
	GetImageRectPart( img, imgCut, rc );
	
	ImageSave( imgCut, strSavePath );

	IMGFREE_MEMCHECK( imgCut );

	return TRUE;
}

void SortRect(ObjRectArray &rect_array)
{
	int nRectNum = rect_array.GetSize();
	for( int i = 0 ; i < nRectNum ; i++ )
	{
		CRect rcTmp = rect_array.GetAt(i);
		rcTmp.NormalizeRect();
		rect_array.SetAt(i,rcTmp);
	}
	CRect recttempi;
	CRect recttempj;

	for(i=0; i<nRectNum-1; i++)  
	{
		for(int j=i+1; j<nRectNum; j++)
		{
			recttempi = rect_array.GetAt(i);
			recttempj = rect_array.GetAt(j);

			if ( recttempi.left > recttempj.left )
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}

		}

	}
}

void VSortRect( ObjRectArray &rect_array )
{
	int nRectNum = rect_array.GetSize();
	for( int i = 0; i < nRectNum; i++ )
	{
		CRect rcTmp = rect_array.GetAt(i);
		if( (rcTmp.left >= rcTmp.right) || (rcTmp.top >= rcTmp.bottom) )
		{
			rect_array.RemoveAt(i);
			i--;
			nRectNum--;
		}
	}

	CRect recttempi;
	CRect recttempj;

	for( i = 0; i < nRectNum; i++ )
	{
		recttempi = rect_array.GetAt(i);
		for( int j = i + 1; j < nRectNum; j++ )
		{
			recttempj = rect_array.GetAt(j);

			if( recttempi.top > recttempj.top )//if rci's top is below rcj's top
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}
		}
	}
}

void SortRect_B2T(ObjRectArray &rect_array)//Sort From bottom to top
{

	int nRectNum = rect_array.GetSize();
	CRect recttempi;
	CRect recttempj;

	for(int i=0; i<nRectNum-1; i++)
	{
		for(int j=i+1; j<nRectNum; j++)
		{
			recttempi = rect_array.GetAt(i);
			recttempj = rect_array.GetAt(j);

			if ( recttempi.top < recttempj.top )//从下到上
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}
			else if( recttempi.top < recttempj.bottom && recttempi.left > recttempj.left )//从左向右
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}

		}

	}
}

BOOL RcsInSameLine( CRect rcCur, CRect rcNext )
{
	int nId1 = max( rcCur.top, rcNext.top );
	int nId2 = min( rcCur.bottom, rcNext.bottom );
	int nVOverlap = nId2 - nId1;
	
	if( nVOverlap <= 0 )
	{
		return FALSE;
	}

	int nMinHeight = min( rcCur.Height(), rcNext.Height() ); 
	//if( nMinHeight < nVOverlap * 2 )
	if( nMinHeight * 3 / 4 < nVOverlap )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL RcsInSameLine( CRect rcCur, CRect rcNext, float fRatio )
{
	int nId1 = max( rcCur.top, rcNext.top );
	int nId2 = min( rcCur.bottom, rcNext.bottom );
	int nVOverlap = nId2 - nId1;
	
	if( nVOverlap <= 0 )
	{
		return FALSE;
	}

	int nMinHeight = min( rcCur.Height(), rcNext.Height() ); 
	if( nMinHeight * fRatio < nVOverlap )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL RcsInSameCol( CRect rcCur, CRect rcNext, float fRatio )
{
	int nId1 = max( rcCur.left, rcNext.left );
	int nId2 = min( rcCur.right, rcCur.right );

	int nVOverlap = nId2 - nId1;

	if( nVOverlap <= 0 )
	{
		return FALSE;
	}

	int nMinWidth= min( rcCur.Width(), rcNext.Width() ); 
	if( nMinWidth * fRatio < nVOverlap )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL RcsInSameCol( CRect rcCur, CRect rcNext )
{
	int nId1 = max( rcCur.left, rcNext.left );
	int nId2 = min( rcCur.right, rcNext.right );
	int nVOverlap = nId2 - nId1;
	
	if( nVOverlap <= 0 )
	{
		return FALSE;
	}

	int nMinWidth = min( rcCur.Width(), rcNext.Width() ); 
	if( nMinWidth < nVOverlap * 2 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//Original Method -- SortRect_T2B
//void SortRect_T2B(ObjRectArray &rect_array)//Sort From bottom to top
//{
//
//	int nRectNum = rect_array.GetSize();
//	CRect recttempi;
//	CRect recttempj;
//
//	for(int i=0; i<nRectNum-1; i++)
//	{
//		for(int j=i+1; j<nRectNum; j++)
//		{
//			recttempi = rect_array.GetAt(i);
//			recttempj = rect_array.GetAt(j);
//
//			if( recttempi.bottom < recttempj.top )
//			{
//				continue;
//			}
//
///*
//			if ( recttempi.top > recttempj.bottom )//从上到下
//			{
//				rect_array.SetAt(j,recttempi);
//				rect_array.SetAt(i,recttempj);
//			}
//			else
//			{
//				//Analyze some replacement in vertical direction
//				int nId1 = max( recttempi.top, recttempj.top );
//				int nId2 = min( recttempi.bottom, recttempj.bottom );
//				int nVOverlap = nId2 - nId1;
//				ASSERT( nVOverlap >= 0 );
//
//				int nMinHeight = min( recttempi.Height(), recttempj.Height() ); 
//				if( nMinHeight < nVOverlap * 2 )
//				{
//					if( recttempi.left > recttempj.left )
//					{
//						rect_array.SetAt(j,recttempi);
//						rect_array.SetAt(i,recttempj);
//					}
//				}
//				else
//				{
//					if ( recttempi.top > recttempj.top )//从上到下
//					{
//						rect_array.SetAt(j,recttempi);
//						rect_array.SetAt(i,recttempj);
//					}
//				}
//				
// 			}
//*/
//
//			if( RcsInSameLine( recttempi, recttempj ) )
//			{
//				if( recttempi.left > recttempj.left )
//				{
//					rect_array.SetAt(j,recttempi);
//					rect_array.SetAt(i,recttempj);
//				}
//			}
//			else
//			{
//				if ( recttempi.top > recttempj.top )//从上到下
//				{
//					rect_array.SetAt(j,recttempi);
//					rect_array.SetAt(i,recttempj);
//				}
// 			}
//		}
//
//
//	}
//
//}
//End -- Original Method -- SortRect_T2B


//Michael Modify 2008-04-17 -- SortRect_T2B
void SortRect_T2B(ObjRectArray &rect_array)//Sort From bottom to top
{
	SortRect( rect_array );

	int nRectNum = rect_array.GetSize();
	CRect recttempi;
	CRect recttempj;

	for(int i=0; i<nRectNum-1; i++)
	{
		for(int j=i+1; j<nRectNum; j++)
		{
			recttempi = rect_array.GetAt(i);
			recttempj = rect_array.GetAt(j);

			if( recttempi.bottom < recttempj.top )
			{
				continue;
			}

			int nh1 = recttempi.Height();
			int nh2 = recttempj.Height();
			int nh = max( nh1, nh2 );

			if(  (recttempj.left - recttempi.right) > 3 * nh )
			{
				//if( RcsInSameLine( recttempi, recttempj, 0.4f ) )//Original Ratio
				if( RcsInSameLine( recttempi, recttempj, 0.4f ) )//Michael Change 2007-05-07
				{
					if( recttempi.left > recttempj.left )
					{
						rect_array.SetAt(j,recttempi);
						rect_array.SetAt(i,recttempj);
					}
				}
				else
				{
					if ( recttempi.top > recttempj.top )//从上到下
					{
						rect_array.SetAt(j,recttempi);
						rect_array.SetAt(i,recttempj);
					}
 				}

				continue;
			}
			
			if( RcsInSameLine( recttempi, recttempj ) )
			{
				if( recttempi.left > recttempj.left )
				{
					rect_array.SetAt(j,recttempi);
					rect_array.SetAt(i,recttempj);
				}
			}
			else
			{
				if ( recttempi.top > recttempj.top )//从上到下
				{
					rect_array.SetAt(j,recttempi);
					rect_array.SetAt(i,recttempj);
				}
 			}
		}
	}
}
//End -- Michael Modify 2008-04-17 -- SortRect_T2B

void SortRectByLength(ObjRectArray &rect_array)//Sort by the width;
{
	int nRectNum = rect_array.GetSize();
	CRect recttempi;
	CRect recttempj;

	for(int i=0; i<nRectNum-1; i++)
	{
		for(int j=i+1; j<nRectNum; j++)
		{
			recttempi = rect_array.GetAt(i);
			recttempj = rect_array.GetAt(j);

			if ( recttempi.Width() < recttempj.Width() )
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}

		}

	}
}

void SortRectByArea( ObjRectArray &rect_array )
{
	int nRectNum = rect_array.GetSize();
	CRect recttempi;
	CRect recttempj;

	for(int i=0; i<nRectNum-1; i++)
	{
		for(int j=i+1; j<nRectNum; j++)
		{
			recttempi = rect_array.GetAt(i);
			recttempj = rect_array.GetAt(j);

			int area1 = recttempi.Width() * recttempi.Height();
			int area2 = recttempj.Width() * recttempj.Height();
			
			if ( area1 < area2 )
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}

		}

	}
}


void ReduceFrames( CImage* pPic )
{
	int w1 = pPic->GetWidth();
	int h1 = pPic->GetHeight();

	CRect rc;
	pPic->GetRect( rc );
	IMAGE r1 = IMGALLOC_MEMCHECK( w1, h1 );
	IMAGE g1 = IMGALLOC_MEMCHECK( w1, h1 );
	IMAGE b1 = IMGALLOC_MEMCHECK( w1, h1 );
	pPic->GetImage( rc, r1[0], g1[0], b1[0] );

	int w2 = w1;
	int h2 = h1 / 2;
	pPic->Create( w2, h2, 24 );
	
	IMAGE r2 = IMGALLOC_MEMCHECK( w2, h2 );
	IMAGE g2 = IMGALLOC_MEMCHECK( w2, h2 );
	IMAGE b2 = IMGALLOC_MEMCHECK( w2, h2 );

	int i = 0;
	int j = 0;
	for( i = 0 ; i < h2 ; i++ )
	{
		for( j = 0 ; j < w2 ; j++ )
		{
			r2[i][j] = r1[i*2][j];
			g2[i][j] = g1[i*2][j];
			b2[i][j] = b1[i*2][j];
		}
	}

	pPic->GetRect( rc );
	pPic->PutImage( rc, r2[0], g2[0], b2[0] );

	IMGFREE_MEMCHECK( r2 );
	IMGFREE_MEMCHECK( g2 );
	IMGFREE_MEMCHECK( b2 );

	IMGFREE_MEMCHECK(r1);
	IMGFREE_MEMCHECK(g1);
	IMGFREE_MEMCHECK(b1);
}

void InsertFrames( CImage* pPic )
{
	int w1 = pPic->GetWidth();
	int h1 = pPic->GetHeight();

	CRect rc;
	pPic->GetRect( rc );
	IMAGE r1 = IMGALLOC_MEMCHECK( w1, h1 );
	IMAGE g1 = IMGALLOC_MEMCHECK( w1, h1 );
	IMAGE b1 = IMGALLOC_MEMCHECK( w1, h1 );
	pPic->GetImage( rc, r1[0], g1[0], b1[0] );

	int w2 = w1;
	int h2 = h1 * 2 ;
	pPic->Create( w2, h2, 24 );
	
	IMAGE r2 = IMGALLOC_MEMCHECK( w2, h2 );
	IMAGE g2 = IMGALLOC_MEMCHECK( w2, h2 );
	IMAGE b2 = IMGALLOC_MEMCHECK( w2, h2 );

	int i = 0;
	int j = 0;
	for( i = 0 ; i < h1 ; i++ )
	{
		for( j = 0 ; j < w2 ; j++ )
		{
			r2[i*2][j] = r1[i][j];
			g2[i*2][j] = g1[i][j];
			b2[i*2][j] = b1[i][j];

			r2[i*2+1][j] = r1[i][j];
			g2[i*2+1][j] = g1[i][j];
			b2[i*2+1][j] = b1[i][j];
		}
	}

//	for( i = 0 ; i < h1 - 1 ; i++ )
//	{
//		for( j = 0 ; j < w2 ; j++ )
//		{
//			r2[i*2+1][j] = (r1[i][j] + r1[i+1][j]) / 2;
//			g2[i*2+1][j] = (g1[i][j] + g1[i+1][j]) / 2;
//			b2[i*2+1][j] = (b1[i][j] + b1[i+1][j]) / 2;
//		}
// 	}

	pPic->GetRect( rc );
	pPic->PutImage( rc, r2[0], g2[0], b2[0] );

	IMGFREE_MEMCHECK( r2 );
	IMGFREE_MEMCHECK( g2 );
	IMGFREE_MEMCHECK( b2 );

	IMGFREE_MEMCHECK(r1);
	IMGFREE_MEMCHECK(g1);
	IMGFREE_MEMCHECK(b1);
}

CRect MergeRects( CRect r1 , CRect r2 )
{
	CRect r;

	r.top = min( r1.top , r2.top );
	r.bottom = max( r1.bottom , r2.bottom );
	r.left = min( r1.left , r2.left );
	r.right = max( r1.right , r2.right );

	return r;
}

void SortRectFromTtoB(ObjRectArray &rect_array)
{
	int nRectNum = rect_array.GetSize();
	for( int i = 0 ; i < nRectNum ; i++ )
	{
		rect_array[i].NormalizeRect();
	}
	CRect recttempi;
	CRect recttempj;

	for(i=0; i<nRectNum-1; i++)
	{
		for(int j=i+1; j<nRectNum; j++)
		{
			recttempi = rect_array.GetAt(i);
			recttempj = rect_array.GetAt(j);

			if( recttempj.top > recttempi.bottom )
			{
				continue;
			}

			if( recttempj.bottom < recttempi.top )
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
				continue;
			}

			if( recttempi.left > recttempj.left )
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}

		}

	}
}

BOOL RcInRgn( CRect rcTest, CRect rcRgn )
{
//	ASSERT( rcTest.right >= rcTest.left );
//	ASSERT( rcTest.bottom >= rcTest.top );
//	ASSERT( rcRgn.right >= rcRgn.left );
// 	ASSERT( rcRgn.bottom >= rcRgn.top );
	if( rcTest.right < rcTest.left ) return FALSE;
	if( rcTest.bottom < rcTest.top ) return FALSE;
	if( rcRgn.right <= rcRgn.left ) return FALSE;
	if( rcRgn.bottom <= rcRgn.top ) return FALSE;

	if( rcTest.left >= rcRgn.left 
		&& rcTest.right <= rcRgn.right
		&& rcTest.top >= rcRgn.top 
		&& rcTest.bottom <= rcRgn.bottom )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void SortRectByCols( ObjRectArray &rect_array )
{
	int nRectNum = rect_array.GetSize();
	for( int i = 0 ; i < nRectNum ; i++ )
	{
		rect_array[i].NormalizeRect();
	}
	CRect recttempi;
	CRect recttempj;

	for(i=0; i<nRectNum-1; i++)
	{
		for(int j=i+1; j<nRectNum; j++)
		{
		
			recttempi = rect_array.GetAt(i);
			recttempj = rect_array.GetAt(j);
//			int Cix = recttempi.CenterPoint().x;
//			int Cjx = recttempj.CenterPoint().x;
//
//			if( Cjx > recttempi.right ) continue;
//			if( Cjx < recttempi.left )
//			{
//				rect_array.SetAt(j,recttempi);
//				rect_array.SetAt(i,recttempj);
//				continue;
//			}

			if( !RcsInSameCol( recttempi, recttempj) )
			{
				if( recttempi.left > recttempj.left )
				{
					rect_array.SetAt( j, recttempi );
					rect_array.SetAt( i, recttempj );
				}

				continue;//Michael Add 20080612
			}

			if( recttempj.bottom < recttempi.top )
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}

		}

	}
}

void SortRectFromBtoT(ObjRectArray &rect_array)
{
	int nRectNum = rect_array.GetSize();
	for( int i = 0 ; i < nRectNum ; i++ )
	{
		rect_array[i].NormalizeRect();
	}
	CRect recttempi;
	CRect recttempj;

	for(i=0; i<nRectNum-1; i++)
	{
		for(int j=i+1; j<nRectNum; j++)
		{
			recttempi = rect_array.GetAt(i);
			recttempj = rect_array.GetAt(j);

			if( recttempj.bottom < recttempi.bottom && recttempj.top < recttempi.top )
			{
				continue;
			}

			if( recttempj.bottom > recttempi.bottom && recttempj.top > recttempi.top )
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);	
				continue;
			}

			if( recttempi.left > recttempj.left )
			{
				rect_array.SetAt(j,recttempi);
				rect_array.SetAt(i,recttempj);
			}

		}

	}
}

BOOL GetImageRectPart(IMAGE &bigImg, IMAGE &partImg, CRect &rect)
{
	int width = ImageWidth(partImg);
	int height = ImageHeight(partImg);

	int BigWidth = ImageWidth(bigImg);
	int BigHeight = ImageHeight(bigImg);

	if (BigWidth < width + rect.left || BigHeight < height + rect.top)
	{
		return FALSE;
	}
	
	for (int i=0; i<height; i++)
	{
		for (int j=0; j<width; j++)
		{
			partImg[i][j] = bigImg[rect.top + i][rect.left + j];
		}
	}
	return TRUE;
}

void Scaleimage( CImage* pPic, float fPercent )
{
	int nPicWidth = pPic->GetWidth();
	int nPicHeight = pPic->GetHeight();

	IMAGE imgR = IMGALLOC_MEMCHECK( nPicWidth , nPicHeight );
	IMAGE imgG = IMGALLOC_MEMCHECK( nPicWidth , nPicHeight );
	IMAGE imgB = IMGALLOC_MEMCHECK( nPicWidth , nPicHeight );
	CRect rc;
	pPic->GetRect( rc );
	pPic->GetImage( rc, imgR[0], imgG[0], imgB[0] );	

	int nW_new = int(nPicWidth * fPercent);
	int nH_new = int(nPicHeight * fPercent);
	ASSERT( nW_new > 0 );
	ASSERT( nH_new > 0 );
	IMAGE imgR_new = IMGALLOC_MEMCHECK( nW_new, nH_new );
	IMAGE imgG_new = IMGALLOC_MEMCHECK( nW_new, nH_new );
	IMAGE imgB_new = IMGALLOC_MEMCHECK( nW_new, nH_new );

	Norm_Image( imgR, imgR_new, rc );
	Norm_Image( imgG, imgG_new, rc );
	Norm_Image( imgB, imgB_new, rc );

	pPic->Create( nW_new, nH_new, 24 );
	CRect rc_new;
	pPic->GetRect( rc_new );
	pPic->PutImage( rc_new, imgR_new[0], imgG_new[0], imgB_new[0] );

	IMGFREE_MEMCHECK( imgR );
	IMGFREE_MEMCHECK( imgG );
	IMGFREE_MEMCHECK( imgB );
	IMGFREE_MEMCHECK( imgR_new );
	IMGFREE_MEMCHECK( imgG_new );
	IMGFREE_MEMCHECK( imgB_new );
}

void Scaleimage( CDIBitmap* pPic, float fPercent ) 
{
	int nPicWidth = pPic->GetWidth();
	int nPicHeight = pPic->GetHeight();

	IMAGE imgR = IMGALLOC_MEMCHECK( nPicWidth , nPicHeight );
	IMAGE imgG = IMGALLOC_MEMCHECK( nPicWidth , nPicHeight );
	IMAGE imgB = IMGALLOC_MEMCHECK( nPicWidth , nPicHeight );
	CRect rc;
	pPic->GetRect( rc );
	pPic->GetImage( rc, imgR[0], imgG[0], imgB[0] );	

	int nW_new = int(nPicWidth * fPercent);
	int nH_new = int(nPicHeight * fPercent);
	ASSERT( nW_new > 0 );
	ASSERT( nH_new > 0 );
	IMAGE imgR_new = IMGALLOC_MEMCHECK( nW_new, nH_new );
	IMAGE imgG_new = IMGALLOC_MEMCHECK( nW_new, nH_new );
	IMAGE imgB_new = IMGALLOC_MEMCHECK( nW_new, nH_new );

	Norm_Image( imgR, imgR_new, rc );
	Norm_Image( imgG, imgG_new, rc );
	Norm_Image( imgB, imgB_new, rc );

	pPic->Create( nW_new, nH_new, 24 );
	CRect rc_new;
	pPic->GetRect( rc_new );
	pPic->PutImage( rc_new, imgR_new[0], imgG_new[0], imgB_new[0] );

	IMGFREE_MEMCHECK( imgR );
	IMGFREE_MEMCHECK( imgG );
	IMGFREE_MEMCHECK( imgB );
	IMGFREE_MEMCHECK( imgR_new );
	IMGFREE_MEMCHECK( imgG_new );
	IMGFREE_MEMCHECK( imgB_new );
}


float randNum()
{
	float rNum = (float)(rand() % 1000) / (float)1000;
	return rNum;
}

BOOL RegulateRect( CRect &rc, int l, int t, int r, int b )
{
	BOOL bFlag = FALSE;
	if( rc.left > rc.right )
	{
		int tmp = rc.left;
		rc.left = rc.right;
		rc.right = tmp;
		bFlag = TRUE;
	}
	if( rc.top > rc.bottom )
	{
		int tmp = rc.top;
		rc.top = rc.bottom;
		rc.bottom = tmp;
		bFlag = TRUE;
	}

	if( rc.left < l )
	{
		rc.left = l;
		bFlag = TRUE;
	}

	if( rc.left > r )
	{
		rc.left = r;
		bFlag = TRUE;
	}

	if( rc.top < t )
	{
		rc.top = t;
		bFlag = TRUE;
	}

	if( rc.top > b )
	{
		rc.top = b;
		bFlag = TRUE;
	}

	if( rc.right > r )
	{
		rc.right = r;
		bFlag = TRUE;
	}

	if( rc.right < l )
	{
		rc.right = l;
		bFlag = TRUE;
	}

	if( rc.bottom > b )
	{
		rc.bottom = b;
		bFlag = TRUE;
	}

	if( rc.bottom < t )
	{
		rc.bottom = t;
		bFlag = TRUE;
	}

	return bFlag;
}

void TransferRectFromL2W( CRect& rc, CRect rcLocal )//Transfer Rect from Local to Whole
{
	int l = rcLocal.left;
	int t = rcLocal.top;

	rc.left += l;
	rc.right += l;
	rc.top += t;
	rc.bottom += t;
}

void TransferRcArrayFromL2W( ObjRectArray& rcArray, CRect rcLocal)//Transfer RcArray from Local to Whole
{
	int l = rcLocal.left;
	int t = rcLocal.top;

	int nCnt = rcArray.GetSize();
	for( int i = 0; i < nCnt; i++ )
	{
		CRect rcCur = rcArray.GetAt(i);
		rcCur.left += l;
		rcCur.right += l;
		rcCur.top += t;
		rcCur.bottom += t;
		rcArray.SetAt(i,rcCur);
	}
}

BOOL WriteRect2Txt( fstream &fInfo, CRect rcCur )
{
	BOOL bSuc = TRUE;
	CString strCurRc = _T("");
	int l = rcCur.left;
	int r = rcCur.right;
	int t = rcCur.top;
	int b = rcCur.bottom;
	int h = rcCur.Height();
	int w = rcCur.Width();
	strCurRc.Format( " (%003d, %003d),\t(%003d, %003d),\t(%003d, %003d) -- hwr : %f", t, b, l, r, h, w, ((float)h/(float)w) );
	fInfo << strCurRc;
	return bSuc;
}
BOOL WriteRect2Txt( ofstream &fInfo, CRect rcCur )
{
	BOOL bSuc = TRUE;
	CString strCurRc = _T("");
	int l = rcCur.left;
	int r = rcCur.right;
	int t = rcCur.top;
	int b = rcCur.bottom;
	int h = rcCur.Height();
	int w = rcCur.Width();
	strCurRc.Format( " (%003d, %003d),\t(%003d, %003d),\t(%003d, %003d) -- hwr : %f", t, b, l, r, h, w, ((float)h/(float)w) );
	fInfo << strCurRc;
	return bSuc;
}
BOOL WriteRcArray2Txt( fstream &fInfo, ObjRectArray& rcArray )
{
	BOOL bSuc = TRUE;

	int nCnt = rcArray.GetSize();
	if( nCnt <= 0 ) return FALSE;
	//SortRect(rcArray);	

	CRect rcCur = rcArray.GetAt(0);
	CRect rcNext;
	for( int ii = 0; ii < nCnt; ii++, rcCur = rcNext )
	{
		fInfo << " _" << ii << _T("\t: ");
		WriteRect2Txt( fInfo, rcCur );
		if( ii < nCnt - 1 )
		{
			rcNext = rcArray.GetAt(ii+1);
			int Dis = rcNext.left - rcCur.right;
			CString strDis;
			strDis.Format( "\tDis : %02d", Dis );
			fInfo << strDis;			
		}
		fInfo << " ; " << endl;
	}

	return bSuc;
}
BOOL WriteRcArray2Txt( ofstream &fInfo, ObjRectArray& rcArray )
{
	BOOL bSuc = TRUE;

	int nCnt = rcArray.GetSize();
	if( nCnt <= 0 ) return FALSE;
	//SortRect(rcArray);	

	CRect rcCur = rcArray.GetAt(0);
	CRect rcNext;
	for( int ii = 0; ii < nCnt; ii++, rcCur = rcNext )
	{
		fInfo << " _" << ii << _T("\t: ");
		WriteRect2Txt( fInfo, rcCur );
		if( ii < nCnt - 1 )
		{
			rcNext = rcArray.GetAt(ii+1);
			int Dis = rcNext.left - rcCur.right;
			CString strDis;
			strDis.Format( "\tDis : %02d", Dis );
			fInfo << strDis;			
		}
		fInfo << " ; " << endl;
	}

	return bSuc;
}


int FindMaxValPos( int *pnArray, int Len, int &nPos )
{
	return FindLimValPos( pnArray, Len, nPos, TRUE );
}

int FindMinValPos( int *pnArray, int Len, int &nPos )
{
	return FindLimValPos( pnArray, Len, nPos, FALSE );
}

int FindLimValPos(int *pnArray, int Len, int &nPos, BOOL bFindMax )
{
	int nLimVal;
	int i; 
	
	nLimVal = pnArray[0];
	int nIndex = 0;
	for( i = 1; i < Len; i++ )
	{
		if( (bFindMax && pnArray[i] > nLimVal)
			|| (!bFindMax && pnArray[i] < nLimVal) )
		{
			nLimVal = pnArray[i];
			nIndex = i;
		}
	}

	nPos = nIndex;
	return nLimVal;
}

void SavePRes2Pic( int *pnArray, int nLen, CString strFilePath, int nNormHi, BOOL bVerProj )
{
	if( pnArray == NULL ) return;
	if( nNormHi < 10 ) nNormHi = 10;

	int nPos = 0;
	int nMaxVal = FindMaxValPos( pnArray, nLen, nPos );

	int w = nLen;
	int h = nNormHi;
	IMAGE imgProj = ImageAlloc( w, h );
	
	
	int i = 0;
	int nCurPts = 0;
	int l = 0;
	int r = 0;
	int t = 0;
	int b = 0;
	for( i = 0; i < nLen; i++ )
	{
		nCurPts = int( pnArray[i] * nNormHi / (float)nMaxVal + 0.5 );
		if( nCurPts > nNormHi ) nCurPts = nNormHi;
		if( nCurPts > 0 )
		{
			if( bVerProj )
			{
				l = i;
				r = i;
				t = nNormHi - nCurPts;
				b = nNormHi - 1;
			}
			else
			{
				l = nNormHi - nCurPts;
				r = nNormHi - 1;
				t = i;
				b = i;
			}

			for( int ii = t; ii <= b; ii++ )
			{
				for( int jj = l; jj <= r; jj++ )
				{
					imgProj[ii][jj] = 255;
				}
			}
		}
	}

	ImageSave( imgProj, strFilePath );

	ImageFree(imgProj);
}

BOOL CheckRcInImg( CRect rcRgn, int nPicWidth, int nPicHeight )
{
	if( rcRgn.top < 0 
		|| rcRgn.left < 0 
		|| rcRgn.right >= nPicWidth
		|| rcRgn.bottom >= nPicHeight )
	{
		return FALSE;
	}

	return TRUE;
}