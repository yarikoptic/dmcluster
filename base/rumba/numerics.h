#ifndef RUMBA_NUMERICS_H
#define RUMBA_NUMERICS_H

/**  	\file numerics.h 
  * 	\brief a documented file 
  */



#include <vector>
#include <complex>

/**
  * The RUMBA namespace
  */
namespace RUMBA
{

/**
 * \fn double RUMBA::gamma_log(double t) 
 * Returns the natural log of the gamma function evaluated at t.
 * It turns out that this is more often used in implementing 
 * numerical computations than the gamma function
 */
double gamma_log(double t);

double gamma(double t);

//! gamma probability distribution function.
double incomplete_gamma ( double x, double a  ); 

//! in-place fft
void fft (std::vector<std::complex<double> >& v);


/**
 * these magic numbers, and the hrf come from Worsley: 
 * fmristat A general statistical analysis for fMRI data
 * a1b1 the time to the peak of the first gamma pdf
 * a2b2 the time to the peak of the second (subtracted) gamma pdf
 * c is a rescale parameter for the second PDF (which should 
 * be smaller in magnitude to control dip size)
 */
double hrf
    (double t, 
     double a1 = 6, 
     double a2 = 12, 
     double b1 = 0.9, 
     double b2 = 0.9, 
     double c = 0.35);

    /** 
     *  Solve x^3 + a2 x^2 + a1 x + a0 
     */
    void solve_cubic ( 
		double a2, double a1, double a0,
		std::complex<double>& r1, 
		std::complex<double>& r2, 
		std::complex<double>& r3 
		);
}
#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
