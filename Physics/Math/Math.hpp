/*
 * Math.hpp
 *
 *  Created on: Aug 8, 2011
 *      Author: sebastian
 */

#ifndef MATH_HPP_
#define MATH_HPP_




	class Vec2{
	public:
		Vec2(double x=0,double y=0);

		void operator += (Vec2);
		void operator *= (double);
		void operator -= (Vec2);
		void operator /= (double);
		Vec2 operator + (Vec2);
		Vec2 operator - (Vec2);
		Vec2 operator * (double);
		Vec2 operator / (double);
		double operator * (Vec2);
		void operator = (Vec2);

		void SetValue(double nx,double ny){x=nx;y=ny;};

		double GetNorm2();

		void Print();

		double x,y;
	};

	double Crossproduct(Vec2,Vec2);

#endif /* MATH_HPP_ */
