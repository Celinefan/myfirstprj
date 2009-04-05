#include "stdafx.h"
#include "stmod.h"

CStModel::CStModel( )
{
	m_stArray = NULL;
	m_maxValArray = NULL;
	m_minValArray = NULL;
	m_averArray = NULL;

	m_nCnt = 0;
	m_nrows = 0;
	m_ncols = 0;
}

CStModel::CStModel( int nRows, int nCols )
{
	m_stArray = NULL;
	m_maxValArray = NULL;
	m_minValArray = NULL;
	m_averArray = NULL;

	m_nCnt = 0;
	m_nrows = 0;
	m_ncols = 0;

	Init( nRows, nCols );
}

CStModel::~CStModel( void )
{
	Free();
}

void CStModel::Init( int nRows, int nCols )
{
	Free();

	m_nCnt = 0;
	m_nrows = nRows;
	m_ncols = nCols;

	m_stArray = FloatArray( nRows, nCols );
	m_maxValArray = FloatArray( nRows, nCols );
	m_minValArray = FloatArray( nRows, nCols );
	m_averArray = FloatArray(nRows, nCols);
}

void CStModel::Free()
{
	if( m_stArray != NULL )
	{
		FreeArray( m_stArray );
	}
	if( m_maxValArray != NULL )
	{
		FreeArray( m_maxValArray );
	}
	if( m_minValArray != NULL )
	{
		FreeArray( m_minValArray );
	}
	if( m_averArray != NULL )
	{
		FreeArray( m_averArray );
	}


	m_nCnt = 0;
	m_nrows = 0;
	m_ncols = 0;
}

bool CStModel::InputArray( float** fArray )
{
	int nRows = ArrayRows(fArray);
	int nCols = ArrayCols(fArray);

	if( nRows != m_nrows || nCols != m_ncols ) return false;

	m_nCnt++;

	int i = 0;
	int j = 0;
	for( i = 0; i < m_nrows; i++)
	{
		for( j = 0; j < m_ncols; j++ )
		{
			m_stArray[i][j] += fArray[i][j];
			if( m_nCnt == 1 || fArray[i][j] > m_maxValArray[i][j] )
			{
				m_maxValArray[i][j] = fArray[i][j];
			}
			if( m_nCnt == 1 || fArray[i][j] < m_minValArray[i][j] )
			{
				m_minValArray[i][j] = fArray[i][j];
			}
		}
	}

	return true;
}

void CStModel::OutRes( ofstream& outfile )
{
	outfile << "Total " << m_nCnt << " files." << endl;
	GetAverVal();
	outfile << "averArray:" << endl;
	WriteFloatArray( outfile, m_averArray );
	outfile << endl;
	outfile << "maxValArray:" << endl;
	WriteFloatArray( outfile, m_maxValArray );
	outfile << endl;
	outfile << "minValArray:" << endl;
	WriteFloatArray( outfile, m_minValArray );
	outfile << endl;
}

void CStModel::GetAverVal()
{
	int i = 0;
	int j = 0;
	for( i = 0; i < m_nrows; i++ )
	{
		for( j = 0; j < m_ncols; j++ )
		{
			m_averArray[i][j] = m_stArray[i][j] / (float)m_nCnt;
		}
	}
}