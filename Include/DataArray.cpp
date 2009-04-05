#include "stdafx.h"
#include "dataArray.h"

float **FloatArray( int row, int col )
{
	int i = row;
	int j = col;
	float **arr = NewArray<float>( row, col );
	for( i = 0; i < row; i++ )
	{
		for( j = 0; j < col; j++ )
		{
			arr[i][j] = 0.0f;
		}
	}
	return arr;
}

void WriteFloatArray( ofstream& infofile, float **arr )
{
	if( arr == NULL ) return;

	int nRows = ArrayRows( arr );
	int nCols = ArrayCols( arr );

	infofile << nRows << "\t" << nCols << endl;
	
	int i = 0;
	int j = 0;
	for( i = 0; i < nRows; i++ )
	{
		for( j = 0; j < nCols; j++ )
		{
			infofile.width( 9 );
			//infofile.fill(' ');
			//infofile.precision( 9 );
			//infofile.setf( ios::)
			infofile << arr[i][j] << "\t";
		}
		infofile << endl;
	}
	infofile.width( );
	//infofile.fill();
	//infofile.precision();
}