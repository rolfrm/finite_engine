/*
 * Math.cpp
 *
 *  Created on: Aug 8, 2011
 *      Author: sebastian
 */

#include "Math.hpp"
#include <math.h>
#include <iostream>


		Vec2::Vec2(double x,double y){
		SetValue(x,y);
	}

	void Vec2::operator += (Vec2 V){
		x+=V.x,y+=V.y;
	}
	void Vec2::operator *= (double d){
		x*=d,y*=d;
	}

	void Vec2::operator -= (Vec2 V){
		x-=V.x,y-=V.y;
	}
	void Vec2::operator /= (double d){
		x/=d,y/=d;
	}
	Vec2 Vec2::operator + (Vec2 V){
		return Vec2(x+V.x,y+V.y);
	}
	Vec2 Vec2::operator - (Vec2 V){
		return Vec2(x-V.x,y-V.y);
	}
	Vec2 Vec2::operator * (double d){
		return Vec2(x*d,y*d);
	}

	Vec2 Vec2::operator / (double d){
		return Vec2(x/d,y/d);
	}

	double Vec2::operator * (Vec2 V){
		return V.x*x+V.y*y;
	}

	double Vec2::GetNorm2(){
		return sqrt(x*x+y*y);
	}

	void Vec2::operator =(Vec2 V){
		x=V.x,y=V.y;
	}

	double Crossproduct(Vec2 V,Vec2 U){
		return V.x*U.y-V.y*U.x;
	}

	void Vec2::Print(){
		std::cout<<"("<<x<<","<<y<<")\n";
	}


