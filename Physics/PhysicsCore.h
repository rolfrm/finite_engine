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
	class Polygon;


	class CollisionNode{
		public:
			Dormir::PhysicsObject * from,* to;
			Vec2 axis,contact,rf,rt,tangent;
			double intersection,I,T;
		Dormir::PhysicsObject * GetFrom(){
			return from;
		}
		Dormir::PhysicsObject * GetTo(){
			return to;
		}
		double GetIntersection(){
			return intersection;
		}		
		double GetI(){
			return I;
		}
		double GetT(){
			return T;
		}
	};

	class Core{
	public:
		Core(unsigned int nMaxNodes);
		~Core();

		void Run();

		bool LoadObject(Dormir::PhysicsObject *);
		bool LoadConstraint(Dormir::Constraint *);
		bool LoadJoint(Dormir::Joint *);
		bool LoadGhostPolygon(Dormir::Polygon *);
		bool UnloadObject(Dormir::PhysicsObject *);
		bool UnloadConstraint(Dormir::Constraint *);
		bool UnloadJoint(Dormir::Joint *);
		bool UnloadGhostPolygon(Dormir::Polygon *);

		void AddCollisionNode(CollisionNode);

		void setGravity(double x,double y){Gravity.x=x,Gravity.y=y;};

		Vec2 getGravity(){return Gravity;};

		void setVelocityDampening(double d){velocityDampening=d;};

		void setRotationDampening(double d){rotationDampening=d;};

		void setSlopOverlap(double d);
		void setBiasOverlap(double d){biasOverlap=Clamp(d,0.1,0.3);};
		double Clamp(double value,double lower,double upper);
		CollisionNode GetNextCollision();
		bool CollisionsReady();



		Dormir::PhysicsObject * PointInsideObject(int x,int y);
		Dormir::PhysicsObject * PointInsideObject(Vec2);


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
		std::list<Dormir::PhysicsObject *> Objects;
		std::vector<Dormir::Joint *> Joints;
		std::list<Polygon *> GhostPolygons;
		CollisionNode * Nodes,* ImpulseNodes;
		std::vector<Dormir::CollisionNode> GhostNodes;

		Vec2 Gravity;
		unsigned int allocatedNodes,allocatedContactNodes,allocatedImpulseNodes,maxNodes,runs,currentCollisionNode;
		double velocityDampening,rotationDampening,slopOverlap,biasOverlap;



		//std::list<CollisionNode> savedNodes;
	};

}


#endif /* PHYSICSCORE_H_ */
