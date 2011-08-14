/*
 * PhysicsCore.h
 *
 *  Created on: Feb 10, 2011
 *      Author: sebastian
 */

#include <list>
#include <vector>
#include "Math/Math.hpp"

#ifndef PHYSICSCORE_H_
#define PHYSICSCORE_H_

namespace Dormir{

	class PhysicsObject;
	class SeperatingAxis;
	class Model;
	class Constraint;
	class Joint;


	struct CollisionNode{
		Dormir::PhysicsObject * from,* to;
		Vec2 axis,contact,rf,rt,tangent;
		double intersection,I,T;
	};

	class Core{
	public:
		Core(unsigned int nMaxNodes);
		~Core();

		void Run();

		bool LoadObject(Dormir::PhysicsObject *);
		bool LoadConstraint(Dormir::Constraint *);
		bool UnloadObject(Dormir::PhysicsObject *);
		bool UnloadConstraint(Dormir::Constraint *);

		void AddCollisionNode(CollisionNode);

		void setGravity(double x,double y){Gravity.x=x,Gravity.y=y;};

		Vec2 getGravity(){return Gravity;};

		void setVelocityDampening(double d){velocityDampening=d;};

		void setRotationDampening(double d){rotationDampening=d;};

		void setSlopOverlap(double d);
		void setBiasOverlap(double d){biasOverlap=Clamp(d,0.1,0.3);};

		double Clamp(double value,double lower,double upper);

		Dormir::PhysicsObject * PointInsideObject(int x,int y);
		Dormir::PhysicsObject * PointInsideObject(Vec2);
		std::list<Dormir::PhysicsObject *> Objects;
		std::vector<Dormir::Joint *> Joints;

		friend class SeperatingAxis;
		friend class LCPSolver;
		friend class Model;
		friend class Impulse;
		friend class Intersection;
		friend class Contact;
	protected:
		void AddImpulseNode(CollisionNode);
		double RoundDouble(double doValue, int nPrecision);
		double Max(double v1,double v2);
		void ExpandCollisionNodes(unsigned int);
		double CalculateImpulseCoeffiecient(Vec2 n,Vec2 rf,Vec2 rt,PhysicsObject * from,PhysicsObject * to);
		double CalculateRelativeVelocity(Vec2 n,Vec2 rf,Vec2 rt,PhysicsObject * from,PhysicsObject * to);
		Dormir::SeperatingAxis * CollisionDetecter;
		std::vector<Dormir::Constraint *> constraints;
		CollisionNode * Nodes,* PrevNodes,* ImpulseNodes,* ContactNodes;
		Vec2 Gravity;
		unsigned int allocatedNodes,allocatedContactNodes,allocatedImpulseNodes,maxNodes,runs;
		double velocityDampening,rotationDampening,slopOverlap,biasOverlap;
	};

}


#endif /* PHYSICSCORE_H_ */
