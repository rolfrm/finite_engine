/*
 * Sprite.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: sebastian
 */

#include "Sprite.hpp"
#include "../../Physics/Shapes/Polygon.h"
#include <math.h>
#include "../../Physics/Objects/PhysicsObject.h"
#include <iostream>
//#include "../../TextureCoords.hpp"

namespace Dormir{

	GLfloat whole [] = {0,1,0,0,1,0,1,1};

	Sprite::Sprite(int nWidth,int nHeight,unsigned int nVertices){
		width=nWidth;
		height=nHeight;
		vertex= new GLfloat[nVertices*2];
	//	textureCords= new GLfloat[8];
		texture=0;
		vertices=4;
		ReferenceObject=NULL;
		setDepth(0);
		std::vector<AnimationFrame> A;
		AnimationFrame AF;
		AF.TexCoords=whole;
		AF.duration=INFINITY;
		A.push_back(AF);
		AddAnimation("default",A);
		SetAnimation("default");
		Left=false;
		SetColor(1,1,1,1);
	}

	Sprite::~Sprite(){
		delete [] vertex;
		//delete [] textureCords;
	}


	void Sprite::Draw(){

		glPushMatrix();

		glColor4f(color[0],color[1],color[2],color[3]);
		glTranslatef(pos[0],pos[1],0);
		glRotatef(pos[2]*180/M_PI,0,0,1);
		if(Left)
			glRotatef(180,0,1,0);

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D,texture);





		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glTexCoordPointer(2,GL_FLOAT,0,textureCords);
		glVertexPointer(2,GL_FLOAT,0,vertex);



		glDrawArrays(GL_POLYGON,0,vertices);


		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glDisable(GL_TEXTURE_2D);

		glPopMatrix();

	}

	void Sprite::Update(){
		vertex[0]=-width/2;
		vertex[1]=-height/2;
		vertex[2]=-width/2;
		vertex[3]=height/2;
		vertex[4]=width/2;
		vertex[5]=height/2;
		vertex[6]=width/2;
		vertex[7]=-height/2;


		if(ReferenceObject!=NULL){
			setPos(ReferenceObject->GetPosition().x,ReferenceObject->GetPosition().y,ReferenceObject->GetAngle());
		}

/*
		textureCords[0]=0;
		textureCords[1]=1;
		textureCords[2]=0;
		textureCords[3]=0;
		textureCords[4]=1;
		textureCords[5]=0;
		textureCords[6]=1;
		textureCords[7]=1;
*/
		if(glfwGetTime()-start>(*CurrentAnimation)[frame].duration){
			frame++;
			if(frame==CurrentAnimation->size()){
				frame=0;
			}
			start=glfwGetTime();
			SetTexCoords();
		}

	}

	void Sprite::SetAnimation(std::string nAni){
		if(CurrentAnimation!=&AnimationDatabase[nAni]){
			CurrentAnimation=&AnimationDatabase[nAni];
			start=glfwGetTime();
			frame=0;
			SetTexCoords();
		}
	}

	void Sprite::SetTexCoords(){
		textureCords=(*CurrentAnimation)[frame].TexCoords;
		/*textureCords[0]=AF.xmin;
		textureCords[1]=AF.ymax;
		textureCords[2]=AF.xmin;
		textureCords[3]=AF.ymin;
		textureCords[4]=AF.xmax;
		textureCords[5]=AF.ymin;
		textureCords[6]=AF.xmax;
		textureCords[7]=AF.ymax;*/
	}

	PhysicSprite::PhysicSprite(Polygon * P):
		Sprite(0,0,P->Vertex.size()){
		texture=0;
		ReferencePolygon=P;
		vertices=P->Vertex.size();
		setPos(0,0,0);
	}

	void PhysicSprite::Update(){
		for(unsigned int i=0;i<ReferencePolygon->Vertex.size();i++){
			vertex[i*2]=ReferencePolygon->Vertex[i].x;
			vertex[i*2+1]=ReferencePolygon->Vertex[i].y;
		}




		if(glfwGetTime()-start>(*CurrentAnimation)[frame].duration){
			frame++;
			if(frame==CurrentAnimation->size()){
				frame=0;
			}
			start=glfwGetTime();
			SetTexCoords();
		}
	}


}
