/*
 * Constraint.cpp
 *
 *  Created on: Jul 26, 2011
 *      Author: sebastian
 */

#include "Constraint.hpp"
#include "PhysicsObject.h"
#include <math.h>

namespace Dormir{

	void Fixpoint::Rotate(double angle){
		double cos0=cos(angle),sin0=sin(angle);
		P.SetValue(cos0*P.x-sin0*P.y,sin0*P.x+cos0*P.y);
	}

	Spring::Spring(double springconstant,double dampening,double rest){
		k=springconstant;
		d=dampening;
		if(rest<0)
			x=0;
		else
			x=rest;
	}

	void Spring::ApplyForce(){
		if(O1!=NULL && O2!=NULL){
			Vec2 v=O2->GetPosition()-O1->GetPosition();
			double vl=v.GetNorm2();
			v/=vl;
			vl-=x;
			v=v*vl*k-(O1->GetVelocity()-O2->GetVelocity())*d;
			O1->AddForce(v);
			O2->AddForce(v*(-1));
		}

	}

	SingleSprint::SingleSprint(double springconstant,double dampening,double rest){
		k=springconstant;
		d=dampening;
		if(rest<0)
			x=0;
		else
			x=rest;
	}

	void SingleSprint::ApplyForce(){
		if(O1!=NULL){
			Vec2 v=FixedPos-O1->GetPosition();
			double vl=v.GetNorm2();
			v/=vl;
			vl-=x;
			v=v*vl*k-O1->GetVelocity()*d;
			O1->AddForce(v);
		}
	}

	void SingleSprint::setSprintRest(double nX){
		if(nX<0)
			x=0;
		else
			x=nX;
	}

}
