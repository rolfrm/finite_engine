#include "PhysicsObject.h"
#include "../Shapes/Polygon.h"
#include <math.h>

namespace Dormir{

	PhysicsObject::PhysicsObject(Vec2 StartPos,double newE,double newMu,double newMass){
		Pos=StartPos;
		Vel.SetValue(0,0);
		angle=0;
		angleSpeed=0;
		setRestitution(newE);
		setFriction(newMu);
		setMass(newMass);
	}

	PhysicsObject::PhysicsObject(Vec2 StartPos,double newE,double newMu)
	{
		PhysicsObject(StartPos,newE,newMu,0);
	}

	PhysicsObject::PhysicsObject(Vec2 StartPos)
	{
		PhysicsObject(StartPos,0,0,0);
	}

	PhysicsObject::PhysicsObject(){
		Pos.SetValue(0,0);
		Vel.SetValue(0,0);
		angle=0;
		angleSpeed=0;
		setRestitution(0);
		setFriction(0);
		setMass(0);
	}

	void PhysicsObject::setMass(double newMass){
		if(newMass!=0)
			invMass=1/newMass;
		else
			invMass=0;

		mass=newMass;
	}

	bool PhysicsObject::LoadPolygon(Dormir::Polygon * P){
		if(P->Vertex.size()==0)
			return false;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++)
			if(*it==P)
				return false;
		if(P->getArea()<0)   // Disse tre linjer antager at polygonen er færdigfremstillet
			P->ReverseVertex();
		P->CalculateAxis();

		Body.push_back(P);
		return true;
	}

	void PhysicsObject::DeleteBody(){
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++){
			if(*it!=NULL)
				delete *it;
		}
		Body.clear();
	}

	void PhysicsObject::CalculateMomentofInertia(){
		double TotalArea=0;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++){
		/*	if((*it)->getArea()<0) // Denne del muligvis fjernes
				(*it)->ReverseVertex();*/
			TotalArea+=(*it)->getArea();
		}
		Pos.SetValue(0,0);
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++)
			Pos+=(*it)->getCentroid()*((*it)->getArea()/TotalArea);

		if(mass==0){
			inertia=0;
			invInertia=0;
			return;
		}
		inertia=0;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++){
			double PartMass=mass*(*it)->getArea()/TotalArea,partInertia=0,denom=0;
			std::vector<Vec2> V=(*it)->getVertex();
			Vec2 C=(*it)->getCentroid();
			for(unsigned int i=0;i<V.size();i++)
				V[i]-=C;
			//V-=Temp;
			for(unsigned int i=0;i<V.size()-1;i++)
				denom+=fabs(Crossproduct(V[i],V[i+1]));

			for(unsigned int i=0;i<V.size()-1;i++)
				partInertia+=fabs(Crossproduct(V[i],V[i+1]))*(V[i]*V[i]+V[i+1]*V[i+1]+V[i]*V[i+1]);
			partInertia/=denom;
			partInertia/=6;
			partInertia*=PartMass;
			inertia+=partInertia+PartMass*((*it)->getCentroid()-Pos).GetNorm2();
		}
		invInertia=1/inertia;
	}

	void PhysicsObject::Advance(){
	/*	angle+=angleSpeed;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++){
			(*it)->Move(-Pos(0),-Pos(1));
			(*it)->Rotate(angleSpeed);
		}
		Pos+=Vel;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++){
			(*it)->Move(Pos(0),Pos(1));
		}
		*/
		/*
		 * A hack is tried in order to improve stability
		 */
		if(fabs(angleSpeed)<1e-9)
			angleSpeed=0;
		if(Vel.GetNorm2()<1e-9)
			Vel.SetValue(0,0);
		angle+=angleSpeed;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++){
			(*it)->Move(-Pos.x,-Pos.y);
			(*it)->Rotate(angleSpeed);
			(*it)->Move(Pos.x,Pos.y);
			(*it)->Move(Vel.x,Vel.y);
		}
		Pos+=Vel;
	}

	void PhysicsObject::Revert(){
		angle-=angleSpeed;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++){
			(*it)->Move(Pos*(-1));
			(*it)->Rotate(-angleSpeed);
		}
		Pos-=Vel;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++)
			(*it)->Move(Pos);
	}

	void PhysicsObject::AddForce(Vec2 Force,Vec2 Arm){
		if((Force*invMass).GetNorm2()>TOLERANCE)
			Vel+=Force*invMass;
		if(fabs((Arm.x*Force.y-Arm.y*Force.x)*invInertia)>TOLERANCE){
			//std::cout<<"adding "<<(Arm(0)*Force(1)-Arm(1)*Force(0))*invInertia<<"\n";
			angleSpeed+=(Arm.x*Force.y-Arm.y*Force.x)*invInertia;
		}
	}

	void PhysicsObject::AddForce(Vec2 Force){
		Vec2 Arm;
		Arm.SetValue(0,0);
		AddForce(Force,Arm);
	}

	void PhysicsObject::AddForce(double x,double y){
		Vec2 Force;
		Force.SetValue(x,y);
		AddForce(Force);
	}

	void PhysicsObject::AdjustPosition(Vec2 v){
		Pos+=v;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++)
			(*it)->Move(v);

	}

	void PhysicsObject::AdjustPosition(double x,double y){
		Vec2 v(x,y);
		AdjustPosition(v);
	}

	void PhysicsObject::FindBounds(){
		for(std::list<Dormir::Polygon *>::iterator itf=Body.begin();itf!=Body.end();itf++){
			(*itf)->FindBounds();
		}
	}

	void PhysicsObject::AdjustAngle(double a){
		angle+=a;
		for(std::list<Dormir::Polygon *>::iterator it=Body.begin();it!=Body.end();it++){
			(*it)->Move(Pos*(-1));
			(*it)->Rotate(a);
			(*it)->Move(Pos);

		}

	}
}
