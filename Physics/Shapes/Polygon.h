/*
 * PhysicsShapes.h
 *
 *  Created on: Feb 25, 2011
 *      Author: sebastian
 */

#ifndef PHYSICSSHAPES_H_
#define PHYSICSSHAPES_H_

#include <vector>
#include "../Math/Math.hpp"

namespace Dormir{

	class PhysicsObject;
	class SeperatingAxis;
	class GraphicsCore;


	class Polygon{
	public:
		Polygon();
		void AddVertex(double x,double y);
		void CalculateAxis();
		void FindBounds();
		void Move(Vec2);
		void Move(double x,double y);
		void Rotate(double);
		void ReverseVertex();

		double getArea();
		Vec2 getCentroid();
		std::vector<Vec2> getVertex(){return Vertex;};
		std::vector<Vec2> getAxis(){return Axis;};
		//arma::mat::fixed<2,2> getBound(){return BoundingBox;};
	//	arma::mat getEdge();
		std::vector<Vec2> Vertex,Axis;
		Vec2 BoundingBox[2];
		friend class SeperatingAxis;
		friend class GraphicsCore;
		friend class PhysicsObject;


	};

}



#endif /* PHYSICSSHAPES_H_ */
