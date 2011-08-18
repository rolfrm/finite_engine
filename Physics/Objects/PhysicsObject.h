/*
 * PhysicsObject.h
 *
 *  Created on: Feb 10, 2011
 *      Author: sebastian
 */

//#include "../../Utils/VectorMath.h"
#include <list>
#include "../Math/Math.hpp"

#ifndef PHYSICSOBJECT_H_
#define PHYSICSOBJECT_H_
#define TOLERANCE 1e-10

namespace Dormir{

	class Core;

	class SeperatingAxis;

	class LCPSolver;

	class Polygon;

	struct CollisionNode;

	class Fixpoint;



	class PhysicsObject{
	public:
		PhysicsObject(Vec2 Pos,double e,double mu,double mass);
		PhysicsObject(Vec2 Pos,double e,double mu);
		PhysicsObject(Vec2);
		PhysicsObject(double mass,double e,double mu);
		PhysicsObject();

		void Advance();
		void Revert();
		void AdjustPosition(Vec2 V);
		void AdjustPosition(double x,double y);
		void SetPosition(Vec2 V){Pos=V;};
		void AdjustAngle(double a);
		void AddForce(Vec2 Force,Vec2 Arm);
		void AddForce(Vec2 Force);
		void AddForce(double x,double y);
		void AddTorque(double T){angleSpeed+=T*invInertia;};

		bool LoadPolygon(Dormir::Polygon P);
		void LoadPolygonList(std::list<Polygon>);
		bool AttachPoint(Fixpoint * F);
		bool DetachPoint(Fixpoint * F);
		void DeleteBody();
		double GetMass(){return mass;};
		double GetInverseMass(){return invMass;};
		double GetInverseInertia(){return invInertia;};
		double GetInertia(){return inertia;};
		double GetAnglespeed(){return angleSpeed;};
		double GetAngle(){return angle;};
		double GetRestitution(){return e;};
		double GetFriction(){return mu;};
		Vec2 GetVelocity(){return Vel;};
		Vec2 GetPosition(){return Pos;};
		void setMass(double newMass);
		void setInertia(double newInertia);
		void setRestitution(double newE){e=newE;};
		void setFriction(double newMu){mu=newMu;};
		void setAnglespeed(double a){angleSpeed=a;};
		void setVelocity(double x,double y){Vel.x=x;Vel.y=y;};
		void FindBounds();
		void CalculateMomentofInertia();

		virtual void OnCollision(CollisionNode *){};

		friend class SeperatingAxis;
		friend class LCPSolver;
		friend class Model;
		friend class Impulse;
		friend class Core;
		std::list<Dormir::Polygon> Body;
		std::list<Fixpoint *> AttachedPoints;
	protected:
		void AdvancePosition();
		void AdvanceAngle();
		void RevertPosition();
		void RevertAngle();

		Vec2 Pos,Vel;
		double e,mu,mass,invMass,invInertia,inertia,angle,angleSpeed;
		Dormir::Core * World;
	};

}


#endif /* PHYSICSOBJECT_H_ */
