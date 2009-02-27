#include "stdafx.h"
#include "DongleCheck.h"

#include "DogCheck\\Code08_1.h"
#include "DogCheck\\Code08_2.h"
#include "DogCheck\\Code08_3.h"
#include "DogCheck\\Code08_4.h"
#include "DogCheck\\Code0a_1.h"
#include "DogCheck\\Code0a_2.h"
#include "DogCheck\\Code0a_3.h"
#include "DogCheck\\Code0a_4.h"
#include "DogCheck\\Code0c_1.h"
#include "DogCheck\\Code0c_2.h"
#include "DogCheck\\Code0c_3.h"
#include "DogCheck\\Code0c_4.h"
#include "DogCheck\\Code0e_1.h"
#include "DogCheck\\Code0e_2.h"
#include "DogCheck\\Code0e_3.h"
#include "DogCheck\\Code0e_4.h"
#include "DogCheck\\Code10_1.h"
#include "DogCheck\\Code10_2.h"
#include "DogCheck\\Code10_3.h"
#include "DogCheck\\Code10_4.h"
#include "DogCheck\\Code12_1.h"
#include "DogCheck\\Code12_2.h"
#include "DogCheck\\Code12_3.h"
#include "DogCheck\\Code12_4.h"
#include "DogCheck\\Code12_5.h"
#include "DogCheck\\Code16_1.h"
#include "DogCheck\\Code16_2.h"
#include "DogCheck\\Code16_3.h"
#include "DogCheck\\Code16_4.h"

BOOL TransformArray(unsigned char* &QueryArray, int num, int random, int& nCellCnt)
{	
	BOOL bSuc = TRUE;
	nCellCnt = 0;
	if (num == 0x08)
	{
		nCellCnt = 8;
		switch(random)
		{
		case 0:
			QueryArray = &QueryResponses08_1[0][0][0];
			bSuc = TRUE;
			break;
		case 1:
			QueryArray = &QueryResponses08_2[0][0][0];
			bSuc = TRUE;
			break;
		case 2:
			QueryArray = &QueryResponses08_3[0][0][0];
			bSuc = TRUE;
			break;
		case 3:
			QueryArray = &QueryResponses08_4[0][0][0];
			bSuc = TRUE;
			break;
		}
	}

	if (num == 0x10)
	{
		nCellCnt = 8;
		switch(random)
		{
		case 0:
			QueryArray = &QueryResponses10_1[0][0][0];
			bSuc = TRUE;
			break;
		case 1:
			QueryArray = &QueryResponses10_2[0][0][0];
			bSuc = TRUE;
			break;
		case 2:
			QueryArray = &QueryResponses10_3[0][0][0];
			bSuc = TRUE;
			break;
		case 3:
			QueryArray = &QueryResponses10_4[0][0][0];
			bSuc = TRUE;
			break;
		}
	}

	if (num == 0x0A)
	{
		nCellCnt = 6;
		switch(random)
		{
		case 0:
			QueryArray = &QueryResponses0A_1[0][0][0];
			bSuc = TRUE;
			break;
		case 1:
			QueryArray = &QueryResponses0A_2[0][0][0];
			bSuc = TRUE;
			break;
		case 2:
			QueryArray = &QueryResponses0A_3[0][0][0];
			bSuc = TRUE;
			break;
		case 3:
			QueryArray = &QueryResponses0A_4[0][0][0];
			bSuc = TRUE;
			break;
		}
	}


	if( num == 0x0C )
	{
		nCellCnt = 10;
		switch(random)
		{
		case 0:
			QueryArray = &QueryResponses0C_1[0][0][0];
			bSuc = TRUE;
			break;
		case 1:
			QueryArray = &QueryResponses0C_2[0][0][0];
			bSuc = TRUE;
			break;
		case 2:
			QueryArray = &QueryResponses0C_3[0][0][0];
			bSuc = TRUE;
			break;
		case 3:
			QueryArray = &QueryResponses0C_4[0][0][0];
			bSuc = TRUE;
			break;
		}
	}

	if( num == 0x0E )
	{
		nCellCnt = 6;
		switch(random)
		{
		case 0:
			QueryArray = &QueryResponses0E_1[0][0][0];
			bSuc = TRUE;
			break;
		case 1:
			QueryArray = &QueryResponses0E_2[0][0][0];
			bSuc = TRUE;
			break;
		case 2:
			QueryArray = &QueryResponses0E_3[0][0][0];
			bSuc = TRUE;
			break;
		case 3:
			QueryArray = &QueryResponses0E_4[0][0][0];
			bSuc = TRUE;
			break;
		}
	}

	
	if( num == 0x12 )
	{
		nCellCnt = 12;
		switch(random)
		{
		case 0:
			QueryArray = &QueryResponses12_1[0][0][0];
			bSuc = TRUE;
			break;
		case 1:
			QueryArray = &QueryResponses12_2[0][0][0];
			bSuc = TRUE;
			break;
		case 2:
			QueryArray = &QueryResponses12_3[0][0][0];
			bSuc = TRUE;
			break;
		case 3:
			QueryArray = &QueryResponses12_4[0][0][0];
			bSuc = TRUE;
			break;
		}
	}

	if( num == 0x16 )
	{
		nCellCnt = 8;
		switch(random)
		{
		case 0:
			QueryArray = &QueryResponses16_1[0][0][0];
			bSuc = TRUE;
			break;
		case 1:
			QueryArray = &QueryResponses16_2[0][0][0];
			bSuc = TRUE;
			break;
		case 2:
			QueryArray = &QueryResponses16_3[0][0][0];
			bSuc = TRUE;
			break;
		case 3:
			QueryArray = &QueryResponses16_4[0][0][0];
			bSuc = TRUE;
			break;
		}
	}


	return bSuc;
}

BOOL  CompareDogData(unsigned int locatequery,unsigned char*response_20, unsigned char* g_QueryArray, int nCellCnt)
{
		//int m_spStatus = memcmp(response_20, &g_QueryArray[20*locatequery*2 + 20], 20);
		int m_spStatus = memcmp(response_20, &g_QueryArray[nCellCnt*locatequery*2 + nCellCnt], nCellCnt);
		if ( 0 != m_spStatus )
		{
			return FALSE;
		}
		return TRUE;
}

BOOL QueryDog(unsigned int k, unsigned int locatequery, unsigned char* response_20, unsigned char* g_QueryArray, int nCellCnt)
{
	RB_SPRO_APIPACKET ApiPacket;
	SP_STATUS spStatusWW = RNBOsproFormatPacket( ApiPacket, sizeof(ApiPacket) );
	if ( 0 != spStatusWW)
	{
		return FALSE;
	}
	spStatusWW = RNBOsproInitialize(ApiPacket);
	if ( 0 != spStatusWW)
	{
		return FALSE;
	}
	spStatusWW = RNBOsproFindFirstUnit( ApiPacket, 0xF230 );
	if ( 0 != spStatusWW )
	{
		return FALSE;
	}
	SP_STATUS spStatus = RNBOsproQuery(ApiPacket,
								 k,
								 //&g_QueryArray[20*locatequery*2],
								 &g_QueryArray[nCellCnt*locatequery*2],
								 response_20,
								 NULL,
								 //20
								 nCellCnt
								 );
	return TRUE;
}


BOOL TestDog( unsigned char num, unsigned int groupNum, unsigned int queryNum )
{
	unsigned char response10[] = {0,0,0,0,0,
							      0,0,0,0,0,
							      0,0,0,0,0,
							      0,0,0,0,0};
	unsigned char* tempQueryArray = NULL;
	int nCellCnt = 0;
	TransformArray(tempQueryArray, num, groupNum, nCellCnt);
	if( !QueryDog(num, queryNum, response10, tempQueryArray, nCellCnt) )
		return FALSE;
	if( !CompareDogData(queryNum, response10, tempQueryArray, nCellCnt) )
		return FALSE;
	return TRUE;
}

BOOL CheckDog()
{
	srand((unsigned)time(NULL));  //generateor the random number
	unsigned int Randomdada = rand();  //get random number
	Randomdada = Randomdada % 4;
	unsigned int RandNum = rand();  //get random number
	RandNum = RandNum % 300;
	return TestDog(0x12, Randomdada, RandNum);
}

BOOL CheckDog( unsigned char num )
{
	srand((unsigned)time(NULL));  //generateor the random number
	unsigned int Randomdada = rand();  //get random number
	Randomdada = Randomdada % 4;
	unsigned int RandNum = rand();  //get random number
	RandNum = RandNum % 300;
	return TestDog(num, Randomdada, RandNum);
}
