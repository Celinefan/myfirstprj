// NumRec_C.h: C interface for the Container Number Recognizer
//
//////////////////////////////////////////////////////////////////////
typedef struct _tagConNumInfo{
	char ConNum[16];//store the recognition result
	long ABCRegion[4];//the first 4 letters' region
	long NumRegion[4];//the latter 7 numbers' region
	BOOL bJudge;//a coarse judgment of the recognition result according to the check character
} CONNUMINFO, *LPCONNUMINFO;

extern "C" __declspec(dllexport) int __stdcall RecConNum( LPBYTE pImg, int nW, int nH, LPCONNUMINFO pInfo);
//pImg should contain 3 color fields, i.e. RGB Color Fields
//nW, nH are the image width and height respectively 
//pImg should be stored as : 
//R[0][0], G[0][0], B[0][0], ... , R[0][nW-1], G[0][nW-1], B[0][nW-1]
//...................................................................
//R[nH-1][0], G[nH-1][0], B[nH-1][0], ... , R[nH-1][nW-1], G[nH-1][nW-1], B[nH-1][nW-1]
//i.e. the size of pImg should be nW * nH * 3 Byte
//
//The last para -- pInfo will return the recognition result : 
//Normally, pInfo->ConNum will contain 11 characters, i.e. 4 letters and 7 letters,
//if pInfo->ConNum = "error", the dll is expired
//if pInfo->ConNum = "", the recognizer doesn't recognize anything.
//
//pInfo->ABCRegion[4], the left, top, right, bottom of the 4 letters' region.
//Sometimes all the value will be 0, this is because the 4 letter's region is included in the 7 numbers' region.
//
//pInfo->NumRegion[4], the left, top, right, bottom of the 7 numbers' region.
//Sometimes this region will also contain the 4 letters' region.
//
//pInfo->bJudge, the dll will check the 11 characters' recognition result by the final check character.
//
//if( pInfo->ConNum != "error" || pInfo->ConNum != "" ) this function will return 1;
//else this function will return 0;