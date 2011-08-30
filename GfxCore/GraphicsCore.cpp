#include "GraphicsCore.hpp"
#include <math.h>
#include "Sprite/Sprite.hpp"
#include "../Physics/Objects/PhysicsObject.h"
#include "../Physics/Shapes/Polygon.h"


#include <iostream>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>


namespace Dormir{

	GraphicsCore::GraphicsCore(unsigned int nWidth,unsigned int nHeight){
		glfwInit();
		ilInit();

		Width=nWidth;
		Height=nHeight;
		glfwOpenWindow(Width,Height,8,8,8,8,8,8,GLFW_WINDOW);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glScaled((double)2/Width,(double)2/Height,1);
	//	glTranslated(-(double)Width,-(double)Height,0);
		Origin[0]=-(double)Width/2;
		Origin[1]=-(double)Height/2;
	}

	GraphicsCore::~GraphicsCore(){
		for(std::map<std::string,GLuint>::iterator it=TextureDatabase.begin();it!=TextureDatabase.end();it++){
			glDeleteTextures(1,&it->second);
		}
		for(std::map<std::string,GLfloat *>::iterator it=TexCoordDatabase.begin();it!=TexCoordDatabase.end();it++){
			delete [] it->second;
		}
	}

	void GraphicsCore::GenerateTexture(std::string path){
		ILuint texid;
		GLuint tex;
		ilGenImages(1,&texid);
		ilBindImage(texid);

		if(ilLoadImage(path.c_str())){
				std::cout<<"texture "<<path<<" loaded\n";
				ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		}else{
			std::cout << "Problems converting image" <<path<<"\n";
		}

		glGenTextures(1,&tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, IL_RGBA,GL_UNSIGNED_BYTE, ilGetData());

		ilDeleteImage(texid);

		TextureDatabase.insert(std::pair<std::string,GLuint>(path,tex));
	}

	void GraphicsCore::GenerateNewTexCoord(std::string name,unsigned int size){
		GLfloat * t = new GLfloat [size*2];
		TexCoordDatabase.insert(std::pair<std::string,GLfloat *>(name,t));
	}

	void GraphicsCore::WriteTexCoord(std::string key,unsigned int index,float value){
		TexCoordDatabase[key][index]=value;
	}


	void GraphicsCore::LoadSprite(Sprite * S){
		Sprites.push_back(S);
		Sprites.sort(LevelComparison);
	}

	void GraphicsCore::UnloadSprite(Sprite * S){
		for(std::list<Sprite *>::iterator it=Sprites.begin();it!=Sprites.end();it++){
			if(*it==S){
				Sprites.erase(it);
			}
		}
	}

	void GraphicsCore::Run(){
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		glTranslatef(Origin[0],Origin[1],0);
		for(std::list<Sprite *>::iterator it=Sprites.begin();it!=Sprites.end();it++){
			(*it)->Update();
			(*it)->Draw();
		}
		glPopMatrix();
		glfwSwapBuffers();
	}





	char *textFileRead(char *fn) {


		FILE *fp;
		char *content = NULL;

		int count=0;

		if (fn != NULL) {
			fp = fopen(fn,"rt");

			if (fp != NULL) {

	      fseek(fp, 0, SEEK_END);
	      count = ftell(fp);
	      rewind(fp);

				if (count > 0) {
					content = (char *)malloc(sizeof(char) * (count+1));
					count = fread(content,sizeof(char),count,fp);
					content[count] = '\0';
				}
				fclose(fp);
			}
		}
		return content;
	}

	int textFileWrite(char *fn, char *s) {

		FILE *fp;
		int status = 0;

		if (fn != NULL) {
			fp = fopen(fn,"w");

			if (fp != NULL) {

				if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
					status = 1;
				fclose(fp);
			}
		}
		return(status);
	}


	bool LevelComparison(Sprite * S1,Sprite * S2){
		return S1->getDepth()<S2->getDepth();
	}









}
