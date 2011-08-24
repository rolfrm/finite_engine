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
			v=v*vl*k;//-((F[1].getPhysicsObject()->GetVelocity()-F[0].getPhysicsObject()->GetVelocity())*d);
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

	Joint::Joint(PhysicsObject * O1,double x1,double y1,PhysicsObject * O2,double x2,double y2){
		v_bias=0.2;
		P[0].SetPos(Vec2(x1,y1));
		P[0].setPhysicsObject(O1);
		O1->AttachPoint(&P[0]);
		P[1].SetPos(Vec2(x2,y2));
		P[1].setPhysicsObject(O2);
		O2->AttachPoint(&P[1]);
	}

	Joint::~Joint(){
		if(P[0].getPhysicsObject()!=NULL){
			P[0].getPhysicsObject()->DetachPoint(&P[0]);
		}
		if(P[1].getPhysicsObject()!=NULL){
			P[1].getPhysicsObject()->DetachPoint(&P[1]);
		}
	}

}
