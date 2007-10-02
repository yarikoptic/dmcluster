#include <rumba/rumba_cast.h>

template <> double rumba_cast<double> ( double x ) { return x; }
template <> double rumba_cast<double> ( float x ) { return static_cast<double> (x); }
template <> double rumba_cast<double> ( int x ) { return static_cast<double> (x); }
template <> double rumba_cast<double> ( short x ) { return static_cast<double> (x); }
template <> double rumba_cast<double> ( char x ) { return static_cast<double> (x); }
template <> double rumba_cast<double> ( unsigned char x ) { return static_cast<double> (x); }

template <> float rumba_cast<float> ( double x ) { return static_cast<float> (x); }
template <> float rumba_cast<float> ( float x ) { return x; }
template <> float  rumba_cast<float> ( int x ) { return static_cast<float> (x); }
template <> float rumba_cast<float> ( short x ) { return static_cast<float> (x); }
template <> float rumba_cast<float> ( char x ) { return static_cast<float> (x); }
template <> float rumba_cast<float> ( unsigned char x ) { return static_cast<float> (x); }

template <> int rumba_cast<int> ( double x ) { return static_cast<int> (x); }
template <> int rumba_cast<int> ( float x ) { return static_cast<int> (x); }
template <> int rumba_cast<int> ( int x ) { return x; }
template <> int rumba_cast<int> ( short x ) { return static_cast<int> (x); }
template <> int rumba_cast<int> ( char x ) { return static_cast<int> (x); }
template <> int rumba_cast<int> ( unsigned char x ) { return static_cast<int> (x); }

template <> short rumba_cast<short> ( double x ) { return static_cast<short> (x); }
template <> short rumba_cast<short> ( float x ) { return static_cast<short> (x); }
template <> short rumba_cast<short> ( int x ) { return static_cast<short> (x); }
template <> short rumba_cast<short> ( short x ) { return x; }
template <> short rumba_cast<short> ( char x ) { return static_cast<short> (x); }
template <> short rumba_cast<short> ( unsigned char x ) { return static_cast<short> (x); }

template <> char rumba_cast<char> ( double x ) { return static_cast<char> (x); }
template <> char rumba_cast<char> ( float x ) { return static_cast<char> (x); }
template <> char rumba_cast<char> ( int x ) { return static_cast<char> (x); }
template <> char rumba_cast<char> ( short x ) { return static_cast<char> (x); }
template <> char rumba_cast<char> ( char x ) { return x; }
template <> char rumba_cast<char> ( unsigned char x ) { return x; }

template <> unsigned char rumba_cast<unsigned char> ( double x ) { return static_cast<unsigned char> (x); }
template <> unsigned char rumba_cast<unsigned char> ( float x ) { return static_cast<unsigned char> (x); }
template <> unsigned char rumba_cast<unsigned char> ( int x ) { return static_cast<unsigned char> (x); }
template <> unsigned char rumba_cast<unsigned char> ( short x ) { return static_cast<unsigned char> (x); }
template <> unsigned char rumba_cast<unsigned char> ( unsigned char x ) { return x; }
template <> unsigned char rumba_cast<unsigned char> ( char x ) { return x; }

// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
