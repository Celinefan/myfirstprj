// Featurefusion.cpp: implementation of the CFeaturefusion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Featurefusion.h"
#include    <fstream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GaborFilter::GaborFilter()
{
	mReal = NULL;
	mImaginary = NULL;
}


// destructor
GaborFilter::~GaborFilter()
{
	int i;
	
	// free up memory
	if ( mReal != NULL )
	{
		for ( i = 0; i < mSizeY; i++ ) 
			delete[] mReal[i];
		delete[] mReal;
	}
	if ( mImaginary != NULL )
	{
		for ( i = 0; i < mSizeY; i++ ) delete[] mImaginary[i];
		delete[] mImaginary;
	}
}

// set up the filter
void GaborFilter::Initialize( int sizey, int sizex, float a, float f, float s, float p )
{
	float x, y, exponential, sincos;
	
// set internal variables
	mSizeY = sizey;
	mSizeX = sizex;
	mSigma = s;
	mAngle = a;
	mPhase = p;
	mFrequency = float(f * M_PI / 2.0);

// find origin of filter
	mYO = mSizeY / 2;
	mXO = mSizeX / 2;
	
// allocate memory for filter
	mReal 	   = new float*[mSizeY];		// real part of filter
	mImaginary = new float*[mSizeY];		// imaginary part of filter

// initialize filter values
	for ( int i = 0; i < mSizeY; i++ )
	{
		mReal[i] 	  = new float[mSizeX];
		mImaginary[i] = new float[mSizeX];
		
		for ( int j = 0; j < mSizeX; j++ )
		{
		// offset from origin
			y = (float)( i - mYO );
			x = (float)( j - mXO );
		// calculate exponential part
			exponential = float(exp( - ( x*x + y*y ) / mSigma ));
		// calculate sin-cos sum
			sincos = float(mFrequency * ( y * cos( mAngle ) - x * sin( mAngle ) ));
			mReal[i][j] 	 = float(exponential * sin( sincos ));
			mImaginary[i][j] = float(exponential * ( cos( sincos ) - exp((-1.0*M_PI*M_PI)/2.0) ));
		}
	}
}


/*
	Description:	Implementation for GaborJet class
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
*/


// default constructor just sets everything to default
GaborJet::GaborJet()
{
	mHeight 	= 0;
	mWidth 		= 0;
	mFilters 	= NULL;
	mPixels		= NULL;
	mResponses	= NULL;
}


// destructor: free up memory
GaborJet::~GaborJet()
{

	if ( mFilters != NULL )
	{
		delete[] mFilters;
	}	

	if ( mResponses != NULL )
	{
		delete[] mResponses;
	}

}

void GaborJet::Initialize( int height, int width, int gx,int gy, int sx,int sy,float f,float s)
{

	if( (mHeight == height)&&(mWidth == width) )
			return;	
	mHeight 	= height;
	mWidth 		= width;
	mSizeX		= gx;
	mSizeY		= gy;	
	mSpacingX 	= sx;
	mSpacingY 	= sy;
	mFreqs 		= f;
	mSigma		= s * M_PI * M_PI;
	mAngles 	= 4;	
	int		i;
	float	angle;	
// allocate memory for filters
	mFilters = new GaborFilter[mAngles]; // angles * freqs = total filters
	for ( i = 0; i < mAngles; i++ )
	{
	// calculate angle
		angle = (float)i * M_PI / (float)mAngles;
	// initialize each one			
		mFilters[i].Initialize( mSizeY, mSizeX, angle, mFreqs, mSigma );	
	}
	
// allocate memory for the responses
	mRespY = (mHeight - mSizeY)/mSpacingY+1;
	mRespX = (mWidth - mSizeX) /mSpacingX+1;

	mResponses = new double[mAngles * mRespY * mRespX];
}

// process an image
double *GaborJet::Filter( float* image)
{	
	int			rx, ry;		// iterating over mResponses
	int			x, y;		// iterating over location
	int			gx, gy;		// iterating over filters
	int			a;		// iterating over angles and frequencies
	int			i, j;		// iterating over filter field
	float		local_sumI, 
				local_sumR;	// sum of imaginary and of real parts  实部和虚部

	mPixels = image;

	
// start collecting responses
	
	for ( a = 0; a < mAngles; a++ )
	{
		y = 0;
		for ( ry = 0; ry < mRespY; ry++ )
		{
			x = 0;
			for ( rx = 0; rx < mRespX; rx++ )
			{
				local_sumI = 0.0;
				local_sumR = 0.0;

				int ypos;
				for ( gy = y; gy < y + mSizeY; gy++ )
				{
					ypos = gy*mWidth;                  
					for ( gx = x; gx < x + mSizeX; gx++ )
					{
						// make sure we are not out of bounds
						if ( gx > mWidth || gy > mHeight ) break;
						// offset to local coordinates of filter
						i = gy - y;
						j = gx - x;
						// get real and imaginary products						
						local_sumR += mPixels[ypos + gx] * mFilters[a].GetReal(i,j);
						local_sumI += mPixels[ypos + gx] * mFilters[a].GetImaginary(i,j);

					}
				}				
				mResponses[a*mRespY*mRespX+ry*mRespX+rx] = sqrt( local_sumR*local_sumR + local_sumI*local_sumI);
				x = x + mSpacingX;
			}	// rx
			y = y + mSpacingY;
		}// ry
	}//a
	Scale(mResponses);
	return mResponses;
}

double *GaborJet::Filter(BYTE* image)
{
	ByteToFloat(m_Image,image,M_BMPSIZE);
    return Filter(m_Image);
}

void GaborJet::ByteToFloat(float* imageout,BYTE *imagein,int count)
{

	for(int i=0; i<count; i++)
	{		
		imageout[i] = imagein[i];	
	}
	return;
}

void GaborJet::Scale(double *responses)
{
    int count = mAngles * mRespX * mRespY;
	double minr,maxr,maxmin,half;
	minr = maxr = responses[0];
	
	for(int i = 0; i < count; i++ )
	{
		if( responses[i] > maxr ) maxr = responses[i];
		if( responses[i] < minr ) minr = responses[i];
	}
	maxmin = maxr - minr;
    half = maxmin/2;
	if(maxmin == 0)
		maxmin = 1;
    for(i=0; i<count; i++)
	{
        responses[i] = (responses[i]-half) /half;
	}	
    return;
}
CFeaturefusion::CFeaturefusion()
{
	LoadModels("feature_char.dat",140+80,m_charmodels);

}

CFeaturefusion::~CFeaturefusion()
{

}
int CFeaturefusion::CharCongnize(IMAGE pimage,float *confidence, BOOL zimu, BOOL num,BOOL use_usermdl)
{ 
	double *resp1;
	resp1 = m_charjet.Filter(pimage[0]);
	double *resp;
	resp = new double[140+80];
	int modelcount,usermodelcount;
	int count = 0; //保存实际使用的模板数量  也就是SUM 实际大小
    
    modelcount = m_charmodels.GetSize();
	profile(pimage);
	for(int i = 0; i < 140 ;i++)//gabor
	{
		resp[i] =resp1[i];
	}
	for(i = 80 ; i < 140;i++)//profile
	{
		resp[i] =double(m_Profile[i]);

	}


	Sum *mysum;
	float he,cha;
	
	mysum = new Sum[modelcount ];
   
   //求欧式距离
	for( i=0; i<modelcount; i++)		
	{		
		if(( !num ) && ( m_charmodels[i].label < 20))  //字母排在后边//两个模板
			continue;
		if(( !zimu )  && ( m_charmodels[i].label >= 20))				
			break;	
		
		he = 0.0f;
		for(int j=0; j<140+80; j++)
		{
			cha = float(m_charmodels[i].data[j] - resp[j]);
			he += cha * cha;				
		}
		mysum[count].sum  =  float(sqrt(he));
		mysum[count].label = m_charmodels[i].label;
		count ++;
	}
		//使用用户模板
	
	if(( usermodelcount > 0 )  &&  ( use_usermdl ))
	{
		for(int i=0; i<usermodelcount; i++)		
		{
			mysum[count].sum = 0.0f;
			if(( !num ) && ( m_charmodels[i].label < 20))  //字母排在后边
				continue;
			if(( !zimu )  && ( m_charmodels[i].label >= 20))				
				continue;	
			
			he = 0.0f;
			for(int j=0; j<140+80; j++)
			{
				cha = float(m_charmodels[i].data[j] - resp[j]);
				he += cha * cha;				
			}
			mysum[count].sum  =  float(sqrt(he));	
			
            mysum[count].label = m_charmodels[i].label;
			count ++;
		}	
	}
    //对sum找最小值
	float min;
	int charpos;	
	min = mysum[0].sum;	
	charpos = mysum[0].label;
	for(i=1; i<count; i++)
	{	
		if( mysum[i].sum < min ) 
		{
			min = mysum[i].sum;				
			charpos = mysum[i].label;
		}
	}
	//找次小值且label与最小值不同的
    float min2;
    min2 = 1000;  //定义一个很大的值  后边循环就好找
    for(i=0; i<count; i++)
	{

		if( mysum[i].label == charpos)
		    continue;
		
		if(zimu && num)/////容易识别错误
		{
			//两个模板  SO /2
			if(charpos/3 == 2 && mysum[i].label/3 == 35)//2 Z
				continue;
			if(charpos/3 == 35 && mysum[i].label/3 == 2)//z.2
				continue;
			if(charpos/3 == 0 && mysum[i].label/3 == 13)//oD
				continue;
			if(charpos/3 == 13 && mysum[i].label/3 == 0)//do
				continue;
			if(charpos/3 == 8 && mysum[i].label/3== 11)//8 b
				continue;
			if(charpos/3 == 11 && mysum[i].label/3 == 8)//b 8
				continue;
			if(charpos/3 == 5 && mysum[i].label/3== 28)//5.s
				continue;
			if(charpos/3 == 28  && mysum[i].label/3 == 5)//s,5
				continue;
		}	

		if(mysum[i].sum < min2)
            min2 = mysum[i].sum;
			
	}
	m_realcharconf = *confidence = 1.0f - min/(min2);
	//置信度映射	
   // *confidence = 1.0f - min/(min2);
		//置信度映射	
	if(*confidence > 0.4)
		*confidence = 1;
	else if(*confidence >0.2)
		*confidence = *confidence * 0.25f + 0.9f;
	else if(*confidence > 0.05)
		*confidence = *confidence + 0.75f;
	else
        *confidence = *confidence *0.775f + 0.5f;	
	//
    delete []mysum;

	//yang 

	charpos = charpos/3;//两个模板
	return charpos;
}
void CFeaturefusion::profile(IMAGE &Img) 
{
	// TODO: Add your control notification handler code here
	int nWidth = ImageWidth(Img);
	int nHeight = ImageHeight(Img);
//	  int vicemin;
	
	  int i,j,k = -2;
	  for(j=0;j<nHeight;j++)
		  for(i=0;i<nWidth;i++)
		  {
			  if(Img[j][i] > 99)
				  Img[j][i] = 255;
			  else
				  Img[j][i] = 0;
		  }
      
	

	
	  for(i=0;i<200;i++)
	  {
		  m_Profile[i] = 0;
	  }
	  BOOL find1;
	  BOOL find2;
	  
       //from left to right
         for(j=0;j<nHeight;j++)
		 { 
			k += 2;
		    find1 = false;
	       // endfind = false;
	        find2 = false;

		       for(i=0;i<nWidth-1;i++)
			   {
				  
			      if(Img[j][i] == 255 && !(find1))
				  {
			          m_Profile[k] =i;
			          find1 = true;
				  }
			      else if((!find1) && (i == nWidth-2))
				  {
				      m_Profile[k] = nWidth - 1 ;
				      m_Profile[k+1] = nWidth - 1;//!!!need further consideration
				      //endfind = true;
				      break;
				  }
			     if (find1 && (!find2))
				 {
				     if((Img[j][ i] == 0) && (Img[j][i+1] == 255))
					 {
				      m_Profile[k+1] = i + 1;
					  find2 = true;
					 // endfind = true;
					  break;
					 }
				    else if((!find2) && (i == nWidth-2))	  
					{
					  m_Profile[k+1] = nWidth - 1;
					 // endfind = true;
					}
				 }
			   }
		 }

        //from right to left
        for(j=0;j<nHeight;j++)
		 { 
			k += 2;
		    find1 = false;
	        find2 = false;

		       for(i=nWidth-1;i>0;i--)
			   {
				  
			      if(Img[j][i] == 255 && !(find1))
				  {
			          m_Profile[k] =i;
			          find1 = true;
			      }
			      else if((!find1) && (i == 1))
				  {
				      m_Profile[k] = 0 ;
				      m_Profile[k+1] = 0;//!!!need further consideration
				      break;
				  }
			     if (find1 && (!find2))
				 {
				     if((Img[j][ i] == 0) && (Img[j][i-1] == 255))
					 {
				      m_Profile[k+1] = i-1;
					  find2 = true;
					  break;
					 }
				    else if((!find2) && (i == 1))	  
					{
					  m_Profile[k+1] = 0;
					}
				 }
			   }
		 }

         //from up to down
	     for(i=0;i<nWidth;i++)
		 { 
			k += 2;
		    find1 = false;
	        find2 = false;

		       for(j=0;j<nHeight-1;j++)
			   {
				  
			      if(Img[j][i] == 255 && !(find1))
				  {
			          m_Profile[k] = j;
			          find1 = true;
			      }
			      else if((!find1) && (j == nHeight-2))
				  {
				      m_Profile[k] = nHeight - 1 ;
				      m_Profile[k+1] = nHeight - 1;//!!!need further consideration
				      break;
				  }
			     if (find1 && (!find2))
				 {
				     if((Img[j][i] == 0) && (Img[j+1][i] == 255))
					 {
				      m_Profile[k+1] = j+1;
					  find2 = true;
					  break;
					 }
				    else if((!find2) && (j == nHeight - 2))	  
					{
					  m_Profile[k+1] = nHeight -1;
					}
				 }
			   }
		 }

		 //from down to up
		 for(i=0;i<nWidth;i++)
		 { 
			k += 2;
		    find1 = false;
	        find2 = false;

		       for(j=nHeight-1;j>0;j--)
			   {
				  
			      if(Img[j][i] == 255 && !(find1))
				  {
			          m_Profile[k] = j;
			          find1 = true;
			      }
			      else if((!find1) && (j == 1))
				  {
				      m_Profile[k] = 0 ;
				      m_Profile[k+1] = 0;//!!!need further consideration
				      break;
				  }
			     if (find1 && (!find2))
				 {
				     if((Img[j][i] == 0) && (Img[j-1][i] == 255))
					 {
				      m_Profile[k+1] = j-1;
					  find2 = true;
					  break;
					 }
				    else if((!find2) && (j == 1))	  
					{
					  m_Profile[k+1] = 0;
					}
				 }
			   }
		 }


}
BOOL CFeaturefusion::LoadModels(char *modelname,int modelwidth,CSampleArray &models)
{
	for(int i=0; i<models.GetSize(); i++)
		delete []models[i].data;		
	int count;
	ifstream in(modelname);
    in >> count;
    if(in.fail())
	{
		//AfxMessageBox("模板文件读取错误，请先训练");
		return FALSE;
	}
	Sample tempsample;
	for(i=0; i<count; i++)
	{
        in >> tempsample.label;
		tempsample.data = new double[modelwidth];
		for(int l=0; l<modelwidth; l++)
			in >> tempsample.data[l];
		models.Add(tempsample);
		
	}
    in.close();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//CECongnize开始

CECongnize::CECongnize()
{
	//汉字
	m_chn.Add("川");//00
	m_chn.Add("云");//01
	m_chn.Add("贵");//02
	m_chn.Add("京");//03
	m_chn.Add("渝");//04
	m_chn.Add("藏");//05
	m_chn.Add("桂");//06
	m_chn.Add("苏");//07
	m_chn.Add("粤");//08
	m_chn.Add("湘");//09
	m_chn.Add("浙");//10
	m_chn.Add("新");//11
	m_chn.Add("甘");//12
	m_chn.Add("吉");//13
	m_chn.Add("皖");//14
	m_chn.Add("闽");//15
	m_chn.Add("沪");//16
	m_chn.Add("蒙");//17
	m_chn.Add("琼");//18
	m_chn.Add("鲁");//19
	m_chn.Add("津");//20
	m_chn.Add("陕");//21
	m_chn.Add("豫");//22
    m_chn.Add("冀");//23
	m_chn.Add("黑");//24
	m_chn.Add("晋");//25
	m_chn.Add("辽");//26
	m_chn.Add("鄂");//27
	m_chn.Add("宁");//28
	m_chn.Add("青");//29
	m_chn.Add("赣");//30
	
	m_chn.Add("甲");//1
	m_chn.Add("乙");//2
	m_chn.Add("丙");//3
	m_chn.Add("丁");//4
	m_chn.Add("戊");//5
	m_chn.Add("已");//6
	m_chn.Add("庚");//7
	m_chn.Add("辛");//8
	m_chn.Add("壬");//9
	m_chn.Add("癸");//10
	m_chn.Add("子");//11
	m_chn.Add("丑");//12
	m_chn.Add("寅");//13
	m_chn.Add("卯");//14
    m_chn.Add("辰");//15
	m_chn.Add("巳");//16
	m_chn.Add("午");//17
	m_chn.Add("未");//18
	m_chn.Add("申");//19
	m_chn.Add("酉");//20
	m_chn.Add("戌");//21
	m_chn.Add("亥");//22

	m_newchnpos = m_chn.GetSize();  //lt add 0321
	m_basechnpos = m_newchnpos;
	//字符
	CString str;
	for(int c=0; c<36; c++)
	{
		if(c < 10)
			str.Format("%d",c);
		else
		    str.Format("%c",c+55);
		m_char.Add(str);
	}
	//初始化GABORJET
//	m_chnjet.Initialize (33, 17, 9, 9, 2, 4, 0.66667f, 1.8f);
//	m_charjet.Initialize(33, 17, 9, 9, 2, 4, 0.66667f, 1.8f);
//	int mRespX = (17 - 9) /2+1;
// 	int mRespY = (33 - 9)/4+1;
	m_chnjet.Initialize (33+9-1, 17+9-1, 9, 9, 2, 4, 0.66667f, 1.8f);
	m_charjet.Initialize(33+9-1, 17+9-1, 9, 9, 2, 4, 0.66667f, 1.8f);
	int mRespX = (17 - 1) / 2 + 1;
	int mRespY = (33 - 1) / 4 + 1;
	m_chnmodelwidth = 4 * mRespX * mRespY;
	m_charmodelwidth = 4 * mRespX * mRespY;
	//读入用户模板	
    LoadUserBmp("data\\userchn.dat",m_userbmp,m_usermodels);
	LoadUserBmp("data\\userchars.dat",m_ucharbmp,m_ucharmodels);
	//读入汉字模板
    LoadModels("data\\chn.dat",m_chnmodelwidth,m_chnmodels);
	//读入字符模板
	LoadModels("data\\char.dat",m_charmodelwidth,m_charmodels);
}


CECongnize::CECongnize(CString strMDir)
{
	//汉字
	m_chn.Add("川");//00
	m_chn.Add("云");//01
	m_chn.Add("贵");//02
	m_chn.Add("京");//03
	m_chn.Add("渝");//04
	m_chn.Add("藏");//05
	m_chn.Add("桂");//06
	m_chn.Add("苏");//07
	m_chn.Add("粤");//08
	m_chn.Add("湘");//09
	m_chn.Add("浙");//10
	m_chn.Add("新");//11
	m_chn.Add("甘");//12
	m_chn.Add("吉");//13
	m_chn.Add("皖");//14
	m_chn.Add("闽");//15
	m_chn.Add("沪");//16
	m_chn.Add("蒙");//17
	m_chn.Add("琼");//18
	m_chn.Add("鲁");//19
	m_chn.Add("津");//20
	m_chn.Add("陕");//21
	m_chn.Add("豫");//22
    m_chn.Add("冀");//23
	m_chn.Add("黑");//24
	m_chn.Add("晋");//25
	m_chn.Add("辽");//26
	m_chn.Add("鄂");//27
	m_chn.Add("宁");//28
	m_chn.Add("青");//29
	m_chn.Add("赣");//30
	
	m_chn.Add("甲");//1
	m_chn.Add("乙");//2
	m_chn.Add("丙");//3
	m_chn.Add("丁");//4
	m_chn.Add("戊");//5
	m_chn.Add("已");//6
	m_chn.Add("庚");//7
	m_chn.Add("辛");//8
	m_chn.Add("壬");//9
	m_chn.Add("癸");//10
	m_chn.Add("子");//11
	m_chn.Add("丑");//12
	m_chn.Add("寅");//13
	m_chn.Add("卯");//14
    m_chn.Add("辰");//15
	m_chn.Add("巳");//16
	m_chn.Add("午");//17
	m_chn.Add("未");//18
	m_chn.Add("申");//19
	m_chn.Add("酉");//20
	m_chn.Add("戌");//21
	m_chn.Add("亥");//22

	m_newchnpos = m_chn.GetSize();  //lt add 0321
	m_basechnpos = m_newchnpos;
	//字符
	CString str;
	for(int c=0; c<36; c++)
	{
		if(c < 10)
			str.Format("%d",c);
		else
		    str.Format("%c",c+55);
		m_char.Add(str);
	}
	//初始化GABORJET
//	m_chnjet.Initialize (33, 17, 9, 9, 2, 4, 0.66667f, 1.8f);
//	m_charjet.Initialize(33, 17, 9, 9, 2, 4, 0.66667f, 1.8f);
//	int mRespX = (17 - 9) /2+1;
//	int mRespY = (33 - 9)/4+1;
	m_chnjet.Initialize (33+9-1, 17+9-1, 9, 9, 2, 4, 0.66667f, 1.8f);
	m_charjet.Initialize(33+9-1, 17+9-1, 9, 9, 2, 4, 0.66667f, 1.8f);
	int mRespX = (17 - 1) / 2 + 1;
	int mRespY = (33 - 1) / 4 + 1;
	m_chnmodelwidth = 4 * mRespX * mRespY;
	m_charmodelwidth = 4 * mRespX * mRespY;

	CString strFilePath;
	//读入用户模板
	strFilePath.Format( "%s\\userchn.dat", strMDir );
    LoadUserBmp( (char*)(LPCSTR(strFilePath)) , m_userbmp , m_usermodels );

	strFilePath.Format( "%s\\userchars.dat", strMDir );
	LoadUserBmp( (char*)(LPCSTR(strFilePath)) , m_ucharbmp , m_ucharmodels);

	//读入汉字模板
	strFilePath.Format( "%s\\chn.dat", strMDir );
    LoadModels( (char*)(LPCSTR(strFilePath)) ,m_chnmodelwidth,m_chnmodels);
	//读入字符模板
	strFilePath.Format( "%s\\char.dat" , strMDir );
	LoadModels( (char*)(LPCSTR(strFilePath)) , m_charmodelwidth , m_charmodels );
}



CECongnize::~CECongnize()
{	
	for(int i=0; i<m_chnmodels.GetSize(); i++)
		delete []m_chnmodels[i].data;
	for(i=0; i<m_charmodels.GetSize(); i++)
		delete []m_charmodels[i].data;
	for(i=0; i<m_usermodels.GetSize(); i++)
		delete []m_usermodels[i].data;
	for(i=0; i<m_userbmp.GetSize(); i++)
		delete []m_userbmp[i].pimage;


}

//汉字识别的相关类
BOOL CECongnize::ChnTrain(char *chntraindir,float mindistance,int maxclass,int maxcycle)
{
		// TODO: Add your control notification handler code here
    Train(chntraindir,mindistance,maxclass,maxcycle,m_chnmodelwidth,m_chnmodels,m_chn,&m_chnjet);
	return SaveModels("data\\chn.dat",m_chnmodelwidth,m_chnmodels);
}

int CECongnize::ChnCongnize(BYTE *pimage,float *confidence, BOOL bMil, BOOL use_usermdl)
{ 
	double *resp;
	resp = m_chnjet.Filter(pimage);
	//军车排在后边
	int modelcount,usermodelcount;
	int count = 0; //保存实际使用的模板数量  也就是SUM 实际大小
    
    modelcount = m_chnmodels.GetSize();
	usermodelcount = m_usermodels.GetSize();

	Sum *mysum;
	float he,cha;
	 
	mysum = new Sum[modelcount + usermodelcount];

	for(int i=0; i<modelcount; i++)
	{
		if((!bMil)&&(m_chnmodels[i].label >= 31))
			break;
			/*
			mysum[count].sum = 0.0f;
			for(int j=0; j<m_chnmodelwidth; j++)
			{
			mysum[count].sum += float((m_chnmodels[i].data[j] - resp[j])*(m_chnmodels[i].data[j] - resp[j]));				
			}
		mysum[count].sum  =  float(sqrt(mysum[count].sum));*/
		//lt  modify 041030
		he = 0.0f;
		for(int j=0; j<m_chnmodelwidth; j++)
		{
			cha = float(m_chnmodels[i].data[j] - resp[j]);
			he += cha * cha;				
		}
		mysum[count].sum  =  float(sqrt(he));
		mysum[count].label = m_chnmodels[i].label;
		count++;
	}
	
	//
	//使用用户模板
	
	if(( usermodelcount > 0 )  &&  ( use_usermdl ))
	{
		for(int i=0; i<usermodelcount; i++)		
		{
			mysum[count].sum = 0.0f;
			if((!bMil)&&(m_usermodels[i].label >= 31 && m_usermodels[i].label<=52))
				continue;
				/*
				for(int j=0; j<m_chnmodelwidth; j++)
				{
				mysum[count].sum += float((m_usermodels[i].data[j] - resp[j])*(m_usermodels[i].data[j] - resp[j]));				
				}
			mysum[count].sum  =  float(sqrt(mysum[count].sum));	*/
			//lt  modify 041030
			he = 0.0f;
			for(int j=0; j<m_chnmodelwidth; j++)
			{
				cha = float(m_usermodels[i].data[j] - resp[j]);
				he += cha * cha;				
			}
			mysum[count].sum  =  float(sqrt(he));	
            mysum[count].label = m_usermodels[i].label;
			count ++;
		}	
	}
	
	//求置信度  以及最小距离

	float min;
	int chnpos;	
	min = mysum[0].sum;	
	chnpos = mysum[0].label;
	for(i=1; i<count; i++)
	{	
		if( mysum[i].sum < min ) 
		{
			min = mysum[i].sum;				
			chnpos = mysum[i].label;
		}
	}
	//找次小值且label与最小值不同的
    float min2;
    min2 = 1000;  //定义一个很大的值  后边循环就好找
    for(i=0; i<count; i++)
	{
		if( mysum[i].label == chnpos)
		    continue;
		if(mysum[i].sum < min2)
            min2 = mysum[i].sum;
			
	}
    *confidence = 1.0f - min/(min2);
	//置信度映射	

	if(*confidence > 0.25)
		*confidence = 1;
	else if(*confidence >0.2)
		*confidence = 0.98f;
	else if(*confidence > 0.15)
		*confidence = 0.91f;
	else if(*confidence > 0.10)
		*confidence = 0.81024f;
	else if(*confidence > 0.05)
		*confidence = 0.6f;
	else
        *confidence = 0.5f;
	//
	delete []mysum;
	return chnpos;
}

CString CECongnize::GetNewChn()
{
	if(m_chn.GetSize()>m_newchnpos)
	{
		m_newchnpos++;
		return m_chn[m_newchnpos-1];
	}
	else
	{
		m_newchnpos = m_basechnpos;
		return "END";
	}
}


BOOL CECongnize::AddNewChn(BYTE *pImage,CString chn)
{
	int x;
	x=m_chn.GetSize();			
	m_chn.Add(chn);
	AddUserMdl(pImage,x);	
    return TRUE;
}



BOOL CECongnize::AddUserMdl(BYTE *pImage,int charpos)
{

    BmpData tempbmp;
    Sample tempsample;
	double *resp;
	tempbmp.pimage = new BYTE[M_BMPSIZE];
	for(int l=0; l<M_BMPSIZE; l++)
	{
		tempbmp.pimage[l] = pImage[l];
	}
	tempbmp.label = charpos;	
	m_userbmp.Add(tempbmp);
	
	tempsample.data = new double[m_chnmodelwidth];
	tempsample.label = charpos;
	resp = m_chnjet.Filter(tempbmp.pimage);
	
	for(l=0; l<m_chnmodelwidth; l++)
		tempsample.data[l] = resp[l];

	m_usermodels.Add(tempsample);
	//保存用户模板位图
	SaveUserBmp("data\\userchn.dat",m_userbmp,m_chn);
	return TRUE;
}

BOOL CECongnize::DelOneUserMdl(int pos)
{
	if((pos > m_userbmp.GetSize())||( pos < 0))
		return FALSE;
    delete []m_usermodels[pos].data;
	delete []m_userbmp[pos].pimage;
	m_usermodels.RemoveAt(pos);
	m_userbmp.RemoveAt(pos);
	SaveUserBmp("data\\userchn.dat",m_userbmp,m_chn);
	return TRUE;
}

BOOL CECongnize::DelAllUserMld()
{
	
	for(int i=0; i<m_usermodels.GetSize(); i++)
		delete []m_usermodels[i].data;
	for(i=0; i<m_userbmp.GetSize(); i++)
		delete []m_userbmp[i].pimage;
	m_usermodels.RemoveAll();
	m_userbmp.RemoveAll();
	SaveUserBmp("data\\userchn.dat",m_userbmp,m_chn);
	return TRUE;
}
//
//字符识别的相关函数

BOOL CECongnize::CharTrain(char *chartraindir,float mindistance,int maxclass,int maxcycle)
{
    Train(chartraindir,mindistance,maxclass,maxcycle,m_charmodelwidth,m_charmodels,m_char,&m_charjet);
	return SaveModels("data\\char.dat",m_charmodelwidth,m_charmodels);
}

int CECongnize::CharCongnize(BYTE *pimage,float *confidence, BOOL zimu, BOOL num,BOOL use_usermdl)
{ 
#ifdef CONF_ANA
	m_fMinDis = 0.0;
#endif

	
	double *resp;
	resp = m_charjet.Filter(pimage);
	int modelcount,usermodelcount;
	int count = 0; //保存实际使用的模板数量  也就是SUM 实际大小
    
    modelcount = m_charmodels.GetSize();
	usermodelcount = m_ucharmodels.GetSize();

	Sum *mysum;
	float he,cha;
	
	mysum = new Sum[modelcount + usermodelcount];
   
   //求欧式距离
	for(int i=0; i<modelcount; i++)		
	{		
		if(( !num ) && ( m_charmodels[i].label < 10))  //字母排在后边
			continue;
		if(( !zimu )  && ( m_charmodels[i].label >= 10))				
			break;	
			/*
			mysum[count].sum = 0.0f;
			for(int j=0; j<m_charmodelwidth; j++)
			{
			mysum[count].sum += float((m_charmodels[i].data[j] - resp[j])*(m_charmodels[i].data[j] - resp[j]));				
			}
		mysum[count].sum  =  float(sqrt(mysum[count].sum));*/
		//lt  modify 041030
		he = 0.0f;
		for(int j=0; j<m_charmodelwidth; j++)
		{
			cha = float(m_charmodels[i].data[j] - resp[j]);
			he += cha * cha;				
		}
		mysum[count].sum  =  float(sqrt(he));
		mysum[count].label = m_charmodels[i].label;
		count ++;
	}
		//使用用户模板
	
	if(( usermodelcount > 0 )  &&  ( use_usermdl ))
	{
		for(int i=0; i<usermodelcount; i++)		
		{
			mysum[count].sum = 0.0f;
			if(( !num ) && ( m_charmodels[i].label < 10))  //字母排在后边
				continue;
			if(( !zimu )  && ( m_charmodels[i].label >= 10))				
				continue;	
				/*
				for(int j=0; j<m_charmodelwidth; j++)
				{
				mysum[count].sum += float((m_ucharmodels[i].data[j] - resp[j])*(m_ucharmodels[i].data[j] - resp[j]));				
				}
			mysum[count].sum  =  float(sqrt(mysum[count].sum));	*/
			//lt  modify 041030
			he = 0.0f;
			for(int j=0; j<m_charmodelwidth; j++)
			{
				cha = float(m_ucharmodels[i].data[j] - resp[j]);
				he += cha * cha;				
			}
			mysum[count].sum  =  float(sqrt(he));	
			
            mysum[count].label = m_ucharmodels[i].label;
			count ++;
		}	
	}
    //对sum找最小值
	float min;
	int charpos;	
	min = mysum[0].sum;	
	charpos = mysum[0].label;
	for(i=1; i<count; i++)
	{	
		if( mysum[i].sum < min ) 
		{
			min = mysum[i].sum;				
			charpos = mysum[i].label;
		}
	}
	//找次小值且label与最小值不同的
    float min2;
    min2 = 1000;  //定义一个很大的值  后边循环就好找
    for(i=0; i<count; i++)
	{

		if( mysum[i].label == charpos)
		    continue;
		
		if(zimu && num)
		{
			if(charpos == 2 && mysum[i].label == 35)
				continue;
			if(charpos == 35 && mysum[i].label == 2)
				continue;
			if(charpos == 0 && mysum[i].label == 13)
				continue;
			if(charpos == 13 && mysum[i].label == 0)
				continue;
			if(charpos == 8 && mysum[i].label == 11)
				continue;
			if(charpos == 11 && mysum[i].label == 8)
				continue;
			if(charpos == 5 && mysum[i].label == 28)
				continue;
			if(charpos == 28  && mysum[i].label == 5)
				continue;
			if(charpos == 29  && mysum[i].label == 34)
				continue;
			if(charpos == 34  && mysum[i].label == 29)
				continue;
		}	

		if(mysum[i].sum < min2)
            min2 = mysum[i].sum;
			
	}
	m_realcharconf = *confidence = 1.0f - min/(min2);

#ifdef CONF_ANA
	m_fMinDis = min;
#endif

	//置信度映射	
   // *confidence = 1.0f - min/(min2);
		//置信度映射	
	if(*confidence > 0.4)
		*confidence = 1;
	else if(*confidence >0.2)
		*confidence = *confidence * 0.25f + 0.9f;
	else if(*confidence > 0.05)
		*confidence = *confidence + 0.75f;
	else
        *confidence = *confidence *0.775f + 0.5f;	
	//
    delete []mysum;
	return charpos;
}


int CECongnize::CharCongnize(BYTE *pimage,float *confidence)//最直接的CharCongnize
{ 

	double *resp;
	resp = m_charjet.Filter(pimage);
	int modelcount;
	//int	usermodelcount;
	int count = 0; //保存实际使用的模板数量  也就是SUM 实际大小
    
    modelcount = m_charmodels.GetSize();
	
	Sum *mysum;
	float he,cha;
	
	mysum = new Sum[modelcount];
   
   //求欧式距离
	for(int i=0; i<modelcount; i++)		
	{		
		he = 0.0f;
		for(int j=0; j<m_charmodelwidth; j++)
		{
			cha = float(m_charmodels[i].data[j] - resp[j]);
			he += cha * cha;
		}
		mysum[count].sum  =  float(sqrt(he));
		mysum[count].label = m_charmodels[i].label;
  		count ++;
	}

    //对sum找最小值
	float min;
	int charpos;	
	min = mysum[0].sum;	
	charpos = mysum[0].label;

	int nIndex = 0;

	for(i=1; i<count; i++)
	{	
		if( mysum[i].sum < min ) 
		{
			min = mysum[i].sum;				
			charpos = mysum[i].label;
			nIndex = i;
		}
	}
	//找次小值且label与最小值不同的
    float min2;
    min2 = 1000;  //定义一个很大的值  后边循环就好找
    for(i=0; i<count; i++)
	{

		if( mysum[i].label == charpos)
		    continue;
		if(mysum[i].sum < min2)
            min2 = mysum[i].sum;
			
	}
	m_realcharconf = *confidence = 1.0f - min/(min2);


	if(*confidence > 0.4)
		*confidence = 1;
	else if(*confidence >0.2)
		*confidence = *confidence * 0.25f + 0.9f;
	else if(*confidence > 0.05)
		*confidence = *confidence + 0.75f;
	else
        *confidence = *confidence *0.775f + 0.5f;	

    delete []mysum;
	return charpos;
}

int CECongnize::CharCongnize(BYTE *pimage,float *confidence,BOOL bNum)//字母数字分开识别
{ 

	double *resp;
	resp = m_charjet.Filter(pimage);
	int modelcount;
	//int	usermodelcount;
	int count = 0; //保存实际使用的模板数量  也就是SUM 实际大小
    
    modelcount = m_charmodels.GetSize();
	
	Sum *mysum;
	float he,cha;
	
	mysum = new Sum[modelcount];
   
   //求欧式距离
	for(int i=0; i<modelcount; i++)		
	{	
		if(( !bNum ) && ( m_charmodels[i].label < 10))  //字母排在后边
			continue;
		if(( bNum )  && ( m_charmodels[i].label >= 10))				
			continue;	
		
		he = 0.0f;
		for(int j=0; j<m_charmodelwidth; j++)
		{
			cha = float(m_charmodels[i].data[j] - resp[j]);
			he += cha * cha;
		}
		mysum[count].sum  =  float(sqrt(he));
		mysum[count].label = m_charmodels[i].label;
  		count ++;
	}

    //对sum找最小值
	float min;
	int charpos;	
	min = mysum[0].sum;	
	charpos = mysum[0].label;

	int nIndex = 0;

	for(i=1; i<count; i++)
	{	
		if( mysum[i].sum < min ) 
		{
			min = mysum[i].sum;				
			charpos = mysum[i].label;
			nIndex = i;
		}
	}
	//找次小值且label与最小值不同的
    float min2;
    min2 = 1000;  //定义一个很大的值  后边循环就好找
    for(i=0; i<count; i++)
	{

		if( mysum[i].label == charpos)
		    continue;
		if(mysum[i].sum < min2)
            min2 = mysum[i].sum;
			
	}
	m_realcharconf = *confidence = 1.0f - min/(min2);


	if(*confidence > 0.4)
		*confidence = 1;
	else if(*confidence >0.2)
		*confidence = *confidence * 0.25f + 0.9f;
	else if(*confidence > 0.05)
		*confidence = *confidence + 0.75f;
	else
        *confidence = *confidence *0.775f + 0.5f;	

    delete []mysum;
	return charpos;
}

BOOL CECongnize::AddUCharMdl(BYTE *pImage,int charpos)
{

    BmpData tempbmp;
    Sample tempsample;
	double *resp;
	tempbmp.pimage = new BYTE[M_BMPSIZE];
	for(int l=0; l<M_BMPSIZE; l++)
	{
		tempbmp.pimage[l] = pImage[l];
	}
	tempbmp.label = charpos;	
	m_ucharbmp.Add(tempbmp);
	
	tempsample.data = new double[m_chnmodelwidth];
	tempsample.label = charpos;
	resp = m_chnjet.Filter(tempbmp.pimage);
	
	for(l=0; l<m_charmodelwidth; l++)
		tempsample.data[l] = resp[l];

	m_ucharmodels.Add(tempsample);
	//保存用户模板位图
	SaveUserBmp("data\\userchars.dat",m_ucharbmp,m_char);
	return TRUE;
}

BOOL CECongnize::DelOneUCharMdl(int pos)
{
	if((pos > m_ucharbmp.GetSize())||( pos < 0))
		return FALSE;
    delete []m_ucharmodels[pos].data;
	delete []m_ucharbmp[pos].pimage;
	m_ucharmodels.RemoveAt(pos);
	m_ucharbmp.RemoveAt(pos);
	SaveUserBmp("data\\userchars.dat",m_ucharbmp,m_char);
	return TRUE;
}

BOOL CECongnize::DelAllUCharMld()
{
	
	for(int i=0; i<m_ucharmodels.GetSize(); i++)
		delete []m_ucharmodels[i].data;
	for(i=0; i<m_ucharbmp.GetSize(); i++)
		delete []m_ucharbmp[i].pimage;
	m_ucharmodels.RemoveAll();
	m_ucharbmp.RemoveAll();
	SaveUserBmp("data\\userchars.dat",m_ucharbmp,m_char);
	return TRUE;
}
// 
//内部共用类

BOOL CECongnize::Train(char *traindir,float mindistance,int maxclass,int maxcycle,int modelwidth,CSampleArray &models,CStringArray &basechar,GaborJet *myjet)
{
	//先清除当前数据
	for(int i=0; i<models.GetSize(); i++)
		delete []models[i].data;
	models.RemoveAll();
	//
   	CRect rect;
	CFileFind finder;
	CImage  myImage;
	IMAGE   pimage = ImageAlloc(17, 33);//[M_BMPSIZE];
	CSampleArray traindata,traindata2;
	Sample tempsample,tempsample2;
	int filenum ;

	double *resp;
	for(int c=0; c<basechar.GetSize(); c++)
	{
		//设定目录
		CString str ;
		str.Format("%s\\%s\\*.bmp",traindir,basechar.GetAt(c));
		//获取特征
		BOOL bWorking = finder.FindFile(str);
		filenum = 0;
		traindata.RemoveAll();
		traindata2.RemoveAll();
		while(bWorking)
		{
			bWorking = finder.FindNextFile();  
			
			myImage.Load(finder.GetFilePath());
			myImage.GetRect(rect);	
			
			if((rect.right != 16 )||(rect.bottom != 32))
			{
				//AfxMessageBox("图象大小不符合");
				//continue;
			}			
			myImage.GetImage(rect,pimage[0], 1);   // wuwei delete 
			
			resp =	myjet->Filter(pimage[0]);

			filenum++;
			tempsample.data = new double[modelwidth];
			tempsample2.data = new double[modelwidth];
			
			for(int i=0; i<modelwidth; i++)
			{
				tempsample.data[i]  = resp[i];
				tempsample2.data[i] = resp[i];
			}
			traindata.Add(tempsample);
			traindata2.Add(tempsample2);			
		}
		//用两个相同的数组来保存特征  维持一个不变 另一个作为中间变量
		int	submodelcount = filenum;//训练样本数;
		int k = submodelcount;//开始都是类		
		if(submodelcount >1 )  //不止一个训练样本 看样本是否能合并
		{
			//谱系聚类  输出的是k个中心
			float dismin = 0.0f;	
			int dismin1,dismin2;  //保存距离最小的两个类的位置
        
			//用一个2维数组来保存距离
			float **dis;
			dis = new float*[submodelcount];
			for(int i=0; i<submodelcount; i++)
				dis[i] = new float[submodelcount];
			for(i=0; i<submodelcount; i++)
				for(int j=0; j<submodelcount; j++)
                    dis[i][j] =  0.0f;
		
			for(i=0; i<k-1; i++)
				for(int j=i+1; j<k; j++)
				{ //欧式距离
					for(int l=0; l<modelwidth; l++)
						dis[i][j] +=  float((traindata2[i].data[l]- traindata2[j].data[l])*(traindata2[i].data[l]- traindata2[j].data[l]));
                    dis[i][j] =  float(sqrt(dis[i][j]));					
				}
			dismin = 0.0f;
			while(1) 
			{
				//找寻最小距离
				dismin = dis[0][1];
				dismin1 = 0;
				dismin2 = 1;
				for(i=0; i<k-1; i++)
					for(int j=i+1; j<k; j++)
					{
						if(dis[i][j] < dismin )
						{
							dismin = dis[i][j];
							dismin1 = i;
							dismin2 = j;
						}
					}
					//看是否满足结束条件
				if(( dismin > mindistance )&&( k <= maxclass ))
					break ; 
				//合并最小距离的两类 具体做法是把其中(dismin1)一个当新类			
				//首先  dismin1 和 dismin2 合并到 较小的一个
				if(dismin2 < dismin1)
				{
					int k;
					k = dismin2;
					dismin2 = dismin1;
					dismin1 = k;
				}
				for(int l=0; l<modelwidth; l++)
					traindata2[dismin1].data[l] = (traindata2[dismin1].data[l] + traindata2[dismin2].data[l])/2;
				//然后	//看另一个类(dismin2)是否在最后 如果不在 就把最后一个拷贝过来
				if(dismin2 != (k-1))
				{
					//把最后一个拷贝到 dismin2 
					for(int l=0; l<modelwidth; l++)
						traindata2[dismin2].data[l] = traindata2[k-1].data[l];
					//拷贝距离
					for(l=0; l<(k-1); l++)
					{
						if( l < dismin2 )
							dis[l][dismin2]  = dis[l][k-1];
						else 
							dis[dismin2][l]  = dis[l][k-1];
					}
				}
				k -- ;
				//看是否满足结束条件
				if(k == 1)
					break;
				//求所有新距离  是否能按书上的办法来减少运算量？
				//只需要计算新的类和别的类之间的距离  先不考虑这个
				//先实现哈  要是花的时间不长  就不用再考虑了
				/*
				for(i=0; i<k; i++)
									for(int j=i+1; j<k; j++)
									{ //欧式距离
										for(int l=0; l<modelwidth; l++)
											dis[i][j] += float((traindata2[i].data[l]- traindata2[j].data[l])*(traindata2[i].data[l]- traindata2[j].data[l]));
										dis[i][j] =  float(sqrt(dis[i][j]));					
									}*/
										
				//计算dismin1到各个聚类中心的新距离
				for(i=0; i<(k-1); i++)
				{
					if(i < dismin1)
					{
						//欧式距离
						for(int l=0; l<modelwidth; l++)
							dis[i][dismin1] += float((traindata2[i].data[l]- traindata2[dismin1].data[l])*(traindata2[i].data[l]- traindata2[dismin1].data[l]));
						dis[i][dismin1] =  float(sqrt(dis[i][dismin1]));
					}else if(i > dismin1)
					{
						//欧式距离
						for(int l=0; l<modelwidth; l++)
							dis[dismin1][i] += float((traindata2[i].data[l]- traindata2[dismin1].data[l])*(traindata2[i].data[l]- traindata2[dismin1].data[l]));
						dis[dismin1][i] =  float(sqrt(dis[i][dismin1]));
					}
				}
			}//谱系聚类结束
			//c -- 均值（类数用k表示）
		    //先申请k个中心的暂存空间
			double **tempdata;
			tempdata = new double*[k];
			for(i=0; i<k; i++)
				tempdata[i] = new double[modelwidth];
			
			float *cdis;
			cdis = new float[k];
			int    *subcount;
			subcount = new int[k];
			int maxc = 0;
			while(1)
			{
				//先初始化数据
				for(i=0; i<k; i++)
				{
					for(int j=0; j<modelwidth; j++)
						tempdata[i][j] = 0;
					cdis[i] = 0.0f;
					subcount[i] = 0;
				}
				//归类
				for(int i=0; i<submodelcount; i++)
				{
					
					//归类  就是把同类的先相加  并记录数量
					//
					for(int j=0; j<k; j++)
					{
						for(int l=0; l<modelwidth; l++)
							cdis[j] += float((traindata[i].data[l]- traindata2[j].data[l])*(traindata[i].data[l]- traindata2[j].data[l]));
						cdis[j] =  float(sqrt(cdis[j]));						
					}
					float min;
					int pos;
					min = cdis[0];
					pos = 0;
					//找到最小距离的位置
					for(j=1; j<k; j++)
					{
						if(cdis[j] < min)
						{
							min = cdis[j];
							pos = j;
						}
					}
					//把这个训练样本累加到对应类中   后边求平均后就是新的聚类中心
                    for(int l=0; l<modelwidth; l++)
						tempdata[pos][l] += traindata[i].data[l];
					subcount[pos] ++;  //记录对应位置类数  后边好用来求平均
				}
				//求新的聚类中心  就是求平均值
                for(i=0; i<k; i++)
				    for(int l=0; l<modelwidth; l++)
					  tempdata[i][l] = tempdata[i][l]/subcount[i]; 
				//看是否满足要求  满足就退出循环
				double sum1,sum2;
				sum1 = sum2 = 0.0;
				for(i=0; i<k; i++)
					for(int l=0; l<modelwidth; l++)
					{
                        sum1 += tempdata[i][l];
						sum2 += traindata2[i].data[l];
					}
				if(sum1 == sum2)
					break;	
				//把新的聚类中心拷贝到traindata2   方便后边保存？
				for(i=0; i<k; i++)
					for(int l=0; l<modelwidth; l++)
						traindata2[i].data[l] = tempdata[i][l];				
			    maxc ++;
                if(maxc >maxcycle)
					break;
			}
			delete []cdis;
			delete []subcount;
			for(i=0; i<k; i++)
				delete []tempdata[i];
			delete []tempdata;
			//c -- 均值 结束
			
			//保存数据  用一个CArray来保存就是
			for(i=0;  i<k;  i++)
			{
				tempsample.data = new double[modelwidth];
				tempsample.label = c;
				for(int l=0; l<modelwidth; l++)
					tempsample.data[l] = traindata2[i].data[l];
				models.Add(tempsample);
			}
			//清除内存
			for(i=0; i<submodelcount; i++)
				delete []dis[i];
			delete []dis;
		
		}//if(submodelcount >1 )  结束
		else if(filenum == 1)// 只有一个样本  直接做类  
		{ 
			tempsample.data = new double[modelwidth];
			tempsample.label = c;
			for(int l=0; l<modelwidth; l++)
				tempsample.data[l] = traindata2[0].data[l];
			models.Add(tempsample);
		}
		else  //没有训练样本 //没有训练样本时就直接设置为1000
		{
			tempsample.data = new double[modelwidth];
			
			tempsample.label = c;
			for(int l=0; l<modelwidth; l++)
				tempsample.data[l] = 1000.0;
			models.Add(tempsample);
		}
		//清除内存
		for(int i=0; i<filenum; i++)
      	{
			delete []traindata[i].data;  
            delete []traindata2[i].data;  
		}
		
	//向外部发送消息
	}//	for(int c=0; c<53; c++) 结束
	
	//清除内存
	return TRUE;
}

BOOL CECongnize::LoadModels(char *modelname,int modelwidth,CSampleArray &models)
{
	for(int i=0; i<models.GetSize(); i++)
		delete []models[i].data;		
	int count;
	ifstream in(modelname);
    in >> count;
    if(in.fail())
	{
		//AfxMessageBox("模板文件读取错误，请先训练");
		return FALSE;
	}
	Sample tempsample;
	for(i=0; i<count; i++)
	{
        in >> tempsample.label;
		tempsample.data = new double[modelwidth];
		for(int l=0; l<modelwidth; l++)
			in >> tempsample.data[l];
		models.Add(tempsample);
		
	}
    in.close();
	return TRUE;
}

BOOL CECongnize::SaveModels(char *modelname,int modelwidth,CSampleArray &models)
{
    ofstream out(modelname);	
	out<<models.GetSize()<<endl;
	for(int i=0; i<models.GetSize(); i++)
	{
        out << models[i].label <<" ";
		for(int j=0; j<modelwidth; j++)
			out << models[i].data[j] <<" ";
		out << endl;
	}	
	out.close();
	return true;
}

BOOL CECongnize::LoadUserBmp(char *modelname,CBmpArray &userbmp,CSampleArray &models)
{
	int count;
	int pos =0 ;//lt add 0321
	CStringArray tempchn;
	int basecount = m_chn.GetSize();
	ifstream in(modelname);
    in >> count;
    if(!(in.fail()))
	{
		for(int i=0; i<count; i++)
		{
			CString str;
            CHAR mychar[10];
			in >> mychar;  //没用....
			//lt modify 0321
			str.Format("%s",mychar);
            tempchn.Add(str);
		}
		
		double *resp;
		BmpData tempbmp;
		Sample  tempsample;
		int temp;
        pos =0;
		for(i=0; i<count; i++)
		{
			in >> tempbmp.label;
			//lt modify 0321
			if(tempbmp.label>=basecount) //肯定是新的汉字字符
			{
				if(tempbmp.label == m_chn.GetSize())
				{
					m_chn.Add(tempchn[pos]);
				}
				else if(tempbmp.label >m_chn.GetSize())
				{
					CString str ="-";
					while(tempbmp.label>m_chn.GetSize())
					{
						m_chn.Add(str);
					}
					m_chn.Add(tempchn[pos]);
					
				}
				else //xiaoyu
				{
					m_chn[tempbmp.label] = tempchn[pos];
				}
			}

			//

			tempbmp.pimage = new BYTE[M_BMPSIZE];
			for(int l=0; l<M_BMPSIZE; l++)
			{
				in >> temp;
				tempbmp.pimage[l] = temp;
			}
			userbmp.Add(tempbmp);			
		
			tempsample.label = tempbmp.label;
			tempsample.data = new double[m_chnmodelwidth];
			resp = m_chnjet.Filter(tempbmp.pimage);
			for(l=0; l<m_chnmodelwidth; l++)
				tempsample.data[l] = resp[l];
			models.Add(tempsample);	
			
			pos++;
		}
	}
    in.close();
	return TRUE;
}

BOOL CECongnize::SaveUserBmp(char *modelname,CBmpArray &userbmp,CStringArray &charstring)
{
	ofstream out(modelname);	
	CString str;
	out.setmode();
	out << userbmp.GetSize() <<endl;
    for(int i=0; i<userbmp.GetSize(); i++)
		out<< charstring[userbmp[i].label] << " ";
	out <<endl<<endl;

	for(i=0; i<userbmp.GetSize(); i++)
	{
		out << userbmp[i].label;
		for(int l=0; l<M_BMPSIZE; l++)
		{
            str.Format(" %d",userbmp[i].pimage[l] );
			out <<str;
		}
		out <<endl;
	}
	out <<endl;
	out.close();
	return TRUE;
}
//共用函数结束