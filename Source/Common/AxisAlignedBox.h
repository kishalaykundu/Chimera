/**
 * @file AxisAlignedBox.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The axis-aligned bounding box class for the Canvas framework
 */
#pragma once

#include "Preprocess.h"
#include "Vector.h"

namespace Sim {

	class AxisAlignedBox {

	protected:
		Vector _v[2];
		Vector _center;
		Vector _halflength;

	public:
		// default constructor
		inline AxisAlignedBox ()
		{
			_v[0] = Vector::ZERO;
			_v[1] = Vector::ZERO;
			_center = Vector::ZERO;
			_halflength = Vector::ZERO;
		}
		// copy constructor
		inline AxisAlignedBox (const AxisAlignedBox &bv)
		: _center (bv._center), _halflength (bv._halflength)
		{
			for (int i = 0; i < 2; ++i){
				_v [i] = bv._v [i];
			}
		}
		// overloaded constructor
		inline AxisAlignedBox (const Vector &v0, const Vector &v1)
		{
			_v [0] = v0;
			_v [1] = v1;
			Update ();
		}
		// overloaded constructor
		inline AxisAlignedBox (const Real* v0, const Real* v1)
		{
			_v [0] = Vector (v0);
			_v [1] = Vector (v1);
			Update ();
		}
		// destructor
		inline ~AxisAlignedBox () {}

		// assignment operator
		inline AxisAlignedBox &operator = (const AxisAlignedBox &bv)
		{
			for (int i = 0; i < 2; ++i){
				_v [i] = bv._v [i];
			}
			_center = bv._center;
			_halflength = bv._halflength;
			return *this;
		}

		// corner accessor
		inline Vector operator [] (const int i) const
		{
			assert (i >= 0 && i < 8);
			switch (i){
				case 0:
					return Vector (_v [0]);
				case 1:
					return Vector (_v [1][0], _v [0][1], _v [0][2]);
				case 2:
					return Vector (_v [0][0], _v [1][1], _v [0][2]);
				case 3:
					return Vector (_v [1][0], _v [1][1], _v [0][2]);
				case 4:
					return Vector (_v [0][0], _v [0][1], _v [1][2]);
				case 5:
					return Vector (_v [1][0], _v [0][1], _v [1][2]);
				case 6:
					return Vector (_v [0][0], _v [1][1], _v [1][2]);
				case 7: // goes out of the switch body and returns _v[1]
					break;
			}
			return Vector (_v [1]);
		}

		inline const Vector& Center () const
		{
			return _center;
		}
		inline const Vector& Halflength () const
		{
			return _halflength;
		}

		// update method to update center and halflength
		inline void Update ()
		{
			_center = _v[0]; _center += _v[1];
			_center *= 0.5;
			_halflength = _center; _halflength -= _v[0];
		}

		// update method with min and max specified
		inline void Update (const Vector& min, const Vector& max)
		{
			_v [0] = min; _v [1] = max;
			Update ();
		}

		// bounding box inside another bounding box test
		inline bool Inside (const AxisAlignedBox &bv) const
		{
			return (bv._v[0][0] >= _v[0][0]) & (bv._v[1][0] <= _v[1][0]) & (bv._v[0][1] >= _v[0][1]) &
					(bv._v[1][1] <= _v[1][1]) & (bv._v[0][2] >= _v[0][2]) & (bv._v[1][2] <= _v[1][2]);
		}
	};
}
