/*
 * Constraint.cpp
 *
 *  Created on: Jul 26, 2011
 *      Author: sebastian
 */

#include "Constraint.hpp"
#include "PhysicsObject.h"
#include <math.h>
#include <iostream>

namespace Dormir{

	void Fixpoint::Rotate(double angle){
		double cos0=cos(angle),sin0=sin(angle);
		P.SetValue(cos0*P.x-sin0*P.y,sin0*P.x+cos0*P.y);
	}

	void Fixpoint::setPhysicsObject(PhysicsObject * O){
		Obj=O;
		//O->AttachPoint(this);
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
		if(F[0].getPhysicsObject()!=NULL && F[1].getPhysicsObject()!=NULL){
			Vec2 v=F[1].getPos()+F[1].getPhysicsObject()->GetPosition()-F[0].getPos()-F[0].getPhysicsObject()->GetPosition();
			double vl=v.GetNorm2();
			v/=vl;
			vl-=x;
			v=v*vl*k;//-v*d*((F[1].getPhysicsObject()->GetVelocity()-F[0].getPhysicsObject()->GetVelocity())*v);
			std::cout<<"spring force ("<<v.x<<","<<v.y<<")\n";
			F[1].getPhysicsObject()->AddForce(v*(-1),F[1].getPos());
			F[0].getPhysicsObject()->AddForce(v,F[0].getPos());
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
