#include "PhysicsObject.h"
#include "Constraint.hpp"
#include "../Shapes/Polygon.h"
#include <math.h>
#include <iostream>

int runningNr = 0;
namespace Dormir{

	PhysicsObject::PhysicsObject(Vec2 StartPos,double newE,double newMu,double newMass){
		
		Init(StartPos,newE,newMu,newMass);
	}

	PhysicsObject::PhysicsObject(Vec2 StartPos,double newE,double newMu)
	{
		Init(StartPos,newE,newMu,0);
	}

	PhysicsObject::PhysicsObject(Vec2 StartPos)
	{
		Init(StartPos,0,0,0);
			
	}

	PhysicsObject::PhysicsObject(double nMass,double nE,double nMu){
		Init(Vec2(0,0),nE,nMu,nMass);
	}

	PhysicsObject::PhysicsObject(){
		Init(Vec2(0,0),0,0,0);
		

	}
	
	void PhysicsObject::Init(Vec2 Pos,double e,double mu,double mass){
		this->Pos=Pos;
		Vel.SetValue(0,0);
		angle=0;
		angleSpeed=0;
		setRestitution(e);
		setFriction(mu); 
		setMass(mass);
		
		
		ID = runningNr;
		runningNr += 1;
	}
	

	void PhysicsObject::setMass(double newMass){
		if(newMass!=0)
			invMass=1/newMass;
		else
			invMass=0;

		mass=newMass;
	}

	void PhysicsObject::setInertia(double newIertia){
		if(newIertia!=0)
			invInertia=1/invInertia;
		else
			invInertia=0;

		inertia=newIertia;
	}

	bool PhysicsObject::LoadPolygon(Dormir::Polygon P){
		if(P.Vertex.size()==0)
			return false;
		if(P.getArea()<0)   // Disse tre linjer antager at polygonen er færdigfremstillet
			P.ReverseVertex();
		P.CalculateAxis();

		Body.push_back(P);
		return true;
	}

	void PhysicsObject::LoadPolygonList(std::list<Polygon> Plist){
		for(std::list<Polygon>::iterator it=Plist.begin();it!=Plist.end();it++){
			LoadPolygon(*it);
		}
	}

	void PhysicsObject::DeleteBody(){
		Body.clear();
	}

	void PhysicsObject::CalculateMomentofInertia(){
		double TotalArea=0;
		for(std::list<Dormir::Polygon>::iterator it=Body.begin();it!=Body.end();it++){
		/*	if((*it)->getArea()<0) // Denne del muligvis fjernes
				(*it)->ReverseVertex();*/
			TotalArea+=it->getArea();
		}
		Pos.SetValue(0,0);
		for(std::list<Dormir::Polygon>::iterator it=Body.begin();it!=Body.end();it++)
			Pos+=it->getCentroid()*(it->getArea()/TotalArea);

		if(mass==0){
			inertia=0;
			invInertia=0;
			return;
		}
		inertia=0;
		for(std::list<Dormir::Polygon>::iterator it=Body.begin();it!=Body.end();it++){
			double PartMass=mass*it->getArea()/TotalArea,partInertia=0,denom=0;
			std::vector<Vec2> V=it->getVertex();
			Vec2 C=it->getCentroid();
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
			inertia+=partInertia+PartMass*(it->getCentroid()-Pos).GetNorm2();
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
		for(std::list<Dormir::Polygon>::iterator it=Body.begin();it!=Body.end();it++){
			it->Move(-Pos.x,-Pos.y);
			it->Rotate(angleSpeed);
			it->Move(Pos.x,Pos.y);
			it->Move(Vel.x,Vel.y);
		}
		for(std::list<Fixpoint *>::iterator it=AttachedPoints.begin();it!=AttachedPoints.end();it++){
			(*it)->Rotate(angleSpeed);
		}
		Pos+=Vel;
	}

	void PhysicsObject::Revert(){
		for(std::list<Fixpoint *>::iterator it=AttachedPoints.begin();it!=AttachedPoints.end();it++){
			(*it)->AdjustPos(Pos*(-1));
			(*it)->Rotate(-angleSpeed);
			(*it)->AdjustPos(Pos);
			(*it)->AdjustPos(Vel*(-1));
		}
		angle-=angleSpeed;
		for(std::list<Dormir::Polygon>::iterator it=Body.begin();it!=Body.end();it++){
			it->Rotate(-angleSpeed);
		}
		Pos-=Vel;
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
		for(std::list<Dormir::Polygon>::iterator it=Body.begin();it!=Body.end();it++)
			it->Move(v);
	}
	
	void PhysicsObject::SetPosition(Vec2 v){
		AdjustPosition(v - Pos);
	}
	

	void PhysicsObject::AdjustPosition(double x,double y){
		Vec2 v(x,y);
		AdjustPosition(v);
	}

	void PhysicsObject::FindBounds(){
		for(std::list<Dormir::Polygon>::iterator itf=Body.begin();itf!=Body.end();itf++){
			itf->FindBounds();
		}
	}

	void PhysicsObject::AdjustAngle(double a){
		angle+=a;
		for(std::list<Dormir::Polygon>::iterator it=Body.begin();it!=Body.end();it++){
			it->Move(Pos*(-1));
			it->Rotate(a);
			it->Move(Pos);

		}
		for(std::list<Fixpoint *>::iterator it=AttachedPoints.begin();it!=AttachedPoints.end();it++){
			(*it)->Rotate(a);
		}

	}

	bool PhysicsObject::AttachPoint(Fixpoint * F){
		for(std::list<Fixpoint *>::iterator it= AttachedPoints.begin();it!=AttachedPoints.end();it++){
			if(F==*it){
				return false;
			}
		}
		F->setPhysicsObject(this);
		AttachedPoints.push_back(F);
		return true;
	}

	bool PhysicsObject::DetachPoint(Fixpoint * F){
		for(std::list<Fixpoint *>::iterator it= AttachedPoints.begin();it!=AttachedPoints.end();it++){
			if(F==*it){
				F->setPhysicsObject(NULL);
				AttachedPoints.erase(it);
				return true;
			}
		}
		return false;
	}
	int PhysicsObject::GetID(){
		return ID;
	}
	
	void PhysicsObject::SetGhost(bool isGhost){
		ghost = isGhost;
	}
	
	void PhysicsObject::IsGhost(){
		return ghost;
	}
	
}
