#include "PhysicsCore.h"
#include "CollisionDetection/SeperatingAxis.hpp"
#include "Objects/PhysicsObject.h"
#include "Objects/Constraint.hpp"
#include "Shapes/Polygon.h"
#include <math.h>
#include <iostream>




namespace Dormir{
	Core::Core(unsigned int nMaxNodes){
		CollisionDetecter=new SeperatingAxis(this);
		allocatedNodes=0;
		allocatedContactNodes=0;
		allocatedImpulseNodes=0;
		maxNodes=nMaxNodes;
		Nodes = new CollisionNode[nMaxNodes];
		ImpulseNodes = new CollisionNode[nMaxNodes];
		ContactNodes = new CollisionNode[nMaxNodes];
		runs=0;

		setGravity(0,0);
		setVelocityDampening(0);
		setRotationDampening(0);
		setSlopOverlap(2);
		setBiasOverlap(0.2);
	}

	Core::~Core(){
		delete CollisionDetecter;
		delete [] Nodes;
	}

	void Core::Run(){
		for(std::list<Dormir::PhysicsObject *>::iterator it=Objects.begin();it!=Objects.end();it++){
			(*it)->Advance();
		}

		CollisionDetecter->Run();

		for(std::list<Dormir::PhysicsObject *>::iterator it=Objects.begin();it!=Objects.end();it++){
			(*it)->AddForce(Gravity*(*it)->GetMass());
			(*it)->AddForce((*it)->GetVelocity()*-velocityDampening);
			(*it)->AddTorque((*it)->GetAnglespeed()*-rotationDampening);
		}


		for(unsigned int i=0;i<constraints.size();i++)
			constraints[i]->ApplyForce();



		double A[allocatedNodes];
		double T[allocatedNodes];
		double K1[Joints.size()],K2[Joints.size()],K3[Joints.size()];
		for(unsigned int i=0;i<allocatedNodes;i++){
			Nodes[i].I=0;
			//A[i]=impulse->CalculateEntryA(Nodes[i].axis,Nodes[i].axis,Nodes[i].rf,Nodes[i].rf,Nodes[i].from)+impulse->CalculateEntryA(Nodes[i].axis,Nodes[i].axis,Nodes[i].rt,Nodes[i].rt,Nodes[i].to);
			A[i]=CalculateImpulseCoeffiecient(Nodes[i].axis,Nodes[i].rf,Nodes[i].rt,Nodes[i].from,Nodes[i].to);
			Nodes[i].T=0;
			//T[i]=impulse->CalculateEntryA(Nodes[i].tangent,Nodes[i].tangent,Nodes[i].rf,Nodes[i].rf,Nodes[i].from)+impulse->CalculateEntryA(Nodes[i].tangent,Nodes[i].tangent,Nodes[i].rt,Nodes[i].rt,Nodes[i].to);
			T[i]=CalculateImpulseCoeffiecient(Nodes[i].tangent,Nodes[i].rf,Nodes[i].rt,Nodes[i].from,Nodes[i].to);
		}
		for(unsigned int i=0;i<Joints.size();i++){
			PhysicsObject * O=Joints[i]->P[0].getPhysicsObject(),* O2=Joints[i]->P[1].getPhysicsObject();
			Vec2 r=Joints[i]->P[0].getPos(),r2=Joints[i]->P[1].getPos();
			double k=O->GetInverseMass()+r.GetNorm2()*r.GetNorm2()*O->GetInverseInertia();
			k+=O2->GetInverseMass()+r2.GetNorm2()*r2.GetNorm2()*O2->GetInverseInertia();
			K1[i]=k-(r.x*r.x*O->GetInverseInertia()+r2.x*r2.x*O2->GetInverseInertia());
			K2[i]=-(r.x*r.y*O->GetInverseInertia()+r2.x*r2.y*O2->GetInverseInertia());
			K3[i]=k-(r.y*r.y*O->GetInverseInertia()+r2.y*r2.y*O2->GetInverseInertia());
			//O->AddForce(Joints[i]->AccummulatedImpulse,r);
			//O2->AddForce(Joints[i]->AccummulatedImpulse*(-1),r2);
			//std::cout<<"constants "<<O2->GetInverseMass()<<" "<<O2->GetInverseInertia()<<"\n";
		//	std::cout<<"booze "<<k<<" "<<K1[i]<<" "<<K2[i]<<" "<<K3[i]<<"\n";
		//	J[i]=CalculateImpulseCoeffiecient(Joints[i])
		}


		for(unsigned int i=0;i<allocatedNodes;i++){
			for(unsigned int j=0;j<allocatedImpulseNodes;j++){
				if(ImpulseNodes[j].to==Nodes[i].to && ImpulseNodes[j].from==Nodes[i].from && (ImpulseNodes[j].contact-Nodes[i].contact).GetNorm2()<1){
					Nodes[i].to->AddForce(Nodes[i].axis*ImpulseNodes[j].I,Nodes[i].rt);
					Nodes[i].from->AddForce(Nodes[i].axis*-ImpulseNodes[j].I,Nodes[i].rf);
					Nodes[i].I=ImpulseNodes[j].I;
					Nodes[i].to->AddForce(Nodes[i].tangent*ImpulseNodes[j].T,Nodes[i].rt);
					Nodes[i].from->AddForce(Nodes[i].tangent*-ImpulseNodes[j].T,Nodes[i].rf);
					Nodes[i].T=ImpulseNodes[j].T;
					break;
				}
			}
		}

		for(unsigned int l=0;l<30;l++){
			double N=0;



			for(unsigned int i=0;i<allocatedNodes;i++){
				double vbias=biasOverlap*Max(0,Nodes[i].intersection-slopOverlap);
				double P=(-CalculateRelativeVelocity(Nodes[i].axis,Nodes[i].rf,Nodes[i].rt,Nodes[i].from,Nodes[i].to)+vbias)/A[i];
			//	std::cout<<i<<" "<<Nodes[i].I<<" impulse\n";
				double tempI=Nodes[i].I;
				Nodes[i].I+=P;
				if(Nodes[i].I<0)
					Nodes[i].I=0;
				Nodes[i].to->AddForce(Nodes[i].axis*(-tempI+Nodes[i].I),Nodes[i].rt);
				Nodes[i].from->AddForce(Nodes[i].axis*-(-tempI+Nodes[i].I),Nodes[i].rf);

				N+=fabs(-tempI+Nodes[i].I);
				double F=-CalculateRelativeVelocity(Nodes[i].tangent,Nodes[i].rf,Nodes[i].rt,Nodes[i].from,Nodes[i].to)/T[i];
				double tempT=Nodes[i].T;
				//double u=0.6;
				double u=sqrt(Nodes[i].to->GetFriction()*Nodes[i].from->GetFriction());
				Nodes[i].T=Clamp(tempT+F,-u*Nodes[i].I,u*Nodes[i].I);
				Nodes[i].to->AddForce(Nodes[i].tangent*(Nodes[i].T-tempT),Nodes[i].rt);
				Nodes[i].from->AddForce(Nodes[i].tangent*-(Nodes[i].T-tempT),Nodes[i].rf);

			}
			for(unsigned int i=0;i<Joints.size();i++){
				double k1=K1[i],k2=K2[i],k3=K3[i];
				PhysicsObject * O=Joints[i]->P[0].getPhysicsObject(),* O2=Joints[i]->P[1].getPhysicsObject();
				Vec2 P,dv=O->GetVelocity(),r=Joints[i]->P[0].getPos(),r2=Joints[i]->P[1].getPos();
				dv+=Vec2(-O->GetAnglespeed()*r.y,O->GetAnglespeed()*r.x);
				dv-=O2->GetVelocity();
				dv-=Vec2(-O2->GetAnglespeed()*r2.y,O2->GetAnglespeed()*r2.x);
				Vec2 dP=O->GetPosition()+Joints[i]->P[0].getPos()-O2->GetPosition()-Joints[i]->P[1].getPos();
				dP*=-Joints[i]->v_bias;
				dv-=dP;
				P.y=(-dv.y+dv.x*k2/k1)/(k3-k2*k2/k1);
				P.x=-(dv.x+P.y*k2)/k1;
				O->AddForce(P,r);
				O2->AddForce(P*(-1),r2);
				Joints[i]->AccummulatedImpulse+=P;
				N+=P.GetNorm2();
			}

			N/=allocatedNodes;
			if(N<1e-7){
				break;
			}
		}

		allocatedImpulseNodes=0;
		for(unsigned int i=0;i<allocatedNodes;i++){
			AddImpulseNode(Nodes[i]);
		}

		for(unsigned int i=0;i<allocatedNodes;i+=2){
			Nodes[i].to->OnCollision(&Nodes[i]);
			Nodes[i].from->OnCollision(&Nodes[i]);
		}



		allocatedNodes=0;
	}

	void Core::AddImpulseNode(Dormir::CollisionNode N){
		if(allocatedImpulseNodes < maxNodes ){
			ImpulseNodes[allocatedImpulseNodes]=N;
			allocatedImpulseNodes++;
		}
	}

	bool Core::LoadObject(Dormir::PhysicsObject * obj){
		for(std::list<Dormir::PhysicsObject *>::iterator it=Objects.begin();it!=Objects.end();it++){
			if(obj==*it)
				return false;
		}
		obj->CalculateMomentofInertia();
		Objects.push_back(obj);
		return true;
	}

	bool Core::LoadJoint(Dormir::Joint * J){
		for(unsigned int i=0;i<Joints.size();i++){
			if(Joints[i]==J)
				return false;
		}
		Joints.push_back(J);
		return true;
	}


	bool Core::UnloadObject(Dormir::PhysicsObject * obj){
		for(std::list<Dormir::PhysicsObject *>::iterator it=Objects.begin();it!=Objects.end();it++){
			if(obj==*it){
				Objects.erase(it);
				return true;
			}
		}
		return false;

	}

	bool Core::UnloadJoint(Dormir::Joint * J){
		for(unsigned int i=0;i<Joints.size();i++){
			if(Joints[i]==J){
				Joints.erase(Joints.begin()+i);
				return true;
			}
		}
		return false;
	}

	void Core::AddCollisionNode(Dormir::CollisionNode N){
		if(allocatedNodes < maxNodes ){
			Nodes[allocatedNodes]=N;
			allocatedNodes++;
		}
		else
			ExpandCollisionNodes(maxNodes*2);
	}

	Dormir::PhysicsObject * Core::PointInsideObject(int x,int y){
		for(std::list<Dormir::PhysicsObject *>::iterator it=Objects.begin();it!=Objects.end();it++){
			for(std::list<Dormir::Polygon>::iterator itf=(*it)->Body.begin();itf!=(*it)->Body.end();itf++){
				itf->FindBounds();
				if(itf->BoundingBox[0].x<x && x<itf->BoundingBox[1].x && itf->BoundingBox[0].y<y && y<itf->BoundingBox[1].y )
					return *it;
			}
		}
		return NULL;
	}

	Dormir::PhysicsObject * Core::PointInsideObject(Vec2 v){
		return PointInsideObject((int)v.x,(int)v.y);
	}

	double Core::RoundDouble(double doValue, int nPrecision){
		static const double doBase = 10.0;
		double doComplete5, doComplete5i;

		doComplete5 = doValue * pow(doBase, (double) (nPrecision + 1));

		if(doValue < 0.0)
		doComplete5 -= 5.0;
		else
		doComplete5 += 5.0;

		doComplete5 /= doBase;
		modf(doComplete5, &doComplete5i);

		return doComplete5i / pow(doBase, (double) nPrecision);
	}

	double Core::Clamp(double value,double lower,double upper){
		if(value<=lower)
			return lower;
		else if(value>=upper)
			return upper;
		else
			return value;
	}



	void Core::setSlopOverlap(double d){
		if(d>0)
			slopOverlap=d;
		else
			slopOverlap=0;
	}

	double Core::Max(double v1,double v2){
		if(v1<v2)
			return v2;
		else
			return v1;
	}

	void Core::ExpandCollisionNodes(unsigned int Count){
		if(Count>maxNodes){
			std::cout<<"expanding collision nodes with "<<Count<<"\n";
			CollisionNode * temp = new CollisionNode[Count];
			for(unsigned int i=0;i<allocatedNodes;i++)
				temp[i]=Nodes[i];
			delete [] Nodes;
			Nodes=temp;
			maxNodes=Count;

			temp=new CollisionNode[Count];

			for(unsigned int i=0;i<allocatedImpulseNodes;i++)
				temp[i]=ImpulseNodes[i];


			delete [] ImpulseNodes;
			ImpulseNodes=temp;

		}
	}

	bool Core::LoadConstraint(Dormir::Constraint * con){
		for(unsigned int i=0;i<constraints.size();i++)
			if(constraints[i]==con)
				return false;
		constraints.push_back(con);
		return true;
	}

	bool Core::UnloadConstraint(Dormir::Constraint * con){
		for(unsigned int i=0;i<constraints.size();i++)
			if(constraints[i]==con){
				constraints.erase(constraints.begin()+i);
				return true;
			}
		return false;
	}

	double Core::CalculateImpulseCoeffiecient(Vec2 n,Vec2 rf,Vec2 rt,PhysicsObject * from,PhysicsObject * to){
		//		arma::vec2 temp=nj*obj->GetInverseMass()-(rj*(arma::dot(ri,nj))-nj*(arma::dot(ri,rj)))*obj->GetInverseInertia();
		//return from->GetInverseMass()+to->GetInverseMass()+Crossproduct(rf,n)*Crossproduct(rf,n)*from->GetInverseInertia()+Crossproduct(rt,n)*Crossproduct(rt,n)*to->GetInverseInertia();
		Vec2 temp=n*from->GetInverseMass()-(rf*(rf*n)-n*(rf*rf))*from->GetInverseInertia();
		temp+=n*to->GetInverseMass()-(rt*(rt*n)-n*(rt*rt))*to->GetInverseInertia();
		return temp*n;
	}

	double Core::CalculateRelativeVelocity(Vec2 n,Vec2 rf,Vec2 rt,PhysicsObject * from,PhysicsObject * to){
		Vec2 temp2,temp3;
		temp2.SetValue(-rt.y*to->GetAnglespeed(),rt.x*to->GetAnglespeed());
		temp2+=to->GetVelocity();
		temp3.SetValue(-rf.y,rf.x);
		temp3*=from->GetAnglespeed();
		temp3+=from->GetVelocity();
		return n*(temp2-temp3);
	}


}
