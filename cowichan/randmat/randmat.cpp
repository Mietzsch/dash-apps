#include <libdash.h>
#include <iostream>

using std::cout;
using std::endl;


template< typename T >
void print2d(const T& mat ) {
  for( int i = 0; i < mat.extent(0); i++ ) {
    for( int j = 0; j < mat.extent(1); j++ ) {
      cout << std::setw(3) <<  mat(i,j) << " ";
    }
    cout << endl;
  }
}

//
// The Cowichan problems require that the output is independent of the
// numbers of processors used. For randmat() a common solution found
// in other implementations is to use a simple linear congruential
// random number generator (LCG) with a separate deterministic seed
// for each row and to parallelize over the rows of the matrix. This
// is also how the DASH solution below works.
//
// A potential alternative would be to use a counter-based random
// number generation scheme (e.g. random123) that can be easily
// parallelized.
//

template< typename T >
void randmat( dash::NArray<T, 2>& mat, uint seed )
{
  const int LCG_A = 1664525, LCG_C = 1013904223;

  int nrows = mat.local.extent( 0 ); // num of local rows
  int ncols = mat.local.extent( 1 ); // num of local cols

  auto gc   = mat.pattern( ).global( {0,0} );
  uint gbeg = gc[0];  // global row of local (0,0)

  if( 0 < mat.local_size( ) ){
    for( uint i = 0; i < nrows; ++i ) {
      uint s = seed + gbeg + i;

      for( int j = 0; j < ncols; ++j ) {
        s = LCG_A * s + LCG_C;
        mat.lbegin( )[i*ncols + j] = ( (unsigned)s ) % 100;
      }
    }
  }
}


int main( int argc, char* argv[] )
{
  typedef unsigned int uint;

  dash::init( &argc,&argv );

  auto myid = dash::myid( );

  if( argc != 4 ) {
    if( 0 == myid )    cout << "3 Parameters expected!" << endl
		      << "Usage: cowichan_randmat nrows ncols seed" << endl;
    dash::finalize( );
    return 0;
  }

  uint nrows = static_cast<uint>( atoi( argv[1] ) );
  uint ncols = static_cast<uint>( atoi( argv[2] ) );
  uint s     = static_cast<uint>( atoi( argv[3] ) );

  dash::NArray<unsigned char, 2> mat( nrows, ncols );

  randmat( mat, s );

  dash::barrier( );
  if( myid==0 ) print2d( mat );

  dash::finalize( );
}
