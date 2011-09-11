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

int PrevRightMouseState;
Dormir::SingleSprint Telekinesis(0.05,0.1,0);

void HandleTelekinesis(){
	if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)){
		int x,y;
		glfwGetMousePos(&x,&y);
		//x+=-(gCore.Width/2+gCore.Origin[0]);

		//x-=gCore.Origin[0];
		y=(gCore.Height-y);

		Telekinesis.SetFixedPos(x,y);
		if(!PrevRightMouseState){
			Dormir::PhysicsObject * Bind = pCore.PointInsideObject(x,y);

			if(Bind!=NULL){
				Telekinesis.SetFirstObject(Bind);
				Telekinesis.setSprintRest((Telekinesis.GetFixedPos()-Bind->GetPosition()).GetNorm2());
				pCore.LoadConstraint(&Telekinesis);
			}
		}
	}
	else if(PrevRightMouseState){
		pCore.UnloadConstraint(&Telekinesis);
		Telekinesis.SetFirstObject(NULL);
	}
	PrevRightMouseState=glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
}


int main(int argc,char ** argv){
	pCore.setGravity(0,-0.4);
	pCore.setVelocityDampening(0.02);
	pCore.setRotationDampening(0.02);

	gCore.GenerateTexture("Sprites/stagePart1.png");
	gCore.GenerateTexture("Sprites/stagePart4.png");

	Dormir::PhysicsObject O;
	O.LoadPolygon(Dormir::GenerateBox(700,650,200,100));

	pCore.LoadObject(&O);

	Dormir::Sprite S(200,100);
	S.SetTexture(gCore.GetTexture("Sprites/stagePart4.png"));
	S.setReference(&O);

	gCore.LoadSprite(&S);
/*
	Dormir::PhysicsObject O2(1);
	O2.LoadPolygon(Dormir::GenerateBox(850,550,100,100));

	pCore.LoadObject(&O2);

	Dormir::PhysicSprite S2(&*O2.Body.begin());
	*/
	std::vector<Dormir::PhysicsObject *> V;
	Dormir::PhysicsObject * O1=&O,* O2;
	for(unsigned int i=0;i<12;i++){
		O2=new Dormir::PhysicsObject(1);
		O2->LoadPolygon(Dormir::GenerateBox(810,575-i*50,20,50));
		//O2->CalculateMomentofInertia();

		V.push_back(O2);
		//pCore.LoadObject(O2);

		Dormir::PhysicSprite * S = new Dormir::PhysicSprite(&*O2->Body.begin());

		gCore.LoadSprite(S);

		pCore.LoadJoint(new Dormir::Joint(O2,O1,810,600-i*50));

		O1=O2;

	}


	//pCore.ObjectClusters.push_back(V);

	pCore.LoadObjectCluster(V);


	int running=!glfwGetKey( GLFW_KEY_ESC ) &&glfwGetWindowParam( GLFW_OPENED );
	int prev_f1=glfwGetKey(GLFW_KEY_F1),prev_f2=glfwGetKey(GLFW_KEY_F2);
	while(running){
		double start=glfwGetTime();

		HandleTelekinesis();
		if(glfwGetKey(GLFW_KEY_F1) && !prev_f1){

			pCore.setGravity(-pCore.getGravity().y,pCore.getGravity().x);
		}
		if(glfwGetKey(GLFW_KEY_F2) && !prev_f2){
			pCore.setGravity(pCore.getGravity()*-1);
		}

		running= !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );

		prev_f1=glfwGetKey(GLFW_KEY_F1);
		prev_f2=glfwGetKey(GLFW_KEY_F2);

		pCore.Run();
		gCore.Run();

		double finish=glfwGetTime();
		if(0.01-(finish-start)>0){
			glfwSleep(0.01-(finish-start));
		}
	}
}
