
// from here  http://stackoverflow.com/questions/3982348/implement-generic-swap-macro-in-c
#define SWAP(x, y, TYPE) TYPE tmp = x; x = y; y = tmp;


/*
template<class TYPE>
inline TYPE * insertN( int i, int nTo, int nFrom, TYPE * to, TYPE * from ){
    TYPE * to_ = new TYPE[nTo+nFrom];
    for(int i=0; i< ; i++){   }
}

template<class TYPE>
inline int insertN( int i0, int len, TYPE * x, TYPE * x ){

}
*/

//1 2 1 2.0000
template< class TYPE >
inline int binSearchBetween( TYPE x, int imax, TYPE * xs ){
	int di   = imax;
	int imin = 0;
	do{
        di  = di>>1;
        int  i  = imin + di;
		if( xs[i] < x ){ imin=i; di = (imax-i); }
		//printf("binSearchBetween %i %i %i %f %f \n", imin, i, di, xs[i], x );
	}while( di > 1 );
	//printf( " %f < %f < %f\n", xs[imin], x, xs[imin+1] );
	return imin;
}

template< class TYPE >
inline int binSearchFrom( TYPE x, int n, TYPE * xs ){
	int   i = 1;
	do{
        i  = i << 1;
		if(i>n) if(xs[n]>x){ return binSearchBetween( x, n, xs ); }else{ return -1; };
		//printf("binSearchForm    %i %f %f \n", i, xs[i], x);
	}while( xs[i] < x );
	return binSearchBetween( x, i, xs );
}

template< class TYPE >
inline void indexArray( int n, int * permut ){ for( int i=0; i<n; i++ ){ permut[i] = i; } }

template< class TYPE >
inline void permute( int * permut, TYPE * input, TYPE * output, int p, int q ){
	for( int i=p; i<q; i++ ){
		output[ i ] = input[ permut[i] ];
        //output[ permut[i] ] = input[ i ];
	}
}

template< class TYPE >
int quickSort_partition( TYPE * A, int * permut, int p, int q){
    //printf( " quickSort_partition %i %i \n", p, q );
    TYPE x = A[ permut[p] ];
    int i = p;
    int j;
    for( j = p+1; j<q; j++ ){
        //printf( " %i %i %i %i \n", permut[p], permut[j],  A[ permut[p] ], A[ permut[j] ] );
        if( A[ permut[j] ] <= x ){
            i++;
            SWAP( permut[i], permut[j], int );
        }
    }
    SWAP( permut[i], permut[p], int );
    return i;
}

template< class TYPE >
void quickSort( TYPE * A, int * permut, int p, int q){
    //printf( " quickSort %i %i \n", p, q );
    if( p < q){
        int r = quickSort_partition<TYPE>( A, permut, p, q );
        quickSort<TYPE>( A, permut, p  , r );
        quickSort<TYPE>( A, permut, r+1, q );
    }
}

template< class TYPE >
int quickSort_partition_inplace( TYPE * A, int p, int q){
    //printf( " substep %i %i \n", p, q );
    TYPE x = A[p];
    int i = p;
    int j;
    for( j = p+1; j<q; j++ ){
        //printf( " %i %i %i %i \n", p, j,  A[ p ], A[ j ] );
        if( A[j] <= x ){
            i++;
            SWAP( A[i], A[j], TYPE );
        }
    }
    SWAP( A[i], A[p], TYPE );
    return i;
}

template< class TYPE >
void quickSort_inplace( TYPE * A, int p, int q){
    if(p<q){
        int r = quickSort_partition_inplace<TYPE>( A, p, q );
        quickSort_inplace<TYPE>( A, p  , r );
        quickSort_inplace<TYPE>( A, r+1, q );
    }
}


