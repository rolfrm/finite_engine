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

	Dormir::PhysicsObject Ground;
	Ground.LoadPolygon(Dormir::GenerateBox(0,0,10000,200));
	pCore.LoadObject(&Ground);

	Dormir::PhysicSprite GroundSprite(&*Ground.Body.begin());

	gCore.LoadSprite(&GroundSprite);

	for(unsigned int i=0;i<15;i++){
		for(unsigned int j=0;j<15;j++){
			Dormir::PhysicsObject * O = new Dormir::PhysicsObject(1);
			O->LoadPolygon(Dormir::GenerateBox(100*i,150+j*100,100,100));
			pCore.LoadObject(O);
		}
	}

	int running=!glfwGetKey( GLFW_KEY_ESC ) &&glfwGetWindowParam( GLFW_OPENED );
	int prev_f1=glfwGetKey(GLFW_KEY_F1),prev_f2=glfwGetKey(GLFW_KEY_F2);

	int n=0,rate=0;
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
	//	gCore.Run();

		double finish=glfwGetTime();
		n++;
		rate+=1/(finish-start);
		std::cout<<"frame rate = "<<rate/n<<"\n";
		if(0.01-(finish-start)>0){
			glfwSleep(0.01-(finish-start));
		}
	}


}
