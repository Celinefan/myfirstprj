// Profile1.cpp: implementation of the CProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "gy.h"
#include "Profile1.h"
#include "math.h"
#include "dibitmap.h"
#include "fstream.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProfile::CProfile()
{
	 m_img =new CImage;
	 m_img->Create(2,2,8);
	int i = 0, num = 0;
	CString str;
	int m,n;
	
		
	//LoadModels("tempprofile.dat",200,m_charmodels);
	LoadModels("17vs47.dat",200,m_charmodels);
	///读图片文件
//   CString temppic;
//	for(m=0;m<35;m++)
//	{
//		for(n=0;n<3;n++)
//		{
//			int temp ;
//			temp =m;
//			if(m ==24)
//			{
//				continue;
//			}
//			if(m>24)
//			{
//                 temp=m-1;
//			}
//			temppic.Format("templatepic\\%d%d.bmp",m,n+1);  
//			m_img->Load(temppic);
//
//			int hei =m_img->GetHeight();
//			int wid =m_img->GetWidth();
//			CRect rect;
//			m_img->GetRect(rect);
//		   tempimage[temp][n] =ImageAlloc(wid,hei);
//
//		   m_img->GetImage(rect,tempimage[temp][n][0],1);
//		}
// 	}


//	CStdioFile fFibo;
//int i = 0,j = 0,num = 0;
//CString str;
//
//if( !fFibo.Open( "average.dat", CFile::modeRead | CFile::typeText))
//{
//#ifdef _DEBUG
//	AfxMessageBox("文件路径不对");
//#endif
//	exit( 1 );
//}
//
//while(fFibo.ReadString(str))
//{
//	
//	m_Sample[num][i] = atoi(str);
//	i ++;
//	if (i == 200)
//	{
//		num ++;
//		i = 0;
//	}
//}
//fFibo.Close();	

}

CProfile::CProfile( CString& strFeatureFile )
{
	m_img =new CImage;
	m_img->Create(2,2,8);

	char* str = (char*)LPCSTR(strFeatureFile);
	LoadModels( str, 200, m_charmodels );
}

CProfile::~CProfile()
{

}
void CProfile::recognice(RecType type,IMAGE mc,float &m_min,float &m_min1,float &confidence,char &m_char,char &m_char1)
{
	int a,b;
	m_char =OnProfile(type,mc,a,b,m_char1);
	m_min =(float)sqrt(a);//a 最小
	m_min1 = (float)sqrt(b);//b次小
	confidence =Confidence(m_min,m_min1);

		
	
}
char CProfile::OnProfile(RecType type,IMAGE &Img,int &miny,int &viceminy,char &vicechar) 
{
	int nWidth = ImageWidth(Img);
	int nHeight = ImageHeight(Img);
	  int vicemin;
	
	  int i,j,k = -2;
	  for(j=0;j<nHeight;j++)
		  for(i=0;i<nWidth;i++)
		  {
			  if(Img[j][i] > 99)
				  Img[j][i] = 255;
			  else
				  Img[j][i] = 0;
		  }
      
	  int m_d[105];//yang

	//for(i=0;i<70;i++)//2005.10.7  34*2
	  for(i=0;i<105;i++)
	  {
		  m_d[i] = 0;
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
				  m_Profile[k] = -1;//nWidth - 1 ;
				  if( Img[j][i+1] == 255 )
					  m_Profile[k+1] = nWidth - 1;//nWidth - 1;//!!!need further consideration
				  else
					  m_Profile[k+1] = -1;
				  break;
			  }
			  if (find1 && (!find2))
			  {
				  if((Img[j][ i] == 0) && (Img[j][i+1] == 255))
				  {
					  m_Profile[k+1] = i + 1;
					  find2 = true;
					  break;
				  }
				  else if((!find2) && (i == nWidth-2))	  
				  {
					  m_Profile[k+1] = -1;//nWidth - 1;
				  }
			  }
		  }
	  }
	  
//	  for(j=0;j<nHeight;j++)
//	  { 
//		  k += 2;
//		  find1 = false;
//		  find2 = false;
//		  
//		  for(i=0;i<nWidth-1;i++)
//		  {
//			  
//			  if(Img[j][i] == 255 && !(find1))
//			  {
//				  m_Profile[k] =i;
//				  find1 = true;
//				  
//			  }
//			  else if((!find1) && (i == nWidth-2))
//			  {
//				  m_Profile[k] = nWidth - 1 ;
//				  m_Profile[k+1] = nWidth - 1;//!!!need further consideration
//				  //endfind = true;
//				  break;
//			  }
//			  if (find1 && (!find2))
//			  {
//				  if((Img[j][ i] == 0) && (Img[j][i+1] == 255))
//				  {
//					  m_Profile[k+1] = i + 1;
//					  find2 = true;
//					  // endfind = true;
//					  break;
//				  }
//				  else if((!find2) && (i == nWidth-2))	  
//				  {
//					  m_Profile[k+1] = nWidth - 1;
//					  // endfind = true;
//				  }
//			  }
//		  }
// 	  }

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


	int temp = 5000;
//   int d[34];
	int min = 0;
//	char result;
	 int s,e;
	 BOOL num,zimu;
	 if(type == ENG)
	 {
		 zimu =TRUE;
		 num =FALSE;
	 }
	 else if(type == NUM)
	 { 
		 zimu =FALSE;
		 num =TRUE;	}
	 else //type == mix
	 { 
		 zimu =TRUE;
		 num =TRUE;
	 }

	 //////////////////////////////////////////////////////////////////////////
	 
	int modelcount = m_charmodels.GetSize();
		int count = 0; //保存实际使用的模板数量  也就是SUM 实际大小


	SumProfile *mysum;
	float he,cha;
	
	mysum = new SumProfile[modelcount];
   
   //求欧式距离
	for( i=0; i<modelcount; i++)		
	{		
//		if(( !num ) && ( m_charmodels[i].label < 10))  //字母排在后边//两个模板
//			continue;
//		if(( !zimu )  && ( m_charmodels[i].label >= 10))				
// 			break;	
		
		he = 0.0f;
		for(int j=0; j<200; j++)
		{
			int temp =m_charmodels[i].data[j] ;
			cha = float(m_charmodels[i].data[j] -m_Profile[j]);
			he += cha * cha;				
		}

		if(m_charmodels[i].label ==1)
		{
			continue;
		}
		mysum[count].sum  =  int(float(sqrt(he))+0.5);
		mysum[count].label = m_charmodels[i].label;
		count ++;
	}

	//找最小

	
	int charpos;	
	min = mysum[0].sum;	
	charpos = mysum[0].label;
	for(i=1; i<count; i++)
	{	

		if(mysum[i].label ==1)
		{
			continue;
		}
		if( mysum[i].sum < min ) 
		{
			min = mysum[i].sum;				
			charpos = mysum[i].label;
		}
	}

	 float min2;
    min2 = 1000;  //定义一个很大的值  后边循环就好找
    for(i=0; i<count; i++)
	{

			if(mysum[i].label ==1)
		{
			continue;
		}
		if( mysum[i].label == charpos)
		    continue;
		
	

		if(mysum[i].sum < min2)
            min2 = mysum[i].sum;
			
	}
	delete []mysum;

	 //////////////////////////////////////////////////////////////////////////
	 

	/*
	if(zhixindu>500)
		{
		    liuresult = '?';
		    jushicount++;
		}
		else*/
	char liuresult =NULL;
	
    {
		switch(charpos)
		{
		case 0 : liuresult = '0';
			break;
		case 1 : liuresult = '1';
			break;
		case 2 : liuresult = '2';
			break;
		case 3 : liuresult = '3';
			break;
		case 4 : liuresult = '4';
			break;
		case 5 : liuresult = '5';
			break;
		case 6 : liuresult = '6';
			break;
		case 7 : liuresult = '7';
			break;
		case 8 : liuresult = '8';
			break;
		case 9 : liuresult = '9';
			break;
		case 10 : liuresult = 'A';
			break;
		case 11 : liuresult = 'B';
			break;
		case 12 : liuresult = 'C';
			break;
		case 13 : liuresult = 'D';
			break;
		case 14 : liuresult = 'E';
			break;
		case 15 : liuresult = 'F';
			break;
		case 16 : liuresult = 'G';
			break;
		case 17 : liuresult = 'H';
			break;
		case 18 : liuresult = 'I';
			break;
		case 19 : liuresult = 'J';
			break;
		case 20 : liuresult = 'K';
			break;
		case 21 : liuresult = 'L';
			break;
		case 22 : liuresult = 'M';
			break;
		case 23 : liuresult = 'N';
			break;
		case 24 : liuresult = 'P';
			break;
		case 25 : liuresult = 'Q';
			break;
		case 26 : liuresult = 'R';
			break;
		case 27 : liuresult = 'S';
			break;
		case 28 : liuresult = 'T';
			break;
		case 29 : liuresult = 'U';
			break;
		case 30 : liuresult = 'V';
			break;
		case 31 : liuresult = 'W';
			break;
		case 32 : liuresult = 'X';
			break;
		case 33 : liuresult = 'Y';
			break;
		case 34 : liuresult = 'Z';
			break;
		default :
			break;
		}
		switch(vicemin)
		{
		case 0 : vicechar = '0';
			break;
		case 1 : vicechar = '1';
			break;
		case 2 : vicechar = '2';
			break;
		case 3 : vicechar = '3';
			break;
		case 4 : vicechar = '4';
			break;
		case 5 : vicechar = '5';
			break;
		case 6 : vicechar = '6';
			break;
		case 7 : vicechar = '7';
			break;
		case 8 : vicechar = '8';
			break;
		case 9 : vicechar = '9';
			break;
		case 10 : vicechar = 'A';
			break;
		case 11 : vicechar = 'B';
			break;
		case 12 : vicechar = 'C';
			break;
		case 13 : vicechar = 'D';
			break;
		case 14 : vicechar = 'E';
			break;
		case 15 : vicechar = 'F';
			break;
		case 16 : vicechar = 'G';
			break;
		case 17 : vicechar = 'H';
			break;
		case 18 : vicechar = 'I';
			break;
		case 19 : vicechar = 'J';
			break;
		case 20 : vicechar = 'K';
			break;
		case 21 : vicechar = 'L';
			break;
		case 22 : vicechar = 'M';
			break;
		case 23 : vicechar = 'N';
			break;
		case 24 : vicechar = 'P';
			break;
		case 25 : vicechar = 'Q';
			break;
		case 26 : vicechar = 'R';
			break;
		case 27 : vicechar = 'S';
			break;
		case 28 : vicechar = 'T';
			break;
		case 29 : vicechar = 'U';
			break;
		case 30 : vicechar = 'V';
			break;
		case 31 : vicechar = 'W';
			break;
		case 32 : vicechar = 'X';
			break;
		case 33 : vicechar = 'Y';
			break;
		case 34 : vicechar = 'Z';
			break;
		default :
			break;
		}

	}	
	
	return charpos;
}
float CProfile::Confidence(float a,float b)//a 最小值 b次小值
{
	float m;
	m =1-float(a)/b;
	m_realcharconf = m;
	//这里映射置信度
	if(m>0.5)
		m = 1;
	else if(m>0.3)
        m=m+0.5f; 
	else if(m>0.25)
        m=6*m-1;
	else
	    m= 0.5;
	return m;
}

char CProfile::ProfileSpe(IMAGE &Img, char &Result1, char &Result2)//轮廓的字符算法
{ 
	int nWidth = ImageWidth(Img);//应该33
	int nHeight = ImageHeight(Img);//应该是17
	

	int m_Profile[200];
	memset(m_Profile, 0, sizeof(int)*200);
	int m_d[105];
	memset(m_d, 0, sizeof(int)*105);


	int j,k = -2;


	BOOL find1;
	BOOL find2;
	  
    //from left to right
    for(j=0;j<nHeight;j++)
		 { 
			k += 2;
		    find1 = false;
	       // endfind = false;
	        find2 = false;

		       for(int i=0;i<nWidth-1;i++)///////为什么不是i<nWidth
			   {
				  
			      if(Img[j][i] == 255 && !(find1))
				  {
			          m_Profile[k] =i;
			          find1 = true;
			           
				  }
			      else if((!find1) && (i == nWidth-2))//为什么不是nWidth-1
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

	   for(int i=nWidth-1;i>0;i--)
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
	 for(int i=0;i<nWidth;i++)
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
	 
/*
	 ofstream out1("pro.txt");
	 for(i=0;i<200;i++)
	 {
		out1 << m_Profile[i] << endl;
	 }
*/

    int Number[2];
	Number[0] = Char_To_Int3(Result1);//还是有用的

	Number[1] = Char_To_Int3(Result2);//还是有用的
	int  modelcount = m_charmodels.GetSize();


	int count_sample =0;
	for( i=0; i<2; i++)	//标准模板	
	{
		for(int k=0;k<modelcount;k++)
		{
			if(m_charmodels[k].label==Number[i])
			{
				count_sample++;
			
			}
		}
	}

	SumProfile *mysum1;
	float he,cha;
	
	mysum1= new SumProfile[count_sample];

		count_sample =0;

		for(i=0; i<2; i++)	//标准模板	
	{
		for(int k=0;k<modelcount;k++)
		{
			if(m_charmodels[k].label==Number[i])
			{
				
				mysum1[count_sample].sum = 0.0;
				
				he = 0.0f;
				for(int j=0; j<200; j++)
				{
					cha = float(m_charmodels[k].data[j] - m_Profile[j]);//yang 
					he += cha * cha;				
				}
				mysum1[count_sample].sum  = int( float(sqrt(he))+0.5);
				mysum1[count_sample].label = m_charmodels[k].label;
				count_sample++;
				
			}
		}
	
	}

	float min;
	int chnpos;	
	min = mysum1[0].sum;	
	chnpos = mysum1[0].label;

	int resultend =0;

	for(i=1; i<count_sample; i++)
	{	
		if( mysum1[i].sum < min ) 
		{
			min = mysum1[i].sum;				
			chnpos = mysum1[i].label;
			resultend =i;
		}
	}

    delete []mysum1;


	 if(chnpos ==Char_To_Int3(Result2))
   {
	   return Result2;
   }
   else
   {
	   	return Result1;
	   
   }
}
char CProfile::ProfileSpe_Part(IMAGE &Img, char &Result1, char &Result2,float left,float right,float top,float bottom)
{
	int height =ImageHeight(Img);
	int width =ImageWidth(Img);

	int top_height =  int(float(top *height)+0.5);
	int bottom_height =int(float(bottom *height)+0.5);
	int left_width =int(float(left *width)+0.5);
	int right_width =int(float(right *width)+0.5);

	//取待测字符的特征
		int nWidth = ImageWidth(Img);//应该33
	int nHeight = ImageHeight(Img);//应该是17
	

	int m_Profile[200];
	memset(m_Profile, 0, sizeof(int)*200);
	int m_d[105];
	memset(m_d, 0, sizeof(int)*105);


	int j,k = -2;


	BOOL find1;
	BOOL find2;
	  
    //from left to right
    for(j=top_height;j<bottom_height;j++)
		 { 
			k += 2;
		    find1 = false;
	       // endfind = false;
	        find2 = false;

		       for(int i=left_width;i<right_width-1;i++)///////为什么不是i<nWidth
			   {
				  
			      if(Img[j][i] == 255 && !(find1))
				  {
			          m_Profile[k] =i;
			          find1 = true;
			           
				  }
			      else if((!find1) && (i == nWidth-2))//为什么不是nWidth-1
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
    for(j=top_height;j<bottom_height;j++)
	 { 
			k += 2;
		    find1 = false;
	        find2 = false;

	   for(int i=right_width-1;i>left_width;i--)
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
	 for(int i= left_width;i<right_width;i++)
	 { 
		k += 2;
		find1 = false;
	    find2 = false;

		   for(j=top_height;j<bottom_height-1;j++)
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
	 for(i=left_width;i<right_width;i++)
	 { 
		k += 2;
		find1 = false;
	    find2 = false;

		   for(j=bottom_height-1;j>top_height;j--)
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
	 
/*
	 ofstream out1("pro.txt");
	 for(i=0;i<200;i++)
	 {
		out1 << m_Profile[i] << endl;
	 }
*/

    int Number[6];
	Number[0] = Char_To_Int3(Result1)*3+0;//还是有用的

	Number[1] = Char_To_Int3(Result1)*3+1;//还是有用的

	Number[2] = Char_To_Int3(Result1)*3+2;//还是有用的

	Number[3] = Char_To_Int3(Result2)*3+0;//还是有用的

	
	Number[4] = Char_To_Int3(Result2)*3+1;//还是有用的

	
	Number[5]= Char_To_Int3(Result2)*3+2;//还是有用的

	///6个字符模板特征
	int m_Samplepart[6][200];

	for(i=0;i<6;i++)
		for(j=0;j<200;j++)
		{
			m_Samplepart[i][j]=0;

		}

   
	
	 int m;
	for(m=0;m<3;m++)
	{
     CString temppic;
	 int num=Char_To_Int3(Result1);
//	 temppic.Format("templatepic\\%d%d.bmp",num,m+1);
//     m_img->Load(temppic);
//	 int hei =m_img->GetHeight();
//	 int wid =m_img->GetWidth();
//	 CRect rect;
//	 m_img->GetRect(rect);
//	 IMAGE temp =ImageAlloc(wid,hei);
//     m_img->GetImage(rect,temp[0],1);
	 //////////////////////////////////////////////////////////////////////////
	 k = -2;
     for(j=top_height;j<bottom_height;j++)
		 { 
			k += 2;
		    find1 = false;
	       // endfind = false;
	        find2 = false;

		       for(int i=left_width;i<right_width-1;i++)///////为什么不是i<nWidth
			   {
				  
			      if(tempimage[num][m][j][i] == 255 && !(find1))
				  {
			          m_Samplepart[m][k] =i;
			          find1 = true;
			           
				  }
			      else if((!find1) && (i == nWidth-2))//为什么不是nWidth-1
				  {
				      m_Samplepart[m][k] = nWidth - 1 ;
				      m_Samplepart[m][k+1] = nWidth - 1;//!!!need further consideration
				      //endfind = true;
				      break;
				  }
			     if (find1 && (!find2))
				 {
				     if((tempimage[num][m][j][ i] == 0) && (tempimage[num][m][j][i+1] == 255))
					 {
				      m_Samplepart[m][k+1] = i + 1;
					  find2 = true;
					 // endfind = true;
					  break;
					 }
				    else if((!find2) && (i == nWidth-2))	  
					{
					  m_Samplepart[m][k+1] = nWidth - 1;
					 // endfind = true;
					}
				 }
			   }
		 }

        //from right to left
    for(j=top_height;j<bottom_height;j++)
	 { 
			k += 2;
		    find1 = false;
	        find2 = false;

	   for(int i=right_width-1;i>left_width;i--)
	   {
				  
		  if(tempimage[num][m][j][i] == 255 && !(find1))
		  {
			  m_Samplepart[m][k] =i;
			  find1 = true;
		  }
		  else if((!find1) && (i == 1))
		  {
			  m_Samplepart[m][k] = 0 ;
			  m_Samplepart[m][k+1] = 0;//!!!need further consideration
			  break;
		  }
		 if (find1 && (!find2))
		 {
			 if((tempimage[num][m][j][ i] == 0) && (tempimage[num][m][j][i-1] == 255))
			 {
			  m_Samplepart[m][k+1] = i-1;
			  find2 = true;
			  break;
			 }
			else if((!find2) && (i == 1))	  
			{
			  m_Samplepart[m][k+1] = 0;
			}
		 }
	   }
	 }

     //from up to down
	 for(int i=left_width;i<right_width;i++)
	 { 
		k += 2;
		find1 = false;
	    find2 = false;

		   for(j=top_height;j<bottom_height-1;j++)
		   {
			  
			  if(tempimage[num][m][j][i] == 255 && !(find1))
			  {
			      m_Samplepart[m][k] = j;
			      find1 = true;
			  }
			  else if((!find1) && (j == nHeight-2))
			  {
				  m_Samplepart[m][k] = nHeight - 1 ;
				  m_Samplepart[m][k+1] = nHeight - 1;//!!!need further consideration
				  break;
			  }
			 if (find1 && (!find2))
			 {
				 if((tempimage[num][m][j][i] == 0) && (tempimage[num][m][j+1][i] == 255))
				 {
				  m_Samplepart[m][k+1] = j+1;
				  find2 = true;
				  break;
				 }
				else if((!find2) && (j == nHeight - 2))	  
				{
				  m_Samplepart[m][k+1] = nHeight -1;
				}
			 }
		   }
	 }

	 //from down to up
	 for(i=left_width;i<right_width;i++)
	 { 
		k += 2;
		find1 = false;
	    find2 = false;

		   for(j=bottom_height-1;j>top_height;j--)
		   {
			  
			  if(tempimage[num][m][j][i] == 255 && !(find1))
			  {
			      m_Samplepart[m][k] = j;
			      find1 = true;
			  }
			  else if((!find1) && (j == 1))
			  {
				  m_Samplepart[m][k] = 0 ;
				  m_Samplepart[m][k+1] = 0;//!!!need further consideration
				  break;
			  }
			 if (find1 && (!find2))
			 {
				 if((tempimage[num][m][j][i] == 0) && (tempimage[num][m][j-1][i] == 255))
				 {
				  m_Samplepart[m][k+1] = j-1;
				  find2 = true;
				  break;
				 }
				else if((!find2) && (j == 1))	  
				{
				  m_Samplepart[m][k+1] = 0;
				}
			 }
		   }
	 }
	 //////////////////////////////////////////////////////////////////////////
	 
	 
 
	}
	for(m=3;m<6;m++)
	{
		CString temppic;
		int num=Char_To_Int3(Result2);
		temppic.Format("templatepic\\%d%d.bmp",num,m-3+1);
		
//		m_img->Load(temppic);
//		int hei =m_img->GetHeight();
//		int wid =m_img->GetWidth();
//		CRect rect;
//		m_img->GetRect(rect);
//		IMAGE temp =ImageAlloc(wid,hei);
//		m_img->GetImage(rect,temp[0],1);
		//////////////////////////////////////////////////////////////////////////
		k = -2;
		for(j=top_height;j<bottom_height;j++)
		{ 
			k += 2;
			find1 = false;
			// endfind = false;
			find2 = false;
			
			for(int i=left_width;i<right_width-1;i++)///////为什么不是i<nWidth
			{
				
				if(tempimage[num][m-3][j][i] == 255 && !(find1))
				{
					m_Samplepart[m][k] =i;
					find1 = true;
					
				}
				else if((!find1) && (i == nWidth-2))//为什么不是nWidth-1
				{
					m_Samplepart[m][k] = nWidth - 1 ;
					m_Samplepart[m][k+1] = nWidth - 1;//!!!need further consideration
					//endfind = true;
					break;
				}
				if (find1 && (!find2))
				{
					if((tempimage[num][m-3][j][ i] == 0) && (tempimage[num][m-3][j][i+1] == 255))
					{
						m_Samplepart[m][k+1] = i + 1;
						find2 = true;
						// endfind = true;
						break;
					}
					else if((!find2) && (i == nWidth-2))	  
					{
						m_Samplepart[m][k+1] = nWidth - 1;
						// endfind = true;
					}
				}
			}
		}
		
        //from right to left
		for(j=top_height;j<bottom_height;j++)
		{ 
			k += 2;
			find1 = false;
			find2 = false;
			
			for(int i=right_width-1;i>left_width;i--)
			{
				
				if(tempimage[num][m-3][j][i] == 255 && !(find1))
				{
					m_Samplepart[m][k] =i;
					find1 = true;
				}
				else if((!find1) && (i == 1))
				{
					m_Samplepart[m][k] = 0 ;
					m_Samplepart[m][k+1] = 0;//!!!need further consideration
					break;
				}
				if (find1 && (!find2))
				{
					if((tempimage[num][m-3][j][ i] == 0) && (tempimage[num][m-3][j][i-1] == 255))
					{
						m_Samplepart[m][k+1] = i-1;
						find2 = true;
						break;
					}
					else if((!find2) && (i == 1))	  
					{
						m_Samplepart[m][k+1] = 0;
					}
				}
			}
		}
		
		//from up to down
		for(int i=left_width;i<right_width;i++)
		{ 
			k += 2;
			find1 = false;
			find2 = false;
			
			for(j=top_height;j<bottom_height-1;j++)
			{
				
				if(tempimage[num][m-3][j][i] == 255 && !(find1))
				{
					m_Samplepart[m][k] = j;
					find1 = true;
				}
				else if((!find1) && (j == nHeight-2))
				{
					m_Samplepart[m][k] = nHeight - 1 ;
					m_Samplepart[m][k+1] = nHeight - 1;//!!!need further consideration
					break;
				}
				if (find1 && (!find2))
				{
					if((tempimage[num][m-3][j][i] == 0) && (tempimage[num][m-3][j+1][i] == 255))
					{
						m_Samplepart[m][k+1] = j+1;
						find2 = true;
						break;
					}
					else if((!find2) && (j == nHeight - 2))	  
					{
						m_Samplepart[m][k+1] = nHeight -1;
					}
				}
			}
		}
		
		//from down to up
		for(i=left_width;i<right_width;i++)
		{ 
			k += 2;
			find1 = false;
			find2 = false;
			
			for(j=bottom_height-1;j>top_height;j--)
			{
				
				if(tempimage[num][m-3][j][i] == 255 && !(find1))
				{
					m_Samplepart[m][k] = j;
					find1 = true;
				}
				else if((!find1) && (j == 1))
				{
					m_Samplepart[m][k] = 0 ;
					m_Samplepart[m][k+1] = 0;//!!!need further consideration
					break;
				}
				if (find1 && (!find2))
				{
					if((tempimage[num][m-3][j][i] == 0) && (tempimage[num][m-3][j-1][i] == 255))
					{
						m_Samplepart[m][k+1] = j-1;
						find2 = true;
						break;
					}
					else if((!find2) && (j == 1))	  
					{
						m_Samplepart[m][k+1] = 0;
					}
				}
			}
		}
		
	}
	//字符模板特征



	int Distance[6] ;
	for(i=0;i<6;i++)
	{
		Distance[i] =0;
	}
		
	for(i=0;i<200;i++)
	{
		Distance[0] += (abs(m_Profile[i] - m_Samplepart[0][i]));

		Distance[1] += (abs(m_Profile[i] -  m_Samplepart[1][i]));

		Distance[2] += (abs(m_Profile[i] -  m_Samplepart[2][i]));
       
		Distance[3] += (abs(m_Profile[i] -  m_Samplepart[3][i])); 
		
		
        Distance[4] += (abs(m_Profile[i] -  m_Samplepart[4][i]));  

	
        Distance[5] += (abs(m_Profile[i] -  m_Samplepart[5][i]));  
	}
	

	int resultend =0;
	int mindistance=Distance[0];
	for(i=1;i<6;i++)
	{
		if(mindistance >Distance[i])
		{
			mindistance =Distance[i];
			resultend =i;
		}
	}

	if(resultend>2)
	{
		return Result2;
	}
	else
	{
		return Result1;
	}

}
BOOL CProfile::LoadModels(char *modelname,int modelwidth,CSampleProfileArray &models)
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
	SampleProfile tempsample;
	for(i=0; i<count; i++)
	{
        in >> tempsample.label;
		tempsample.data = new int[200];
		for(int l=0; l<modelwidth; l++)
			in >> tempsample.data[l];
		models.Add(tempsample);
		
	}
    in.close();
	return TRUE;
}

int CProfile::Char_To_Int3(char &Character)//字母到数字的转换
{
    int Number;
	switch(Character)
	{
	case '0' : Number = 0;
		break;
	case '1' : Number = 1;
		break;
	case '2' : Number = 2;
		break;
	case '3' : Number = 3;
		break;
	case '4' : Number = 4;
		break;
	case '5' : Number = 5;
		break;
	case '6' : Number = 6;
		break;
	case '7' : Number = 7;
		break;
	case '8' : Number = 8;
		break;
	case '9' : Number = 9;
		break;
	case 'A' : Number = 10;
		break;
	case 'B' : Number = 11;
		break;
	case 'C' : Number = 12;
		break;
	case 'D' : Number = 13;
		break;
	case 'E' : Number = 14;
		break;
	case 'F' : Number = 15;
		break;
	case 'G' : Number = 16;
		break;
	case 'H' : Number = 17;
		break;
	case 'I' : Number = 18;
		break;
	case 'J' : Number = 19;
		break;
	case 'K' : Number = 20;
		break;
	case 'L' : Number = 21;
		break;
	case 'M' : Number = 22;
		break;
	case 'N' : Number = 23;
		break;
	case 'P' : Number = 24;
		break;
	case 'Q' : Number = 25;
		break;
	case 'R' : Number = 26;
		break;
	case 'S' : Number = 27;
		break;
	case 'T' : Number = 28;
		break;
	case 'U' : Number = 29;
		break;
	case 'V' : Number = 30;
		break;
	case 'W' : Number = 31;
		break;
	case 'X' : Number = 32;
		break;
	case 'Y' : Number = 33;
		break;
	case 'Z' : Number = 34;
		break;
	default : AfxMessageBox("error");
	}
	return Number;
}
