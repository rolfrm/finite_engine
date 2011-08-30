#include "../Math/Math.hpp"

namespace Dormir{
class IntersectionSolver;
	class Core;
	class PhysicsObject;
	class Polygon;

	class SeperatingAxis{
	public:
		SeperatingAxis(Dormir::Core * nCore);
		~SeperatingAxis();
		void Run();
		void RunPreemptive();
		void FindIntersection(Vec2 P1,Vec2 P2,Vec2 P3,Vec2 P4,Vec2 * C);
	private:
		void CollisionDetection(Dormir::PhysicsObject * obj1,Dormir::PhysicsObject * obj2);
		void CollisionDetection(Dormir::PhysicsObject * obj,Dormir::Polygon * Ghost);
		void SAT(Dormir::Polygon * P1,Dormir::Polygon * P2,Dormir::PhysicsObject * obj1,Dormir::PhysicsObject * obj2);
		void SAT(Dormir::Polygon * GP,Dormir::Polygon * P,Dormir::PhysicsObject * obj);
		bool PreCollisionDetection(Dormir::Polygon * obj1,Dormir::Polygon * obj2);
		double FindMax(double [],unsigned int);
		double FindMin(double [],unsigned int);
		Dormir::Core * core;
	};


}

/*
 * 	polygonFrame frame1(*frame1in), frame2(*frame2in);


	bool first=true;
	//std::cout<<"Sat starting\n";
//	frame1.PrintVertex();
//	frame2->PrintVertex();
	if (!frame1.AxisCalculated){
		frame1.CalculateAxis();
		frame1.AxisCalculated=true;
	}
	if (!frame2.AxisCalculated){
		frame2.CalculateAxis();
		frame2.AxisCalculated=true;
	}
	std::list<Utils::Vec2>::iterator it,itend,it2,itend2;
	//std::list<double> frame1Projection,frame2Projection,contactlist1,contactlist2;
	double frame1Projection[frame1.vertex.size()],frame2Projection[frame2.vertex.size()],contactlist1[frame1.vertex.size()],contactlist2[frame2.vertex.size()];
	double maxframe1,maxframe2,minframe1,minframe2;
	int i;
	Utils::Vec2 candidate1(0,0),candidate2(0,0);
	for (it=frame1.axis.begin(),itend=frame1.axis.end();it!=itend;it++){
		//frame1Projection.clear();
		//frame2Projection.clear();
		//it->print();
		for (it2=frame1.vertex.begin(),itend2=frame1.vertex.end(),i=0;it2!=itend2;it2++,i++){
			//frame1Projection.push_back((*it)*(*it2));
			frame1Projection[i]=(*it)*(*it2);
		}
		for (it2=frame2.vertex.begin(),itend2=frame2.vertex.end(),i=0;it2!=itend2;it2++,i++){
			//frame2Projection.push_back((*it)*(*it2));
			frame2Projection[i]=(*it)*(*it2);
		}
		maxframe1=FindMax(frame1Projection,frame1.vertex.size());
		minframe1=FindMin(frame1Projection,frame1.vertex.size());
		maxframe2=FindMax(frame2Projection,frame2.vertex.size());
		minframe2=FindMin(frame2Projection,frame2.vertex.size());

		//std::cout<<"projections "<<maxframe1<<" "<<maxframe2<<" "<<minframe1<<" "<<minframe2<<"\n";
		if (minframe2>=maxframe1 || maxframe2<=minframe1){
			//std::cout<<"Aborted\n";
			DataCarrier.Collision=false;
			DataCarrier.SeperatingVector.null();
			return;
		}
		candidate1=*it;
		candidate2=*it;
		candidate1*=(maxframe2-minframe1);
		candidate2*=(maxframe1-minframe2);
		//std::cout<<"canidates "<<candidate1.length()<<" "<<candidate2.length()<<" "<<DataCarrier.SeperatingVector->length()<<"\n";
		/*
		if (fabs(maxframe2-minframe1)<fabs(minframe2-maxframe1)){
			if(first || DataCarrier.SeperatingVector.length()>candidate1.length()){
				DataCarrier.SeperatingVector=candidate1;
				DataCarrier.CollisionAxis=*it;
				//DataCarrier.CollisionAxis*=-1;
				DataCarrier.Collision=true;
				first=false;
				DataCarrier.axis_from=1;
			}
		}
		else{
			if(first || DataCarrier.SeperatingVector.length()>candidate2.length()){
				DataCarrier.SeperatingVector=candidate2;
				DataCarrier.CollisionAxis=*it;
				DataCarrier.CollisionAxis*=-1;
				DataCarrier.Collision=true;
				first=false;
				DataCarrier.axis_from=1;
			}
		}

		if(first || DataCarrier.SeperatingVector.length()>candidate2.length()){
			DataCarrier.SeperatingVector=candidate2;
			//DataCarrier.SeperatingVector*=-1;
			DataCarrier.CollisionAxis=*it;
			DataCarrier.CollisionAxis*=-1;
			DataCarrier.Collision=true;
			first=false;
			DataCarrier.axis_from=1;
		}

	}
	for (it=frame2.axis.begin(),itend=frame2.axis.end();it!=itend;it++){
				//frame1Projection.clear();
				//frame2Projection.clear();
				for (it2=frame1.vertex.begin(),itend2=frame1.vertex.end(),i=0;it2!=itend2;it2++,i++){
					//frame1Projection.push_back((*it)*(*it2));
					frame1Projection[i]=(*it)*(*it2);
				}
				for (it2=frame2.vertex.begin(),itend2=frame2.vertex.end(),i=0;it2!=itend2;it2++,i++){
					//frame2Projection.push_back((*it)*(*it2));
					frame2Projection[i]=(*it)*(*it2);
				}
				maxframe1=FindMax(frame1Projection,frame1.vertex.size());
				minframe1=FindMin(frame1Projection,frame1.vertex.size());
				maxframe2=FindMax(frame2Projection,frame2.vertex.size());
				minframe2=FindMin(frame2Projection,frame2.vertex.size());
			//	std::cout<<"projections "<<maxframe1<<" "<<maxframe2<<" "<<minframe1<<" "<<minframe2<<"\n";
				if (minframe2>=maxframe1 || maxframe2<=minframe1){
					//std::cout<<"ABORTED\n";
					DataCarrier.Collision=false;
					DataCarrier.SeperatingVector.null();
					return;
				}
				candidate1=*it;
				candidate2=*it;
				candidate1*=(minframe1-maxframe2);
				candidate2*=(minframe2-maxframe1);
				/*
				if (fabs(maxframe2-minframe1)<fabs(minframe2-maxframe1)){
					if(DataCarrier.SeperatingVector.length()>candidate1.length()){
						DataCarrier.SeperatingVector=candidate1;
						DataCarrier.CollisionAxis=*it;
						//DataCarrier.CollisionAxis*=-1;
						DataCarrier.Collision=true;
						first=false;
						DataCarrier.axis_from=2;
					}
				}
				else{
					if(DataCarrier.SeperatingVector.length()>candidate2.length()){
						DataCarrier.SeperatingVector=candidate2;
						DataCarrier.CollisionAxis=*it;
						DataCarrier.CollisionAxis*=-1;
						DataCarrier.Collision=true;
						first=false;
						DataCarrier.axis_from=2;
					}
				}

				if(DataCarrier.SeperatingVector.length()>candidate1.length()){
					DataCarrier.SeperatingVector=candidate1;
					//DataCarrier.SeperatingVector*=-1;
					DataCarrier.CollisionAxis=*it;
					DataCarrier.Collision=true;
					first=false;
					DataCarrier.axis_from=2;
				}
			}//det var så collisionsdetektionen, nu kommer findelsen af kontakt punkt
	//DataCarrier.CollisionAxis.print();
	double threshold=0.1,proj1,proj2,dis;
	Utils::Vec2 axis(0,0);
	axis = DataCarrier.CollisionAxis;
	dis = DataCarrier.SeperatingVector.length();
	Utils::Vec2 intersectingVertex1[2],intersectingVertex2[4];
	int i1=0,i2=0;
	bool appended;
	std::list<Utils::Vec2>::iterator it1temp,it2temp,temp;
	for (it=frame1.vertex.begin(),itend=frame1.vertex.end();it!=itend;it++){
		proj1=*it*axis;
		appended=false;
		for (it2=frame2.vertex.begin(),itend2=frame2.vertex.end();it2!=itend2;it2++){
				proj2=(*it2)*axis;

				if (fabs(proj1-proj2)-dis<=threshold){
					if (i1<2 && !appended){
						appended = true;
						intersectingVertex1[i1]=*it;
						it1temp=it;
						i1++;
					}
					if (i2<4){
						intersectingVertex2[i2]=*it2;
						it2temp=it2;
						i2++;
					}
				}
		}
	}
	Utils::Vec2 formerNeighbourVertex,nextNeighbourVertex,toFormerVertex,toNextVertex;
	if (i1==1 && i2>=1){
	//	std::cout<<"and now\n";
	//	DataCarrier.ContactPoint=intersectingVertex1[0];
		if(it1temp==frame1.vertex.begin()){
			temp=frame1.vertex.end();
		}
		else{
			temp=it1temp;
		}
		temp--;
		formerNeighbourVertex=*temp;
		temp=it1temp;
		temp++;
		if(temp==frame1.vertex.end()){
			temp=frame1.vertex.begin();
		}
		nextNeighbourVertex=*temp;
		toFormerVertex=formerNeighbourVertex;
		toNextVertex=nextNeighbourVertex;
		toFormerVertex-=*it1temp;
		toNextVertex-=*it1temp;
		toFormerVertex.normalize();
		toNextVertex.normalize();
		DataCarrier.ContactPoint1=*it1temp+toFormerVertex*(DataCarrier.SeperatingVector.length()/(toFormerVertex*DataCarrier.CollisionAxis));
		DataCarrier.ContactPoint2=*it1temp+toNextVertex*(DataCarrier.SeperatingVector.length()/(toNextVertex*DataCarrier.CollisionAxis));
		DataCarrier.extraContacts=false;
		DataCarrier.ContactPoint3=*it1temp;
		return;
	}
	else if(i1==2 && i2==2){
	//	std::cout<<"number one\n";
		//DataCarrier.ContactPoint=intersectingVertex2[0];
		if(it2temp==frame2.vertex.begin()){
			temp=frame2.vertex.end();
		}
		else{
			temp=it2temp;
		}
		temp--;
		formerNeighbourVertex=*temp;
		temp=it2temp;
		temp++;
		if(temp==frame2.vertex.end()){
			temp=frame2.vertex.begin();
		}
		nextNeighbourVertex=*temp;
		toFormerVertex=formerNeighbourVertex;
		toNextVertex=nextNeighbourVertex;
		toFormerVertex-=*it2temp;
		toNextVertex-=*it2temp;
		toFormerVertex.normalize();
		toNextVertex.normalize();
		DataCarrier.ContactPoint1=*it2temp+toFormerVertex*(DataCarrier.SeperatingVector.length()/(toFormerVertex*DataCarrier.CollisionAxis));
		DataCarrier.ContactPoint2=*it2temp+toNextVertex*(DataCarrier.SeperatingVector.length()/(toNextVertex*DataCarrier.CollisionAxis));
		DataCarrier.extraContacts=false;
		DataCarrier.ContactPoint3=*it2temp;
		return;
	}
	else{
		DataCarrier.extraContacts=true;
		axis.rotate90();
		double edgeProjection1[2],edgeProjection2[2];
		edgeProjection1[0]=axis*intersectingVertex1[0];
		edgeProjection1[1]=axis*intersectingVertex1[1];
		edgeProjection2[0]=axis*intersectingVertex2[0];
		edgeProjection2[1]=axis*intersectingVertex2[1];
		if (edgeProjection1[0]<edgeProjection1[1]){
			Utils::Vec2 tempCarrier;
			double tempCarrierf;
			tempCarrierf=edgeProjection1[0];
			edgeProjection1[0]=edgeProjection1[1];
			edgeProjection1[1]=tempCarrierf;
			tempCarrier=intersectingVertex1[0];
			intersectingVertex1[0]=intersectingVertex1[1];
			intersectingVertex1[1]=tempCarrier;
		}
		if (edgeProjection2[0]<edgeProjection2[1]){
					Utils::Vec2 tempCarrier;
					double tempCarrierf;
					tempCarrierf=edgeProjection2[0];
					edgeProjection2[0]=edgeProjection2[1];
					edgeProjection2[1]=tempCarrierf;
					tempCarrier=intersectingVertex2[0];
					intersectingVertex2[0]=intersectingVertex2[1];
					intersectingVertex2[1]=tempCarrier;
		}


		//		std::cout<<edgeProjection1[0]<<" "<<edgeProjection1[1]<<" "<<edgeProjection2[0]<<" "<<edgeProjection2[1]<<"\n";


				if (edgeProjection1[1]>=edgeProjection2[1] && edgeProjection1[0]<=edgeProjection2[0]){
					//std::cout<<"kontakt1 er her\n";

					DataCarrier.ContactPoint3=intersectingVertex1[0];
					DataCarrier.ContactPoint3+=intersectingVertex1[1];
					DataCarrier.ContactPoint3/=2.0;
					DataCarrier.ContactPoint1=intersectingVertex1[0];
					DataCarrier.ContactPoint2=intersectingVertex1[1];
					return;
				}
				else if (edgeProjection1[1]<=edgeProjection2[1] && edgeProjection1[0]>=edgeProjection2[0]){
					//std::cout<<"kontakt2 er her\n";

					DataCarrier.ContactPoint3=intersectingVertex2[0];
					DataCarrier.ContactPoint3+=intersectingVertex2[1];
					DataCarrier.ContactPoint3/=2.0;

					DataCarrier.ContactPoint1=intersectingVertex2[0];
					DataCarrier.ContactPoint2=intersectingVertex2[1];
					return;
				}
				else if (edgeProjection1[1]<=edgeProjection2[1] && edgeProjection1[0]<edgeProjection2[0]){
					//std::cout<<"kontakt3 er her\n";

					DataCarrier.ContactPoint3=intersectingVertex1[0];
					DataCarrier.ContactPoint3+=intersectingVertex2[1];
					DataCarrier.ContactPoint3/=2.0;

					DataCarrier.ContactPoint1=intersectingVertex1[0];
					DataCarrier.ContactPoint2=intersectingVertex2[1];
					return;
				}
				else if (edgeProjection1[1]>edgeProjection2[1] && edgeProjection1[0]>=edgeProjection2[0]){
					//std::cout<<"kontakt4 er her\n";

					DataCarrier.ContactPoint3=intersectingVertex1[1];
					DataCarrier.ContactPoint3+=intersectingVertex2[0];
					DataCarrier.ContactPoint3/=2.0;

					DataCarrier.ContactPoint1=intersectingVertex1[1];
					DataCarrier.ContactPoint2=intersectingVertex2[0];
					return;
				}
	}
 */
