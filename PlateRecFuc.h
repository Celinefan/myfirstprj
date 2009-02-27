// PlateRecFuc.h : General Plate Rec fucs
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(SCU_III_PLATERECFUC_INCLUDED_)
#define SCU_III_PLATERECFUC_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMGFUC.h"

const int WIDTH_CUT_EDGE = 5;
const int X_DISTANCE = 60;//85;
const int X_MINDISTANCE = 1;
const int MIN_PIX = 5;//10;

//const int PLATE_MIN_WIDTH = 80;
const int PLATE_MIN_WIDTH = 60;
const int PLATE_MIN_HEIGHT = 16;//20;//25;

const float MAX_PLATE_WHRATIO = 13.0f;
const float MIN_PLATE_WHRATIO = 1.0f;

const int NOISE_CHAR_WIDTH = 2;
const int MIN_CHAR_WIDTH = 15;//12;
const int MAX_CHAR_WIDTH = 50;//25;
const int MIN_CHAR_HEIGHT = 14;//;18;//20;//25;//20;//50;
const int MAX_CHAR_HEIGHT = 70;//40;//120;

const int MIN_TOP_CHAR_WIDTH = 10;
const int MAX_TOP_CHAR_WIDTH = 55;
const int MIN_TOP_CHAR_HEIGHT = 10;
const int MAX_TOP_CHAR_HEIGHT = 50;

const float CHAR_HW_RATIO1 = 1.6f;
const float CHAR_HW_RATIO_COMP1 = 0.4f;

const float CHAR_HW_RATIO_MAX = 2.5f;
const float CHAR_HW_RATIO_MIN = 1.5f;

const float CHAR_I_HW_RATIO_MIN = 3.0f;
const float TOP_CHAR_I_HW_RATIO_MIN = 1.5f;

const int NORM_CHAR_WIDTH = 17;//归一化字符宽度
const int NORM_CHAR_HEIGHT = 33;//归一化字符高度

typedef struct
{
	int y;
	int xStart;
	int xEnd;
	int lab;  //标号
} HORIZlINE;
typedef CArray<HORIZlINE,HORIZlINE> CHorizline;

PPRUNLENGTH GETRUNLENGTH_MEMCHECK( IMAGE img, IMAGE a, int obj, int& number, int neighbor=IP_NEIGHBORFOUR);
void FREERUNLENGTH_MEMCHECK( int num, PPRUNLENGTH runlen );
int GetRunLenAlloced();

void HoriEnhance(IMAGE InImg, IMAGE OutImg, int length, float fRatio );
void VerEnhance(IMAGE InImg, IMAGE OutImg, int length, float fRatio );
void VerEnhance_WholeAdjust(IMAGE InImg, IMAGE OutImg, int length, float fRatio );
void CheckVerEnhanceVal( IMAGE imgGray, IMAGE imgVer, int nThre );
void GetPerPlateRect(IMAGE InImg, CRect m_ProcessRect, ObjRectArray &m_RectArray, BOOL bEliminNoise = FALSE );
BOOL GetPerVerPlateRect(IMAGE InImg, CRect m_ProcessRect, ObjRectArray &m_RectArray, BOOL bEliminNoise = FALSE);
void GetAllRunLengthTest(CHorizline &lineArray, ObjRectArray &RectArray);
void GetAllVerRunLengthTest(CHorizline &lineArray, ObjRectArray &RectArray);
void SmoothEdge(CHorizline *ObjlineArray, int count, ObjRectArray& RectArray );
void SmoothEdge(CHorizline *ObjlineArray, int count, ObjRectArray& RectArray, float fRatio );
void SortDecline( int *list, int num );
BOOL MergeSplitPlateRcs( ObjRectArray &rcArray );
void EreaseNoiseRect(ObjRectArray &RectArray);
void EreaseNoiseVerRect(ObjRectArray &RectArray);

BOOL DajinBin(IMAGE imgIn, IMAGE imgOut, CRect RPlate);
BOOL DajinBin( IMAGE imgIn, IMAGE imgOut, CRect RPlate, float fRatio );//此函数需要拆分，分为阈值计算和二值化两部分
void LLTFast(IMAGE inImg, IMAGE outImg, int w , int ithreshold);
void GetObjRect(IMAGE &image, ObjRectArray &VObjArray);
void GetObjRect(IMAGE &image, ObjRectArray &VObjArray, ObjRectArray &AllRectArray);
void GetObjRectForTopView(IMAGE &image, ObjRectArray &VObjArray, ObjRectArray &AllRectArray);

void GetAverRcWH( ObjRectArray &rcArray, int &w, int &h );
void GetAverRcWHForTopView( ObjRectArray &rcArray, int &w, int &h );
BOOL GetRcDis( ObjRectArray &charArray, int &nRcDis );
BOOL GetRcDisForTopView( ObjRectArray &charArray, int &nRcDis );

BOOL ImageVerProj(IMAGE imgIn, CRect rcProc, int *pProj);


#endif // !defined(SCU_III_IMGFUC_INCLUDED_)