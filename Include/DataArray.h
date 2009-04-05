#if !defined(MICHAEL_CHAN_2DARRAY_)
#define MICHAEL_CHAN_2DARRAY_

#pragma once

#include <iostream>
//using namespace std;
#include <cstdlib>
#include <new>

template <typename T>
T **NewArray(int row, int col)
{
	int size = sizeof(T);
	void **arr = (void **) malloc(sizeof(void *) * row + size * row * col + sizeof(int) * 2 );

	//new ((unsigned char*)arr) int;
	*((int*)arr) = row;
	arr = (void**)((int*)arr + 1);

	//new ((unsigned char*)arr) int;
	*((int*)arr) = col;
	arr = (void**)((int*)arr + 1);

	if (arr != NULL)
	{
		unsigned char * head;
		head = (unsigned char *) arr + sizeof(void *) * row;
		for (int i = 0; i < row; ++i)
		{
			arr[i] =  head + size * i * col;
			for (int j = 0; j < col; ++j)
				new (head + size * (i * col + j)) T;
		}
	}
	return (T**) arr;
}

template <typename T>
void FreeArray(T **arr)
{
	if( arr == NULL ) return;

	int i = 0;
	int j = 0;
	int row = 0;
	int col = 0;

	row = ArrayRows(arr);
	col = ArrayCols(arr);

	for ( i = 0; i < row; ++i)
		for ( j = 0; j < col; ++j)
			arr[i][j].~T();

	arr = (T**)((int*)arr - 2);
	if (arr != NULL)
		free((void **)arr);
}

template <typename T>
int ArrayCols(T **arr)
{
	if( arr == NULL ) return 0;
	int* pnVal = NULL;
	pnVal = (int*)arr - 1;
	return *pnVal;
}

template <typename T>
int ArrayRows(T **arr)
{
	if( arr == NULL ) return 0;
	int* pnVal = NULL;
	pnVal = (int*)arr - 2;
	return *pnVal;
}


float **FloatArray( int row, int col );
void WriteFloatArray( ofstream& infoflie, float **arr );

#endif//End -- MICHAEL_CHAN_2DARRAY_