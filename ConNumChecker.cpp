#include "StdAfx.h"
#include <math.h>
#include "ConNumChecker.h"

char Getcheckchar(CString strnum);
int GetCharValue(char c);

BOOL JudgeRecRes( CString strRes )
{
	ASSERT( strRes.GetLength() == 11 );
	char cLast = strRes.GetAt(10);

	return cLast == Getcheckchar(strRes);
}

char Getcheckchar(CString strnum)
{
	int sum = 0;
	char jiaoyanwei;
	int jiaoyanma;
	int charvalue;
	int mm = strnum.GetLength();
	int i;
	for(i = 0; i < mm - 1; i++)
	{
 		char c = strnum.GetAt(i);
	
 		charvalue = GetCharValue(c);
		int qiumizhi = pow(2,i);
 		sum = sum + charvalue*qiumizhi;
	}
	jiaoyanma = sum%11;
	if( jiaoyanma == 10 )
	{
		jiaoyanma = 0;
	}
	jiaoyanwei = char(jiaoyanma+int('0')); 
	return jiaoyanwei;

}

int GetCharValue(char c)
{
	int chartovalue;
	if('A' <= c && c <= 'Z')
	{
		if(c == 'A')
		{ 
			chartovalue = 10;
		}
		else if(c == 'B')
		{
			chartovalue = 12;
		}
		else if(c == 'C')
		{
			chartovalue = 13;
		}
		else if(c == 'D')
		{
			chartovalue = 14;
		}
		else if(c == 'E')
		{
			chartovalue = 15;
		}
		else if(c == 'F')
		{
			chartovalue = 16;
		}
		else if(c == 'G')
		{
			chartovalue = 17;
		}
		else if(c == 'H')
		{
			chartovalue = 18;
		}
		else if(c == 'I')
		{
			chartovalue = 19;
		}
		else if(c == 'J')
		{
			chartovalue = 20;
		}
		else if(c == 'K')
		{
			chartovalue = 21;
		}
		else if(c == 'L')
		{
			chartovalue = 23;
		}
		else if(c == 'M')
		{
			chartovalue = 24;
		}
		else if(c == 'N')
		{
			chartovalue = 25;
		}
		else if(c == 'O')
		{
			chartovalue = 26;
		}
		else if(c == 'P')
		{
			chartovalue = 27;
		}
		else if(c == 'Q')
		{
			chartovalue = 28;
		}
		else if(c == 'R')
		{
			chartovalue = 29;
		}
		else if(c == 'S')
		{
			chartovalue = 30;
		}
		else if(c == 'T')
		{
			chartovalue = 31;
		}
		else if(c == 'U')
		{
			chartovalue = 32;
		}
		else if(c == 'V')
		{
			chartovalue = 34;
		}
		else if(c == 'W')
		{
			chartovalue = 35;
		}
		else if(c == 'X')
		{
			chartovalue = 36;
		}
		else if(c == 'Y')
		{
			chartovalue = 37;
		}
		else if(c == 'Z')
		{
			chartovalue = 38;
		}
	}
	if('0' <= c && c <= '9')
	{
		if(c == '0')
		{
			chartovalue = 0;
		}
		else if(c == '1')
		{
			chartovalue = 1;
		}
		else if(c == '2')
		{
			chartovalue = 2;
		}
		else if(c == '3')
		{
			chartovalue = 3;
		}
		else if(c == '4')
		{
			chartovalue = 4;
		}
		else if(c == '5')
		{
			chartovalue = 5;
		}
		else if(c == '6')
		{
			chartovalue = 6;
		}
		else if(c == '7')
		{
			chartovalue = 7;
		}
		else if(c == '8')
		{
			chartovalue = 8;
		}
		else if(c == '9')
		{
			chartovalue = 9;
		}
 	}
	
	return chartovalue;

}
