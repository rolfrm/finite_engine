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

Dormir::Core pCore(400);
Dormir::GraphicsCore gCore(1440,900);



int main(int argc,char ** argv){
	pCore.setGravity(0,-0.4);

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
	O2.LoadPolygon(Dormir::GenerateBox(700,450,100,100));

	pCore.LoadObject(&O2);

	Dormir::Sprite S2(100,100);
	S2.SetTexture(gCore.GetTexture("Sprites/stagePart1.png"));
	S2.setReference(&O2);

	gCore.LoadSprite(&S2);

	Dormir::Joint J(&O,&O2,700,550);

	pCore.Joints.push_back(&J);

	int running=!glfwGetKey( GLFW_KEY_ESC ) &&glfwGetWindowParam( GLFW_OPENED );
	while(running){
		double start=glfwGetTime();
		glClear(GL_COLOR_BUFFER_BIT);
		pCore.Run();
		gCore.Run();
		running= !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
		glfwSwapBuffers();
		double finish=glfwGetTime();
		if(0.01-(finish-start)>0){
			glfwSleep(0.01-(finish-start));
		}
	}


}
