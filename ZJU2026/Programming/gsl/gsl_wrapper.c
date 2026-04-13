#include <gsl/gsl_sf_bessel.h>
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
/* 返回 J0(x) = gsl_sf_bessel_J0(x) */
EXPORT double my_bessel_j0(double x) {
    return gsl_sf_bessel_J0(x);
}