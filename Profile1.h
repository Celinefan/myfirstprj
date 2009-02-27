// Profile1.h: interface for the CProfile class.
//
//////////////////////////////////////////////////////////////////////
//#include "SpecialRec.h"
#if !defined(AFX_PROFILE1_H__FB87F2F6_EB73_4B89_95D3_BCD67565D860__INCLUDED_)
#define AFX_PROFILE1_H__FB87F2F6_EB73_4B89_95D3_BCD67565D860__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include"scuu.h"
#include "Projection.h"
struct SampleProfile
{
	int label;
	int *data;
};

typedef CArray <SampleProfile,SampleProfile&> CSampleProfileArray;

struct SumProfile{
	int label;
	int sum;
};
class CProfile  
{
public:

	//////////////////////////////////////////////////////////////////////////
	
	  CImage *m_img;

	  IMAGE tempimage[35][3];

	  CSampleProfileArray  m_charmodels;     //标准模板数据	//yang 
	   BOOL LoadModels(char *modelname,int modelwidth,CSampleProfileArray &models); //yang
	

	//////////////////////////////////////////////////////////////////////////
	
	CProfile();
	virtual ~CProfile();
	CProfile( CString& strFeatureFile );
	void recognice(RecType type,IMAGE mc,float &m_min,float &m_min1,float &confidence,char &m_char,char &m_char1);
	char ProfileSpe(IMAGE &Img, char &Result1, char &Result2);//轮廓的字符算法
	int Char_To_Int3(char &Character);//字母到数字的转换
		char ProfileSpe_Part(IMAGE &Img, char &Result1, char &Result2,float left,float right,float top,float bottom);
		

	int	m_Profile[200];
	float m_min;
	float m_min1;
	char m_char,m_char1;
	IMAGE m_mc;
    float GetRealCharConf(){ return m_realcharconf;}
private:
	char OnProfile(RecType type,IMAGE &Img,int &miny,int &viceminy,char &vicechar);
//	int  m_Sample[70][200];
	int  m_Sample[105][200];
	float Confidence(float a,float b);
	float m_realcharconf;

};

#endif // !defined(AFX_PROFILE1_H__FB87F2F6_EB73_4B89_95D3_BCD67565D860__INCLUDED_)
