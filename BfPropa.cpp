#include "StdAfx.h"
#include "BfPropa.h"

bool initObNode( ObNode& obj, int nFeaLen )
{
	int i = 0;
	if( obj.pfFea != NULL )
	{
		delete[] obj.pfFea;
		obj.pfFea = NULL;
	}
	obj.nFeaLen = 0;

	obj.nFeaLen = nFeaLen;
	obj.pfFea = new float[nFeaLen];
	
	for( i = 0; i < nFeaLen; i++ )
	{
		obj.pfFea[i] = 0.0f;
	}

	return true;
}

void freeObNode( ObNode& obj )
{
	if( obj.pfFea != NULL )
	{
		delete[] obj.pfFea;
		obj.pfFea = NULL;
	}
	obj.nFeaLen = 0;
}

void writeObNodes( ofstream& ndInfoFile, ObNode* pObj, int nNodes, int nFealen )
{
	int i = 0;
	int j = 0;
	for( i = 0 ; i < nNodes; i++ )
	{
		ndInfoFile << i << "th node\t:\t";
		for( j = 0; j < nFealen; j++ )
		{
			ndInfoFile << pObj[i].pfFea[j] << "\t";
		}
		ndInfoFile << endl;
	}
	ndInfoFile << endl;
}
void writeHiNodes( ofstream& ndInfoFile, HiNode* pObj, int nNodes,int nCands, int nFealen )
{
	int i = 0;
	int j = 0;
	int k = 0;
	for( i = 0; i < nNodes; i++ )
	{
		ndInfoFile << i << "th node : " << endl;
		for( j = 0; j < nCands; j++ )
		{
			//ndInfoFile << "\t" << j << "th cand : " << "\t";
			ndInfoFile << "( ";
			for( k = 0; k < nFealen; k++ )
			{
				ndInfoFile << pObj[i].pfFea[ j * nFealen + k ];
				if( k != nFealen - 1 )
				{
					ndInfoFile <<"; ";
				}
			}
			ndInfoFile << " )\t";
		}
		ndInfoFile << endl;
	}
	ndInfoFile << endl;
}
void freeHiNode( HiNode& obj )
{
	if( obj.pfFea != NULL )
	{
		delete[] obj.pfFea;
		obj.pfFea = NULL;
	}
	obj.nFeaLen = 0;
	obj.nCandsNum = 0;
}

void freeCandMessage( CandMessage& CdMessage )
{
	if( CdMessage.msg != NULL )
	{
		delete[] CdMessage.msg;
		CdMessage.msg = NULL;
	}
	CdMessage.nNbCandsNum = 0;
}
void freeNodeMessage( NodeMessage& NdMessage )
{
	int i = 0;
	for( i = 0; i < NdMessage.nCandsNum; i++ )
	{
		freeCandMessage( NdMessage.CdMsg_L[i] );
		freeCandMessage( NdMessage.CdMsg_R[i] );
	}

	if( NdMessage.CdMsg_L != NULL )
	{
		delete[] NdMessage.CdMsg_L;
		NdMessage.CdMsg_L = NULL;
	}
	if( NdMessage.CdMsg_R != NULL )
	{
		delete[] NdMessage.CdMsg_R;
		NdMessage.CdMsg_R = NULL;
	}

	if( NdMessage.OHMsg != NULL )
	{
		delete[] NdMessage.OHMsg;
		NdMessage.OHMsg = NULL;
	}
	if( NdMessage.HsMsg != NULL )
	{
		delete[] NdMessage.HsMsg;
		NdMessage.HsMsg = NULL;
	}

	NdMessage.nCandsNum = 0;
}

void freeBeliefMessage( BeliefMessage& BfMessage )
{
	int i = 0;
	for( i = 0; i < BfMessage.nNodesNum; i++ )
	{
		freeNodeMessage( BfMessage.NdMsg[i] );
	}

	if( BfMessage.NdMsg != NULL )
	{
		delete[] BfMessage.NdMsg;
		BfMessage.NdMsg = NULL;
	}

	BfMessage.nNodesNum = 0;
}
bool getMessageInfo( float* &msg, int &nRows, int &nCols, BeliefMessage* BfMessage, int nthNode, int nthItem )
//nthItem = 0 : pNdMsg->OHMsg;
//nthItem = 1 : pNdMsg->HsMsg;
//nthItem = 2 : pNdMsg->CdMsg_R;
//nthItem = 3 : pNdMsg->CdMsg_L;
{
	msg = NULL;
	nRows = 0;
	nCols = 0;
	if( BfMessage == NULL ) return false;
	int nNodesNum = BfMessage->nNodesNum;
	if( nthNode >= nNodesNum ) return false;


	NodeMessage* pNdMsg = &(BfMessage->NdMsg[nthNode]);
	int nCands = pNdMsg->nCandsNum;
	float* pf = NULL;
	CandMessage* pCdMsg = NULL;
	switch( nthItem )
	{
	case 0:
		nRows = 1;
		nCols = nCands;
		pf = pNdMsg->OHMsg;
		break;
	case 1:
		nRows = nCands;
		nCols = nCands;
		pf = pNdMsg->HsMsg;
		break;
	case 2:
		nRows = nCands;
		nCols = nCands;
		pCdMsg = pNdMsg->CdMsg_R;
		break;
	case 3:
		nRows = nCands;
		nCols = nCands;
		pCdMsg = pNdMsg->CdMsg_L;
		break;
	default:
		return false;
	}

	msg = new float[nRows*nCols];

	int i = 0;
	int j = 0;
	switch( nthItem )
	{
	case 0:
	case 1:
		for( i = 0; i < nRows * nCols; i++ )
		{
			msg[i] = pf[i];
		}
		break;
	case 2:
	case 3:
		for( i = 0; i < nRows; i++ )
		{
			for( j = 0; j < nCols; j++ )
			{
				msg[ i * nCols + j ] = pCdMsg[i].msg[j];
			}
		}
		break;
	}

	return true;
}
CBfPropa::CBfPropa(void)
{
	m_obNodes = NULL;
	m_hiNodes = NULL;
	m_nSelection = NULL;

	m_pBfMsg = NULL;
#ifdef DEBUG_CBFPROPA
	m_strMainDebugDir = "BfPropaDebugDir\\";
	CreateDirectory( m_strMainDebugDir, NULL );
	m_strSubDebugDir = "";
	m_strCurDebugDir = "";
	m_strCurDebugFilename = "";
#endif
}

CBfPropa::~CBfPropa(void)
{
#ifdef _EX2FUC
	//delete[] m_obNodes;
	//delete[] m_hiNodes;
	m_obNodes = NULL;
	m_hiNodes = NULL;
	delete[] m_nSelection;
	m_nSelection = NULL;

	freeBeliefMessage( *m_pBfMsg );
	delete m_pBfMsg;
	m_pBfMsg = NULL;
#endif

	freeMem();
}



bool CBfPropa::SetStates( ObNode* obNodes, HiNode* hiNodes, int nNodes )//Only copy the pointers' address,
																										//do not release the corresponding mem before beliefPropagation
{
	freeMem();
	if( obNodes == NULL || hiNodes == NULL ) return false;

	m_obNodes = obNodes;
	m_hiNodes = hiNodes;
	m_nNodes = nNodes;

	bool bSuc = CreateBfMsgStruct();

	return bSuc;
}

bool CBfPropa::beliefPropagation( )
{
	bool bSuc = BfPropagation();

	if( bSuc )
	{
		bSuc = GetBfPropaRes();
	}

	return bSuc;
}

bool CBfPropa::GetResult( int* &res, int& nlen, float &fConf )
{
	fConf = 0.0f;
	if( res != NULL ) return false;
	if( m_nSelection == NULL ) return false;

	int i = 0;

	res = new int[m_nNodes];
	nlen = m_nNodes;

	for( i = 0; i < m_nNodes;i++ )
	{
		res[i] = m_nSelection[i];
	}
	fConf = m_fConf;
	return true;
}

BeliefMessage* CBfPropa::GetBfMsgPointer()
{
	return m_pBfMsg;
}

bool CBfPropa::BfPropagation( )
{
	bool bSuc = true;

	bSuc = IniBfMsg();
	if( !bSuc ) return false;
#ifdef DEBUG_CBFPROPA
	m_strCurDebugDir = m_strMainDebugDir + "BfMsgInitialization\\";
	CreateDirectory( m_strCurDebugDir, NULL );
	writeBfMsgInfo( );
#endif

	bSuc = BfMsgPropa( 1 );
	if( !bSuc ) return false;

#ifdef DEBUG_CBFPROPA
	m_strCurDebugDir = m_strMainDebugDir + "BfMsgPropagation\\";
	CreateDirectory( m_strCurDebugDir, NULL );
	writeBfMsgInfo( );
#endif


	bSuc = GetBfPropaRes( );

	return bSuc;
}

bool CBfPropa::CreateBfMsgStruct()
{
	if( m_hiNodes == NULL ) return false;
	if( m_pBfMsg != NULL )
	{
		freeBeliefMessage( *m_pBfMsg );
		delete m_pBfMsg;
		m_pBfMsg = NULL;
	}

	m_pBfMsg = new BeliefMessage;

	int i = 0;
	int j = 0;
	m_pBfMsg->nNodesNum = m_nNodes;
	m_pBfMsg->NdMsg = new NodeMessage[m_nNodes];
	int nCandsNum = 0;
	void* p = NULL;
	for( i = 0; i < m_nNodes; i++ )
	{
		NodeMessage* pNodeMsg = &(m_pBfMsg->NdMsg[i]);
		pNodeMsg->OHMsg = NULL;
		pNodeMsg->HsMsg = NULL;
		pNodeMsg->CdMsg_L = NULL;
		pNodeMsg->CdMsg_R = NULL;
		nCandsNum = m_hiNodes[i].nCandsNum;
		pNodeMsg->nCandsNum = nCandsNum;
		pNodeMsg->OHMsg = new float[nCandsNum];
		int ii = 0;
		for( ii = 0; ii < nCandsNum; ii++ )
		{
			pNodeMsg->OHMsg[ii] = 0.0f;
		}
		pNodeMsg->HsMsg = new float[ nCandsNum * nCandsNum ];
		for( ii = 0; ii < nCandsNum * nCandsNum; ii ++ )
		{
			pNodeMsg->HsMsg[ii] = 0.0f;
		}
		pNodeMsg->CdMsg_L = new CandMessage[nCandsNum];
		pNodeMsg->CdMsg_R = new CandMessage[nCandsNum];
		for( j = 0; j < nCandsNum; j++ )
		{
			CandMessage* pCandMsg = &(m_pBfMsg->NdMsg[i].CdMsg_L[j]);
			pCandMsg->msg = NULL;
			pCandMsg->nNbCandsNum = nCandsNum;
			pCandMsg->msg = new float[nCandsNum];
			//memset( pCandMsg->msg, 0, sizeof(float)*nCandsNum );
			int jj = 0;
			for( jj = 0; jj < nCandsNum; jj++ )
			{
				pCandMsg->msg[jj] = 0.0f;
			}

			pCandMsg = &( m_pBfMsg->NdMsg[i].CdMsg_R[j] );
			pCandMsg->msg = NULL;
			pCandMsg->nNbCandsNum = nCandsNum;
			pCandMsg->msg = new float[nCandsNum];
			//memset( pCandMsg->msg, 0, sizeof(float)*nCandsNum );
			for( jj = 0; jj < nCandsNum; jj++ )
			{
				pCandMsg->msg[jj] = 0.0f;
			}
		}
	}

	return true;
}
bool CBfPropa::IniBfMsg( )//equal to 1 Time propagation
{
	//if( !CreateBfMsgStruct() ) return false;

	bool bSuc = true;

	int i = 0;
	int j = 0;
	int k = 0;

	int nCands = 0;
	for( i = 0; i < m_nNodes; i++ )//Input OH dis
	{
		nCands = m_hiNodes[i].nCandsNum;
		float* fOHMsg = NULL;

		bSuc = getOHMsg( m_obNodes[i], m_hiNodes[i], fOHMsg );
		if(!bSuc) break;

		for( j = 0; j < nCands; j++ )
		{
			for( k = 0; k < nCands; k++ )
				m_pBfMsg->NdMsg[i].OHMsg[k] = fOHMsg[k];
		}
		delete[] fOHMsg;				

	}
	//ASSERT( bSuc );
	if( !bSuc ) return false;

	for( i = 0; i < m_nNodes - 1; i++ )
	{
		float* fHsMsg = NULL;
		bSuc = getHsMsg( m_hiNodes[i], m_hiNodes[i+1], fHsMsg );
		if( !bSuc ) break;

		for( j = 0; j < nCands; j++ )
		{
			for( k = 0; k < nCands; k++ )
			{
				float fMsg = fHsMsg[ j * nCands + k ];
				m_pBfMsg->NdMsg[i].HsMsg[ j * nCands + k ] = fMsg;
			}
		}
		delete[] fHsMsg;

	}

	return true;

}
bool CBfPropa::BfMsgPropa( int nTimes )
{
	if( m_pBfMsg == NULL ) return false;
	if( m_nNodes <= 1 ) return false;
	bool bSuc = true;

	int i = 0;
	int j = 0;
	int k = 0;

	int nCands = 0;
	nCands = m_pBfMsg->NdMsg[0].nCandsNum;
	if( nTimes == 0 ) nTimes = nCands - 1;

	int nCurTimes = 0;

	for( nCurTimes = 0; nCurTimes < nTimes; nCurTimes++ )
	{
		for( i = 0; i < m_nNodes - 1; i++ )
		{
			nCands = m_pBfMsg->NdMsg[i].nCandsNum;
			NodeMessage* pCurNode = &(m_pBfMsg->NdMsg[i]);
			NodeMessage* pNxtNode = &(m_pBfMsg->NdMsg[i+1]);
			for( j = 0; j < nCands; j++ )
			{
				for( k = 0; k < nCands; k++ )
				{
					float fMaxVal = 0.0f;
					int nMaxValID = 0;
					bSuc = getMaxMsg( fMaxVal, nMaxValID, pNxtNode->CdMsg_L[k].msg, nCands );
					if( !bSuc ) break;
					//pCurNode->CdMsg_R[j].msg[k] += pCurNode->HsMsg[ j * nCands + k ]
					//+ pNxtNode->OHMsg[k] + fMaxVal;
					//First save the right part in the following val, i.e. not immediately update pCurNode->CdMsg_R[j].msg[k];
					//Because the previous value of pCurNode->CdMsg_R[j].msg[k] is still in need.
					float NewCurRMsg = pCurNode->HsMsg[ j * nCands + k ]
					+ pNxtNode->OHMsg[k] + fMaxVal;


					bSuc = getMaxMsg( fMaxVal, nMaxValID, pCurNode->CdMsg_R[j].msg, nCands );
					if( !bSuc ) break;
					//pNxtNode->CdMsg_L[k].msg[j] += pCurNode->HsMsg[ j * nCands + k ] 
					//+ pCurNode->OHMsg[j] + fMaxVal;
					float NewNxtLMsg = pCurNode->HsMsg[ j * nCands + k ] 
					+ pCurNode->OHMsg[j] + fMaxVal;

					pCurNode->CdMsg_R[j].msg[k] += NewCurRMsg;
					pNxtNode->CdMsg_L[k].msg[j] += NewNxtLMsg;
				}

				if( !bSuc ) break;
			}
			pCurNode = NULL;
			pNxtNode = NULL;

			if( !bSuc ) break;
		}

		if( !bSuc ) break;
	}


	return bSuc;
}
bool CBfPropa::GetBfPropaRes( )
{
	if( m_pBfMsg == NULL ) return false;
	if( m_nNodes <= 0 ) return false;

	if( m_nSelection != NULL )
	{
		delete[] m_nSelection;
		m_nSelection = NULL;
	}
	m_nSelection = new int[m_nNodes];

	int i = 0;
	int j = 0;

	int nCands = m_pBfMsg->NdMsg[0].nCandsNum;
	float* candsProb = new float[nCands];

	for( i = 0; i < m_nNodes; i++ )
	{
		nCands = m_hiNodes[i].nCandsNum;
		for( j = 0; j < nCands; j++ )
		{
			NodeMessage* pCurNdMsg = &( m_pBfMsg->NdMsg[i] );
			float fMaxVal_L = 0.0f;
			float fMaxVal_R = 0.0f;
			int nMaxValID = 0;

			getMaxMsg( fMaxVal_L, nMaxValID, pCurNdMsg->CdMsg_L[j].msg, nCands );
			getMaxMsg( fMaxVal_R, nMaxValID, pCurNdMsg->CdMsg_R[j].msg, nCands );

			candsProb[j] = pCurNdMsg->OHMsg[j] + fMaxVal_L + fMaxVal_R;

			pCurNdMsg = NULL;
		}

		float fMaxVal = 0.0f;
		int nID = 0;
		getMaxMsg( fMaxVal, nID, candsProb, nCands );
		m_nSelection[i] = nID;

	}

	m_fConf = 0.0f;
	int nFeaLen = m_hiNodes[0].nFeaLen;
	for( i = 0; i < m_nNodes; i++ )
	{
		for( j = 0; j < nFeaLen; j++ )
		{
			int nCandID = m_nSelection[i];
			float dis = m_obNodes[i].pfFea[j] - m_hiNodes[i].pfFea[ nCandID * nFeaLen + j];
			m_fConf += dis * dis;
		}
	}
	for( i = 0; i < m_nNodes - 1 ; i++ )
	{
		for( j = 0; j < nFeaLen; j++ )
		{
			int nCurCandID = m_nSelection[i];
			int nNxtCandID = m_nSelection[i+1];
			float dis = m_hiNodes[i].pfFea[ nCurCandID * nFeaLen + j ]
			- m_hiNodes[i+1].pfFea[ nNxtCandID * nFeaLen + j ];
			m_fConf += dis * dis;
		}
	}

	delete[] candsProb;

	return true;
}
bool CBfPropa::getMaxMsg( float& fMaxVal, int& nMaxValID, float* pArray, int nLen )
{
	if( pArray == NULL ) return false;

	fMaxVal = pArray[0];
	nMaxValID = 0;
	int i = 0;
	for( i = 0; i < nLen; i++ )
	{
		if( pArray[i] > fMaxVal )
		{
			fMaxVal = pArray[i];
			nMaxValID = i;
		}
	}

	return true;
}
bool CBfPropa::getOHMsg( ObNode obNode, HiNode hiNode, float* &fOHMsg )
{
	int nFeaLen = obNode.nFeaLen;
	int nFealen_cmp = hiNode.nFeaLen;
	if( nFeaLen != nFealen_cmp ) return false;

	if( fOHMsg != NULL ) return false;

	int nCandsNum = hiNode.nCandsNum;
	fOHMsg = new float[nCandsNum];

	int i = 0;
	int j = 0;
	for( i = 0; i < nCandsNum; i++ )
	{
		float nTotalFeaDis = 0;
		for	( j = 0; j < nFeaLen; j++ )
		{
			float dis = obNode.pfFea[j] - hiNode.pfFea[ i * nFeaLen + j];
			nTotalFeaDis += dis * dis;
		}
		fOHMsg[i] = -nTotalFeaDis;
	}

	return true;
}

bool CBfPropa::getHsMsg( HiNode curNode, HiNode nxtNode, float* &fHsMsg )
{
	int nFeaLen = curNode.nFeaLen;
	int nFeaLen_cmp = nxtNode.nFeaLen;
	if( nFeaLen != nFeaLen_cmp ) return false;

	int nCands = curNode.nCandsNum;
	int nCands_cmp = nxtNode.nCandsNum;
	if( nCands != nCands_cmp ) return false;

	if( fHsMsg != NULL ) return false;
	fHsMsg = new float[nCands*nCands];
	//memset( fHsMsg, 0, sizeof( float ) * nCands * nCands );
	int nn = 0;
	for( nn = 0; nn < nCands*nCands; nn++ )
	{
		fHsMsg[nn] = 0.0f;
	}

	int i = 0;
	int j = 0;
	int k = 0;
	for( i = 0; i < nCands; i++ )
	{
		for( j = 0; j < nCands; j++ )
		{
			float nTotalFeaDis = 0;
			for( k = 0; k < nFeaLen; k++ )
			{
				float dis = curNode.pfFea[ i * nFeaLen + k ] - nxtNode.pfFea[ j * nFeaLen + k ];
				nTotalFeaDis += dis * dis;
			}
			fHsMsg[i * nCands + j] = -nTotalFeaDis;
		}
	}

	return true;
}

void CBfPropa::freeMem()
{
	m_obNodes = NULL;
	m_hiNodes = NULL;
	delete[] m_nSelection;
	m_nSelection = NULL;
	m_fConf = 0.0f;

	if( m_pBfMsg != NULL )
	{
		freeBeliefMessage( *m_pBfMsg );
		delete m_pBfMsg;
		m_pBfMsg = NULL;
	}

}
#ifdef DEBUG_CBFPROPA
bool CBfPropa::writeBfMsgInfo( )
{
	if( m_pBfMsg == NULL ) return false;
	int nNodes = m_pBfMsg->nNodesNum;
	if( nNodes == NULL ) return false;
	CString strFilename = "MsgInfo.txt";
	int i = 0;
	int j = 0;
	for( i = 0; i < nNodes; i++ )
	{
		CString strPrefix;
		strPrefix.Format( _T("Node%d_"), i );
		m_strCurDebugFilename = m_strCurDebugDir + strPrefix + strFilename;
		ofstream fileDebug;
		fileDebug.open( m_strCurDebugFilename, ios::out|ios::trunc );
		float* msg = NULL;
		int nRows = 0;
		int nCols = 0;
		for( j = 0; j < 4; j++ )
		{
			getMessageInfo( msg, nRows, nCols, m_pBfMsg, i, j );
			CString strTitle = _T("");
			switch( j )
			{
			case 0:
				strTitle = _T("OHMsg : ");
				fileDebug << "OHMsg : " << endl;
				break;
			case 1:
				strTitle = _T("HsMsg : ");
				fileDebug << "HsMsg : " << endl;
				break;
			case 2:
				strTitle = _T("CdMsg_R : ");
				fileDebug << "CdMsg_R : " << endl;
				break;
			case 3:
				strTitle = _T("CdMsg_L : ");
				fileDebug << "CdMsg_L : " << endl;
				break;
			default:
				strTitle = _T("UNKNOWN : ");
				fileDebug << "UNKNOWN : " << endl;
			}
			//fileDebug << strTitle << endl;
			int ii = 0;
			int jj = 0;
			for( ii = 0; ii < nRows; ii++ )
			{
				for( jj = 0; jj < nCols; jj++ )
				{
					fileDebug << msg[ ii * nCols + jj ] << "\t";
				}
				fileDebug << endl;
			}
			fileDebug << endl;
			delete[] msg;
		}
		fileDebug.close();
	}

	return true;
}
#endif





//######################################################
//----------------------------------Generate Nodes Functions-----------------------------------
//######################################################
//typedef CArray<int,int> INSMOD;//Insert mode
void writeOBJFEAStruct( ofstream& outfile, OBJFEA& item )
{
	outfile << "( " << item.cenx << ", " 
		<< item.ceny << ", "
		<< item.w << ", "
		<< item.h << " )"; 
}
void freeINSMOD( INSMOD& Item )
{
	delete[] Item.insertMode;
	Item.nlen = 0;
}
bool writePACStruct( ofstream& outFile, PACStruct& Item )
{
	int i = 0;
	int j = 0;

	int nPbs = Item.nPbs;
	if( nPbs <= 0 ) return false;

	outFile << nPbs << " possibilities : " << endl;
	for( i = 0; i < Item.nPbs; i++ )
	{
		int nlen = Item.InsModes[i].nlen;
		for( j = 0; j < nlen; j++ )
		{
			outFile << Item.InsModes[i].insertMode[j] << "\t";
		}
		outFile << endl;
	}

	return true;
}
void freePACStruct( PACStruct& Item )
{
	int i = 0;
	for( i = 0; i < Item.nPbs; i++ )
	{
		freeINSMOD(Item.InsModes[i]);
	}
	Item.InsModes.RemoveAll();
}
void FreeObjFramesStruct( objFrames& obj )
{
	delete[] obj.inputCharsTopBorder;
	delete[] obj.inputCharsBtmBorder;
	delete[] obj.inputCharsLeftBorder;
	delete[] obj.inputCharsRightBorder;
	//delete[] obj.inputCharsheight;
	//delete[] obj.inputCharsWidth;
	//delete[] obj.inputDisArray;
	//delete[] obj.inputSeqCharsArray;
	//delete[] obj.inputSeqDistsArray;	

	obj.nChars = 0;
	obj.averDis = 0;
	obj.averHeight = 0;
	obj.averWidth = 0;
	obj.inputCharsTopBorder = NULL;
	obj.inputCharsBtmBorder = NULL;
	obj.inputCharsLeftBorder = NULL;
	obj.inputCharsRightBorder = NULL;
	//obj.inputCharsheight = NULL;
	//obj.inputCharsWidth = NULL;
	//obj.inputDisArray = NULL;
	//obj.inputSeqCharsArray = NULL;
	//obj.inputSeqDistsArray = NULL;
}
void InitializeNodes( ObNode* &obNodes, HiNode* &hiNodes, int nNodes, int nFealen, int nCands )
{
	int i = 0;
	int j = 0;

	if( obNodes != NULL )
	{
		delete[]obNodes;
		obNodes = NULL;
	}

	if( hiNodes != NULL )
	{
		delete[]hiNodes;
		hiNodes = NULL;
	}

	obNodes = new ObNode[nNodes];
	hiNodes = new HiNode[nNodes];

	for( i = 0; i < nNodes; i++ )
	{
		obNodes[i].pfFea = NULL;
		obNodes[i].nFeaLen = 0;

		hiNodes[i].nCandsNum = 0;
		hiNodes[i].nFeaLen = 0;
		hiNodes[i].pfFea = NULL;
	}

	for( i = 0; i < nNodes; i++ )
	{
		obNodes[i].pfFea = new float[nFealen];
		for( j = 0; j < nFealen; j++ )
		{
			obNodes[i].pfFea[j] = 0.0f;
		}
		obNodes[i].nFeaLen = nFealen;

		hiNodes[i].nCandsNum = nCands;
		hiNodes[i].nFeaLen = nFealen;
		hiNodes[i].pfFea = new float[ nCands * nFealen ];
		for( j = 0; j < nCands * nFealen; j++ )
		{
			hiNodes[i].pfFea[j] = 0.0f;
		}		
	}
}
bool preCombJudge( int* pCurSelMode , int nTotalInsPos, CArray<int,int>& seqDistsArray, CArray<int,int>& seqCharsArray )
//Judge the validation of the insertion, after which the mode should be "4-7" or "11" or "4-6-1" or "10-1"
{
	int nSeqDistsNum = seqDistsArray.GetSize();
	int nSeqCharsNum = seqCharsArray.GetSize();
	ASSERT( nSeqDistsNum == nSeqCharsNum + 1 );
	if( nSeqDistsNum != nSeqCharsNum + 1 )
	{
		return false;
	}


	int i = 0;
	int j = 0;
	//Analyze the mode after filling selected blanks
	int nCurAnaID = 0;
	CArray<int,int> AlignMode;
	int nCurSeqChars = 0;
	int nSeqDists = (int)seqDistsArray.GetSize();
	bool bAbNorm = false;
	for( i = 1; i < seqDistsArray.GetAt(nSeqDists-1); i++ )//if the insertion is at the tail, all the chars should be adjacent
	{
		if( pCurSelMode[ nTotalInsPos - i ] == 1 && pCurSelMode[nTotalInsPos - i - 1 ] == 0 )
		{
			bAbNorm = true;
		}
	}
	for( i = 0; i < seqDistsArray.GetSize(); i++ )
	{
		int nCurBlanks = seqDistsArray.GetAt( i );
		for( j = 0; j < nCurBlanks; j++ )
		{
			if( pCurSelMode[nCurAnaID] == 0 )
			{
				if( nCurSeqChars != 0 )
				{
					AlignMode.Add( nCurSeqChars );
				}
				nCurSeqChars = 0;
			}

			if( pCurSelMode[nCurAnaID] == 1 )
			{
				nCurSeqChars++;
			}

			nCurAnaID++;
		}

		if( i != seqDistsArray.GetSize() - 1 )
		{
			nCurSeqChars += seqCharsArray.GetAt( i );
		}				
	}
	if( nCurSeqChars != 0 )
	{
		AlignMode.Add( nCurSeqChars );
	}
	bool bPosMode = false;
	int nCurModeSeqs = (int)AlignMode.GetSize();
	switch( nCurModeSeqs )
	{
	case 1:
		if( AlignMode.GetAt(0) == 11 ) bPosMode = true;
		if( AlignMode.GetAt(0) == 7 ) bPosMode = true;//Michael Add 20090404, for num region insertion.
		break;
	case 2:
		if( AlignMode.GetAt(0) == 4 && AlignMode.GetAt(1) == 7 ) bPosMode = true;
		else if( AlignMode.GetAt(0) == 10 && AlignMode.GetAt(1) == 1 ) bPosMode = true;
		else if( AlignMode.GetAt(0) == 6 && AlignMode.GetAt(1) == 1 ) bPosMode = true;//Michael Add 20090404, for num region insertion.
		break;
	case 3:
		if( AlignMode.GetAt(0) == 4 && AlignMode.GetAt(1) == 6 && AlignMode.GetAt(2) == 1 ) bPosMode = true;
		break;
	}
	//end -- Analyze the mode after filling selected blanks

	bPosMode = bPosMode && !bAbNorm;
	return bPosMode;
}
bool getPACStruct( PACStruct& allPosModes, int nMisChars,  int nTotalInsPos, CArray<int,int>& seqDistsArray, CArray<int,int>& seqCharsArray )
{
	int i = 0;
	int j = 0;

	if( allPosModes.nPbs != 0 ) return false;

	int* pCurSelMode = new int[nTotalInsPos];
	for( i = 0; i < nTotalInsPos; i++ )
	{
		if( i < nMisChars )
		{
			pCurSelMode[i] = 1;
		}
		else
		{
			pCurSelMode[i] = 0;
		}
	}

	//Judge the validation of the insertion, after which the mode should be "4-7" or "11" or "4-6-1" or "10-1"
	bool bPosMode = preCombJudge( pCurSelMode , nTotalInsPos, seqDistsArray, seqCharsArray );
	int nTotalCombs = 0;
	//ATTENTION CARRAY's VALUE COPYING
	INSMOD curMode;
	if( bPosMode )//If this is a valid insertion mode, just add it to allPosModes
	{
		curMode.nlen = nTotalInsPos;
		curMode.insertMode = new int[nTotalInsPos];
		for( i = 0; i < nTotalInsPos; i++ )
		{
			curMode.insertMode[i] = pCurSelMode[i];
		}
		allPosModes.InsModes.Add( curMode );
		allPosModes.nPbs = (int)allPosModes.InsModes.GetSize();
		ASSERT( allPosModes.InsModes[allPosModes.nPbs-1].insertMode == curMode.insertMode );
		curMode.nlen = 0;
		curMode.insertMode = NULL;
		nTotalCombs++;
	}
	//end -- ATTENTION CARRAY's VALUE COPYING

	bool bFinished = false;
	while( !bFinished )
	{
		for( i = 0; i < nTotalInsPos - 1; i++ )
		{
			if( pCurSelMode[i] == 1 && pCurSelMode[i+1] == 0 )
			{
				//Change the leftmost "10" to "01"
				pCurSelMode[i] = 0;
				pCurSelMode[i+1] = 1;
				//end -- Change the leftmost "10" to "01"
				//Move all "1"s left to i to the array's leftmost 
				for( j = 0; j < i; j++ )
				{
					if( pCurSelMode[j] != 0 ) break;
				}
				if( j != 0 && j != i )
				{
					int nFirstID = j;
					for( j = 0; j < nFirstID; j++ )
					{
						ASSERT( ( j < nTotalInsPos - 1 ) && ( j >= 0 ) && ( i > j ));
						pCurSelMode[j] = 1;
						pCurSelMode[ i - 1 - j ] = 0;
					}
				}
				//end -- Move all "1"s left to i to the array's leftmost
				break;
			}
		}

		if( i != nTotalInsPos - 1 )
		{
			bPosMode = preCombJudge( pCurSelMode , nTotalInsPos, seqDistsArray, seqCharsArray );
			if( bPosMode )
			{
				curMode.nlen = nTotalInsPos;
				curMode.insertMode = new int[nTotalInsPos];
				for( i = 0; i < nTotalInsPos; i++ )
				{
					curMode.insertMode[i] = pCurSelMode[i];
				}			
				//ATTENTION CARRAY's VALUE COPYING
				allPosModes.InsModes.Add( curMode );
				allPosModes.nPbs = (int)allPosModes.InsModes.GetSize();
				ASSERT( allPosModes.InsModes[allPosModes.nPbs-1].insertMode == curMode.insertMode );
				curMode.nlen = 0;
				curMode.insertMode = NULL;
				//end -- ATTENTION CARRAY's VALUE COPYING
				nTotalCombs++;
			}
		}
		else
		{
			bFinished = true;
		}
	}

	delete[] pCurSelMode;

	return true;
}

void normFeas( ObNode* &obNodes, HiNode* &hiNodes, int nNodes, int nPosModes )
{
	int i = 0;
	int j = 0;
	int k = 0;
	int nlen = 4;
	float fRatio[4] = { 1.0f, 0.5f, 2.5f, 0.5f };
	for( i = 0; i < nPosModes; i++ )
	{
		int nOffset = i * nNodes;
		for( j = 0; j < nNodes; j++ )
		{
			int nFealen = obNodes[ nOffset + j ].nFeaLen;
			ASSERT( nFealen == nlen );
			for( k = 0; k < nFealen; k++ )
			{
				obNodes[ nOffset + j ].pfFea[k] *= fRatio[k];
			}

			int nCands = hiNodes[ nOffset + j ].nCandsNum;
			nFealen = hiNodes[ nOffset + j ].nFeaLen;
			ASSERT( nFealen == nlen );
			int j1 = 0;
			for( j1 = 0; j1 < nCands; j1++ )
			{
				for( k = 0; k < nFealen; k++ )
				{
					hiNodes[ nOffset + j ].pfFea[ j1 * nFealen + k ] *= fRatio[k];
				}
			}
		}
	}
}
void ExNodesFea( ObNode* &obNodes, HiNode* &hiNodes, int& nNodes, int& nPosModes, int* &insertModes, int& nSeqs, objFrames& objInfo, 
				CArray<int, int>& inputSeqCharsArray, CArray<int, int>& inputSeqDistsArray )
{
#ifdef _ReferData
	HMM TEST MID RES
		(093, 166),	(073, 420),	(073, 347) -- hwr : 0.210375 : 
_0	:  (117, 145),	(097, 115),	(028, 018) -- hwr : 1.555556	Dis : 14 ; 
_1	:  (118, 145),	(129, 143),	(027, 014) -- hwr : 1.928571	Dis : 19 ; 
_2	:  (117, 144),	(162, 175),	(027, 013) -- hwr : 2.076923	Dis : 16 ; 
_3	:  (117, 144),	(191, 202),	(027, 011) -- hwr : 2.454545	Dis : 21 ; 
_4	:  (117, 143),	(223, 234),	(026, 011) -- hwr : 2.363636	Dis : 54 ; 
_5	:  (117, 143),	(288, 299),	(026, 011) -- hwr : 2.363636	Dis : 19 ; 
_6	:  (115, 142),	(318, 330),	(027, 012) -- hwr : 2.250000	Dis : 23 ; 
_7	:  (114, 141),	(353, 365),	(027, 012) -- hwr : 2.250000	Dis : 18 ; 
_8	:  (110, 140),	(383, 398),	(030, 015) -- hwr : 2.000000 ; 
	ST RES -- w : 13, h : 27
		ST RES -- RcDis : 19
		SEQ Chars : 5 - 4
		SEQ Dists : 1
#endif

	int i = 0;
	int j = 0;
	int k = 0;

	//Test Data Preparing
	int nChars = objInfo.nChars;
	int nMisChars = 11 - nChars;

	if( nMisChars <= 0 )
	{
		return;
	}

	int averDis = objInfo.averDis;
	int averWidth = objInfo.averWidth;
	int averHeight = objInfo.averHeight;
	int* inputCharsTopBorder = objInfo.inputCharsTopBorder;
	int* inputCharsBtmBorder = objInfo.inputCharsBtmBorder;
	int* inputCharsLeftBorder = objInfo.inputCharsLeftBorder;
	int* inputCharsRightBorder = objInfo.inputCharsRightBorder;

	//CArray<int,int> disArray;
	CArray<int,int> seqCharsArray;
	CArray<int,int> seqDistsArray;
	//disArray.RemoveAll();
	seqCharsArray.RemoveAll();
	seqDistsArray.RemoveAll();
	//for( i = 0; i < inputDisArray.GetSize(); i++ )
	//{
	//	disArray.Add( inputDisArray.GetAt(i) );
	//}
	//disArray.Add( averDis );
	for( i = 0; i < inputSeqCharsArray.GetSize(); i++ )
	{
		seqCharsArray.Add( inputSeqCharsArray.GetAt(i) );
	}
	int nTotalInsPos = 0;
	seqDistsArray.Add( nMisChars );
	nTotalInsPos += nMisChars;
	for( i = 0; i < inputSeqDistsArray.GetSize(); i++ )
	{
		seqDistsArray.Add( inputSeqDistsArray.GetAt(i) );
		nTotalInsPos += inputSeqDistsArray.GetAt(i);
	}
	seqDistsArray.Add( nMisChars );
	nTotalInsPos += nMisChars;

	averDis = averDis + averWidth;

	CArray<OBJFEA, OBJFEA> feature;
	for( i = 0; i < nChars; i++)
	{
		OBJFEA curObjFea;
		if( i != nChars -1 )
		{
			curObjFea.cenx = ( inputCharsRightBorder[i+1] + inputCharsLeftBorder[i+1] ) / 2
				- ( inputCharsRightBorder[i] + inputCharsLeftBorder[i]) / 2;
		}
		else
		{
			curObjFea.cenx = averDis;
		}
		curObjFea.ceny = ( inputCharsTopBorder[i] + inputCharsBtmBorder[i] ) / 2;
		curObjFea.h = inputCharsBtmBorder[i] - inputCharsTopBorder[i] + 1;
		curObjFea.w = inputCharsRightBorder[i] - inputCharsLeftBorder[i] + 1;
		feature.Add( curObjFea );
	}
#ifdef SAVE_INPUT_DATA
	CString strInputInfoDir = "InputInfo\\";
	CreateDirectory( strInputInfoDir, NULL );
	CString strInputInfoFile = strInputInfoDir + _T("inputInfo.txt");
	ofstream fInputInfo;
	fInputInfo.open( strInputInfoFile, ios::out | ios::trunc );
	ASSERT( nChars == feature.GetSize() );
	for( i = 0; i < nChars; i++ )
	{
		fInputInfo << i << "th Char's feature\t:\t";
		OBJFEA curObjFea = feature.GetAt(i);
		writeOBJFEAStruct( fInputInfo, curObjFea );
		fInputInfo << ";" << endl;
	}
	fInputInfo.close();
#endif
	//End -- Test Data Preparing

	//Possible insertion modes generation
	PACStruct allPosModes;
	allPosModes.nPbs = 0;
	getPACStruct( allPosModes, nMisChars, nTotalInsPos, seqDistsArray, seqCharsArray );
	//end -- Possible insertion modes generation

#ifdef SAVE_PACSTRUCT_INFO
	CString strPACInfoDir = "InputInfo\\";
	CreateDirectory( strPACInfoDir, NULL );
	CString strPACInfoFile = strPACInfoDir + _T("PACStruct.txt");
	ofstream outFile;
	outFile.open( strPACInfoFile, ios::out | ios::trunc );
	writePACStruct( outFile, allPosModes );
	outFile.close();
#endif

	nNodes = 11;
	nPosModes = allPosModes.nPbs;
	int nBlanks = (int)seqDistsArray.GetSize();
	int nFealen = 4;
	int nCands = 3;

	InitializeNodes( obNodes, hiNodes, nNodes * nPosModes, nFealen, nCands	);
	if( insertModes != NULL )
	{
		delete[] insertModes;
		insertModes = NULL;
	}
	nSeqs = 2 * nBlanks - 1 ;
	insertModes = new int[ nPosModes * nSeqs ];//Record all possible insertion modes
	for( i = 0; i < nPosModes * nSeqs; i++ )
	{
		insertModes[i] = 0;
	}
		
	for( i = 0; i < nPosModes; i++ )
	{
		int* pCurInsMode = allPosModes.InsModes.GetAt(i).insertMode;
		int nlen = allPosModes.InsModes.GetAt(i).nlen;

		int nCurID = 0;
		int nCurCharsID = 0;
		int nCurNodeID = 0;
		int nCurInsModID = 0;
		for( j = 0; j < nBlanks; j++ )
		{
			int nCurInsPoses = seqDistsArray.GetAt( j );//The number of chars which can be inserted into current blank
			int nZeros = 0;
			for( k = 0; k < nCurInsPoses; k++ )
			{
				if( pCurInsMode[nCurID] == 1 )
				{
					ASSERT( nCurInsModID == 2 * j );
					insertModes[ i * nSeqs + nCurInsModID ]++;

					if( j == 0 )
					{
						obNodes[ i * nNodes + nCurNodeID ].pfFea[0] = (float)averDis;

						obNodes[ i * nNodes + nCurNodeID ].pfFea[1] = (float)feature.GetAt( 0 ).ceny;
						obNodes[ i * nNodes + nCurNodeID ].pfFea[2] = (float)averHeight;
						obNodes[ i * nNodes + nCurNodeID ].pfFea[3] = (float)averWidth;
					}
					else if( j == (nBlanks - 1) )
					{
						obNodes[ i * nNodes + nCurNodeID ].pfFea[0] = (float)averDis;

						ASSERT( feature.GetSize() == nChars );
						obNodes[ i * nNodes + nCurNodeID ].pfFea[1] = (float)feature.GetAt( nChars - 1 ).ceny;
						obNodes[ i * nNodes + nCurNodeID ].pfFea[2] = (float)averHeight;
						obNodes[ i * nNodes + nCurNodeID ].pfFea[3] = (float)averWidth;
					}
					else
					{
						int nZeroCharsDis = nZeros * averDis;
						int nSubDis = (nZeros+1) * averDis;
						int nBlankDis = (int)obNodes[ i * nNodes + nCurNodeID - 1 ].pfFea[0];
						obNodes[ i * nNodes + nCurNodeID ].pfFea[0] = (float)( nBlankDis - nSubDis );
						obNodes[ i * nNodes + nCurNodeID - 1 ].pfFea[0] = (float)( nZeroCharsDis + averDis );

						obNodes[ i * nNodes + nCurNodeID ].pfFea[1] = 
							(float)( feature.GetAt( nCurCharsID - 1 ).ceny + feature.GetAt( nCurCharsID ).ceny ) / 2;
						obNodes[ i * nNodes + nCurNodeID ].pfFea[2] = (float)averHeight;
						obNodes[ i * nNodes + nCurNodeID ].pfFea[3] = (float)averWidth;
					}

					nCurNodeID++;
					nZeros = 0;
				}
				else
				{
					nZeros++;
				}
				nCurID++;
			}

			nCurInsModID++;
			
			if( j != (nBlanks - 1) )
			{
				int nCurSeqChars = seqCharsArray.GetAt( j );//The number of current seq chars'.
				ASSERT( nCurInsModID == 2 * j + 1 );
				insertModes[ i * nSeqs + nCurInsModID ] = nCurSeqChars;
				for( k = 0; k < nCurSeqChars; k++ )
				{
					int nCurDis = feature.GetAt(nCurCharsID).cenx;//disArray.GetAt( nCurCharsID );
					obNodes[ i * nNodes + nCurNodeID ].pfFea[0] = (float)nCurDis;

					obNodes[ i * nNodes + nCurNodeID ].pfFea[1] = (float)feature.GetAt(nCurCharsID).ceny;
					obNodes[ i * nNodes + nCurNodeID ].pfFea[2] = (float)feature.GetAt(nCurCharsID).h;
					obNodes[ i * nNodes + nCurNodeID ].pfFea[3] = (float)feature.GetAt(nCurCharsID).w;
					nCurCharsID++;
					nCurNodeID++;
				}
				nCurInsModID++;
			}
		}
	}

//////////////////////////////ADJUST SOME FEATURE//////////////////////////////////////////////
	for( i = 0; i < nPosModes; i++ )
	{
		int nOffset = i * nNodes;
		obNodes[ nOffset + 3 ].pfFea[0] = averDis;
		obNodes[ nOffset + 9 ].pfFea[0] = averDis;
		obNodes[ nOffset + 10 ].pfFea[2] = averHeight;
		obNodes[ nOffset + 10 ].pfFea[3] = averWidth;
	}
//////////////////////////////////////////////////////////////////////////////////////////////

	for( i = 0; i < nPosModes; i++ )
	{
		int nOffset = i * nNodes;
		for( j = 0; j < nNodes; j++ )
		{
			int nCurCenx = (int)obNodes[ nOffset + j ].pfFea[0];
			int nCurCeny = (int)obNodes[ nOffset + j ].pfFea[1];
			int nCurH = (int)obNodes[ nOffset + j ].pfFea[2];
			int nCurW = (int)obNodes[ nOffset + j ].pfFea[3];

			hiNodes[ nOffset + j ].nCandsNum = nCands;
			hiNodes[ nOffset + j ].nFeaLen = nFealen;

			int j1 = 0;
			for( j1 = 0; j1 < 3; j1++ )
			{
				float fCandVal_Cenx = 0.0f;
				switch( j1 )
				{
				case 0:
					fCandVal_Cenx = (float)nCurCenx;
					break;
				case 1:
					fCandVal_Cenx = (float)averDis;
					break;
				case 2:
					fCandVal_Cenx = (float)( nCurCenx + averDis ) / 2;
					break;
				}
				hiNodes[ nOffset + j ].pfFea[j1 * nFealen] = fCandVal_Cenx;
				hiNodes[ nOffset + j ].pfFea[j1 * nFealen + 1 ] = (float)nCurCeny;
				hiNodes[ nOffset + j ].pfFea[j1 * nFealen + 2 ] = (float)nCurH;
				hiNodes[ nOffset + j ].pfFea[j1 * nFealen + 3 ] = (float)nCurW;
			}

		}
	}
	normFeas( obNodes, hiNodes, nNodes, nPosModes );

	freePACStruct( allPosModes );

#ifdef SAVE_PACSTRUCT_INFO
	CString strNodesInfoFile = strPACInfoDir + _T("Nodes.txt");
	outFile.open( strNodesInfoFile, ios::out | ios::trunc );
	outFile << nPosModes << " Possible Modes : " << endl;
	outFile << "Observed States : " << endl;
	for( i = 0; i < nPosModes; i++ )
	{
		outFile << i << "th Possible Mode : " << endl;
		writeObNodes( outFile, &obNodes[i*nNodes], nNodes, nFealen );
	}
	outFile << endl;
	outFile << "Hidden States : " << endl;
	for( i = 0; i < nPosModes; i++ )
	{
		outFile << i << "th Possible Mode : " << endl;
		writeHiNodes( outFile, &hiNodes[i*nNodes], nNodes, nCands, nFealen );
	}
	outFile << endl;
	outFile.close();
#endif

}
//####################--------END--------#######################
//----------------------------------Generate Nodes Functions-----------------------------------
//####################--------END--------#######################