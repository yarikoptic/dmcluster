#ifndef RUMBA_FFT_H
#define RUMBA_FFT_H

#include <rumba/numerics.h>
// using std::vector;
// using std::complex;

namespace RUMBA
{

/**
  * Generic fft implementation. Container is any class that the put_element
  * and get_element template functions are specialised for. THe size is the 
  * container size (number of complex numbers), must be a power of 2. The
  * transform is performed in-place.
  */
template<class CONTAINER>
void generic_fft (CONTAINER& v, int size, bool inv = false);

/**
  * Simple dct that uses summation forumula. Input may be any size. This is 
  * slow and should only be used when flexiblity is very important. 
  */
std::vector<std::complex<double> > 
slow_dft(const std::vector<std::complex<double > > & v, bool inv = false);

/**
  * Test fft
  */
bool test_generic_fft(int );



/**
  * Real fft implementation. Again, v is the argument, size is the number of
  * real numbers in the container. get_element and put_element need to be 
  * specialiased with methods that re-interpret the container of doubles v
  * as a container of complex numbers in the sense that 
  * (a1,b1,a2,b2,a3,b3) should be reinterpreted as (a1+ib1, a2+ib2, a3+ib3)
  */
template <class CONTAINER> 
void real_fft(CONTAINER& v, int Size);

/**
  * Fast DCT implementation. Performs transform on data set of size Size + 1.
  * Size must be a power of 2. 
  */
template <class double_iterator> 
void dct ( double_iterator it, int Size);

/**
  * Naive implementation that uses the summation formula. Has the advantage
  * that there are no size requirements, good for small data sets.
  */
std::vector<double> slow_dct(const std::vector<double>& v);

/**
  * "Pads" the data to produce an even function, then computes fft. Only good
  * for testing purposes.
  */
std::vector<double> slow_dct2(const std::vector<double>& v);

/**
  * Extract the ith complex number from a container
  */
template<class CONTAINER>
std::complex<double> get_element(const CONTAINER &, int);

/**
  * Write to the ith complex number in a container
  */
template<class CONTAINER>
void put_element(CONTAINER &, int, const std::complex<double>& );

/**
  * Specialise get/put_element for vector<complex< double> >, vector<double>,
  * and double*
  */
template <> std::complex<double> 
get_element( const std::vector<std::complex<double> >& v, int i);

template <> void
put_element< std::vector<std::complex<double> >  >
( std::vector<std::complex<double> >& v, int i, const std::complex<double>& x);

template <> std::complex<double> 
get_element( const std::vector<double >& v, int i);

template <> void
put_element< std::vector<double >  >
( std::vector<double >& v, int i, const std::complex<double>& x);

typedef std::vector<double>::iterator dptr;
template <> std::complex<double> 
get_element<dptr> ( const dptr & v, int i);

template <> void
put_element<dptr>
(dptr & v, int i, const std::complex<double>& x);

/**
  * Test dct
  */
bool test_dct(int size);
/**
  * Test real fft.
  */
bool test_real_fft();

/**
  * Debugging functions
  */
template <class CONTAINER>
void generic_bit_reverse_transform(CONTAINER& v, int size);

template <class CONTAINER> void print_container
(const CONTAINER& v, std::ostream& s, bool newline = true);

/**
  * Utility function
  */
bool is_power_of_2(int x);


/* 
 * Note on computing roots of unity: in NR in C, this trick is used:
 * First compute a multiplier: m = e^{2\pi i / n} -1. This can be computed
 * as (-2*sin^2 (\theta /2 ), sin(\theta) ) using double angle cosine formula.
 * Then use e^{e\pi k / n} = x_n = x_{n-1} * e^{2\pi i /n} = 
 * x_{n-1} + m * x_{n-1}
 */

} // namespace RUMBA


#include "fft_impl.h"





// Template function definitions

//------------------------ adapted fft code -------------------------
template <class CONTAINER>
void RUMBA::generic_bit_reverse_transform(CONTAINER& v, int size)
{
	int s = size;
	int new_size = 1;
	int nbits = 0;
	int j;
	std::complex<double> tmp;

	while (s>new_size) 
	{
		new_size<<=1;
		++nbits;
	}
//	if ( s != new_size ) 
//		v.resize(new_size);

	for (int i = 0; i<new_size; ++i)
	{
		if (i <RUMBA::bit_reverse(i,nbits))
		{
			j = RUMBA::bit_reverse(i,nbits);
			tmp = get_element(v,j);
			put_element(v,j,(get_element(v,i)));
			put_element(v,i,tmp);
		}
	}
}
	
template<class CONTAINER>
void RUMBA::generic_fft (CONTAINER& v, int size, bool inv )
{
	int n=size,lgn=RUMBA::base_2_log(n);	
	int m;
	std::complex<double> u,t;
	static const double pi = 4*std::atan(1.0);
	std::complex<double> omega_m;
	std::complex<double> omega;

	generic_bit_reverse_transform(v,size);


	for ( int s = 1; s <= lgn; ++s )
	{
		m = 1<<s;
		omega_m = exp(2*pi*std::complex<double>(0,1)/std::complex<double>(m));
		if (inv)
			omega_m = conj(omega_m);
		omega = 1;
		for (int j=0;j<=m/2-1;++j)
		{
			for ( int k=j; k<n; k+=m )
			{
				t = omega * get_element(v,k+m/2);
				u = get_element(v,k);
				put_element(v, k, u+t);
				put_element(v,k+m/2,u-t);
			}
			omega *= omega_m;
		}
	}

	// in case of inverse transform, divide by N.
	if (inv)
	{	
		for (int i = 0; i < size; ++i )
			put_element(v,i,get_element(v,i)/(double)size);
	}
}

/* 
   v must be a container of doubles for which get_element and put_element
   has been specialised. Size is the number of doubles
 */
template <class CONTAINER> 
void RUMBA::real_fft(CONTAINER& v, int Size)
{
	int j;
	static const double pi = 4*std::atan(1.0);
	std::complex<double> x1, x2;
	const double theta = pi / (Size/2);
	// Root of unity - 1
	const std::complex<double> multiplier_base =
		std::complex<double>( -2*sin(0.5*theta) * sin(0.5*theta), sin(theta));
	// i_th power of root of unity. Initialise to i = 1
	std::complex<double> multiplier = multiplier_base + 1.0;

	if (!is_power_of_2(Size))
		throw "Size of argument must be a power of 2 in real_fft";
	RUMBA::generic_fft(v,Size/2);

	for (int i = 1; i < Size/4; ++i)
	{
		j = Size/2 - i ;
		x1 = 0.5 * ( get_element(v,i) + conj(get_element(v,j)) );
		x2 = 0.5 * std::complex<double>(0,-1) * 
			(get_element(v,i) - conj(get_element(v,j)));
		put_element(v, i, x1 + multiplier * x2);
		put_element(v, j, conj(x1 + multiplier * (-x2)) );
		multiplier += multiplier * multiplier_base;
	}

	x1 = get_element(v,0);
	put_element(v,0,
			std::complex<double>(x1.real()+x1.imag(), x1.real()-x1.imag())); 

}


//  Size = some power of 2. Array must contain (Size+1) points !!!
template <class double_iterator> 
void RUMBA::dct ( double_iterator it, int Size)
{
	int j;
	double tmp1,tmp2,sum=0;
	static const double pi = 4*std::atan(1.0);
	std::complex<double> x1, x2;
	const double theta = pi / (Size);
	// Root of unity - 1
	const std::complex<double> multiplier_base =
		std::complex<double>( -2*sin(0.5*theta) * sin(0.5*theta), sin(theta));

	// i_th power of root of unity. Initialise to i = 1
	std::complex<double> multiplier =  1.0 + multiplier_base;

	if (!is_power_of_2(Size ))
		throw "Size of argument must be a power of 2 in dct";


	double_iterator it1 = it+1, it2 = it+Size;

	sum = 0.5 * ( *it - *it2);
	*it = 0.5 * ( *it + *it2 );
	--it2;
	// aux function
	for (it1 = it+1; it1 != it + Size/2 ; ++it1,--it2 )
	{
		// boundary condition: size of array is Size+1		
		tmp1 = 0.5* ( *it1 + *it2 ); 
		tmp2 = ( *it1 - *it2 ); 
		*it1 = tmp1 - multiplier.imag() * tmp2;
		*it2 = tmp1 + multiplier.imag() * tmp2;

		sum += multiplier.real() * tmp2;

		multiplier += multiplier_base * multiplier;
	}
	
	real_fft(it,Size);

	*(it + Size) = *(it+1);
	*(it+1) = sum;
	for (int i = 3; i < Size; i+=2 )
	{
		sum += *(it+i);
		*(it+i) = sum;
	}
}

//---------------------------- end template function defs ---------------------


#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
