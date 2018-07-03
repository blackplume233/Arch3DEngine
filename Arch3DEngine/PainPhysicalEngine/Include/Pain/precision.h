

#ifndef PAIN_PRECISION_H
#define PAIN_PRECISION_H

#include <float.h>

namespace Pain 
{
#if 0
	// Defines we're in single precision mode,for any code
	// that needs to be conditionally compiled
    #define SINGLE_PRECISION
	
	// Define a real number precision,Pain can be compiled in single or double
	// precision versions. By default single precision is provided
	typedef float real;

	// Define the highest value for the real number
    #define REAL_MAX FLT_MAX
	
	// Defines the precision of the square root operator
#define real_sqrt sqrtf

	// Define the precision of the absolute magnitude operator
#define real_abs fabsf

#define real_sin sinf

#define real_cos cosf

#define real_exp expf

#define real_pow powf

#define real_fmod fmod

#define real_epsilon FLT_EPSILON

#define R_PI 3.14159f

#else
    #define DOUBLE_PRECISION
	typedef double real;
    #define REAL_MAX DBL_MAX
	// Defines the precision of the square root operator
    #define real_sqrt sqrt

	// Define the precision of the absolute magnitude operator
    #define real_abs fabs

    #define real_sin sin

    #define real_cos cos

    #define real_exp exp

    #define real_pow pow

    #define real_fmod fmod

    #define real_epsilon DBL_EPSILON

    #define R_PI 3.14159265358979f
#endif


};

#endif // PAIN_PRECISION_H