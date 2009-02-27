// Projection.h: interface for the CProjection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTION_H__7B6DAEBF_0C1A_4158_B50F_4D7DA749A145__INCLUDED_)
#define AFX_PROJECTION_H__7B6DAEBF_0C1A_4158_B50F_4D7DA749A145__INCLUDED_
//#include "SpecialRec.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include"scuu.h"

enum RecType{ 
		ENG,		
		MIX,
		NUM,
		CHN
	};


class CProjection  
{
public:
	CProjection();
	virtual ~CProjection();
	float m_confidence;
    float m_min,m_min1;
	char m_char,m_char1;
	int m_recognicenum;

	float m_fTemplSC[36][13];
	float Shadow_Code[13];
	char int_To_char(int &num);
	IMAGE m_mc;//RecType type
	void recognize( RecType  type,float &m_min,float &m_min1,float &m_confidence,int &m_char,int &m_char1);//recognize 1代表字母，2 代表数字3代表综合
private:
	int EngCharBP(/*float *Shadow_Code,*/float& fMinDist,float &fviceMinDist,int &yangvice);
	
	float Confidence(float a,float b);
	void  Projection13(/*IMAGE &mc*/);

};

#endif // !defined(AFX_PROJECTION_H__7B6DAEBF_0C1A_4158_B50F_4D7DA749A145__INCLUDED_)
