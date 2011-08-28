#include "GfxCore/GraphicsCore.hpp"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "GfxCore/Sprite/Sprite.hpp"
#include "GfxCore/Sprite/PolySprite.hpp"
#include <math.h>
#include "Physics/Shapes/PolygonGenerator.hpp"
//#include "Dare/TextureCoords.hpp"
#include "Physics/PhysicsCore.h"
#include "Physics/Objects/PhysicsObject.h"
#include "Physics/Objects/Constraint.hpp"


#include <map>

Dormir::Core pCore(400);
Dormir::GraphicsCore gCore(1440,900);



int main(int argc,char ** argv){
/*	pCore.setGravity(0,-0.4);

	gCore.GenerateTexture("Sprites/stagePart1.png");
	gCore.GenerateTexture("Sprites/stagePart4.png");

	Dormir::PhysicsObject O;
	O.LoadPolygon(Dormir::GenerateBox(700,650,200,100));

	pCore.LoadObject(&O);

	Dormir::Sprite S(200,100);
	S.SetTexture(gCore.GetTexture("Sprites/stagePart4.png"));
	S.setReference(&O);

	gCore.LoadSprite(&S);

	Dormir::PhysicsObject O2(1);
	O2.LoadPolygon(Dormir::GenerateBox(850,550,100,100));

	pCore.LoadObject(&O2);

	Dormir::Sprite S2(100,100);
	S2.SetTexture(gCore.GetTexture("Sprites/stagePart1.png"));
	S2.setReference(&O2);

	gCore.LoadSprite(&S2);


//	Dormir::PhysicsObject O3(1);
//	O3.LoadPolygon(Dormir::GenerateBox(1200,650,100,100));

//	pCore.LoadObject(&O3);

//	Dormir::Sprite S3(100,100);
//	S3.SetTexture(gCore.GetTexture("Sprites/stagePart1.png"));
//	S3.setReference(&O3);

//	gCore.LoadSprite(&S3);

	Dormir::Joint J(&O,&O2,800,600,0);


	pCore.LoadJoint(&J);


	//Dormir::Joint J2(&O2,&O3,1150,650);
	//pCore.LoadJoint(&J2);

	int running=!glfwGetKey( GLFW_KEY_ESC ) &&glfwGetWindowParam( GLFW_OPENED );
	while(running){
		double start=glfwGetTime();
		glClear(GL_COLOR_BUFFER_BIT);
		if(glfwGetKey(GLFW_KEY_F1)){
			pCore.setGravity(0,-0.4);
		}
		if(glfwGetKey(GLFW_KEY_F2)){
			pCore.setGravity(0.4,0);
		}
		pCore.Run();
		gCore.Run();
		running= !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
		glfwSwapBuffers();
		double finish=glfwGetTime();
		if(0.01-(finish-start)>0){
			glfwSleep(0.01-(finish-start));
		}
	}
*/

	std::multimap<int,int> M;
	M.insert(std::pair<int,int>(1,2));
	M.insert(std::pair<int,int>(2,4));
	M.insert(std::pair<int,int>(1,3));
	std::multimap<int,int>::iterator it=M.find(1);
	for(unsigned int i=0;i<M.count(1);i++,it++)
		std::cout<<it->second<<" found\n";
}
