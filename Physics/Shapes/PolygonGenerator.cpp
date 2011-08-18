/*
 * PolygonGenerator.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: sebastian
 */

#include "PolygonGenerator.hpp"
#include <iostream>

namespace Dormir{

	Polygon GenerateBox(double x,double y,int width,int height){
		Polygon P;
		P.AddVertex(-width/2,-height/2);
		P.AddVertex(-width/2,height/2);
		P.AddVertex(width/2,height/2);
		P.AddVertex(width/2,-height/2);
		P.Move(x,y);
		return P;
	}

	Polygon GenerateGeneralPolygon(double x,double y,double Vertex[],unsigned int vertices){
		Polygon P;
		for(unsigned int i=0;i<vertices;i++){
			P.AddVertex(Vertex[i*2],Vertex[i*2+1]);
		}
		P.Move(x,y);
		return P;
	}

	void split(std::vector<float> Input,std::vector<std::vector<float> > * Output){

		for(unsigned int i=2;i<Input.size();i++){
			float prev=Input[i-2];
			float current=Input[i-1];
			float future=Input[i];
			if(2*current-(future+prev)<0){
				std::vector<float> temp1,temp2;
				for(unsigned int j=0;j<i;j++)
					temp1.push_back(Input[j]);
				for(unsigned int j=i-1;j<Input.size();j++)
					temp2.push_back(Input[j]);
				Output->push_back(temp1);
				split(temp2,Output);
				return;
			}
		}

		Output->push_back(Input);

	};


	std::list<Polygon> GenerateLand(double hlist[],unsigned int size,double Length,double Height,double Depth){
		std::vector<std::vector<float> > Output;
		std::vector<float > Input;
		for(unsigned int i=0;i<size;i++)
			Input.push_back(hlist[i]);

		split(Input,&Output);

		std::list<Polygon> ReturnP;

		for(unsigned int i=0,step=0;i<Output.size();i++){
			Polygon P;
			P.AddVertex(step*Length,-Depth);
			for(unsigned int j=0;j<Output[i].size();j++,step++){
				P.AddVertex(step*Length,Output[i][j]*Height);
			}

			step--;
			P.AddVertex((step)*Length,-Depth);
			ReturnP.push_back(P);

		}

		return ReturnP;
	};

}
