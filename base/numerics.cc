#include <cmath>
#include <iostream>
#include <rumba/numerics.h>


#include <rumba/fft_impl.h>

using std::map;
using std::vector;
using std::make_pair;
using std::complex;

namespace
{
// compute log of gamma. NOT FOR GENERAL USE
double hrf_gammapdf_log(double t, double a, double b)
{
    const double d = a*b;
    return a * std::log(t/d) + (d-t)/b;
}

// unscaled gamma pdf  given by (t/d)^a * e^{ (d-t) / b } with d = a*b
// NOT FOR GENERAL USE. In particular, it's not scaled as a PDF, and a,b are
// NOT the same as conventional shape/scale parameters.
double hrf_gamma_pdf(double t, double a, double b)
{
    return std::exp(hrf_gammapdf_log(t,a,b));
}

} /* namespace */

// returns log ( \Gamma(t))
double RUMBA::gamma_log(double t)
{
	t-=1;
	static const double pi = 4 * atan(1);
	static const double coefficients[] =
	{
		1.000000000190015,
		76.18009172947146,
		-86.50532032941677,
		24.01409824083091,
		-1.231739572450155,
		0.1208650973866179e-2,
		-0.5395239384953e-5
	};
	const int gamma = 5;
	double tmp = t + gamma + 0.5;
	double result = log ( tmp ) * (t+0.5) - tmp;
	double sum=coefficients[0];

	for (int i = 1; i<=gamma; ++i )
		sum += coefficients[i]  / (t+=1) ;

	sum *= sqrt( 2 * pi );

	result += log( sum );
	return result;

}

double RUMBA::gamma(double t)
{
	return exp(gamma_log(t));
}


#ifdef TEST_GAMMA
int main()
{
	cout << gamma(1) << endl;
	cout << gamma(2) << endl;
	cout << gamma(3) << endl;
	cout << gamma(4) << endl;
	cout << gamma(5) << endl;

}
#endif

double RUMBA::incomplete_gamma ( double x, double a  )
{
	// compute sum = \sum \frac{ x^n }{ \Gamma ( a + 1 + n )

	double sum = 0;

	// g is   x^n / \Gamma (a+1+n)
	double g = 1/gamma (a+1);


	for ( int i = 0; i < 100 && g > 1e-20; ++i )
	{
		sum += g;
		g *= x;
		g /= (a+i+1);
	}

#ifdef DEBUG
	std::cout << "error: " << g << std::endl;
#endif

	// now compute incomplete gamma function

	sum *= exp(-x);
	sum *= pow(x,a);

	return sum;
}

#ifdef TEST_INCOMPLETE_GAMMA

int main()
{
	cout << "1,a=10 " << incomplete_gamma ( 1,10) << endl;
	cout << "6,a=10 " << incomplete_gamma ( 6,10) << endl;
	cout << "11,a=10 " << incomplete_gamma ( 11,10) << endl;

	cout << "6,a=1 " << incomplete_gamma ( 6,1) << endl;
	cout << "10,a=1 " << incomplete_gamma ( 10,1) << endl;
	cout << "6,a=3 " << incomplete_gamma ( 6,3) << endl;
	cout << "6, a=.5 " << incomplete_gamma ( 6,0.5) << endl;

}





#endif


int RUMBA::bit_reverse_impl(int x, int nbits)
{
	int y = 0;
	for (int i =0; i<nbits; ++i)
	{
		y<<=1;
		if (x%2)
			++y;
		x>>=1;
	}
	return y;
}

RUMBA::BitReverseFunctor RUMBA::bit_reverse;

int RUMBA::BitReverseFunctor::operator()(int x, int nbits)
{
	map<int, vector<int> >::iterator it = table.find(nbits);
	if (it == table.end())
	{
		compute_row(nbits);
		it = table.find(nbits);
	}
	return it->second[x];
}

void RUMBA::BitReverseFunctor::compute_row(int nbits)
{
	int max = 1 << (nbits+1);
	vector<int> v;
	table.insert(make_pair(nbits,v));
	table[nbits].resize(max);

	for (int i=0;i<max;++i)
		table[nbits][i] = bit_reverse_impl(i,nbits);
}


void RUMBA::bit_reverse_transform(vector<complex<double> >& v)
{
	int s = v.size();
	int new_size = 1;
	int nbits = 0;

	while (s>new_size)
	{
		new_size<<=1;
		++nbits;
	}
	if ( s != new_size )
		v.resize(new_size);

	for (int i = 0; i<new_size; ++i)
		if (i <bit_reverse(i,nbits))
			std::swap(v[i],v[bit_reverse(i,nbits)]);
}

int RUMBA::base_2_log(int n)
{
	if (n<=0)
		throw 0;
	int res=0;
	--n;
	do
	{
		++res;
	} while (n>>=1);
	return res;
}

void RUMBA::fft (vector<complex<double> >& v)
{
	int n,lgn;
	int m;
	complex<double> u,t;
	static const double pi = 4*atan(1);
	complex<double> omega_m;
	complex<double> omega;

	bit_reverse_transform(v);
	n = v.size();
	lgn = base_2_log(n);


	for ( int s = 1; s <= lgn; ++s )
	{
		m = 1<<s;
		omega_m = exp ( 2 *  pi * complex<double>(0,1) / complex<double>(m));
		omega = 1;
		for (int j=0;j<=m/2-1;++j)
		{
			for ( int k=j; k<n; k+=m )
			{
				t = omega * v[k+m/2];
				u = v[k];
				v[k] = u+t;
				v[k+m/2] = u-t;
			}
			omega *= omega_m;
		}
	}
}

#ifdef TEST_RUMBA_FFT
#include <iostream>

int main()
{

	vector<complex<double> > v;
	double x,y;

	char resp='y';
	do
	{
		if (cin >> x >> y)
			v.push_back (complex<double>(x,y));
		else
			break;

		cout << "enter data, y/n ?\n";
		cin >> resp;

	}while (resp != 'n');

	fft(v);

	copy (v.begin(),v.end(),ostream_iterator<complex<double> > (cout, " " ));
	cout << endl;
}
#endif




double RUMBA::hrf(double t, double a1, double a2, double b1, double b2, double c)
{
    return hrf_gamma_pdf(t,a1,b1) - c * hrf_gamma_pdf(t,a2,b2);
}

namespace {
    inline std::complex<double> cube_root(double x) {
	return std::pow(std::complex<double>(x), 1/3.);
} /* cube_root() */
} /* anon */

void RUMBA::solve_cubic (
		double a2, double a1, double a0,
		std::complex<double>& r1,
		std::complex<double>& r2,
		std::complex<double>& r3
		)
{
	static const double epsilon = 1e-6;
	static const complex<double> omega1 =
		std::complex<double>(-0.5,std::sqrt(3.)/2.0);
	static const complex<double> omega2 =
		std::complex<double>(-0.5,-std::sqrt(3.)/2.0);

	const double p = (3*a1-a2*a2)/3.0;
	const double q = (9*a1*a2 - 27*a0 - 2*a2*a2*a2) / 27.0;

	if (fabs(p)<epsilon)
	{
		r1 = cube_root(q)-a2/3.0;
		r2 = cube_root(q)*omega1 - a2/3.0;
		r3 = cube_root(q)*omega2-a2/3.0;
		return;
	}

	complex<double> w1 = pow ((0.5*q + std::sqrt(
				complex<double>(q*q/4 + 1/27.0 * p*p*p ))), 1/3.0
			);

	complex<double> w2 = w1*omega1;
	complex<double> w3 = w2*omega1;

	r1 = w1 - p/(3.*w1) - a2/3.0;
	r2 = w2 - p/(3.*w2) - a2/3.0;
	r3 = w3 - p/(3.*w3) - a2/3.0;

}




// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
