#ifndef RUMBA_FFT_IMPL_H
#define RUMBA_FFT_IMPL_H

#include <map>

namespace RUMBA {

// cached bit-reverse, used by fft.
class BitReverseFunctor; 
extern BitReverseFunctor bit_reverse;

// the function itself
int bit_reverse_impl(int x, int nbits);

// swap v[i] with v[bit_reverse[i]]
void bit_reverse_transform(std::vector<std::complex<double> >& v);

// base 2 log rounded up
int base_2_log(int n);

class BitReverseFunctor
{
	public:
		int operator()(int x,int nbits);
	private:
		void compute_row(int nbits);
		std::map<int, std::vector<int> > table;

};

} //namespace RUMBA

#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
