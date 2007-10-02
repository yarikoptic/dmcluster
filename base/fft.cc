#include <rumba/fft.h>
#include <iostream>
#include <iterator>

using std::vector;
using std::complex;

bool RUMBA::is_power_of_2(int x)
{
 	return ! (x&(~x^(x-1)));
}



template <> std::complex<double> 
RUMBA::get_element<std::vector<std::complex<double> > >
( const std::vector<complex<double> >& v, int i)
{
	return v[i];
}

template <> void
RUMBA::put_element< vector<complex<double> >  >
( vector<complex<double> >& v, int i, const complex<double>& x)
{
	v[i]=x;
}


// for testing purposes only. Use this to sanity-check fast version
// Use NR in C, 12.3.17

std::vector<double> RUMBA::slow_dct(const std::vector<double>& v)
{
	std::vector<double>::size_type N = v.size() -1;
	std::vector<double> w ( v);
	double sum = 0, pi = 4*atan(1);

	double last = v[N]; 
	double first = v[0];

	for (int k = 0; k < N; ++k )
	{
		sum = 0;
		for (int j = 1; j < N; ++j )
			sum += v[j] * cos ((pi*k*j)/N);

		w[k] = 0.5 * ( first + ((k%2) ? (-last) : last) ) + sum;
	}
	return w;
}

std::vector<complex<double> > 
RUMBA::slow_dft(const std::vector<complex<double > > & v, bool inv)
{
	int N = v.size();
	const double pi = 4*atan(1);
	std::vector<complex<double> > w(v);
	complex<double> sum = 0;
	complex<double> omega = exp(2*pi*complex<double>(0,1) / (double)N );
	if (inv) 
		omega = conj(omega);
	for (int k = 0; k < N; ++k )
	{
		sum = 0;
		for (int j = 0; j < N; ++j )
			sum += pow(omega,j*k) * v[j];
		w[k] = sum;
	}
	if (inv)
	{
		for (int i = 0; i < w.size(); ++i )
			w[i] /= w.size();
	}
	return w;

}


std::vector<double> RUMBA::slow_dct2(const std::vector<double>& v)
{
	int N = v.size()-1;
	std::vector<double> w( 2* (N) + 1);
	for (int i = 0; i < N; ++i )
	{
		w[i] = v[i];
		w[2*N - i ] =  v[i];
	}
	w[N] = v[N];

	RUMBA::real_fft(w,2*N);
	for (int i = 0; i < w.size(); ++i ) w[i] *= 0.5;
	return w;
}








template <> complex<double> 
RUMBA::get_element<std::vector<double> >( const vector<double >& v, int i)
{
	return complex<double>(v[2*i], v[2*i+1]);
}

template <> void
RUMBA::put_element< vector<double >  >
( vector<double >& v, int i, const complex<double>& x)
{
	v[2*i]=x.real();
	v[2*i+1]=x.imag();
}


template <> complex<double> 
RUMBA::get_element<RUMBA::dptr> ( const RUMBA::dptr & v, int i)
{
	return complex<double>(v[2*i], v[2*i+1]);
}

template <> void
RUMBA::put_element<RUMBA::dptr>
(RUMBA::dptr & v, int i, const complex<double>& x)
{
	v[2*i]=x.real();
	v[2*i+1]=x.imag();
}

template <class CONTAINER> void RUMBA::print_container
(const CONTAINER& v, std::ostream& s, bool newline = true)
{
	s << "(";
	std::copy(v.begin(), v.end(), 
			std::ostream_iterator<typename CONTAINER::value_type>(s,",") );
	s << ")";
	if (newline)
		s << std::endl;

}

// test on vector of length 2^{size}
bool RUMBA::test_dct(int size)
{
	bool flag = true;
	std::vector<double> v1 ( (2 << size) + 1);
	for (int i = 0; i < v1.size(); ++i )
		v1[i] = rand() / (double)RAND_MAX;

	std::vector<double> v2 = slow_dct2(v1);
	std::vector<double> v3 = slow_dct(v1);


	dct(v1.begin(),v1.size()-1);

	for (int i = 0; i < v1.size() /2 ; ++i )
	{
		if ( fabs(v2[2*i] - v3[i]) > 0.0001 )
		{
			std::cerr << "test Dct failed at " <<i << std::endl;
			print_container(v2,std::cerr);
			print_container(v3,std::cerr);
			return false;
		}
	}
	std::cerr << "test versions of DCT passed" << std::endl;
	for (int i = 0; i < v1.size() /2 ; ++i )
	{
		if ( fabs(v1[i] - v3[i]) > 0.0001 )
		{
			std::cerr << "Dct failed at " <<i << std::endl;
			print_container(v1,std::cerr);
			print_container(v3,std::cerr);
			return false;
		}
	}
	std::cerr << "DCT passed" << std::endl;

	return true;
}

bool RUMBA::test_generic_fft(int size)
{
	bool flag = true;
	std::vector<complex<double> > v1 ( (2 << size) );
	for (int i = 0; i < v1.size(); ++i )
		v1[i] = rand() / (double)RAND_MAX;

	std::vector<complex<double> > v2 = slow_dft(v1);
	generic_fft(v1,v1.size());


	for (int i = 0; i < v1.size() ; ++i )
	{
		if ( abs(v1[i] - v2[i]) > 0.0001 )
		{
			std::cerr << "Dct failed at " <<i << std::endl;
			print_container(v1,std::cerr);
			print_container(v2,std::cerr);
			return false;
		}
	}
	std::cerr << "FFT passed" << std::endl;

	return true;
}

bool RUMBA::test_real_fft()
{
	bool flag = true;
	std::vector<double> v1 ( 1024);
	for (int i = 0; i < v1.size(); ++i )
		v1[i] = rand() / (double)RAND_MAX;

	std::vector<complex<double> > v2(v1.begin(),v1.end());


	RUMBA::real_fft(v1,v1.size());
	RUMBA::generic_fft(v2,v2.size());
	for (int i = 1; i < v2.size()/2; ++i )
	{
		if (! (norm(v2[i] - complex<double> ( v1[2*i],v1[2*i+1] ) ) < 0.0001) )
		{
			flag = false;
			std::cerr << "real_fft failure at " << i << std::endl;
			std::cerr << complex<double> ( v1[2*i],v1[2*i+1] ) << std::endl;
			std::cerr << v2[i] << std::endl;
			print_container (v1,std::cerr);
			print_container (v2,std::cerr);
			break;
		}
	}

	if ( fabs ( v1[1] - v2[v2.size()/2].real() ) > 0.0001 )
	{
		std::cerr << v1[1] << " " << v2[v2.size()/2] << std::endl;
		flag = false;
	}

	if ( fabs ( v1[0] - v2[0].real() ) > 0.0001 )
		flag = false;


	return flag;


}


// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
