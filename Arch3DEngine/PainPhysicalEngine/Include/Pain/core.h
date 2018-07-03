#pragma once
/*
* Interface file for core components and functions
* Parts of the Pain physics system.
*/

#include <cmath>

/**
* The core contains utility functions,helps and a basic set of
* mathematical types
*/
#ifndef PAIN_CORE_H
#define PAIN_CORE_H

#include "precision.h"

/**
* The pain name space include all pain functions and 
* classes. It is defined as a namespace to allow function and class
* name to be simple without causing conflicts.
*/
namespace Pain
{
	/**
	* Hold the value for energy under which a body will be put to
	* sleep. This is a global value for the whole solution. By default
	* it is 0.1,which is fine for simulation when gravity is about 20 units
	* per seconds squared,masses are about one,and other forces are around that
	* of gravity. It may need tweaking if your simulation is drastically different
	* to this
	*/
	extern real sleepEpsilon;

	/**
	* Set the current sleep epsilon value: the kinetic energy under which a body
	* may be put to sleep. Bodies are put to sleep if they appear to have a stable
	* kinetic energy less than this value. For simulations that often have low
	* values (such as slow moving,or light objects), this may be reducing.
	* 
	*
	*/
	void setSleepEpsilon(real value);

	// Get the current value of the sleep epsilon parameter
	real getSleepEpsilon();

	/**
	* Holds a vector in 3 dimensions. Four data members are allocated
	* to ensure aligments in an array
	* Note: This class contains a lot of inline methods for basic mathmetics.
	* The  implementations are included in the header file
	*/
	class Vector3
	{
	public:
		// Holds the value along the x axis.
		real x;

		// Holds the value along the y axis
		real y;

		// Holds the value along the z axis
		real z;

	private:
		// Padding to ensure 4 word alignment
		real pad;

	public:
		// the default constructor creates a zero vector
		Vector3() :x(0), y(0), z(0) {}

		// The explicit constructor creates a vector with the given components
		Vector3(const real x, const real y, const real z);

		const static Vector3 GRAVITY;
		const static Vector3 HIGH_GRAVITY;
		const static Vector3 UP;
		const static Vector3 RIGHT;
		const static Vector3 OUT_OF_SCREEN;
		const static Vector3 X;
		const static Vector3 Y;
		const static Vector3 Z;

		// .... Other Vector3 code as before

		real operator[](unsigned i)const
		{
			if (i == 0)  return x;
			if (i == 1) return  y;
			if (i == 2) return z;
			return 0;
		}

		real operator[](unsigned i)
		{
			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;
			return 0;
		}

		// add the given vector to this
		void operator+=(const Vector3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}

		// Return the value of the given vector added to this
		Vector3 operator+(const Vector3& v)const
		{
			return Vector3(x + v.x, y + v.y, z + v.z);
		}

		// Subtracts the given vector from this
		void operator-=(const Vector3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		// Return the value of the given vector subtracted from this
		Vector3 operator -(const Vector3& v)const
		{
			return Vector3(x - v.x, y - v.y, z - v.z);
		}

		// Multiplies this vector by the given scalar
		void operator*=(const real value)
		{
			x *= value;
			y *= value;
			z *= value;
		}

		// Return a copy of this vector scaled the given value
		Vector3 operator*(const real value)const
		{
			return Vector3(x*value, y*value, z*value);
		}

		// Calculate and return a component-wise product of this vector with the given vector
		Vector3 componentProduct(const Vector3& vector)const
		{
			return Vector3(x*vector.x, y*vector.y, z*vector.z);
		}

		// Performs a component-wise product with the given vector and sets this
		// vector to its result
		void componentProductUpdate(const Vector3& vector)
		{
			x *= vector.x;
			y *= vector.y;
			z *= vector.z;
		}

		// Calculates and returns the vector product of this vector
		// with the given vector
		Vector3 vectorProduct(const Vector3& vector)const
		{
			return Vector3(y*vector.z - z * vector.y,
				z*vector.x - x * vector.z,
				x*vector.y - y * vector.x);
		}

		// Updates this vector to be the vector product of its current value and
		// the given vector
		void operator%=(const Vector3& vector)
		{
			*this = vectorProduct(vector);
		}

		// Calculates and returns the vector product of this vector
		// with the given vector
		Vector3 operator%(const Vector3& vector)const
		{
			return Vector3(y*vector.z - z * vector.y,
				z*vector.x - z * vector.z,
				x*vector.y - y * vector.x);
		}

		// Calculates and returns the scalar product of this vector with the given vector
		real scalarProduct(const Vector3& vector)const
		{
			return x * vector.x + y * vector.y + z * vector.z;
		}

		// Calculate and returns the scalar product of this vector with the given vector
		real operator*(const Vector3& vector)const
		{
			return x * vector.x + y * vector.y + z * vector.z;
		}

		// Add the given vector to this,scaled by the given amount
		void addScaledVector(const Vector3& vector, real scale)
		{
			x += vector.x*scale;
			y += vector.y*scale;
			z += vector.z*scale;
		}

		// Get the magnitude of this vector
		real magnitude()const
		{
			return real_sqrt(x*x + y * y + z * z);
		}

		// Gets the squared magnitude of this vecotr
		real squareMagnitude()const
		{
			return x * x + y * y + z * z;
		}

		// Limits the size of the vector to the given maximum
		void trim(real size)
		{
			if (squareMagnitude() > size)
			{
				normalise();
				x *= size;
				y *= size;
				z *= size;
			}
		}


		// Turns a non-zero vector into a vector of unit length
		void normalise()
		{
			real length = magnitude();
			if (length > 0)
			{
				(*this) *= ((real)1 / length);
			}
		}

		// Returns the normalized version of a vector
		Vector3 unit()const
		{
			Vector3 result = *this;
			result.normalise();
			return result;
		}

		// Checks if the two vectors have identical components
		bool operator==(const Vector3& other)const {
			return x == other.x &&
				y == other.y &&
				z == other.z;
		}

		// Check if the two vectors have non-identical components
		bool operator!=(const Vector3& other)const {
			return !(*this == other);
		}

		// Checks if this vector is component-by-component less than the other
		// Note: This does not behave like a single-value comparison:
		// !(a < b) does not imply (b > = a)
		bool operator< (const Vector3& other)const
		{
			return x < other.x && y < other.y && z < other.z;
		}

		bool operator >(const Vector3& other)const {
			return x > other.x && y > other.y && z > other.z;
		}

		bool operator <=(const Vector3& other)const {
			return x <= other.x && y <= other.y && z <= other.z;
		}

		bool operator>=(const Vector3& other)const
		{
			return x >= other.x && y >= other.y && z >= other.z;
		}

		// Zero all the components of the vector
		void clear()
		{
			x = y = z = 0;
		}

		// Flips all the components of the vector
		void invert()
		{
			x = -x;
			y = -y;
			z = -z;
		}

	}; // end class Vector3

	//////////////////////////////////////////////////////////////////////////

	/**
	* Holds a three degree of freedom orientation.
	* 
	* Quaternions have
	* several mathematical properties that make them useful for representing
	* orientations, but require four items of data to hold the three degrees
	* of freedom. These four items of data can be viewed as the coefficients
	* of a complex number with three imaginary parts. The mathematics of the
	* quaternion is then defined and is roughly correspondent to the math of 3D
	* rotations. A quaternion is only a valid rotation if it is normalized: i.e.
	* it has a length of 1.
	*/
	class Quaternion
	{
	public:
		union 
		{
			struct
			{
				// Hold the real component of the quaternion
				real r;

				// Hold the first complex component of the quaternion
				real i;

				// hold the second complex component of the quaternion
				real j;

				// Hold the third complex component of the quaternion
				real k;
			};
			
			// Hold the quaternion data in array form
			real data[4];
		}; // end union

		// The default constructor creates a quaternion representing a zero rotation
		Quaternion() :r(0), i(0), j(0), k(0) {}

		// The explicit constructor creates a quaternion with the given components
		Quaternion(const real realValue, const real iValue, const real jValue, const real kValue) :r(realValue), i(iValue), j(jValue), k(kValue) {}

		// Normalizes the quaternion to unit length, marking it a valid orientation quaternion
		void normalise()
		{
			real d = r * r + i * i + j * j + k * k;

			// Check for zero length quaternion, and use the no-rotation quaternion in that case
			if (d < real_epsilon)
			{
				r = 1;
				return;
			}
			d = ((real)1.0) / real_sqrt(d);
			r *= d;
			i *= d;
			j *= d;
			k *= d;
		}

		// Multiplies the quaternion by the given quaternion

		void operator*=(const Quaternion& multiplier)
		{
			Quaternion q = *this;
			i = q.r*multiplier.i + q.i*multiplier.r + 
				q.j*multiplier.k - q.k*multiplier.j;
			j = q.r*multiplier.j + q.j*multiplier.r +
				q.k*multiplier.i - q.i*multiplier.k;
			k = q.r*multiplier.k + q.k*multiplier.r +
				q.i*multiplier.j - q.j*multiplier.i;
			r = q.r*multiplier.r - q.i*multiplier.i -
				q.j*multiplier.j - q.k*multiplier.k;

		}
		
		// Add the given vector to this, scaled by the given amount
		// This is used to update the orientation quaternion by a rotation and time.
		void addScaledVector(const Vector3& vector, real scale)
		{
			Quaternion q(0, 
				         vector.x*scale,
				         vector.y*scale,
				         vector.z*scale);
			q *= (*this);
			r += q.r*((real)0.5);
			i += q.i*((real)0.5);
			j += q.j*((real)0.5);
			k += q.k*((real)0.5);
		}

		void rotateByVector(const Vector3& vector)
		{
			Quaternion q(0, vector.x, vector.y, vector.z);
			(*this) *= q;
		}

	}; // end class Quaternion

	/**
	* Hold a transform matrix,consisting of a rotation matrix and a position.
	* The matrix has 12 elements, it is assumed that the remaining four are
	* (0,0,0,1); producing a homogenous matrix.
	*/
	class Matrix
	{
	public:
		// Hold the transform matrix data in array form
		real data[12];

		// creates an identity matrix
		Matrix()
		{
			data[0] = 1; data[1] = data[2] = data[3] = 0;
			data[4] = 1; data[5] = 0; data[6] = data[7] = 0;
			data[8] = data[9] = 0; data[10] = 1; data[11] = 0;
		}

		// Set the matrix to be a diagonal matrix with the given coefficients

		void setDiagonal(real a, real b, real c)
		{
			data[0] = a;
			data[5] = b;
			data[10] = c;
		}

		// Returns a matrix which is this matrix multiplied by the given other matrix
		Matrix operator*(const Matrix& o)const 
		{
			Matrix result;
			result.data[0] = (data[0] * o.data[0]) + (data[1] * o.data[4]) + (data[2] * o.data[8]);
			result.data[4] = (data[4] * o.data[0]) + (data[5] * o.data[4]) + (data[6] * o.data[8]);
			result.data[8] = (data[8] * o.data[0]) + (data[9]*o.data[4])

		}

	};



}




#endif
