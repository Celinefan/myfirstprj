// Featurefusion.h: interface for the CFeaturefusion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEATUREFUSION_H__57141ADD_58BA_4584_81E1_91C7017E162F__INCLUDED_)
#define AFX_FEATUREFUSION_H__57141ADD_58BA_4584_81E1_91C7017E162F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include 	<math.h>
#include <afxtempl.h>
struct Sample
{
	int label;
	double *data;
};

typedef CArray<Sample,Sample&> CSampleArray;

struct BmpData
{
	int label;
	BYTE *pimage;
};

typedef CArray <BmpData,BmpData&> CBmpArray;

struct Sum{
	int label;
	float sum;
};

#define     M_PI			3.141592654f
#define     M_BMPSIZE		1025

#ifndef __GABORFILTER__
#define __GABORFILTER__
class GaborFilter
{
public:

	GaborFilter();
	~GaborFilter();
	
	void	Initialize( int y, int x, float a, float f, float s, float p = 0 );

	inline float 	GetReal( int y, int x ) { return mReal[y][x]; }
	inline float 	GetImaginary( int y, int x ) { return mImaginary[y][x]; }
	
protected:

	int			mYO;			// vertical origin
	int			mXO;			// horizontal origin
	int			mSizeY;			// vertical size of filter
	int			mSizeX;			// horizontal size of filter
	float		mSigma;			// curve of gaussian (sually set to PI)
	float		mAngle;			// orientation of filter (theta)
	float		mPhase;			// phase of filter (rho)
	float		mFrequency;		// wavelengths of filter (omega)
	float**		mReal;			// real part of filter
	float**		mImaginary;		// imaginary part of filter
};

#endif

//gaborjet类

#ifndef __GABORJET__
#define __GABORJET__

class GaborJet
{
public:

	GaborJet();
	~GaborJet();

	void	 Initialize( int height, int width,int gx,int gy, int sx,int sy,float f,float s);
	double*  Filter( BYTE* image);  
    void     ByteToFloat(float* imageout,BYTE *imagein,int count);

protected:
	double*	 Filter( float* image); 
	void Scale(double *responses);	

	float		    m_Image[M_BMPSIZE];
	int				mHeight;	// vertical size of image
	int				mWidth;		// horizontal size of image
	int				mSpacingY;	// vertical amount of pixels between subsequent GFs
	int				mSpacingX;	// horizontal amount of pixels between subsequent GFs
	float			mSigma;		// modulator for standard deviation sigma
	int				mAngles;	// number of orientations
	float			mFreqs;		// number of frequencies
	int				mSizeY;		// vertical size of filter
	int				mSizeX;		// horizontal size of filter
	int				mRespY;		// height of response matrix
	int				mRespX;		// width of response matrix
	GaborFilter*	mFilters;	// set of filters in use
	float*			mPixels;	// the pixel matrix to filter
	double*		   mResponses;	// the gabor filtered image

};

#endif



class CFeaturefusion  
{
public:
	//CProfile m_profile;
     GaborJet      m_charjet; 
	int	 m_chnmodelwidth;
    CSampleArray  m_charmodels;     //标准模板数据	
	int CharCongnize(IMAGE pimage,float *confidence, BOOL zimu, BOOL num,BOOL use_usermdl);
	CFeaturefusion();
	virtual ~CFeaturefusion();
	int m_Profile[200];
	void profile(IMAGE &Img);
	BOOL LoadModels(char *modelname,int modelwidth,CSampleArray &models);
	float GetRealCharConf(){return m_realcharconf;}
	float m_realcharconf;	  

};

class CECongnize
{
public:
      int			m_chnmodelwidth;

#ifdef CONF_ANA
	  float m_fMinDis;
#endif

	  CECongnize();
	  CECongnize( CString strDir );
	  ~CECongnize();
	  //汉字相关的


	  BOOL		ChnTrain(char *chntraindir,float mindistance,int maxclass,int maxcycle);
	  int	    ChnCongnize(BYTE *pimage,float *confidence,BOOL bMil,BOOL use_usermdl);
	  int       ChnGetModelCount() {return m_chnmodels.GetSize();}
	  CString   GetChnString(int chnpos) { return m_chn[chnpos];}
	      //汉字用户模板
      BOOL      AddUserMdl(BYTE *pImage,int charpos);
	  BOOL      DelAllUserMld();   //删除所有汉字用户模板
	  BOOL      DelOneUserMdl(int pos); //删除所有汉字用户模板中的第POS个模板	
	  int       GetUserMdlCount() { return m_usermodels.GetSize();}
	  CBmpArray &GetUserBMP()  { return m_userbmp;}
      CSampleArray  m_usermodels;   //用户模板数据
	  //0321 lt加入新字符
	  BOOL      AddNewChn(BYTE *pImage,CString chn);
	  CString    GetNewChn();
	  int m_newchnpos;
	  int m_basechnpos;		  
	  //字符相关的
	  BOOL		CharTrain(char *chartraindir,float mindistance,int maxclass,int maxcycle);
	  int		CharCongnize(BYTE *pimage,float *confidence, BOOL zimu, BOOL num,BOOL use_usermdl);
	  int		CharCongnize(BYTE *pimage,float *confidence);//最直接的CharCongnize
	  int		CharCongnize(BYTE *pimage,float *confidence,BOOL bNums);//字母、数字分开识别
	  int       CharGetModelCount(){ return m_charmodels.GetSize();}
	  CString   GetCharString(int charpos) { return m_char[charpos];}
	      //字符用户模板
	  BOOL      AddUCharMdl(BYTE *pImage,int charpos);
	  BOOL      DelAllUCharMld();   //删除所有用户字符用户模板
	  BOOL      DelOneUCharMdl(int pos); //删除所有用户字符模板中的第POS个模板	
	  int       GetUCharMdlCount() { return m_ucharmodels.GetSize();}
	  CBmpArray &GetUCharBMP()  { return m_ucharbmp;} 
	CBmpArray     m_ucharbmp;
	//lt add 041209 得到真实置信度的
	float GetRealCharConf(){return m_realcharconf;}
	  GaborJet      m_chnjet;  
private:
	 
	//汉字相关的
	 
	  CStringArray  m_chn;
	  CSampleArray  m_chnmodels;    //标准模板数据
	      //用户模板
//	  CSampleArray  m_usermodels;   //用户模板数据
	  CBmpArray     m_userbmp;      //用户模板位图  用来显示的
	  //字符相关的
	  GaborJet      m_charjet; 
	  int			m_charmodelwidth;
	  CStringArray  m_char;
	  CSampleArray  m_charmodels;     //标准模板数据	
	      //用户模板
	  CSampleArray  m_ucharmodels;
//	  CBmpArray     m_ucharbmp;
	  //041209 lt add 得到真实置信度的
	  float m_realcharconf;	  
protected:
	 //以下是内部共用函数  减少代码 方便维护
	  BOOL LoadModels(char *modelname,int modelwidth,CSampleArray &models);
	  BOOL SaveModels(char *modelname,int modelwidth,CSampleArray &models);
	  BOOL SaveUserBmp(char *modelname,CBmpArray &userbmp,CStringArray &charstring);
	  BOOL LoadUserBmp(char *modelname,CBmpArray &userbmp,CSampleArray &models);
	  BOOL Train(char *traindir,float mindistance,int maxclass,int maxcycle,int modelwidth,CSampleArray &models,CStringArray &basechar,GaborJet *myjet);
};

#endif // !defined(AFX_FEATUREFUSION_H__57141ADD_58BA_4584_81E1_91C7017E162F__INCLUDED_)
