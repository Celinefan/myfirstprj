//#if !defined(BFPROPA_H__9CAF7A1E_249C_4721_A4C9_391A000BG61E__INCLUDED_)
//#define BFPROPA_H__9CAF7A1E_249C_4721_A4C9_391A000BG61E__INCLUDED_
#pragma once

struct BpProbParas//The Covariance of Gaussian Distribution -- the delta para
{
	int nNodes;
	//O stands for observations ; H stands for hidden states
	float* OHFactor;//The Covariance of the emitting probability (the probability between Oi and Hi)
	float* HHFactor;//The Covariance of the transition probability(the probability between Hi and Hi+1)
};
struct ObNode//observed node
{
	float *pfFea;//feature
	int nFeaLen;//feature length
	
	ObNode()
	{
		pfFea = NULL;
		nFeaLen = 0;
	}
};
bool initObNode( ObNode& obj, int nFeaLen );
void freeObNode( ObNode& obj );
void writeObNodes( ofstream& ndInfoFile, ObNode* pObj, int nNodes, int nFealen );
struct HiNode//hidden state node
{
	float *pfFea;//feature
	int nFeaLen;//feature length
	int nCandsNum;//the number of pre-selected states
};
void freeHiNode( HiNode& obj );
void writeHiNodes( ofstream& ndInfoFile, HiNode* pObj, int nNodes,int nCands, int nFealen );
struct CandMessage
{
	int nNbCandsNum;//the number of corresponding node cands
	float* msg;
};
void freeCandMessage( CandMessage& CdMessage );
struct NodeMessage
{
	int nCandsNum;//the number of the cands in current node
	float* OHMsg;//the message between the observation and the candidates of current node
						// Dim : 1 * nCandsNum
	float* HsMsg;//the message between current node and next node
						// Dim : nCandsNum * nCandsNum
	CandMessage* CdMsg_L;//accumulative message from the left
										// Dim : nCandsNum( CandMessage ) * nCandsNum( msg )
	CandMessage* CdMsg_R;//accumulative message from the right
										// Dim : nCandsNum( CandMessage ) * nCandsNum( msg )
};
void freeNodeMessage( NodeMessage& NdMessage );
struct BeliefMessage
{
	int nNodesNum;//the number of nodes
	NodeMessage* NdMsg;//record the message info of every node
};
void freeBeliefMessage( BeliefMessage& BfMessage );
bool getMessageInfo( float* &msg, int &nRows, int &nCols, BeliefMessage* BfMessage, int nthNode, int nthItem );
//nthItem = 0 : pNdMsg->OHMsg;
//nthItem = 1 : pNdMsg->HsMsg;
//nthItem = 2 : pNdMsg->CdMsg_R;
//nthItem = 3 : pNdMsg->CdMsg_L;

class CBfPropa
{
public:
	CBfPropa(void);
public:
	~CBfPropa(void);

public:
	bool SetStates( ObNode* obNodes, HiNode* hiNodes, int nNodes );//Only copy the pointers' address,
																							   //do not release the corresponding mem before beliefPropagation
	bool beliefPropagation( );
	bool GetResult( int* &res, int& nlen, float &fConf );
	BeliefMessage* GetBfMsgPointer();
private:
	BpProbParas m_BpParas;
	int m_nNodes;//the number of nodes(for both m_obNodes and m_hiNOdes)
	ObNode* m_obNodes;//the observed nodes
	HiNode* m_hiNodes;//the hidden nodes

	int* m_nSelection;//the hidden states' result after BpPropagation()
	float m_fConf;//the output probability of BpPropagation()

	BeliefMessage* m_pBfMsg;
#ifdef DEBUG_CBFPROPA
	CString m_strMainDebugDir;
	CString m_strSubDebugDir;
	CString m_strCurDebugDir;
	CString m_strCurDebugFilename;
#endif

	bool BfPropagation( );
	bool CreateBfMsgStruct();
	bool IniBfMsg( );//equal to 1 Time propagation
	bool BfMsgPropa( int nTimes = 0 );
	bool GetBfPropaRes( );
	bool getMaxMsg( float& fMaxVal, int& nMaxValID, float* pArray, int nLen );
	bool getOHMsg( ObNode obNode, HiNode hiNode, float* &fOHMsg );
	bool getHsMsg( HiNode curNode, HiNode nxtNode, float* &fHsMsg );
	void freeMem();
#ifdef DEBUG_CBFPROPA
	bool writeBfMsgInfo( );
#endif
};


struct objFrames
{
	int nChars;
	int averDis;
	int averWidth;
	int averHeight;
	int* inputCharsTopBorder;
	int* inputCharsBtmBorder;
	int* inputCharsLeftBorder;
	int* inputCharsRightBorder;
	//int* inputCharsheight;
	//int* inputCharsWidth;
	//int* inputDisArray;
	//int* inputSeqCharsArray;
	//int* inputSeqDistsArray;

	objFrames()
	{
		nChars = 0;
		averDis = 0;
		averWidth = 0;
		averHeight = 0;
		inputCharsTopBorder = NULL;
		inputCharsBtmBorder = NULL;
		inputCharsLeftBorder = NULL;
		inputCharsRightBorder = NULL;
	}

	~objFrames()
	{
		nChars = 0;
		averDis = 0;
		averWidth = 0;
		averHeight = 0;
		inputCharsTopBorder = NULL;
		inputCharsBtmBorder = NULL;
		inputCharsLeftBorder = NULL;
		inputCharsRightBorder = NULL;
	}
};
void FreeObjFramesStruct( objFrames& obj );


void ExNodesFea( ObNode* &obNodes, HiNode* &hiNodes, int& nNodes, int& nPosModes, int* &insertModes, int& nSeqs, objFrames& objInfo, 
				CArray<int, int>& inputSeqCharsArray, CArray<int, int>& inputSeqDistsArray );

//#endif //END OF -- #if !defined(BFPROPA_H__9CAF7A1E_249C_4721_A4C9_391A000BG61E__INCLUDED_)