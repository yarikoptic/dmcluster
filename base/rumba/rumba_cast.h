#ifndef RUMBA_CAST_H
#define RUMBA_CAST_H
#include <rumba/exception.h>


template<class TYPE, class TYPEa>
TYPE rumba_cast ( TYPEa x )
{
	throw RUMBA::BadConvert();
}


template <> double rumba_cast<double> ( double x );
template <> double rumba_cast<double> ( float x ) ;
template <> double rumba_cast<double> ( int x ) ;
template <> double rumba_cast<double> ( short x );
template <> double rumba_cast<double> ( char x );
template <> double rumba_cast<double> ( unsigned char x );

template <> float rumba_cast<float> ( double x );
template <> float  rumba_cast<float> ( float x );
template <> float  rumba_cast<float> ( int x );
template <> float rumba_cast<float> ( short x );
template <> float rumba_cast<float> ( char x );
template <> float rumba_cast<float> ( unsigned char x );

template <> int rumba_cast<int> ( double x );
template <> int rumba_cast<int> ( float x );
template <> int rumba_cast<int> ( int x );
template <> int rumba_cast<int> ( short x );
template <> int rumba_cast<int> ( char x );
template <> int rumba_cast<int> ( unsigned char x );

template <> short rumba_cast<short> ( double x );
template <> short rumba_cast<short> ( float x );
template <> short rumba_cast<short> ( int x );
template <> short rumba_cast<short> ( short x );
template <> short rumba_cast<short> ( char x );
template <> short rumba_cast<short> ( unsigned char x );

template <> char rumba_cast<char> ( double x );
template <> char rumba_cast<char> ( float x );
template <> char rumba_cast<char> ( int x );
template <> char rumba_cast<char> ( short x );
template <> char rumba_cast<char> ( char x );
template <> char rumba_cast<char> ( unsigned char x );

template <> unsigned char rumba_cast<unsigned char> ( double x );
template <> unsigned char rumba_cast<unsigned char> ( float x );
template <> unsigned char rumba_cast<unsigned char> ( int x );
template <> unsigned char rumba_cast<unsigned char> ( short x );
template <> unsigned char rumba_cast<unsigned char> ( char x );
template <> unsigned char rumba_cast<unsigned char> ( unsigned char x );



#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
