/*
 * PolySprite.cpp
 *
 *  Created on: Aug 7, 2011
 *      Author: sebastian
 */

#include "PolySprite.hpp"
#include <iostream>
#include "../../Physics/Objects/PhysicsObject.h"




namespace Dormir{




	PolySprite::PolySprite(){
		SetReferenceObject(NULL);
		SetPos(0,0);
		SetRotation(0);
		SetColor(1,1,1,1);
		CurrentAnimation=AnimationDatabase.end();
	}

	void PolySprite::Draw(){

		glPushMatrix();

		glColor4f(color[0],color[1],color[2],color[3]);
		glTranslatef(Pos[0],Pos[1],0);
		glRotatef(Rotation,0,0,1);

		glEnableClientState(GL_VERTEX_ARRAY);

		glVertexPointer(2,GL_FLOAT,0,vertex);


		glDrawArrays(GL_QUADS,0,4);

		glDisableClientState(GL_VERTEX_ARRAY);

		for(std::list<PolySprite *>::iterator it=SubLevel.begin();it!=SubLevel.end();it++){
			(*it)->Draw();
		}

		glPopMatrix();
	}

	void PolySprite::ToggleAnimation(std::string Key){
		CurrentAnimation=AnimationDatabase.find(Key);
		if(CurrentAnimation!=AnimationDatabase.end()){
			CurrentAnimation->second.InitializeX();
			CurrentAnimation->second.InitializeY();
			CurrentAnimation->second.InitializeA();
		}

	}

	void PolySprite::AddAnimation(std::string Key,Animation A){
		AnimationDatabase.insert(std::pair<std::string,Animation>(Key,A));
	}

	GLfloat Animation::GetX(){
		GLfloat Remainder=(glfwGetTime()-X_Start)/X_Current->Time;
		if(Remainder>1){
			X_Start+=X_Current->Time;
			X_Current++;
			if(X_Current==X_Animation.end())
				X_Current=X_Animation.begin();
			X_Next++;
			if(X_Next==X_Animation.end())
				X_Next=X_Animation.begin();
			return GetX();
		}
		else{
			return X_Current->Coord+(X_Next->Coord-X_Current->Coord)*Remainder;
		}

	}

	void Animation::InitializeX(){
		X_Start=glfwGetTime();
		X_Current=X_Animation.begin();
		X_Next=X_Current;
		X_Next++;
	}

	void Animation::AddX(GLfloat x,GLfloat time){
		Frame F;
		F.Coord=x;
		F.Time=time;
		X_Animation.push_back(F);
	}

	GLfloat Animation::GetY(){
		GLfloat Remainder=(glfwGetTime()-Y_Start)/Y_Current->Time;
		if(Remainder>1){
			Y_Start+=Y_Current->Time;
			Y_Current++;
			if(Y_Current==Y_Animation.end())
				Y_Current=Y_Animation.begin();
			Y_Next++;
			if(Y_Next==Y_Animation.end())
				Y_Next=Y_Animation.begin();
			return GetY();
		}
		else{
			return Y_Current->Coord+(Y_Next->Coord-Y_Current->Coord)*Remainder;
		}

	}

	void Animation::InitializeY(){
		Y_Start=glfwGetTime();
		Y_Current=Y_Animation.begin();
		Y_Next=Y_Current;
		Y_Next++;
	}

	void Animation::AddY(GLfloat y,GLfloat time){
		Frame F;
		F.Coord=y;
		F.Time=time;
		Y_Animation.push_back(F);
	}

	GLfloat Animation::GetA(){
		GLfloat Remainder=(glfwGetTime()-A_Start)/A_Current->Time;
		if(Remainder>1){
			A_Start+=A_Current->Time;
			A_Current++;
			if(A_Current==A_Animation.end())
				A_Current=A_Animation.begin();
			A_Next++;
			if(A_Next==A_Animation.end())
				A_Next=A_Animation.begin();
			return GetA();
		}
		else{
			return A_Current->Coord+(A_Next->Coord-A_Current->Coord)*Remainder;
		}

	}

	void Animation::InitializeA(){
		A_Start=glfwGetTime();
		A_Current=A_Animation.begin();
		A_Next=A_Current;
		A_Next++;
	}

	void Animation::AddA(GLfloat a,GLfloat time){
		Frame F;
		F.Coord=a;
		F.Time=time;
		A_Animation.push_back(F);
	}

}
