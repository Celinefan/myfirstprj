// DongleCheck.h : General Plate Rec fucs
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(SCU_III_DONGLECHECK_INCLUDED_)
#define SCU_III_DONGLECHECK_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BOOL TransformArray(unsigned char* &QueryArray, int num, int random, int& nCellCnt);
BOOL CompareDogData(unsigned int locatequery,unsigned char*response_20, unsigned char* g_QueryArray, int nCellCnt);
BOOL QueryDog(unsigned int k, unsigned int locatequery, unsigned char* response_20, unsigned char* g_QueryArray, int nCellCnt );
BOOL TestDog(unsigned char num, unsigned int groupNum, unsigned int queryNum);
BOOL CheckDog();
BOOL CheckDog( unsigned char num );
/*
#define  CompareDogData(locatequery,response_20, g_QueryArray)  \
{\
		int m_spStatus = memcmp(response_20, &g_QueryArray[20*locatequery*2 + 20], 20);\
		if ( 0 != m_spStatus )\
		{\
			exit(0);\
		}\
}

#define QueryDog(k, locatequery, response_20, g_QueryArray)   \
{\
	RB_SPRO_APIPACKET ApiPacket;\
	SP_STATUS spStatusWW = RNBOsproFormatPacket( ApiPacket, sizeof(ApiPacket) ); \
	if ( 0 != spStatusWW) \
	{ \
		exit(0);\
	} \
	spStatusWW = RNBOsproInitialize(ApiPacket); \
	if ( 0 != spStatusWW) \
	{ \
		exit(0);\
	} \
	spStatusWW = RNBOsproFindFirstUnit( ApiPacket, 0xF230 ); \
	if ( 0 != spStatusWW ) \
	{ \
		exit(0);\
	} \
	SP_STATUS spStatus = RNBOsproQuery(ApiPacket,   \
								 k, \
								 &g_QueryArray[20*locatequery*2], \
								 response_20, \
								 NULL,\
								 20 );\
}


#define TestDog(num, groupNum, queryNum) \
{	\
	unsigned char response10[] = {0,0,0,0,0,\
							      0,0,0,0,0,\
							      0,0,0,0,0,\
							      0,0,0,0,0}; \
	unsigned char* tempQueryArray = NULL;\
	TransformArray(tempQueryArray, num, groupNum); \
	QueryDog(num, queryNum, response10, tempQueryArray); \
	CompareDogData(queryNum, response10, tempQueryArray); \
}
//*/

#endif