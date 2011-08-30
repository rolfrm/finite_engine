/*
 * Sprite.hpp
 *
 *  Created on: Aug 6, 2011
 *      Author: sebastian
 */

#ifndef SPRITE_HPP_
#define SPRITE_HPP_
#include <vector>
#include <string>
#include <map>
#include <GL/gl.h>
#include <time.h>
#include <GL/glfw.h>

namespace Dormir{

	class PhysicsObject;
	class Polygon;
	class GraphicsCore;

	struct AnimationFrame{
		GLfloat * TexCoords;
		double duration;
	};

	class Sprite{
	public:
		Sprite(int nWidth,int nHeight,unsigned int vertices=4);

		~Sprite();

		virtual void Draw();

		virtual void Update();

		void setReference(Dormir::PhysicsObject * Obj){ReferenceObject=Obj;};

		void setWidth(int w){width=w;};

		void setHeight(int h){height=h;};

		void setPos(double x,double y,double a){pos[0]=x;pos[1]=y;pos[2]=a;};

		void setLeft(bool nLeft){Left=nLeft;};

		void setDepth(double nZ){z=nZ;};

		double getDepth(){return z;};

		void SetTexture(GLuint nTexture){texture=nTexture;};

		void SetAnimation(std::string nAni);

		void AddAnimation(std::string key,std::vector<AnimationFrame> Animation){AnimationDatabase.insert(std::pair<std::string,std::vector<AnimationFrame> >(key,Animation));};

		void SetColor(GLfloat r,GLfloat g,GLfloat b,GLfloat a){color[0]=r,color[1]=g,color[2]=b,color[3]=a;};
	protected:
		void SetTexCoords();
		unsigned int frame;
		int width,height,vertices;
		double pos[3],start,z;
		bool Left;
		GLuint texture;
		GLfloat * vertex;
		GLfloat * textureCords;
		GLfloat color[4];
		std::vector<AnimationFrame> * CurrentAnimation;
		std::map<std::string,std::vector<AnimationFrame> > AnimationDatabase;
		Dormir::PhysicsObject * ReferenceObject;
	};

	class PhysicSprite:public Sprite{
	public:
		PhysicSprite(Polygon *);
		~PhysicSprite();

		void Update();
	private:
		Polygon * ReferencePolygon;
	};

}

#endif /* SPRITE_HPP_ */
