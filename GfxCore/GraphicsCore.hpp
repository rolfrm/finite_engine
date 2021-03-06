/*
 * GraphicsCore.hpp
 *
 *  Created on: Apr 11, 2011
 *      Author: sebastian
 */

#ifndef GRAPHICSCORE_HPP_
#define GRAPHICSCORE_HPP_
//#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glfw.h>
#include <vector>
#include <map>
#include <string>
#include <list>
#include "IL/il.h"
#include "IL/ilu.h"

namespace Dormir{


	class Polygon;

	class Sprite;


	class GraphicsCore{
	public:
		GraphicsCore(unsigned int Width,unsigned int Height);

		void LoadSprite(Sprite * S);
		void GenerateTexture(std::string path);
		GLuint GetTexture(std::string path){return TextureDatabase[path];};
		void Run();
		int Origin[2];
		unsigned int Width,Height;

	private:
		std::map<std::string,GLuint> TextureDatabase;
		std::list<Sprite *> Sprites;

	};

	char *textFileRead(char *fn);
	int textFileWrite(char *fn, char *s);
	bool LevelComparison(Sprite * S1,Sprite * S2);


}

#endif /* GRAPHICSCORE_HPP_ */
