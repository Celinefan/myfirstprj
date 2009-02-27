#include "StdAfx.h"
#include "PlateRecFuc.h"

static int nRunLenAlloced = 0;

PPRUNLENGTH GETRUNLENGTH_MEMCHECK( IMAGE a, int obj, int& number, int neighbor=IP_NEIGHBORFOUR)
{
	nRunLenAlloced++;
	return GetAllRunLength( a, obj, number, neighbor );
}
void FREERUNLENGTH_MEMCHECK( int num, PPRUNLENGTH runlen )
{
	nRunLenAlloced--;
	FreeRunLength( num, runlen );
}
int GetRunLenAlloced()
{
	return nRunLenAlloced;
}

void HoriEnhance(IMAGE InImg, IMAGE OutImg, int length = 9, float fRatio = 0.1f )
{
	int nMinVal = 0;

	int width = ImageWidth(InImg);
    int height = ImageHeight(InImg);
	
	int THRESHOLD = 10;
	int nThreMin = 4;
	int nThreMax = 13;
	int itemp;

	if (fRatio > 0.14f)
	{
		nThreMin = 4;
		nThreMax = 13;
#ifdef TEST_20060624
		nThreMax = 50;
		nMinVal = 80;
#endif
	}
	else if (fRatio > 0.09)
	{
		nThreMin = 4;
		nThreMax = 11;
	}
	else if (fRatio > 0.05) 
	{
		nThreMin = 3;
		nThreMax = 9;
	}
	else
	{
		nThreMin = 4;
		nThreMax = 9;
	}
	

	BOOL bfirst = FALSE;
	for (int i=0 ; i<width ; i++) // this is modify for fast
	{
		bfirst = TRUE;
		itemp = 0;
		
		for (int j=0+length/2; j<height-length/2; j++)
		{
			if (bfirst)
			{
				for (int k=-length/2; k<=length/2; k++)
				{
					ASSERT( (j+k) < height );
					ASSERT( i < width );
					itemp += InImg[j+k][i];				
				}
				bfirst = FALSE;
				if( fRatio>0.0 )
				{
					THRESHOLD = max( int((itemp/length)*fRatio) , nThreMin );
					THRESHOLD = min( THRESHOLD , nThreMax );
				}
				else
				{
					THRESHOLD = 10;
				}
				ASSERT( j < height );
				ASSERT( i < width );
				if (InImg[j][i] - itemp / length > THRESHOLD && InImg[j][i] > nMinVal)
					OutImg[j][i] = 255;
//				else
// 					OutImg[j][i] = 0;
				
			}
			else
			{
				ASSERT( (j - length / 2 - 1 ) >= 0 );
				ASSERT( (j - length / 2 - 1 ) < height );
				ASSERT( i < width );
				itemp = itemp - InImg[j-length/2-1][i] + InImg[j+length/2][i];
				if( fRatio>0.0 )
				{
					THRESHOLD = max( int((itemp/length)*fRatio) , nThreMin );
					THRESHOLD = min( THRESHOLD , nThreMax );
				}
				else
				{
					THRESHOLD = 10;
				}
				ASSERT( j < height );
				ASSERT( i < width );
				if (InImg[j][i] - itemp / length > THRESHOLD && InImg[j][i] > nMinVal )
					OutImg[j][i] = 255;
//				else
// 					OutImg[j][i] = 0;
			}
		}
	}
}

void VerEnhance(IMAGE InImg, IMAGE OutImg, int length = 9, float fRatio = 0.1f ) 
{
	int nMinVal = 0;

	int width = ImageWidth(InImg);
    int height = ImageHeight(InImg);
	
	int THRESHOLD = 10;
	int nThreMin = 4;
	int nThreMax = 13;
	int itemp;

	if (fRatio > 0.14f)
	{
		nThreMin = 4;
		nThreMax = 13;
#ifdef TEST_20060624
		nThreMax = 50;
		nMinVal = 80;
#endif
	}
	else if (fRatio > 0.09)
	{
		nThreMin = 4;
		nThreMax = 11;
	}
	else if (fRatio > 0.05) 
	{
		nThreMin = 3;
		nThreMax = 9;
	}
	else
	{
		nThreMin = 4;
		nThreMax = 9;
	}
	

	BOOL bfirst = FALSE;
	for (int i=0 ; i<height ; i++) // this is modify for fast
	{
		bfirst = TRUE;
		itemp = 0;
		
		for (int j=0+length/2; j<width-length/2; j++)
		{
			if (bfirst)
			{
				for (int k=-length/2; k<=length/2; k++)
				{
					itemp += InImg[i][j+k];				
				}
				bfirst = FALSE;
				if( fRatio>0.0 )
				{
					THRESHOLD = max( int((itemp/length)*fRatio) , nThreMin );
					THRESHOLD = min( THRESHOLD , nThreMax );
				}
				else
				{
					THRESHOLD = 10;
				}
				if (InImg[i][j] - itemp / length > THRESHOLD && InImg[i][j] > nMinVal)
					OutImg[i][j] = 255;
//				else
// 					OutImg[i][j] = 0;
				
			}
			else
			{
				itemp = itemp - InImg[i][j-length/2-1] + InImg[i][j+length/2];
				if( fRatio>0.0 )
				{
					THRESHOLD = max( int((itemp/length)*fRatio) , nThreMin );
					THRESHOLD = min( THRESHOLD , nThreMax );
				}
				else
				{
					THRESHOLD = 10;
				}
				if (InImg[i][j] - itemp / length > THRESHOLD && InImg[i][j] > nMinVal )
					OutImg[i][j] = 255;
//				else
// 					OutImg[i][j] = 0;
			}
		}
	}
	
}

void VerEnhance_WholeAdjust(IMAGE InImg, IMAGE OutImg, int length = 9, float fRatio = 0.1f )
{
#ifdef CHECK_ERROR
	ASSERT( InImg != NULL && OutImg != NULL );
#endif
	if( InImg == NULL || OutImg == NULL ) return;

	int nImgWidth = ImageWidth( InImg );
	int nImgHeight = ImageHeight( InImg );

	int nWRef = ImageWidth( OutImg );
	int nHRef = ImageHeight( OutImg );

	int i = 0;
	int j = 0;

	IMAGE imgProc = ImageAlloc( nImgWidth, nImgHeight );

	int nTotal = 0;
	int nTCnt = 0;
	for( i = 0; i < nImgHeight; i++ )
	{
		for( j = 0; j < nImgWidth; j++ )
		{
			int nPixVal = InImg[i][j];
			nTotal += nPixVal;
			nTCnt++;
		}
	}
	int nAverVal = nTotal / nTCnt;

	for( i = 0; i < nImgHeight; i++ )
	{
		for( j = 0; j < nImgWidth; j++ )
		{
			int nPixVal = InImg[i][j];
			nPixVal -= nAverVal;
			if( nPixVal < 0 ) nPixVal = 0;
			imgProc[i][j] = nPixVal;			
		}
	}

	VerEnhance( imgProc, OutImg, 10, 0.05f );

	ImageFree( imgProc );
}

void CheckVerEnhanceVal( IMAGE imgGray, IMAGE imgVer, int nThre )
{
	int nImgWidth = ImageWidth( imgGray );
	int nImgHeight = ImageHeight( imgGray );

	int nWref = ImageWidth( imgVer );
	int nHref = ImageHeight( imgVer );

#ifdef CHECK_ERROR
	ASSERT( nImgWidth == nWref && nImgHeight == nHref );
#endif

	if( nImgWidth != nWref || nImgHeight != nHref ) return;
	int i = 0;
	int j = 0;

//	int nTVal = 0;
//	int nTCnt = 0;
//	int nPixVal = 0;
//	for( i = 0; i < nImgHeight; i++ )
//	{
//		for( j = 0; j < nImgWidth; j++ )
//		{
//			if( imgVer[i][j] == 255 )
//			{
//				nPixVal = imgGray[i][j];
//				nTVal += nPixVal;
//				nTCnt++;
//			}
//		}
// 	}


	for( i = 0; i < nImgHeight; i++ )
	{
		for( j = 0; j < nImgWidth; j++ )
		{
			if( imgVer[i][j] == 255 )
			{
				if( imgGray[i][j] < nThre )
				{
					imgVer[i][j] = 0;
				}
			}
		}
	}
}

void GetPerPlateRect(IMAGE InImg, CRect m_ProcessRect, ObjRectArray &m_RectArray, BOOL bEliminNoise )
{
	int i = 0;
	int height = ImageHeight(InImg);
	int width = ImageWidth(InImg);

	const int  ObjColor=255;
	const int BKColor=0;
	
	IMAGE OutImage = IMGALLOC_MEMCHECK(width, height);
	int objNumber;// this possibily consider
	int xMin, xMax, yMin, yMax;
	PPRUNLENGTH Runlength = GETRUNLENGTH_MEMCHECK(InImg, ObjColor, objNumber, 1);
	
	int plateHeight;
	int plateWidth;


	for (i=0; i<objNumber; i++)
	{
		GetObjectXYLimit(Runlength[i], xMin, xMax, yMin, yMax);
		plateHeight = yMax - yMin;
		plateWidth = xMax - xMin;

		//if (plateHeight > MAX_CHAR_HEIGHT || plateHeight <= 1)
		if ( plateHeight > MAX_CHAR_HEIGHT )
		{
			ImgFillObject(InImg, Runlength[i], 0);
			continue;
		}
		
		if( bEliminNoise )
		{
			if ( plateHeight < MIN_CHAR_HEIGHT / 1.5f )
			{
				ImgFillObject(InImg, Runlength[i], 0);
				continue;
			}

			if ( (float)plateWidth / (float)plateHeight > 2.0f)
			{
				ImgFillObject(InImg, Runlength[i], 0);
				continue;
 			}
		}
 	}


	FREERUNLENGTH_MEMCHECK(objNumber, Runlength);
	
	int *iArray = new int[height];  // 水平投影数组
	memset(iArray, 0, sizeof(int)*height);

	int *iArray2 = new int[height];
	memset(iArray2, 0, sizeof(int)*height);
	
	BOOL hasSecond = FALSE;
	BOOL bfirstcomplite = FALSE;

	CPoint *PtArray = new CPoint[height];
	memset(PtArray, 0, sizeof(CPoint)*height);

	BOOL bflag1 = FALSE;
	BOOL bfindfirst = FALSE;
	int xper, xcur;

	typedef CArray<HORIZlINE,HORIZlINE> CHorizline;
	HORIZlINE line;

	CHorizline lineArray;
 
	int linecount = 0;

	BOOL btest = FALSE;

	for ( i=m_ProcessRect.bottom-WIDTH_CUT_EDGE; i>=m_ProcessRect.top+WIDTH_CUT_EDGE; i--)  // this is modify for fast
	{
		bflag1 = FALSE;
		bfindfirst = FALSE;
		int xdistance;
		for (int j=m_ProcessRect.left+WIDTH_CUT_EDGE; j<m_ProcessRect.right-WIDTH_CUT_EDGE; j++)
		{
			if (((!InImg[i][j]) && InImg[i][j+1]) || (InImg[i][j] && (!InImg[i][j+1])))
				OutImage[i][j] = ObjColor;

			if ((!bfindfirst) && (OutImage[i][j]))
			{
				bflag1 = TRUE;
				bfindfirst = TRUE;
				xper = j;
				xcur = j;
				iArray[i] = 1;
				PtArray[i].x = j;
				line.y = i;
				line.xStart = j;
			}
			else//if( bfindfirst || (!OutImage[i][j]) )
			{
				if (bflag1 && (OutImage[i][j]))
				{
					xper = xcur;
					xcur = j;
					xdistance = xcur - xper;
					
					if ( xdistance < X_DISTANCE )
					{
						if (xdistance <= X_MINDISTANCE)
						{
							;
						}
						else
						{
							iArray[i]++;
							PtArray[i].y = j;
							line.xEnd = j;
						}
					}
					else
					{
						if (iArray[i] < MIN_PIX)
						{
							iArray[i] = 0;
							bflag1 = FALSE;
							bfindfirst = FALSE;
							j--;
						}
						else
						{
							line.lab = -1;
							lineArray.Add(line);
							bflag1 = FALSE;
							bfindfirst = FALSE;
							iArray[i] = 0;
							j--;
							continue;
						}

						
					}
				}
			}

			if ( (iArray[i] > MIN_PIX)
				&& j == (int)(m_ProcessRect.right-WIDTH_CUT_EDGE-1)
				)  //如果线段有扫描线而且扫描到边上，结束
			{
				line.lab = -1;
				lineArray.Add(line);
			}
	
		}
	}

	GetAllRunLengthTest(lineArray, m_RectArray);

	//画扫描线
// 	int nCnt = lineArray.GetSize();
// 	for( int n = 0; n < nCnt; n++ )
// 	{
// 		int i = lineArray.GetAt(n).y;
// 
// 		int xStart = lineArray.GetAt(n).xStart;
// 		int xEnd = lineArray.GetAt(n).xEnd;
// 
// 		for(int j = xStart; j <= xEnd; j++ )
// 		{
// 			InImg[i][j] = 255;
// 		}
// 	}

//	memcpy( InImg[0], OutImage[0], height * width );//跳变点图像
	//针对跳变点图画扫描线
//	int nCnt = lineArray.GetSize();
//	for( int n = 0; n < nCnt; n++ )
//	{
//		int i = lineArray.GetAt(n).y;
//
//		int xStart = lineArray.GetAt(n).xStart;
//		int xEnd = lineArray.GetAt(n).xEnd;
//
//		for(int j = xStart; j <= xEnd; j++ )
//		{
//			InImg[i][j] = 255;
//		}
// 	}

	IMGFREE_MEMCHECK(OutImage);
	delete []PtArray;
	delete []iArray;
	delete []iArray2;
	
}

BOOL GetPerVerPlateRect(IMAGE InImg, CRect m_ProcessRect, ObjRectArray &m_RectArray, BOOL bEliminNoise )
{
	BOOL bNorm = TRUE;//Michael Add 2008-04-30
	int MIN_JPPTS = 5;
	int MAX_DIS = 45;

	int i = 0;
	int height = ImageHeight(InImg);
	int width = ImageWidth(InImg);

	const int  ObjColor=255;
	const int BKColor=0;
	
	IMAGE OutImage = IMGALLOC_MEMCHECK(width, height);

	int objNumber;// this possibily consider
	int xMin, xMax, yMin, yMax;
	PPRUNLENGTH Runlength = GETRUNLENGTH_MEMCHECK(InImg, ObjColor, objNumber, 1);
	
	int plateHeight;
	int plateWidth;


	for (i=0; i<objNumber; i++)
	{
		GetObjectXYLimit(Runlength[i], xMin, xMax, yMin, yMax);
		plateHeight = yMax - yMin;
		plateWidth = xMax - xMin;

		if ( plateWidth > MAX_CHAR_WIDTH * 0.8 )
		{
			ImgFillObject(InImg, Runlength[i], 0);
			continue;
		}

		
		if( bEliminNoise )
		{
			if( plateWidth <= 2 || plateHeight <= 2 )
			{
				ImgFillObject( InImg, Runlength[i], 0 );
				continue;
			}
		}
 	}


	FREERUNLENGTH_MEMCHECK(objNumber, Runlength);

	int *iArray = new int[width];  //垂直投影数组
	memset(iArray, 0, sizeof(int)*width);

	int *iArray2 = new int[width];
	memset(iArray2, 0, sizeof(int)*width);
	
	BOOL hasSecond = FALSE;
	BOOL bfirstcomplite = FALSE;

	CPoint *PtArray = new CPoint[width];
	memset(PtArray, 0, sizeof(CPoint)*width);

	BOOL bflag1 = FALSE;
	BOOL bfindfirst = FALSE;
	int xper, xcur;

	typedef CArray<HORIZlINE,HORIZlINE> CHorizline;
	HORIZlINE line;
	CHorizline lineArray;
 
	int linecount = 0;

	BOOL btest = FALSE;

	for ( i=m_ProcessRect.right-WIDTH_CUT_EDGE; i>=m_ProcessRect.left+WIDTH_CUT_EDGE; i--)  // this is modify for fast
	{
		bflag1 = FALSE;
		bfindfirst = FALSE;
		int xdistance;
		for (int j=m_ProcessRect.top+WIDTH_CUT_EDGE; j<m_ProcessRect.bottom-WIDTH_CUT_EDGE; j++)
		{
			ASSERT( j + 1 < height );
			ASSERT( i < width );
			if (((!InImg[j][i]) && InImg[j+1][i]) || (InImg[j][i] && (!InImg[j+1][i])))
				OutImage[j][i] = ObjColor;

			if ((!bfindfirst) && (OutImage[j][i]))
			{
				bflag1 = TRUE;
				bfindfirst = TRUE;
				xper = j;
				xcur = j;
				ASSERT( i < width );
				iArray[i] = 1;
				PtArray[i].x = j;
				line.y = i;
				line.xStart = j;
			}
			else//if( bfindfirst || (!OutImage[j][i]) )
			{
				ASSERT( j < height );
				ASSERT( i < width );
				if (bflag1 && (OutImage[j][i]))
				{
					xper = xcur;
					xcur = j;
					xdistance = xcur - xper;
					
					if ( xdistance < MAX_DIS )
					{
						if (xdistance <= X_MINDISTANCE)
						{
							;
						}
						else
						{
							iArray[i]++;
							PtArray[i].y = j;
							line.xEnd = j;
						}
					}
					else
					{
						if (iArray[i] < MIN_JPPTS)
						{
							iArray[i] = 0;
							bflag1 = FALSE;
							bfindfirst = FALSE;
							j--;
						}
						else
						{
							line.lab = -1;
							lineArray.Add(line);
							bflag1 = FALSE;
							bfindfirst = FALSE;
							iArray[i] = 0;
							j--;
							continue;
						}

						
					}
				}
			}

			if ( (iArray[i] >= MIN_JPPTS)
				&& j == (int)(m_ProcessRect.bottom-WIDTH_CUT_EDGE-1)
				)  //如果线段有扫描线而且扫描到边上，结束
			{
				line.lab = -1;
				lineArray.Add(line);
			}
	
		}
	}

	//画扫描线
	int nCnt = lineArray.GetSize();
	for( int n = 0; n < nCnt; n++ )
	{
		int i = lineArray.GetAt(n).y;

		int xStart = lineArray.GetAt(n).xStart;
		int xEnd = lineArray.GetAt(n).xEnd;

		for(int j = xStart; j <= xEnd; j++ )
		{
			ASSERT( j >= 0 );
			ASSERT( j < height );
			ASSERT( i >= 0 );
			ASSERT( i < width );
			InImg[j][i] = 255;
		}
 	}

	GetAllVerRunLengthTest(lineArray, m_RectArray);
	MergeSplitPlateRcs( m_RectArray );
	
	int nRcCnt = m_RectArray.GetSize(); 
	for( i = 0; i < nRcCnt; i++ )
	{
		CRect rcCur = m_RectArray.GetAt(i);
		CRect rcRot = CRect( rcCur.top, rcCur.left, rcCur.bottom, rcCur.right );
		m_RectArray.SetAt( i, rcRot );

		if( rcCur.Width() * rcCur.Height() > 640 * 480 / 2 )//Michael Add 2008-04-30
		{
			bNorm = FALSE;
		}
	}

	EreaseNoiseVerRect( m_RectArray );

	IMGFREE_MEMCHECK(OutImage);
	delete []PtArray;
	delete []iArray;
	delete []iArray2;
	
	return bNorm;
}

void GetAllVerRunLengthTest(CHorizline &lineArray, ObjRectArray &RectArray)
{
	RectArray.RemoveAll();
	int count = lineArray.GetSize();

	if (count == 0)
	{
		return;
	}

	BOOL bfirstGoal = FALSE;
	int number = 0;

	for (int i=count-1; i>=0; i--)
	{
		if (lineArray[i].lab == -1)
		{		
			lineArray[i].lab = number;
			number++;
		}

		for (int j=i-1; j>=0; j--)
		{
			if (lineArray[j].y - lineArray[i].y >=2)
			{
				break;
			}

			if (lineArray[j].y - lineArray[i].y == 1)
			{
				if (lineArray[i].xStart < lineArray[j].xEnd - 6 &&
					lineArray[i].xEnd > lineArray[j].xStart - 6 &&
					lineArray[j].lab == -1)
				{
					lineArray[j].lab = lineArray[i].lab;
				}
			}
		}

	}

	CHorizline *ObjLineArray = new CHorizline[number]; 



	//Get Object
	for ( i=count-1; i>=0; i--)
	{
		ASSERT(lineArray[i].lab >= 0);

		if (lineArray[i].lab >= 0)
		{
			ObjLineArray[lineArray[i].lab].Add(lineArray[i]);		
		}
	}

	SmoothEdge(ObjLineArray, number, RectArray, 0.1f);

	delete []ObjLineArray;
}

void GetAllRunLengthTest(CHorizline &lineArray, ObjRectArray &RectArray)
{
	RectArray.RemoveAll();
	int count = lineArray.GetSize();

	if (count == 0)
	{
		return;
	}

	BOOL bfirstGoal = FALSE;
	int number = 0;

	for (int i=count-1; i>=0; i--)
	{
		if (lineArray[i].lab == -1)
		{		
			lineArray[i].lab = number;
			number++;
		}

		for (int j=i-1; j>=0; j--)
		{
			if (lineArray[j].y - lineArray[i].y >=2)
			{
				break;
			}

			if (lineArray[j].y - lineArray[i].y == 1)
			{
				if (lineArray[i].xStart < lineArray[j].xEnd - 6 &&
					lineArray[i].xEnd > lineArray[j].xStart - 6 &&
					lineArray[j].lab == -1)
				{
					lineArray[j].lab = lineArray[i].lab;
				}
			}
		}

	}

	CHorizline *ObjLineArray = new CHorizline[number]; 



	//Get Object
	for ( i=count-1; i>=0; i--)
	{
		ASSERT(lineArray[i].lab >= 0);

		if (lineArray[i].lab >= 0)
		{
			ObjLineArray[lineArray[i].lab].Add(lineArray[i]);		
		}
	}

	SmoothEdge(ObjLineArray, number, RectArray);

	delete []ObjLineArray;
}

void SmoothEdge(CHorizline *ObjlineArray, int count, ObjRectArray& RectArray )
{

	int xMin, yMin, xMax, yMax;
	
	int tempStart = 0;
	int tempEnd = 0;
	for (int i=0; i<count; i++)
	{
		int lineNum = ObjlineArray[i].GetSize();
		if (lineNum == 0)
		{
			continue;
		}
		tempStart = 0;
		tempEnd = 0;
		xMin = yMin = 0xffff;
		xMax = yMax = 0;

		int *lineProStart = new int[lineNum];
		int *lineProEnd = new int[lineNum];

		for (int j=0; j<lineNum; j++)
		{	
			lineProStart[j] = ObjlineArray[i][j].xStart;
			lineProEnd[j] = ObjlineArray[i][j].xEnd;

			tempStart += ObjlineArray[i][j].xStart;
			tempEnd += ObjlineArray[i][j].xEnd;
			yMin = min(yMin, ObjlineArray[i][j].y);
			yMax = max(yMax, ObjlineArray[i][j].y);
		}

		SortDecline(lineProStart, lineNum); 
		xMin = lineProStart[lineNum / 3];
		SortDecline(lineProEnd, lineNum);
		xMax = lineProEnd[lineNum * 2 / 3];

		CRect rect = CRect(xMin, yMin ,xMax, yMax);
		RectArray.Add(rect);

		delete []lineProStart;
		delete []lineProEnd;
	}
	ObjRectArray tempArray;
	
	count = RectArray.GetSize();
	BOOL bInterSect = FALSE;
	for (i=0; i<count; i++)
	{
		if (RectArray[i] == CRect(0,0,0,0))
		{
			continue;
		}
		CRect r3;
		bInterSect = FALSE;
		for (int j=i+1; j<count; j++)
		{
			if (RectArray[j] == CRect(0,0,0,0))
			{
				continue;
			}
			CRect r1 = RectArray[i];
			CRect r2 = RectArray[j];
			r1.InflateRect(1 , 1);
			r2.InflateRect(1 , 1);
			if (r3.IntersectRect(&r1, &r2))
			{
				r3.left = min(r1.left, r2.left);
				r3.right = max(r1.right, r2.right);
				r3.top = min(r1.top, r2.top);
				r3.bottom = max(r1.bottom, r2.bottom);
				
				bInterSect = TRUE;
				
				RectArray[i] = CRect(0,0,0,0);
				RectArray[j] = CRect(0,0,0,0);
				tempArray.Add(r3);
			}
		}
		
		tempArray.Add(RectArray[i]);
	}
	
	count = tempArray.GetSize();
	RectArray.RemoveAll();
	RectArray.Copy(tempArray);
	
}

void SmoothEdge(CHorizline *ObjlineArray, int count, ObjRectArray& RectArray, float fRatio )
{

	int xMin, yMin, xMax, yMax;
	
	int tempStart = 0;
	int tempEnd = 0;
	for (int i=0; i<count; i++)
	{
		int lineNum = ObjlineArray[i].GetSize();
		if (lineNum == 0)
		{
			continue;
		}
		tempStart = 0;
		tempEnd = 0;
		xMin = yMin = 0xffff;
		xMax = yMax = 0;

		int *lineProStart = new int[lineNum];
		int *lineProEnd = new int[lineNum];

		for (int j=0; j<lineNum; j++)
		{	
			lineProStart[j] = ObjlineArray[i][j].xStart;
			lineProEnd[j] = ObjlineArray[i][j].xEnd;

			tempStart += ObjlineArray[i][j].xStart;
			tempEnd += ObjlineArray[i][j].xEnd;
			yMin = min(yMin, ObjlineArray[i][j].y);
			yMax = max(yMax, ObjlineArray[i][j].y);
		}

		SortDecline(lineProStart, lineNum);
		int nId1 = (int)(lineNum * fRatio);
		xMin = lineProStart[nId1];
		SortDecline(lineProEnd, lineNum);
		int nId2 = (int)(lineNum * (1.0f - fRatio));
		xMax = lineProEnd[nId2];

		CRect rect = CRect(xMin, yMin ,xMax, yMax);
		RectArray.Add(rect);

		delete []lineProStart;
		delete []lineProEnd;
	}
	ObjRectArray tempArray;
	
	count = RectArray.GetSize();
	BOOL bInterSect = FALSE;
	for (i=0; i<count; i++)
	{
		if (RectArray[i] == CRect(0,0,0,0))
		{
			continue;
		}
		CRect r3;
		bInterSect = FALSE;
		for (int j=i+1; j<count; j++)
		{
			if (RectArray[j] == CRect(0,0,0,0))
			{
				continue;
			}
			CRect r1 = RectArray[i];
			CRect r2 = RectArray[j];
			r1.InflateRect(1 , 1);
			r2.InflateRect(1 , 1);
			if (r3.IntersectRect(&r1, &r2))
			{
				r3.left = min(r1.left, r2.left);
				r3.right = max(r1.right, r2.right);
				r3.top = min(r1.top, r2.top);
				r3.bottom = max(r1.bottom, r2.bottom);
				
				bInterSect = TRUE;
				
				RectArray[i] = CRect(0,0,0,0);
				RectArray[j] = CRect(0,0,0,0);
				tempArray.Add(r3);
			}
		}
		
		tempArray.Add(RectArray[i]);
	}
	
	count = tempArray.GetSize();
	RectArray.RemoveAll();
	RectArray.Copy(tempArray);
	
}

void SortDecline(int *list,int num)
{
	//  冒泡排序  
	int i,j,temp;

	for ( i=0; i<num-1; i++ )
		for ( j=i+1; j<num; j++ )
		{
			if (list[j] <list[i] )
			{ 
			  temp=list[i];
			  list[i]=list[j];
			  list[j]=temp;
			}
		}
}

void EreaseNoiseRect(ObjRectArray &RectArray)
{
	int count = RectArray.GetSize();
	int xMin,yMin,xMax,yMax;
	//去除不是车牌预选区域的矩形
	for (int i=count-1; i>=0; i--)
	{
		xMin = RectArray[i].left;
		xMax =  RectArray[i].right;
		yMin =  RectArray[i].top;
		yMax =  RectArray[i].bottom;
		
		
		int plateExpWidth = xMax - xMin + 1;
		int plateExpHeight = yMax - yMin + 1;

		if ( plateExpWidth/plateExpHeight >= MAX_PLATE_WHRATIO || (float)plateExpWidth/plateExpHeight < MIN_PLATE_WHRATIO )
		{
			RectArray.RemoveAt(i);
			continue;
		}
		
		if (plateExpHeight < PLATE_MIN_HEIGHT )
		{
			RectArray.RemoveAt(i);
			continue;
		}
		
		if (plateExpWidth < PLATE_MIN_WIDTH)
		{
			RectArray.RemoveAt(i);
			continue;
		}
	}
}


void EreaseNoiseVerRect(ObjRectArray &RectArray)
{
	//int MIN_VERNUMRGN_HEIGHT = 200;
	int MIN_VERNUMRGN_HEIGHT = 150;
	int MIN_VERNUMRGN_WIDTH = 8;//10;//12;
	float MAX_VERNUMRGN_WHRATIO = 40.0f;
	float MIN_VERNUMRGN_WHRATIO = 8.0f;
	int count = RectArray.GetSize();
	int xMin,yMin,xMax,yMax;
	//去除不是车牌预选区域的矩形
	for (int i=count-1; i>=0; i--)
	{
		xMin = RectArray[i].left;
		xMax =  RectArray[i].right;
		yMin =  RectArray[i].top;
		yMax =  RectArray[i].bottom;
		
		
		int plateExpWidth = xMax - xMin + 1;
		int plateExpHeight = yMax - yMin + 1;

		if ((float)plateExpHeight/plateExpWidth >= MAX_VERNUMRGN_WHRATIO || (float)plateExpHeight/plateExpWidth < MIN_VERNUMRGN_WHRATIO )
		{
			RectArray.RemoveAt(i);
			continue;
		}
		
		if (plateExpHeight < MIN_VERNUMRGN_HEIGHT )
		{
			RectArray.RemoveAt(i);
			continue;
		}
		
		if (plateExpWidth < MIN_VERNUMRGN_WIDTH )
		{
			RectArray.RemoveAt(i);
			continue;
		}
	}
}

BOOL MergeSplitPlateRcs( ObjRectArray &rcArray )
{
	int nCnt = rcArray.GetSize();
	if( nCnt <= 1 ) return FALSE;

	BOOL bMerge = FALSE;
	SortRectFromTtoB( rcArray );
	CRect rc1;
	CRect rc2;
	int i, j;
	for( i = nCnt - 1 ; i >= 1 ; i-- )
	{
		rc1 = rcArray.GetAt( i );
		for( j = i - 1 ; j >= 0 ; j-- )
		{
			rc2 = rcArray.GetAt( j );

			int nVSpace = rc1.top - rc2.bottom;
			if( nVSpace <= 3 )//if( nVSpace <= 2 ) -- Michael Changed 20080725
			{
				int idx1 = max( rc1.left, rc2.left );
				int idx2 = min( rc1.right, rc2.right );
				int nHShare = idx2 - idx1;
				int nHOrg = max( rc1.Width() , rc2.Width() );
				if( (float)nHShare / (float)nHOrg >= 0.8 )
				{
					CRect rcMerge = MergeRects( rc1, rc2 );
					rcArray.RemoveAt( i );
					rcArray.SetAt( j, rcMerge );
					bMerge = TRUE;
					break;
				}
			}
		}
	}

	return bMerge;
}

BOOL DajinBin(IMAGE imgIn, IMAGE imgOut, CRect RPlate)
{
	int w1 = RPlate.Width() + 1;
	int h1 = RPlate.Height() + 1;

	int w2 = ImageWidth(imgOut);
	int h2 = ImageHeight(imgOut);

	if( (h2 < h1) || (w2 < w1) )
	{
		return FALSE;
	}

	CRect r = RPlate;
	r.top = min( r.top+2, r.bottom );
	r.bottom = max( r.bottom-3, r.top );
	r.left = RPlate.left + (w1-1) / 3;
 	r.right = RPlate.left + (w1-1) * 2/ 3;

	IMAGE img;
	img = IMGALLOC_MEMCHECK(r.Width()+1, r.Height()+1);
	GetImageRectPart(imgIn, img, r);
	int nThre = ImgDaJinThreshold(img);
	//nThre = int( nThre * 1.1 );
	IMGFREE_MEMCHECK( img );

	img = IMGALLOC_MEMCHECK( w1, h1 );
	GetImageRectPart( imgIn, img, RPlate );
	ImgSingleSegment(img, imgOut, nThre);
	IMGFREE_MEMCHECK(img);

	return TRUE;
}

BOOL DajinBin( IMAGE imgIn, IMAGE imgOut, CRect RPlate, float fRatio )//此函数需要拆分，分为阈值计算和二值化两部分
{
	BOOL bAb = FALSE;

	int w1 = RPlate.Width() + 1;
	int h1 = RPlate.Height() + 1;

	int w2 = ImageWidth(imgOut);
	int h2 = ImageHeight(imgOut);

	if( (h2 < h1) || (w2 < w1) )
	{
		return FALSE;
	}

	CRect r = RPlate;
	r.top = min( r.top+2, r.bottom );
	r.bottom = max( r.bottom-3, r.top );
	r.left = RPlate.left + (w1-1) / 3;
 	r.right = RPlate.left + (w1-1) * 2/ 3;

	IMAGE img;
	img = IMGALLOC_MEMCHECK(r.Width()+1, r.Height()+1);
	GetImageRectPart(imgIn, img, r);
	int nThre = ImgDaJinThreshold(img);
	nThre = int( nThre * (1.0f + fRatio) );

	if( nThre > 255 )
	{
		nThre = 250;
		bAb = TRUE;
 	}

	IMGFREE_MEMCHECK( img );

	img = IMGALLOC_MEMCHECK( w1, h1 );
	GetImageRectPart( imgIn, img, RPlate );
	ImgSingleSegment(img, imgOut, nThre);
	IMGFREE_MEMCHECK(img);

	return !bAb;
}

void LLTFast(IMAGE inImg, IMAGE outImg, int w , int ithreshold)
{
	int width = ImageWidth(inImg);
	int height = ImageHeight(outImg);

	memset(outImg[0], 0, height*width*sizeof(char));

	IMAGE AverageImg = IMGALLOC_MEMCHECK(width, height);
	IMAGE AverageImgV = IMGALLOC_MEMCHECK(width, height);

	int iaverage = 0;
	int itotle = 0;
	BOOL bfirst = TRUE;

	int itemp;
	int length = 2 * w + 1;
  	for (int i=0; i<height; i++)  //计算水平方向平均
	{
		bfirst = TRUE;
		itemp = 0;
		for (int j=w; j<width-w; j++)
		{
			if (bfirst)
			{
				for (int k=-w; k<=w; k++)
				{
					itemp +=  inImg[i][j+k];				
				}
				AverageImg[i][j] = itemp / (2*w+1);
				bfirst = FALSE;
			}
			else
			{
			   itemp = itemp - inImg[i][j-w-1] + inImg[i][j+w];
			   AverageImg[i][j] = itemp / length;
			}
		
		}
	}

	for (i=0; i<width; i++)  //计算垂直方向平均
	{
		bfirst = TRUE;
		itemp = 0;
		for (int j=w; j<height-w; j++)
		{
			if (bfirst)
			{
				for (int k=-w; k<=w; k++)
				{
					itemp +=  AverageImg[j+k][i];				
				}
				AverageImgV[j][i] = itemp / (2*w+1);
				bfirst = FALSE;
			}
			else
			{
			   itemp = itemp - AverageImg[j-w-1][i] + AverageImg[j+w][i];
			   AverageImgV[j][i] = itemp / length;
			}
		
		}
	}


	BOOL bGoal = FALSE;
	BOOL bBack = FALSE;

	for (i=w; i<height-w; i++)
	{
		for (int j=w; j<width-w; j++)
		{

			#define  p0  AverageImgV[i][j-w]
			#define  p1  AverageImgV[i+w][j-w]
			#define  p2  AverageImgV[i+w][j]	
			#define  p3  AverageImgV[i+w][j+w]	
			#define  p4  AverageImgV[i][j+w]
			#define  p5  AverageImgV[i-w][j+w]
			#define  p6  AverageImgV[i-w][j]
			#define  p7  AverageImgV[i-w][j-w]

			bGoal = FALSE;
			if ( p0 - inImg[i][j] > ithreshold  && p1 - inImg[i][j] > ithreshold  
				&& p4 - inImg[i][j] > ithreshold  && p5 - inImg[i][j] > ithreshold )
			{
				bGoal = TRUE;
			}
			else if (p1 - inImg[i][j] > ithreshold  && p2 - inImg[i][j] >ithreshold
				&& p5 - inImg[i][j] > ithreshold  && p6 - inImg[i][j] > ithreshold)
			{
				bGoal = TRUE;
			}
			else if (p2 - inImg[i][j] > ithreshold  && p3 - inImg[i][j] > ithreshold 
				&& p6 - inImg[i][j] > ithreshold  && p7 - inImg[i][j] > ithreshold)
			{
				bGoal = TRUE;
			}
			else if (p3 - inImg[i][j] > ithreshold  && p4 - inImg[i][j] > ithreshold  
				&& p7 - inImg[i][j] > ithreshold  && p0 - inImg[i][j] > ithreshold )
			{
				bGoal = TRUE;
			}

			if (bGoal)
			{
				outImg[i][j] = 255;
			}		
		}
	}

//	IMGFREE_MEMCHECK(ModelImg);
	IMGFREE_MEMCHECK(AverageImg);
	IMGFREE_MEMCHECK(AverageImgV);
}

void GetObjRect(IMAGE &image, ObjRectArray &VObjArray)
{
	VObjArray.RemoveAll();

	int width = ImageWidth(image);
	int height = ImageHeight(image);
	
	if( width < 5 || height < 5 ) return;

	//CRect rect(0, 0, ImageWidth(image)-1, ImageHeight(image)-1);
	////寻找目标
	
	int xMin,xMax,yMin,yMax;
	
	float tg2=0.0;
	float ratio=0.0;

	
	float ratiohw = 0.0;
	
	int yMinNum = 0;
	int yMinAver = 0;
	int yMaxAver = 0;
	
	
	int runlennum=0;
	PPRUNLENGTH runlen = GETRUNLENGTH_MEMCHECK( image, 255, runlennum,1);	// 目标行程  //new
	
	for(int i=0; i<runlennum; i++)
	{
		GetObjectXYLimit(runlen[i],xMin,xMax,yMin,yMax);
		VObjArray.Add(CRect(xMin, yMin, xMax, yMax));
		
	}
	
	FREERUNLENGTH_MEMCHECK(runlennum, runlen);	// delete
	
	
	// 消除无用的太小的矩形
	for(i=runlennum-1; i>=0; i--)
	{
		CRect r = VObjArray.GetAt(i);
		int w = r.Width() + 1;
		int h = r.Height() + 1;
		if( w <= 1 || h <= 1 )
		{
			VObjArray.RemoveAt(i);
			continue;
		}
	}

	runlennum = VObjArray.GetSize();
	for( i = runlennum - 1; i >= 0; i-- )
	{
		CRect r = VObjArray.GetAt(i);
		int w = r.Width() + 1;
		int h = r.Height() + 1;
		ratiohw = (float)h/(float)w;
		if(  w < MAX_CHAR_WIDTH && h > MIN_CHAR_HEIGHT && h < MAX_CHAR_HEIGHT )
		{
			continue;
		}
		VObjArray.RemoveAt(i);
	}
}

void GetObjRect(IMAGE &image, ObjRectArray &VObjArray, ObjRectArray &AllRectArray)
{
	VObjArray.RemoveAll();
	AllRectArray.RemoveAll();
	
	int width = ImageWidth(image) ;
	int height = ImageHeight(image) ;

	if( width <= 5 || height <= 5 ) return;
	
	//CRect rect(0, 0, ImageWidth(image)-1, ImageHeight(image)-1);
	////寻找目标
	
	int xMin,xMax,yMin,yMax;
	
	float tg2=0.0;
	float ratio=0.0;

	
	float ratiohw = 0.0;
	
	int yMinNum = 0;
	int yMinAver = 0;
	int yMaxAver = 0;
	
	
	int runlennum=0;
	PPRUNLENGTH runlen = GETRUNLENGTH_MEMCHECK( image, 255, runlennum,1);	// 目标行程  //new

	for(int i=0; i<runlennum; i++)
	{
		GetObjectXYLimit(runlen[i], xMin, xMax, yMin, yMax);
		int nArea = ObjArea(runlen[i]);
		int nh = yMax - yMin + 1;
		int nw = xMax - xMin + 1;

		if ( nh * 2.5 > nArea )
		{
			ImgFillObject(image, runlen[i], 0);
			continue;
		}

		if( nh <= 1 || nw <= 1 )
		{
			ImgFillObject( image, runlen[i], 0 );
			continue;
		}
		
		VObjArray.Add(CRect(xMin, yMin, xMax, yMax));
 	}
	
//	for(int i=0; i<runlennum; i++)
//	{
//		GetObjectXYLimit(runlen[i],xMin,xMax,yMin,yMax);
//		VObjArray.Add(CRect(xMin, yMin, xMax, yMax));
//		
// 	}
	
	FREERUNLENGTH_MEMCHECK(runlennum, runlen);	// delete
	
	
	// 消除无用的太小的矩形
	runlennum = VObjArray.GetSize();
	for(i=runlennum-1; i>=0; i--)
	{
		CRect r = VObjArray.GetAt(i);
		int w = r.Width() + 1;
		int h = r.Height() + 1;
		if( w <= 1 || h <= 1 )
		{
			VObjArray.RemoveAt(i);
			continue;
		}
	}

	AllRectArray.RemoveAll();
	AllRectArray.Copy( VObjArray );
	runlennum = VObjArray.GetSize();

	for( i = runlennum - 1; i >= 0; i-- )
	{
		CRect r = VObjArray.GetAt(i);
		int w = r.Width() + 1;
		int h = r.Height() + 1;
		ratiohw = (float)h/(float)w;
		if(  w < MAX_CHAR_WIDTH && h > MIN_CHAR_HEIGHT && h < MAX_CHAR_HEIGHT && w > NOISE_CHAR_WIDTH )
		{
			continue;
		}
		VObjArray.RemoveAt(i);
	}
}

void GetObjRectForTopView(IMAGE &image, ObjRectArray &VObjArray, ObjRectArray &AllRectArray)
{
	VObjArray.RemoveAll();
	AllRectArray.RemoveAll();

	int width = ImageWidth(image) ;
	int height = ImageHeight(image) ;

	if( width < 5 || height < 5 ) return;
	
	//CRect rect(0, 0, ImageWidth(image)-1, ImageHeight(image)-1);
	////寻找目标
	
	int xMin,xMax,yMin,yMax;
	
	float tg2=0.0;
	float ratio=0.0;

	
	float ratiohw = 0.0;
	
	int yMinNum = 0;
	int yMinAver = 0;
	int yMaxAver = 0;
	
	
	int runlennum=0;
	PPRUNLENGTH runlen = GETRUNLENGTH_MEMCHECK( image, 255, runlennum,1);	// 目标行程  //new
	
	for(int i=0; i<runlennum; i++)
	{
		GetObjectXYLimit(runlen[i],xMin,xMax,yMin,yMax);
		VObjArray.Add(CRect(xMin, yMin, xMax, yMax));
		
	}
	
	FREERUNLENGTH_MEMCHECK(runlennum, runlen);	// delete
	
	
	// 消除无用的太小的矩形
	for(i=runlennum-1; i>=0; i--)
	{
		CRect r = VObjArray.GetAt(i);
		int w = r.Width() + 1;
		int h = r.Height() + 1;
		if( w <= 1 || h <= 1 )
		{
			VObjArray.RemoveAt(i);
			continue;
		}
	}

	AllRectArray.RemoveAll();
	AllRectArray.Copy( VObjArray );
	runlennum = VObjArray.GetSize();

	for( i = runlennum - 1; i >= 0; i-- )
	{
		CRect r = VObjArray.GetAt(i);
		int w = r.Width() + 1;
		int h = r.Height() + 1;
		ratiohw = (float)h/(float)w;
		if(  w < MAX_TOP_CHAR_WIDTH && h >= MIN_TOP_CHAR_HEIGHT && h < MAX_TOP_CHAR_HEIGHT && w > NOISE_CHAR_WIDTH )
		{
			continue;
		}
		VObjArray.RemoveAt(i);
	}
}

//void GetAverRcWH( ObjRectArray &rcArray, int &w, int &h )
//{
//	int i;
//	int nCnt = rcArray.GetSize();
//
//	if( nCnt > 11 * 2 )//Too many rects! Drop!
//	{
//		w = 0;
//		h = 0;
//		return;
//	}
//
//	w = 0;
//	h = 0;
//
//	int nRcHmin = 0;
//	int nRcHmax = 0;
//	for( i = 0; i < nCnt; i++ )
//	{
//		CRect rcTmp = rcArray.GetAt(i);
//		int nRcH = rcTmp.Height();
//
//		if( nRcHmin == 0 ) nRcHmin = nRcH;
//		if( nRcHmax == 0 ) nRcHmax = nRcH;
//		if( nRcH < nRcHmin ) nRcHmin = nRcH;
//		if( nRcH > nRcHmax ) nRcHmax = nRcH;
//	}
//
//	int *nHValueArray = new int[ nRcHmax - nRcHmin + 1 ];
//	memset( nHValueArray, 0, (nRcHmax - nRcHmin + 1)*sizeof(int) );
//	for( i = 0; i < nCnt; i++ )
//	{
//		CRect rcTmp = rcArray.GetAt(i);
//		int nRcH = rcTmp.Height();
//		nHValueArray[ nRcH - nRcHmin ]++;
//	}
//
//	int nHCur = nRcHmin;
//	int nCharH1 = nRcHmin;//first
//	int nCharH2 = nRcHmin;//last
//	int nIndex = 0;
//	int nRcCnt = 0;
//	int nRcMaxCnt = 0;
//	for( i = 0; i < nRcHmax - nRcHmin + 1; i++ )
//	{
//		if( nHValueArray[i] > 0 )
//		{
//			if( (i - nIndex) < (float)( i + nRcHmin ) * 0.15f )
//			{
//				nRcCnt += nHValueArray[i];
//			}
//			else
//			{				
//				if( nRcCnt > nRcMaxCnt )
//				{
//					nCharH1 = nHCur;
//					nCharH2 = nIndex + nRcHmin;
//					nRcMaxCnt = nRcCnt;
//				}
//				nHCur = i + nRcHmin;
//				nRcCnt = nHValueArray[i];
//			}
//			nIndex = i;
//		}
//	}
//
//	if( nRcCnt > nRcMaxCnt )
//	{
//		nCharH1 = nHCur;
//		nCharH2 = nRcHmax;
//	}
//
//	delete[] nHValueArray;
//
//
//	int nValH = 0;
//	int nValW = 0;
//
//	int w2 = 0;
//	int nValW2 = 0;
//
//	for( i = 0; i < nCnt; i++ )
//	{
//		CRect rcTmp = rcArray.GetAt(i);
//
//		int nRcH = rcTmp.Height();
//		int nRcW = rcTmp.Width();
//
//		if( nRcW == 0 ) continue;
//		float fHWRatio = (float)nRcH / (float)nRcW;
//
//		if( (nCharH1 <= nRcH) && (nCharH2 >= nRcH) )
//		{
//			h += nRcH;
//			nValH++;
//			//if( ((3*nRcW) >= (2*nRcH)) && ( nRcW < nRcH ) )
//			if( fHWRatio > 1.5 && fHWRatio < 2.5 )
//			{
//				w += nRcW;
//				nValW++;
//			}
//			else if( fHWRatio < 3.0 )
//			{
//				w2 += nRcW;
//				nValW2++;
//			}
//		}
//	}
//
//	if( nValH != 0 ) h /= nValH;
//	
//	if( nValW != 0 ) w /= nValW;
//	else if ( nValW2 != 0 ) w = w2 / nValW2;
//}

void GetAverRcWH( ObjRectArray &rcArray, int &w, int &h )
{
	int i;
	int nCnt = rcArray.GetSize();

	if( nCnt > 11 * 2 )//Too many rects! Drop!
	{
		w = 0;
		h = 0;
		return;
	}

	w = 0;
	h = 0;

	int nRcHmin = 0;
	int nRcHmax = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcTmp = rcArray.GetAt(i);
		int nRcH = rcTmp.Height();

		if( nRcHmin == 0 ) nRcHmin = nRcH;
		if( nRcHmax == 0 ) nRcHmax = nRcH;
		if( nRcH < nRcHmin ) nRcHmin = nRcH;
		if( nRcH > nRcHmax ) nRcHmax = nRcH;
	}

	int *nHValueArray = new int[ nRcHmax - nRcHmin + 1 ];
	memset( nHValueArray, 0, (nRcHmax - nRcHmin + 1)*sizeof(int) );
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcTmp = rcArray.GetAt(i);
		int nRcH = rcTmp.Height();
		nHValueArray[ nRcH - nRcHmin ]++;
	}

	int nHCur = nRcHmax;//nRcHmin;
	int nCharH1 = nRcHmin;//first
	int nCharH2 = nRcHmin;//last
	int nIndex = nRcHmax-nRcHmin;//from last to first
	//int nCurIndex = nRcHmax-nRcHmin;
	int nRcCnt = 0;
	int nRcMaxCnt = 0;
	//for( i = 0; i < nRcHmax - nRcHmin + 1; i++ )
	for( i = nRcHmax - nRcHmin; i >= 0; i-- )
	{
		if( nHValueArray[i] > 0 )
		{
			if( (nIndex - i) < (float)( i + nRcHmin ) * 0.15f )
			{
				nRcCnt += nHValueArray[i];
			}
			else
			{				
				if( nRcCnt > nRcMaxCnt )
				{
					nCharH1 = nIndex+nRcHmin;
					nCharH2 = nHCur;
					nRcMaxCnt = nRcCnt;
				}

				if( nRcCnt >= 5 ) 
				{
					break;//focus on the larger ones
				}
				
				nHCur = i + nRcHmin;//Update nHCur
				nRcCnt = nHValueArray[i];

				//nCurIndex = i;
			}
			nIndex = i;
		}
	}

	if( nRcCnt > nRcMaxCnt )
	{
		nCharH1 = nIndex+nRcHmin;
		nCharH2 = nHCur;
	}

	delete[] nHValueArray;


	int nValH = 0;
	int nValW = 0;

	int w2 = 0;
	int nValW2 = 0;

	for( i = 0; i < nCnt; i++ )
	{
		CRect rcTmp = rcArray.GetAt(i);

		int nRcH = rcTmp.Height();
		int nRcW = rcTmp.Width();

		if( nRcW == 0 ) continue;
		float fHWRatio = (float)nRcH / (float)nRcW;

		if( (nCharH1 <= nRcH) && (nCharH2 >= nRcH) )
		{
			h += nRcH;
			nValH++;
			//if( ((3*nRcW) >= (2*nRcH)) && ( nRcW < nRcH ) )
			if( fHWRatio > 1.5 && fHWRatio < 2.5 )
			{
				w += nRcW;
				nValW++;
			}
			else if( fHWRatio < 3.0 )
			{
				w2 += nRcW;
				nValW2++;
			}
		}
	}

	if( nValH != 0 ) h /= nValH;
	
	if( nValW != 0 ) w /= nValW;
	else if ( nValW2 != 0 ) w = w2 / nValW2;
}


void GetAverRcWHForTopView( ObjRectArray &rcArray, int &w, int &h )
{
	int i;
	int nCnt = rcArray.GetSize();

	if( nCnt > 11 * 2 )//Too many rects! Drop!
	{
		w = 0;
		h = 0;
		return;
	}

	w = 0;
	h = 0;

	int nRcHmin = 0;
	int nRcHmax = 0;
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcTmp = rcArray.GetAt(i);
		int nRcH = rcTmp.Height();

		if( nRcHmin == 0 ) nRcHmin = nRcH;
		if( nRcHmax == 0 ) nRcHmax = nRcH;
		if( nRcH < nRcHmin ) nRcHmin = nRcH;
		if( nRcH > nRcHmax ) nRcHmax = nRcH;
	}

	int *nHValueArray = new int[ nRcHmax - nRcHmin + 1 ];
	memset( nHValueArray, 0, (nRcHmax - nRcHmin + 1)*sizeof(int) );
	for( i = 0; i < nCnt; i++ )
	{
		CRect rcTmp = rcArray.GetAt(i);
		int nRcH = rcTmp.Height();
		nHValueArray[ nRcH - nRcHmin ]++;
	}

	int nHCur = nRcHmin;
	int nCharH1 = nRcHmin;//first
	int nCharH2 = nRcHmin;//first
	int nIndex = 0;
	int nRcCnt = 0;
	int nRcMaxCnt = 0;
	for( i = 0; i < nRcHmax - nRcHmin + 1; i++ )
	{
		if( nHValueArray[i] > 0 )
		{
			if( (i - nIndex) < (float)( i + nRcHmin ) * 0.15f )
			{
				nRcCnt += nHValueArray[i];
			}
			else
			{				
				if( nRcCnt > nRcMaxCnt )
				{
					nCharH1 = nHCur;
					nCharH2 = nIndex + nRcHmin;
					nRcMaxCnt = nRcCnt;
				}
				nHCur = i + nRcHmin;
				nRcCnt = nHValueArray[i];
			}
			nIndex = i;
		}
	}

	if( nRcCnt > nRcMaxCnt )
	{
		nCharH1 = nHCur;
		nCharH2 = nRcHmax;
	}

	delete[] nHValueArray;


	int nValH = 0;
	int nValW = 0;

	int w2 = 0;
	int nValW2 = 0;

	for( i = 0; i < nCnt; i++ )
	{
		CRect rcTmp = rcArray.GetAt(i);

		int nRcH = rcTmp.Height();
		int nRcW = rcTmp.Width();

		if( nRcW == 0 ) continue;
		float fHWRatio = (float)nRcH / (float)nRcW;

		if( (nCharH1 <= nRcH) && (nCharH2 >= nRcH) )
		{
			h += nRcH;
			nValH++;
			//if( ((3*nRcW) >= (2*nRcH)) && ( nRcW < nRcH ) )
			if( fHWRatio > 0.75 && fHWRatio < 1.25 )
			{
				w += nRcW;
				nValW++;
			}
			else if( fHWRatio < 1.8 )
			{
				w2 += nRcW;
				nValW2++;
			}
		}
	}

	if( nValH != 0 ) h /= nValH;
	
	if( nValW != 0 ) w /= nValW;
	else if ( nValW2 != 0 ) w = w2 / nValW2;
}


BOOL GetRcDis( ObjRectArray &charArray, int &nRcDis )
{
	int i;
	int nCnt1 = charArray.GetSize();
	if( nCnt1 <= 1 ) return FALSE;
	
	SortRect_T2B(charArray);

	int *nDisArray = new int[nCnt1-1];
	memset( nDisArray, 0, sizeof(int) * (nCnt1-1) );
	int *nDisArray2 = new int[nCnt1-1];
	memset( nDisArray2, 0, sizeof(int) * (nCnt1-1) );
	int *nAddCnt = new int[nCnt1-1];
	memset( nAddCnt, 0, sizeof(int) * (nCnt1-1) );
	int *nAddCnt2 = new int[nCnt1-1];
	memset( nAddCnt2, 0, sizeof(int) * (nCnt1-1) );

	CRect rcCur = charArray.GetAt(0);
	CRect rcNext;
	int nCurPos1 = 0;
	int nCurPos2 = 0;

	for( i = 0; i < nCnt1 - 1; i++, rcCur = rcNext )
	{
		rcNext = charArray.GetAt(i+1);

		if( !RcsInSameLine(rcCur,rcNext) ) continue;

		int w1 = rcCur.Width();
		int h1 = rcCur.Height();

		int w2 = rcNext.Width();
		int h2 = rcNext.Height();

		BOOL bCharI = FALSE;
		if( ((float)h1 / (float)w1 >= CHAR_I_HW_RATIO_MIN) || ((float)h2 / (float)w2 >= CHAR_I_HW_RATIO_MIN) )
		{
			bCharI = TRUE;
		}

		int nDisCur = rcNext.left - rcCur.right;

		for( int ii = 0; ii < nCurPos1; ii++ )
		{
			int nDisRef = nDisArray[ii] / nAddCnt[ii];
			float fDifRatio = abs(nDisCur-nDisRef) / (float)( max( max(nDisCur,nDisRef), 10 ) );
			if( fDifRatio < 0.30f && !bCharI )
			{
				nDisArray[ii] += nDisCur;
				nAddCnt[ii]++;
			}
		}

		if( ii == nCurPos1 )
		{
			ASSERT( nCurPos1 < nCnt1 - 1 );
			nDisArray[nCurPos1] = nDisCur;
			nAddCnt[nCurPos1]++;
			nCurPos1++;
		}

		for( ii = 0; ii < nCurPos2; ii++ )
		{
			int nDisRef = nDisArray[ii] / nAddCnt[ii];
			float fDifRatio = abs(nDisCur-nDisRef) / (float)( max( max(nDisCur,nDisRef), 10 ) );
			if( fDifRatio < 0.30f )
			{
				nDisArray2[ii] += nDisCur;
				nAddCnt2[ii]++;
			}
		}

		if( ii == nCurPos2 )
		{
			ASSERT( nCurPos2 < nCnt1 - 1 );
			nDisArray2[nCurPos2] = nDisCur;
			nAddCnt2[nCurPos2]++;
			nCurPos2++;
		}

	}

	//BOOL bGetRcDis = TRUE;
	BOOL bGetRcDis = FALSE;
	
	int nPos = 0;
	int nMaxNum = nAddCnt[0];
	if( nMaxNum > 1 ) bGetRcDis = TRUE;
	for( i = 1; i < nCurPos1; i++ )
	{
		if( nAddCnt[i] > nMaxNum )
		{
			nPos = i;
			nMaxNum = nAddCnt[i];
			bGetRcDis = TRUE;
		}
		else if( nAddCnt[i] == nMaxNum )
		{
			int nRcDis1 = nDisArray[nPos] / (float)nAddCnt[nPos] + 0.5;
			int nRcDis2 = nDisArray[i] / (float)nAddCnt[i] + 0.5;
			float fDifRatio = abs(nRcDis1-nRcDis2) / (float)( max( max(nRcDis1,nRcDis2), 10 ) );
			if( fDifRatio < 0.30f )
			{
				nDisArray[i] = (nDisArray[nPos] + nDisArray[i])/2 +0.5;
				nPos = i;
				nMaxNum = nAddCnt[i];
				bGetRcDis = TRUE;
			}
			else
			{
				bGetRcDis = FALSE;
			}

		}
	}

	if( bGetRcDis )
	{
		nRcDis = nDisArray[nPos] / (float)nAddCnt[nPos] + 0.5;
	}
	else
	{
		bGetRcDis = TRUE;
		nPos = 0;
		nMaxNum = nAddCnt2[0];
		for( i = 1; i < nCurPos2; i++ )
		{
			if( nAddCnt2[i] > nMaxNum )
			{
				nPos = i;
				nMaxNum = nAddCnt2[i];
				bGetRcDis = TRUE;
			}
			else if( nAddCnt2[i] == nMaxNum )
			{
				int nRcDis1 = nDisArray2[nPos] / (float)nAddCnt2[nPos] + 0.5;
				int nRcDis2 = nDisArray2[i] / (float)nAddCnt2[i] + 0.5;
				float fDifRatio = abs(nRcDis1-nRcDis2) / (float)( max( max(nRcDis1,nRcDis2), 10 ) );
				if( fDifRatio < 0.30f )
				{
					nDisArray2[i] = (nDisArray2[nPos] + nDisArray2[i])/2 +0.5;
					nPos = i;
					nMaxNum = nAddCnt2[i];
					bGetRcDis = TRUE;
				}
				else
				{
					bGetRcDis = FALSE;
				}

			}
		}

		if( bGetRcDis )
		{
			nRcDis = nDisArray2[nPos] / (float)nAddCnt2[nPos] + 0.5;
		}
	}


	delete[] nAddCnt;
	delete[] nAddCnt2;
	delete[] nDisArray;
	delete[] nDisArray2;

	return bGetRcDis;	
}

BOOL GetRcDisForTopView( ObjRectArray &charArray, int &nRcDis )
{
	int i;
	int nCnt1 = charArray.GetSize();
	if( nCnt1 <= 1 ) return FALSE;
	
	SortRect_T2B(charArray);

	int *nDisArray = new int[nCnt1-1];
	memset( nDisArray, 0, sizeof(int) * (nCnt1-1) );
	int *nDisArray2 = new int[nCnt1-1];
	memset( nDisArray2, 0, sizeof(int) * (nCnt1-1) );
	int *nAddCnt = new int[nCnt1-1];
	memset( nAddCnt, 0, sizeof(int) * (nCnt1-1) );
	int *nAddCnt2 = new int[nCnt1-1];
	memset( nAddCnt2, 0, sizeof(int) * (nCnt1-1) );

	CRect rcCur = charArray.GetAt(0);
	CRect rcNext;
	int nCurPos1 = 0;
	int nCurPos2 = 0;

	for( i = 0; i < nCnt1 - 1; i++, rcCur = rcNext )
	{
		rcNext = charArray.GetAt(i+1);

		if( !RcsInSameLine(rcCur,rcNext) ) continue;

		int w1 = rcCur.Width() + 1;
		int h1 = rcCur.Height() + 1;

		int w2 = rcNext.Width() + 1;
		int h2 = rcNext.Height() + 1;

		BOOL bCharI = FALSE;
		if( ((float)h1 / (float)w1 >= TOP_CHAR_I_HW_RATIO_MIN) || ((float)h2 / (float)w2 >= TOP_CHAR_I_HW_RATIO_MIN) )
		{
			bCharI = TRUE;
		}

		int nDisCur = rcNext.left - rcCur.right;
		if( nDisCur < 0 ) nDisCur = 0;

		for( int ii = 0; ii < nCurPos1; ii++ )
		{
			int nDisRef = nDisArray[ii] / nAddCnt[ii];
			float fDifRatio = abs(nDisCur-nDisRef) / (float)( max( max(nDisCur,nDisRef), 10 ) );
			if( fDifRatio < 0.30f && !bCharI )
			{
				nDisArray[ii] += nDisCur;
				nAddCnt[ii]++;
			}
		}

		if( ii == nCurPos1 )
		{
			ASSERT( nCurPos1 < nCnt1 - 1 );
			nDisArray[nCurPos1] = nDisCur;
			nAddCnt[nCurPos1]++;
			nCurPos1++;
		}

		for( ii = 0; ii < nCurPos2; ii++ )
		{
			int nDisRef = nDisArray[ii] / nAddCnt[ii];
			float fDifRatio = abs(nDisCur-nDisRef) / (float)( max( max(nDisCur,nDisRef), 10 ) );
			if( fDifRatio < 0.30f )
			{
				nDisArray2[ii] += nDisCur;
				nAddCnt2[ii]++;
			}
		}

		if( ii == nCurPos2 )
		{
			ASSERT( nCurPos2 < nCnt1 - 1 );
			nDisArray2[nCurPos2] = nDisCur;
			nAddCnt2[nCurPos2]++;
			nCurPos2++;
		}

	}

	BOOL bGetRcDis = FALSE;
		
	int nPos = 0;
	int nMaxNum = nAddCnt[0];
	if( nMaxNum > 1 ) bGetRcDis = TRUE;
	for( i = 1; i < nCurPos1; i++ )
	{
		if( nAddCnt[i] > nMaxNum )
		{
			nPos = i;
			nMaxNum = nAddCnt[i];
			bGetRcDis = TRUE;
		}
		else if( nAddCnt[i] == nMaxNum )
		{
			int nRcDis1 = nDisArray[nPos] / (float)nAddCnt[nPos] + 0.5;
			int nRcDis2 = nDisArray[i] / (float)nAddCnt[i] + 0.5;
			float fDifRatio = abs(nRcDis1-nRcDis2) / (float)( max( max(nRcDis1,nRcDis2), 10 ) );
			//float fDifRatio = abs(nRcDis1-nRcDis2) / (float)( min(max( max(nRcDis1,nRcDis2), 15 ),5) );
			if( fDifRatio < 0.30f )
			{
				nDisArray[i] = (nDisArray[nPos] + nDisArray[i])/2 +0.5;
				nPos = i;
				nMaxNum = nAddCnt[i];
				bGetRcDis = TRUE;
			}
			else
			{
				bGetRcDis = FALSE;
			}

		}
	}

	if( bGetRcDis )
	{
		nRcDis = nDisArray[nPos] / (float)nAddCnt[nPos] + 0.5;
	}
	else
	{
		bGetRcDis = TRUE;
		nPos = 0;
		nMaxNum = nAddCnt2[0];
		for( i = 1; i < nCurPos2; i++ )
		{
			if( nAddCnt2[i] > nMaxNum )
			{
				nPos = i;
				nMaxNum = nAddCnt2[i];
				bGetRcDis = TRUE;
			}
			else if( nAddCnt2[i] == nMaxNum )
			{
				int nRcDis1 = nDisArray2[nPos] / (float)nAddCnt2[nPos] + 0.5;
				int nRcDis2 = nDisArray2[i] / (float)nAddCnt2[i] + 0.5;
				float fDifRatio = abs(nRcDis1-nRcDis2) / (float)( max( max(nRcDis1,nRcDis2), 10 ) );
				//float fDifRatio = abs(nRcDis1-nRcDis2) / (float)( min(max( max(nRcDis1,nRcDis2), 15 ),5) );
				if( fDifRatio < 0.30f )
				{
					nDisArray2[i] = (nDisArray2[nPos] + nDisArray2[i])/2 +0.5;
					nPos = i;
					nMaxNum = nAddCnt2[i];
					bGetRcDis = TRUE;
				}
				else
				{
					bGetRcDis = FALSE;
				}

			}
		}

		if( bGetRcDis )
		{
			nRcDis = nDisArray2[nPos] / (float)nAddCnt2[nPos] + 0.5;
			if( nRcDis == 0 ) nRcDis = 1;
		}
	}


	delete[] nAddCnt;
	delete[] nAddCnt2;
	delete[] nDisArray;
	delete[] nDisArray2;

	return bGetRcDis;	
}

BOOL ImageVerProj(IMAGE imgIn, CRect rcProc, int *pProj)
{
#ifdef CHECK_ERROR
	ASSERT( pProj != NULL );
#endif
	if( pProj == NULL )
	{
		return FALSE;
	}

	int w = rcProc.Width() + 1;
	int h = rcProc.Height() + 1;
	int w1 = ImageWidth(imgIn);
	int h1 = ImageHeight(imgIn);

#ifdef CHECK_ERROR
	ASSERT( rcProc.left >= 0 );
	ASSERT( rcProc.right < w1 );
	ASSERT( rcProc.top >= 0);
	ASSERT( rcProc.bottom < h1 );
#endif

	if( rcProc.left < 0 
		|| rcProc.right >= w1 
		|| rcProc.top < 0
		|| rcProc.bottom >= h1 )
	{
		return FALSE;
	}
	

	int i, j;
	memset( pProj, 0, sizeof(int) * w );

//	for( j=rcProc.left ; j<=rcProc.right; j++ )
//	{
//		int nIndex = j - rcProc.left;
//		for( i=rcProc.top ; i<=rcProc.bottom ; i++ )
//		{
//			if ( imgIn[i][j] > 0 )
//			{
//				pProj[nIndex]++;
//			}
//		}
// 	}

	for( i = rcProc.top; i <= rcProc.bottom; i++ )
	{
		for( j = rcProc.left; j <= rcProc.right; j++ )
		{
			int nId = j - rcProc.left;

			if( imgIn[i][j] > 0 )
			{
				pProj[nId]++;
			}
		}
	}

	return TRUE;
}
