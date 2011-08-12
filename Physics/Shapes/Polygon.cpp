/*
 * PhysicsShape.cpp
 *
 *  Created on: Feb 28, 2011
 *      Author: sebastian
 */

#include "Polygon.h"
#include <math.h>

namespace Dormir{

	Polygon::Polygon(){
		//Vertex.reshape(2,0);
		//Axis.reshape(2,0);
	}

	void Polygon::AddVertex(double x,double y){
	//	Vertex.reshape(Vertex.n_rows,Vertex.n_cols+1);
	//	Vertex(0,Vertex.n_cols-1)=x;
	//	Vertex(1,Vertex.n_cols-1)=y;
		Vertex.push_back(Vec2(x,y));
	}

	void Polygon::CalculateAxis(){
	//	arma::mat B(Vertex.n_rows,Vertex.n_cols);
		/*for(unsigned int i=0;i<Vertex.n_rows;i++){
			for(unsigned int j=1;j<=Vertex.n_cols;j++){
				B(i,j-1)=Vertex(i,j%Vertex.n_cols);
			}
		}*/
		Axis.clear();
		for(unsigned int i=1;i<=Vertex.size();i++){
			Axis.push_back(Vertex[i%Vertex.size()]);
		}
		//B-=Vertex;
		for(unsigned int i=0;i<Vertex.size();i++){
			Axis[i]-=Vertex[i];
		}
		//arma::mat Q(2,2);
		//Q.zeros();
		//Q(1,0)=-1;
		//Q(0,1)=1;
		for(unsigned int i=0;i<Vertex.size();i++){
			Axis[i].SetValue(-Axis[i].y,Axis[i].x);
			Axis[i]/=Axis[i].GetNorm2();
		}

	}
/*
	arma::mat Polygon::getEdge(){
		arma::mat B(Vertex.n_rows,Vertex.n_cols);
		for(unsigned int i=0;i<Vertex.n_rows;i++){
			for(unsigned int j=1;j<=Vertex.n_cols;j++){
				B(i,j-1)=Vertex(i,j%Vertex.n_cols);
			}
		}
		return B-Vertex;
	}
*/
	void Polygon::FindBounds(){
		BoundingBox[0].x=Vertex[0].x;
		BoundingBox[0].y=Vertex[0].y;
		BoundingBox[1].x=Vertex[0].x;
		BoundingBox[1].y=Vertex[0].y;
		for(unsigned int i=1;i<Vertex.size();i++){
			if(Vertex[i].x<BoundingBox[0].x)
				BoundingBox[0].x=Vertex[i].x;
			else if(Vertex[i].x>BoundingBox[1].x)
				BoundingBox[1].x=Vertex[i].x;
			if(Vertex[i].y<BoundingBox[0].y)
				BoundingBox[0].y=Vertex[i].y;
			else if(Vertex[i].y>BoundingBox[1].y)
				BoundingBox[1].y=Vertex[i].y;
		}
	}

	void Polygon::Move(Vec2 Displacement){
		for(unsigned int i=0;i<Vertex.size();i++){
			Vertex[i]+=Displacement;
		}
	}
	void Polygon::Move(double x,double y){
		for(unsigned int i=0;i<Vertex.size();i++){
			Vertex[i].x+=x;
			Vertex[i].y+=y;
		}
	}

	void Polygon::Rotate(double angle){
		//if(fabs(angle)<1e-10) //eet midlertidigt hak der sætter en grænse for hvor lave vinkler der må benyttes
		//	return;
		double cosO=cos(angle),sinO=sin(angle);
		for(unsigned int i=0;i<Vertex.size();i++){
			double v0=Vertex[i].x,v1=Vertex[i].y,a0=Axis[i].x,a1=Axis[i].y;
			Vertex[i].x=v0*cosO-v1*sinO;
			Vertex[i].y=v0*sinO+v1*cosO;
			Axis[i].x=a0*cosO-a1*sinO;
			Axis[i].y=a0*sinO+a1*cosO;
		}
	}

	double Polygon::getArea(){
		double Area=0;
		for(unsigned int i=0;i<Vertex.size()-1;i++)
			Area+=Crossproduct(Vertex[i],Vertex[i+1]);
		Area+=Crossproduct(Vertex[Vertex.size()-1],Vertex[0]);
		Area/=2;
		return Area;
	}

	Vec2 Polygon::getCentroid(){
		Vec2 C;
		for(unsigned int i=0;i<Vertex.size()-1;i++){
			C.x+=(Vertex[i].x+Vertex[i+1].x)*Crossproduct(Vertex[i],Vertex[i+1]);
			C.y+=(Vertex[i].y+Vertex[i+1].y)*Crossproduct(Vertex[i],Vertex[i+1]);
		}
		C.x+=(Vertex[0].x+Vertex[Vertex.size()-1].x)*Crossproduct(Vertex[Vertex.size()-1],Vertex[0]);
		C.y+=(Vertex[0].y+Vertex[Vertex.size()-1].y)*Crossproduct(Vertex[Vertex.size()-1],Vertex[0]);
		C/=6;
		C/=getArea();
		return C;
	}

	void Polygon::ReverseVertex(){
		std::vector<Vec2> Temp=Vertex;
		for(unsigned int i=0;i<Vertex.size();i++){
			Vertex[i].x=Temp[Vertex.size()-(i+1)].x;
			Vertex[i].y=Temp[Vertex.size()-(i+1)].y;
		}
	}
}
