#include "../Shapes/Polygon.h"
#include "../Objects/PhysicsObject.h"
#include "../PhysicsCore.h"
#include "SeperatingAxis.hpp"
#include <iostream>
#include <math.h>

namespace Dormir{

	SeperatingAxis::SeperatingAxis(Dormir::Core * nCore){
		core=nCore;
	}

	SeperatingAxis::~SeperatingAxis(){
	}

	void SeperatingAxis::Run(){
		for(std::list<Dormir::PhysicsObject *>::iterator it=core->Objects.begin();it!=core->Objects.end();it++){
			for(std::list<Dormir::Polygon>::iterator itf=(*it)->Body.begin();itf!=(*it)->Body.end();itf++){
				itf->FindBounds();
			}
		}
		for(std::list<Dormir::Polygon *>::iterator it=core->GhostPolygons.begin();it!=core->GhostPolygons.end();it++){
			(*it)->FindBounds();
		}

		/*
		 * Experimental part for chains
		 */
		for(unsigned int i=0;i<core->ObjectClusters.size();i++){
			for(unsigned int j=0;j<core->ObjectClusters[i].size();j++){
				for(std::list<Dormir::Polygon>::iterator it=core->ObjectClusters[i][j]->Body.begin();it!=core->ObjectClusters[i][j]->Body.end();it++){
					it->FindBounds();
				}
			}
		}

		for(unsigned int i=0;i<core->ObjectClusters.size();i++){
			for(unsigned int j=0;j<core->ObjectClusters[i].size();j++){
				for(std::list<Dormir::PhysicsObject *>::iterator it=core->Objects.begin();it!=core->Objects.end();it++){
					CollisionDetection(core->ObjectClusters[i][j],*it);
				}
			}
		}


		/*
		 * End
		 */

		for(std::list<Dormir::PhysicsObject *>::iterator it=core->Objects.begin();it!=core->Objects.end();it++){
			std::list<Dormir::PhysicsObject *>::iterator it2=it;
			for(it2++;it2!=core->Objects.end();it2++){
				if((*it)->GetMass()!=0 || (*it2)->GetMass()!=0)
					CollisionDetection(*it,*it2);
			}
			for(std::list<Dormir::Polygon *>::iterator itf=core->GhostPolygons.begin();itf!=core->GhostPolygons.end();itf++){
				CollisionDetection(*it,*itf);
			}
		}
	}

	void SeperatingAxis::RunPreemptive(){
		for(std::list<Dormir::PhysicsObject *>::iterator it=core->Objects.begin();it!=core->Objects.end();it++){
			(*it)->FindBounds();
		}
		for(std::list<Dormir::PhysicsObject *>::iterator it=core->Objects.begin();it!=core->Objects.end();it++){
		//	if((*it)->GetMass()!=0){
				(*it)->Advance();
				(*it)->FindBounds();
				std::list<Dormir::PhysicsObject *>::iterator it2=it;
				for(it2++;it2!=core->Objects.end();it2++){
					unsigned int c=core->allocatedNodes;
					if((*it)->GetMass()!=0 || (*it2)->GetMass()!=0){
						CollisionDetection(*it,*it2);
						if(c==core->allocatedNodes){
							(*it)->Revert();
							(*it)->FindBounds();
							(*it2)->Advance();
							(*it2)->FindBounds();
							CollisionDetection(*it,*it2);
							(*it2)->Revert();
							(*it2)->FindBounds();
							(*it)->Advance();
							(*it)->FindBounds();
						}
					}
				}
				(*it)->Revert();
				(*it)->FindBounds();
		/*	}
			else{
				std::list<Dormir::PhysicsObject *>::iterator it2=it;
				for(it2++;it2!=core->Objects.end();it2++){
					(*it2)->Advance();
					for(std::list<Dormir::Polygon *>::iterator itf=(*it2)->Body.begin();itf!=(*it2)->Body.end();itf++){
						(*itf)->FindBounds();
					}
					if((*it)->GetMass()!=0 || (*it2)->GetMass()!=0)
						CollisionDetection(*it,*it2);
					(*it2)->Revert();
					for(std::list<Dormir::Polygon *>::iterator itf=(*it2)->Body.begin();itf!=(*it2)->Body.end();itf++){
						(*itf)->FindBounds();
					}
				}
			}*/
		}
	}

	void SeperatingAxis::CollisionDetection(Dormir::PhysicsObject * obj1,Dormir::PhysicsObject * obj2){
		for(std::list<Dormir::Polygon>::iterator it=obj1->Body.begin();it!=obj1->Body.end();it++){
			for(std::list<Dormir::Polygon>::iterator it2=obj2->Body.begin();it2!=obj2->Body.end();it2++){
				if(PreCollisionDetection(&*it,&*it2)){
					SAT(&*it,&*it2,obj1,obj2);
				}
			}
		}
	}

	void SeperatingAxis::CollisionDetection(Dormir::PhysicsObject * obj,Dormir::Polygon * Ghost){
		for(std::list<Dormir::Polygon>::iterator it=obj->Body.begin();it!=obj->Body.end();it++){
			if(PreCollisionDetection(&*it,Ghost)){
				SAT(Ghost,&*it,obj);
			}
		}
	}


	void SeperatingAxis::SAT(Dormir::Polygon * P1,Dormir::Polygon * P2,Dormir::PhysicsObject * obj1,Dormir::PhysicsObject * obj2){
		CollisionNode Node,Node2;
		Vec2 Contact1,Contact2;
		Node.intersection=INFINITY;
		double cMax1,cMin2;
		for(unsigned int w=0;w<2;w++){
			/*
			 * The Collision Detection algorithm is run twice,
			 * Firstly with the given arguments
			 * And secondly with the reverse arguments
			 * Meaning that first the algorithm is run with the first objects axis
			 * And after with the second objects axis
			 */
			/*arma::mat PM1=arma::trans(P1->Vertex)*P1->Axis,PM2=arma::trans(P2->Vertex)*P1->Axis;
			for(unsigned int i=0;i<PM1.n_cols;i++){
				double min2=arma::min(PM2.col(i)),max1=arma::max(PM1.col(i));
				if(min2>=max1)
					return;
				else if(max1-min2<Node.intersection){
					*
					 * If the shortest intersection is found then basic data is read into a collision node
					 * These are as following
					 *
					Node.intersection=max1-min2; //Proportion of intersectionev
					Node.axis=P1->Axis.col(i); //Axis of the collision
					Node.from=obj1; //Reference to the object from which the axis originates
					Node.to=obj2; //Reference to the opposing object
					cMax1=max1;
					cMin2=min2;


				}
			}*/

			for(unsigned int i=0;i<P1->Axis.size();i++){
				double min2=P1->Axis[i]*P2->Vertex[0],max1=P1->Axis[i]*P1->Vertex[0];
				for(unsigned int j=1;j<P1->Vertex.size();j++){
					double temp=P1->Axis[i]*P1->Vertex[j];
					if(temp>max1){
						max1=temp;
					}
				}
				for(unsigned int j=1;j<P2->Vertex.size();j++){
					double temp=P1->Axis[i]*P2->Vertex[j];
					if(temp<min2){
						min2=temp;
					}
				}
				if(min2>=max1)
					return;
				else if(max1-min2<Node.intersection){
					Node.intersection=max1-min2; //Proportion of intersectionev
					Node.axis=P1->Axis[i]; //Axis of the collision
					Node.from=obj1; //Reference to the object from which the axis originates
					Node.to=obj2; //Reference to the opposing object
					cMax1=max1;
					cMin2=min2;
				}

			}
			Dormir::PhysicsObject * temp=obj1;
			obj1=obj2,obj2=temp;
			Dormir::Polygon * tempP=P1;
			P1=P2,P2=tempP;
		}
		if(Node.from==obj2){
			Dormir::PhysicsObject * temp=obj1;
			obj1=obj2,obj2=temp;
			Dormir::Polygon * tempP=P1;
			P1=P2,P2=tempP;
		}
	//		arma::mat PM1=arma::trans(P1->Vertex)*Node.axis,PM2=arma::trans(P2->Vertex)*Node.axis;
		/*
		 * Next the vertexes of the edge of contact will be calculated
		 * As of now I have found no general method for determening this
		 * So it'll be done with a series of special cases
		 * Firstly the vertex from opposing object which overlaps the originating object
		 */
		unsigned int index2=0,I2[2];
		for(unsigned int j=0;j<P2->Vertex.size();j++){
			double temp=P2->Vertex[j]*Node.axis;
			if(temp<=cMax1){
				I2[index2]=j;
				index2++;
				if(index2==2)
					break;
			}
		}
		/*
		 * Hereafter the overlapping vertexes for the originating object is found
		 */
		unsigned int index1=0,I1[2];
		for(unsigned int j=0;j<P1->Vertex.size();j++){
			double temp=P1->Vertex[j]*Node.axis;
			if(temp>=cMin2){
				I1[index1]=j;
				index1++;
				if(index1==2)
					break;
			}
		}
		/*
		 * Only max 2 vertex of overlap will be considered
		 * A higher order of overlap is possible but unlikely for reasonable polygons
		 */
		if(index2==1){
		/*
		 * This covers the ascii illustrated case
		 * 			V20 ----------- V22
		 * 			    \		  /
		 * 			     \		 / <--- Opposing object
		 * 			  V10 \		/  V11
		 *				-----------
		 * 			    \	\/	  /
		 * 			     \	 V21 / <--- Originating object
		 * 			      \		/
		 * 				   \   /
		 * 					\ /
		 */
		//arma::mat::fixed<2,3> V2;
		Vec2 V2[3];
		V2[1]=P2->Vertex[I2[0]];

		if(I2[0]==0)
			V2[0]=P2->Vertex[P2->Vertex.size()-1];
		else
			V2[0]=P2->Vertex[I2[0]-1];

		if(I2[0]==P2->Vertex.size()-1)
			V2[2]=P2->Vertex[0];
		else
			V2[2]=P2->Vertex[I2[0]+1];
	//	arma::mat::fixed<2,2> V1;
		Vec2 V1[2];
		V1[0]=P1->Vertex[I1[0]];
		V1[1]=P1->Vertex[I1[1]];
		FindIntersection(V1[0],V1[1],V2[0],V2[1],&Contact1);
		//arma::vec2 temp;
		FindIntersection(V1[0],V1[1],V2[2],V2[1],&Contact2);
		//Node.contact+=temp;
		//Node.contact/=2;
		}
		else{
			/*
			 * If two intersecting vertexes are present then the situation will be treated as shown below
			 * 			 /\
			 * 		    /  \ <--- Opposing object
			 * 	V10	   /    \    V11
			 * 	------------------
			 *	 \	 / 	      \	 /
			 *	  \	 ----------	/
			 *	   \ V20    V21/
			 *	    \		  / <-- Originating Object
			 *	     \		 /
			 *	      \		/
			 *	       \   /
			 *	       	\ /
			 */
			Vec2 Edge;
			Edge.SetValue(-Node.axis.y,Node.axis.x);
			Vec2 V1[2],V2[2];
			V1[0]=P1->Vertex[I1[0]];
			V1[1]=P1->Vertex[I1[1]];

			V2[0]=P2->Vertex[I2[0]];
			V2[1]=P2->Vertex[I2[1]];
		//	arma::vec2 PEM1=arma::trans(V1)*Edge,PEM2=arma::trans(V2)*Edge;
			double PEM1[2],PEM2[2];
			PEM1[0]=V1[0]*Edge;
			PEM1[1]=V1[1]*Edge;
			PEM2[0]=V2[0]*Edge;
			PEM2[1]=V2[1]*Edge;
			if(PEM1[0]>PEM1[1]){
				double temp=PEM1[0];
				PEM1[0]=PEM1[1],PEM1[1]=temp;
			}

			if(PEM2[0]>PEM2[1]){
				double temp=PEM2[0];
				PEM2[0]=PEM2[1],PEM2[1]=temp;
			}
			/*
			 * The following corrections of the indices arrays ensures
			 * that the neighbouring vertex can always be found
			 * by subtracting 1 from the lower entry and
			 * adding 1 to the upper entry
			 * within bound of course
			 */

			if(I1[0]==0 && I1[1]==P1->Vertex.size()-1){
				I1[0]=P1->Vertex.size()-1;
				I1[1]=0;
			}

			if(I2[0]==0 && I2[1]==P2->Vertex.size()-1){
				I2[0]=P2->Vertex.size()-1;
				I2[1]=0;
			}




			if(PEM1[0]<=PEM2[0] && PEM2[1]<=PEM1[1]){
				unsigned int n2;
				if(I2[0]==0)
					n2=P2->Vertex.size()-1;
				else
					n2=I2[0]-1;
				FindIntersection(P1->Vertex[I1[0]],P1->Vertex[I1[1]],P2->Vertex[I2[0]],P2->Vertex[n2],&Contact1);
				//arma::vec2 temp;
				if(I2[1]==P2->Vertex.size()-1)
					n2=0;
				else
					n2=I2[1]+1;
				FindIntersection(P1->Vertex[I1[0]],P1->Vertex[I1[1]],P2->Vertex[I2[1]],P2->Vertex[n2],&Contact2);
				//Node.contact+=temp;
				//Node.contact*=0.5;
			}
			else if(PEM1[0]<=PEM2[0] && PEM1[1]<PEM2[1]){
				unsigned int n1;
				if(I1[1]==P1->Vertex.size()-1)
					n1=0;
				else
					n1=I1[1]+1;
				FindIntersection(P2->Vertex[I2[0]],P2->Vertex[I2[1]],P1->Vertex[I1[1]],P1->Vertex[n1],&Contact1);
				unsigned int n2;
				if(I2[1]==P2->Vertex.size()-1)
					n2=0;
				else
					n2=I2[1]+1;
				//arma::vec2 temp;
				FindIntersection(P1->Vertex[I1[0]],P1->Vertex[I1[1]],P2->Vertex[I2[1]],P2->Vertex[n2],&Contact2);
				//Node.contact+=temp;
				//Node.contact*=0.5;

			}
			else if(PEM2[0]<PEM1[0] && PEM2[1]<=PEM1[1]){
				unsigned int n2;
				if(I2[0]==0)
					n2=P2->Vertex.size()-1;
				else
					n2=I2[0]-1;
				FindIntersection(P1->Vertex[I1[0]],P1->Vertex[I1[1]],P2->Vertex[I2[0]],P2->Vertex[n2],&Contact1);
				unsigned int n1;
				if(I1[0]==0)
					n1=P1->Vertex.size()-1;
				else
					n1=I1[0]-1;
				FindIntersection(P2->Vertex[I2[1]],P2->Vertex[I2[0]],P1->Vertex[I1[0]],P1->Vertex[n1],&Contact2);
			}
			else{
				unsigned int n1;
				if(I1[0]==0)
					n1=P1->Vertex.size()-1;
				else
					n1=I1[0]-1;
				FindIntersection(P2->Vertex[I2[0]],P2->Vertex[I2[1]],P1->Vertex[I1[0]],P1->Vertex[n1],&Contact1);
				//arma::vec2 temp;
				if(I1[1]==P1->Vertex.size()-1)
					n1=0;
				else
					n1=I1[1]+1;
				FindIntersection(P2->Vertex[I2[0]],P2->Vertex[I2[1]],P1->Vertex[I1[1]],P1->Vertex[n1],&Contact2);
				//	Node.contact+=temp;
				//	Node.contact*=0.5;
			}
		}

		Node.tangent.SetValue(-Node.axis.y,Node.axis.x);
		Node.contact=Contact1;


		//if(arma::norm(Contact1-Contact2,2)>1e-1){
		Node2=Node;
		Node2.contact=Contact2;
		Node2.rf=Node2.contact-Node2.from->Pos;
		Node2.rt=Node2.contact-Node2.to->Pos;

		core->AddCollisionNode(Node2);
		Node.rf=Node.contact-Node.from->Pos;
		Node.rt=Node.contact-Node.to->Pos;
		core->AddCollisionNode(Node);
	}

	void SeperatingAxis::SAT(Dormir::Polygon * GP,Dormir::Polygon * P,Dormir::PhysicsObject * obj){
		GhostNode Node;
		Node.intersection=INFINITY;
			/*
			 * The Collision Detection algorithm is run twice,
			 * Firstly with the given arguments
			 * And secondly with the reverse arguments
			 * Meaning that first the algorithm is run with the first objects axis
			 * And after with the second objects axis
			 */
			/*arma::mat PM1=arma::trans(P1->Vertex)*P1->Axis,PM2=arma::trans(P2->Vertex)*P1->Axis;
			for(unsigned int i=0;i<PM1.n_cols;i++){
				double min2=arma::min(PM2.col(i)),max1=arma::max(PM1.col(i));
				if(min2>=max1)
					return;
				else if(max1-min2<Node.intersection){
					*
					 * If the shortest intersection is found then basic data is read into a collision node
					 * These are as following
					 *
					Node.intersection=max1-min2; //Proportion of intersectionev
					Node.axis=P1->Axis.col(i); //Axis of the collision
					Node.from=obj1; //Reference to the object from which the axis originates
					Node.to=obj2; //Reference to the opposing object
					cMax1=max1;
					cMin2=min2;


				}
			}*/

			for(unsigned int i=0;i<GP->Axis.size();i++){
				double min2=GP->Axis[i]*P->Vertex[0],max1=GP->Axis[i]*GP->Vertex[0];
				for(unsigned int j=1;j<GP->Vertex.size();j++){
					double temp=GP->Axis[i]*GP->Vertex[j];
					if(temp>max1){
						max1=temp;
					}
				}
				for(unsigned int j=1;j<P->Vertex.size();j++){
					double temp=GP->Axis[i]*P->Vertex[j];
					if(temp<min2){
						min2=temp;
					}
				}
				if(min2>=max1)
					return;
				else if(max1-min2<Node.intersection){
					Node.intersection=max1-min2; //Proportion of intersectionev
					Node.obj=obj;
				}

			}

			for(unsigned int i=0;i<P->Axis.size();i++){
				double min2=GP->Axis[i]*P->Vertex[0],max1=GP->Axis[i]*GP->Vertex[0];
				for(unsigned int j=1;j<GP->Vertex.size();j++){
					double temp=GP->Axis[i]*GP->Vertex[j];
					if(temp>max1){
						max1=temp;
					}
				}
				for(unsigned int j=1;j<P->Vertex.size();j++){
					double temp=GP->Axis[i]*P->Vertex[j];
					if(temp<min2){
						min2=temp;
					}
				}
				if(min2>=max1)
					return;
				else if(max1-min2<Node.intersection){
					Node.intersection=max1-min2; //Proportion of intersectionev
					Node.obj=obj;
				}

			}

			core->GhostNodes.push_back(Node);


	}

	bool SeperatingAxis::PreCollisionDetection(Dormir::Polygon * obj1,Dormir::Polygon * obj2){
		//arma::mat::fixed<2,2> B1=obj1->getBound(),B2=obj2->getBound();
		//if(B2(0,0)>B1(0,1) || B1(0,0)>B2(0,1) || B2(1,0)>B1(1,1) || B1(1,0)>B2(1,1))
		//	return false;
		if(obj2->BoundingBox[0].x>obj1->BoundingBox[1].x || obj1->BoundingBox[0].x>obj2->BoundingBox[1].x || obj2->BoundingBox[0].y>obj1->BoundingBox[1].y || obj1->BoundingBox[0].y>obj2->BoundingBox[1].y)
			return false;
		return true;
	}

	void SeperatingAxis::FindIntersection(Vec2 P1,Vec2 P2,Vec2 P3,Vec2 P4,Vec2 * C){
		(*C).SetValue(((P1.x*P2.y-P1.y*P2.x)*(P3.x-P4.x)-(P1.x-P2.x)*(P3.x*P4.y-P3.y*P4.x))/((P1.x-P2.x)*(P3.y-P4.y)-(P1.y-P2.y)*(P3.x-P4.x))
				,((P1.x*P2.y-P1.y*P2.x)*(P3.y-P4.y)-(P1.y-P2.y)*(P3.x*P4.y-P3.y*P4.x))/((P1.x-P2.x)*(P3.y-P4.y)-(P1.y-P2.y)*(P3.x-P4.x)));
	}

	double SeperatingAxis::FindMax(double Values[],unsigned int size){
		double temp=Values[0];
		for(unsigned int i=1;i<size;i++){
			if(temp<Values[i])
				temp=Values[i];
		}
		return temp;
	}

	double SeperatingAxis::FindMin(double Values[],unsigned int size){
		double temp=Values[0];
		for(unsigned int i=1;i<size;i++){
			if(temp>Values[i])
				temp=Values[i];
		}
		return temp;
	}

}

