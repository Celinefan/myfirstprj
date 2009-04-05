#pragma  once

#include <DataArray.h>

class CStModel
{
public:
	float** m_stArray;
	float** m_maxValArray;
	float** m_minValArray;
	int m_nCnt;
	int m_nrows;
	int m_ncols;

	CStModel();
	CStModel( int nRows, int nCols );
	~CStModel();	
	void Init( int nRows, int nCols );
	void Free();
	bool InputArray( float** fArray );
	void OutRes( ofstream& outfile );
private:
	float** m_averArray;
	void GetAverVal();
};