/*
 * PolySprite.hpp
 *
 *  Created on: Aug 7, 2011
 *      Author: sebastian
 */

#ifndef POLYSPRITE_HPP_
#define POLYSPRITE_HPP_
#include <map>
#include <list>
#include <vector>
#include <string>
#include <GL/gl.h>
#include <time.h>
#include <GL/glfw.h>



namespace Dormir{
	class PhysicsObject;

	struct Frame{
		GLfloat Coord,Time;
	};

	class Animation{
	public:
		Animation(){};

		GLfloat GetX();
		GLfloat GetY();
		GLfloat GetA();

		void InitializeX();
		void InitializeY();
		void InitializeA();

		void AddX(GLfloat x,GLfloat time);
		void AddY(GLfloat y,GLfloat time);
		void AddA(GLfloat a,GLfloat time);
	private:
		std::list<Frame> X_Animation,Y_Animation,A_Animation;
		std::list<Frame>::iterator X_Current,X_Next,Y_Current,Y_Next,A_Current,A_Next;
		GLfloat X_Start,Y_Start,A_Start;
	};


	class PolySprite{
	public:
		PolySprite();

		void Draw();

		void SetVertex(GLfloat * V){vertex=V;};

		void SetReferenceObject(PhysicsObject * O){ReferenceObject=O;};

		void SetPos(GLfloat x,GLfloat y){Pos[0]=x;Pos[1]=y;};

		void SetRotation(GLfloat r){Rotation=r;};

		void SetColor(GLfloat r,GLfloat g,GLfloat b,GLfloat a){color[0]=r;color[1]=g;color[2]=b;color[3]=a;};

		void AddSubLevelSprite(PolySprite * P){SubLevel.push_back(P);};

		void AddAnimation(std::string Key,Animation A);

		void ToggleAnimation(std::string Key);

	private:
		std::map<std::string,Animation>::iterator CurrentAnimation;
		std::map<std::string,Animation> AnimationDatabase;
		std::list<PolySprite *> SubLevel;
		GLfloat * vertex,color[4];
		PhysicsObject * ReferenceObject;
		GLfloat Pos[2],Rotation;
	};

}

#endif /* POLYSPRITE_HPP_ */
