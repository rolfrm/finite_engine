#include <GL/glew.h>
#include <GL/glfw.h>
#include <GL/gl.h>
#include "gfw.h"
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <list>
void printLog(GLuint obj)
{
    int infologLength = 0;
    char infoLog[1024];
 
	if (glIsShader(obj))
		glGetShaderInfoLog(obj, 1024, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, 1024, &infologLength, infoLog);
 
    if (infologLength > 0)
		std::cout << infoLog << "\n";
}




std::list<KeyEvent> KeyEvents;
std::list<MouseEvent> MouseEvents;
float X;
float Y;
void GLFWCALL mouseposcallback(int x, int y){
	X = x;
	Y = y;
}
void GLFWCALL keycallback( int key, int action ){
	KeyEvent newKeyEv;
	newKeyEv.key = key;  
	newKeyEv.action = action;
	if(key < 255){
		newKeyEv.charKey = (char)key;
		}else{
		newKeyEv.charKey = (char)-1;
			}
	KeyEvents.push_back(newKeyEv);
}

void GLFWCALL mousebuttoncallback(int button, int action){
	MouseEvent mEv;
	mEv.button = button;
	mEv.action = action;
	MouseEvents.push_back(mEv);
	}

std::list<KeyEvent> GetKeyEvents(){
	std::list<KeyEvent> out = KeyEvents;
	KeyEvents = std::list<KeyEvent>();
	return out;
	}
std::list<MouseEvent> GetMouseEvents(){ 
	std::list<MouseEvent> out = MouseEvents;
	MouseEvents = std::list<MouseEvent>();
	return out;
	}
Vec GetMousePos(){
		Vec out;
		out.X = X;
		out.Y = Y;
		return out;
}
Shader ActiveShader;
void SetActiveShader(Shader s){
		ActiveShader = s;
		glUseProgram(s.ShaderProgram);
}

Vec currentZoomLevel;
void Zoom(float x, float y){
	currentZoomLevel.X = x;
	currentZoomLevel.Y = y;
	ActiveShader.SetUniform2f(x,y,"Zoom");
	}

Vec ScreenSize;
void Init(int width,int height, bool fullscreen){
	glfwInit();
	ScreenSize.X = width;
	ScreenSize.Y = height;
	
	if(fullscreen){
		glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_FULLSCREEN);
	}else{
		glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_WINDOW);
	}
	glfwSetKeyCallback(keycallback);
	glfwSetMouseButtonCallback(mousebuttoncallback);
	glfwSetMousePosCallback(mouseposcallback);
	std::cout << "Window opened\n";
	
	std::cout << "setting stuff.\n";
	glLoadIdentity();
	glPointSize(3);
	glColor4f(1,1,1,1);
	
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error setting up GLEW!\n";
	}
}

Vec ScreenToWorldCoordinates(Vec in){
		Vec out;
		
		std::cout << in.Y <<" " << 2*(in.Y/ScreenSize.Y - 0.5) << "\n";
		out.X = currentZoomLevel.X*(in.X/ScreenSize.X - 0.5)*2;
		out.Y = -currentZoomLevel.Y*(in.Y/ScreenSize.Y - 0.5)*2;
		return out;
}
Vec WorldToScreenCoordinates(Vec in){
	Vec out;
	out.X = (in.X/currentZoomLevel.X+1)*ScreenSize.X/2;
	out.Y = -(in.Y/currentZoomLevel.Y+1)*ScreenSize.Y/2;
	return out;
	}
void Refresh(){
		glfwSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT);
}

void DeInit(){
	glfwTerminate();
}

void Draw(float x,float y, float rotation, Drawable* drw){
	ActiveShader.SetUniform1f(x,"Xoff");
	ActiveShader.SetUniform1f(y,"Yoff");
	ActiveShader.SetUniform1f(rotation,"Rotation");
	drw->Draw();
}

Texture::Texture(char* data, int width, int height){
	this->data = (unsigned char*)data;
	this->width = width;
	this->height = height;
	gltex = -1;
		
	}
unsigned int Texture::GetGLTexture(){
	if(gltex == (unsigned int)-1){
		
		glGenTextures(1,&gltex);
		glBindTexture(GL_TEXTURE_2D, gltex);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->width,this->height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
	}
	return gltex;
	
}


Shader::Shader( const char * vertexsrc, const char * fragmentsrc){
	const char* vsrc = vertexsrc;
	const char* fsrc = fragmentsrc;
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vs,1, &vsrc,NULL);
	glShaderSource(fs,1, &fsrc,NULL);
	glCompileShader(vs);
	glCompileShader(fs);
	
	GLuint sp = glCreateProgram();
	glAttachShader(sp,vs);
	glAttachShader(sp,fs);
	glLinkProgram(sp);
	ShaderProgram = sp;
	printLog(sp);
	
}

Shader::Shader(){
}

void Shader::SetUniform1f(float value,const char * uniformname){
		glUniform1f(GetUniformLocation(uniformname),value);
	}
void Shader::SetUniform2f(float v1,float v2,const char * uniformname){
		glUniform2f(GetUniformLocation(uniformname),v1,v2);
	}
void Shader::SetUniform3f(float v1, float v2, float v3,const char * uniformname){
		glUniform3f(GetUniformLocation(uniformname),v1,v2,v3);
	}
void Shader::SetUniform1i(int value, const char * uniformname){
		int loc = GetUniformLocation(uniformname);
		glUniform1i(loc,value);
	}
void Shader::SetUniform3fv(float * data,unsigned int count, const char * uniformname){
		int loc = GetUniformLocation(uniformname);
		glUniform3fv(loc,count,data);
}
void Shader::SetUniform2fv(float * data,unsigned int count, const char * uniformname){
		int loc = GetUniformLocation(uniformname);
		glUniform2fv(loc,count,data);
}



unsigned int Shader::GetUniformLocation(const char * uniformname){
	return glGetUniformLocation(ShaderProgram,uniformname);
	}

#define MAXTEXTURES 2
Drawable::Drawable(){
	for(int i= 0; i < MAXTEXTURES;i++){
		boundTextures[i] = NULL;
		}
	}
void Drawable::Draw(){
	
	}
void Drawable::AddTexture(Texture * tex,int textureChannel){
	if(textureChannel >= 0 && textureChannel < 2){
		boundTextures[textureChannel] = tex;
		
	}
}

void Drawable::ActivateTextures(){
	for(int i = 0; i < 2;i++){
			if(boundTextures[i] != NULL){
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, boundTextures[i]->GetGLTexture());
				char buf[10];
				sprintf(buf,"tex%i",i);
				ActiveShader.SetUniform1i(i,buf);
			}
	}
}

DrawableTest::DrawableTest(){
	
	unsigned int vboId;
	float data[6] = {0,0 ,-0.5,-0.5,0.5,-0.5};
	glGenBuffersARB(1,&vboId);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,2*4*3,data,GL_STATIC_DRAW_ARB);
	testVBO = vboId;
	
}

void DrawableTest::Draw(){
		int posAttribLoc = glGetAttribLocation(ActiveShader.ShaderProgram,"pos");
		glEnableVertexAttribArray(posAttribLoc);
		
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, testVBO);
		glVertexAttribPointer(posAttribLoc,2,GL_FLOAT,0,0,0);
		
		glDrawArrays(GL_POINTS,0, 3);
		glDisableVertexAttribArray(posAttribLoc);
}

Polygon::Polygon(std::vector<float> vertexes, std::vector<int> indices, std::vector<float> colors, std::vector<float> uvs){
	this->vertexes = vertexes;
	refreshVbos();
	}
Polygon::Polygon(char * rawdata_verts,unsigned int lv,char* rawdata_indices, unsigned int li, char * rawdata_color, unsigned int lc, char* rawdata_uvs, unsigned int luv){
	this->vertexes = std::vector<float>((float*)rawdata_verts,((float*)rawdata_verts) + lv);
	this->indices = std::vector<unsigned int>((unsigned int*)rawdata_indices,((unsigned int*)rawdata_indices)+li);
	
	if(lc > 0){
	this->colors = std::vector<float>((float*)rawdata_color,((float*)rawdata_color)+lc);
	
	usingColor = true;
	}else{
		usingColor = false;
	
	}
	if(luv > 0){
		usingUV = true;
		this->uvs = std::vector<float>((float*)rawdata_uvs,((float*)rawdata_uvs)+ luv);
	}else{
		usingUV = false;
	}
	
	
	refreshVbos();
	}

void Polygon::refreshVbos(){
	//std::cout << vertexes.size() << "\n";
	glGenBuffersARB(1,&vertVbo);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertVbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,4*vertexes.size(),&vertexes[0],GL_STATIC_DRAW_ARB);
	
	glGenBuffersARB(1,&(this->indiceVbo));
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indiceVbo);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER,4*indices.size(),&indices[0],GL_STATIC_DRAW_ARB);
	
	if(usingColor){
		glGenBuffersARB(1,&colorVbo);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,colorVbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,4*colors.size(),&colors[0],GL_STATIC_DRAW_ARB);
		}
	if(usingUV){
		glGenBuffersARB(1,&uvVbo);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,uvVbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,4*uvs.size(),&uvs[0],GL_STATIC_DRAW_ARB);
		}
	
	}
	
void Polygon::Draw(){
	
	int posAttribLoc = glGetAttribLocation(ActiveShader.ShaderProgram,"pos");
	int colorAttribLoc = glGetAttribLocation(ActiveShader.ShaderProgram,"color");
	int uvAttribLoc = glGetAttribLocation(ActiveShader.ShaderProgram,"uv");
	glEnableVertexAttribArray(posAttribLoc);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertVbo);
	glVertexAttribPointer(posAttribLoc,2,GL_FLOAT,0,0,NULL);
	if(usingColor){
		glEnableVertexAttribArray(colorAttribLoc);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorVbo);
		glVertexAttribPointer(colorAttribLoc,3,GL_FLOAT,0,0,0);
		
	}
	
	if(usingUV){
			//glEnable(GL_TEXTURE_2D);
			ActivateTextures();
			glEnableVertexAttribArray(uvAttribLoc);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,uvVbo);
			glVertexAttribPointer(uvAttribLoc,2,GL_FLOAT,0,0,0);
		
	}
	
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indiceVbo);	
	glDrawElements(GL_QUADS,indices.size(),GL_UNSIGNED_INT,0);
	glDisableVertexAttribArray(posAttribLoc);
	
	if(usingColor){
		
		glDisableVertexAttribArray(colorAttribLoc);
	}
	if(usingUV){
		glDisableVertexAttribArray(uvAttribLoc);
		}
	
	}
	
Vec::Vec(){
	X = 0;
	Y = 0;
	}
Vec::Vec(float x,float y){
	X = x;
	Y = y;
	}

Light::Light(float x, float y, float r, float g, float b, float i){
	X = x;
	Y = y;
	R = r;
	G = g;
	B = b;
	intensity = i;
	}
LightSystem::LightSystem(int nrChannels){
	for(int i = 0 ; i< nrChannels;i++){
		lights.push_back(Light(0,0,0,0,0,0));
		}
	}
	
void LightSystem::Activate(){
	float * xydata = new float[lights.size()*2];
	float * rgbdata = new float[lights.size()*3];
	for(int i= 0; i < lights.size();i++){
		xydata[i*2] = lights[i].X;
		xydata[i*2+1] = lights[i].Y;
		rgbdata[i*3] = lights[i].R;
		rgbdata[i*3+1] = lights[i].G;
		rgbdata[i*3+2] = lights[i].B;
		}
	ActiveShader.SetUniform2fv(xydata,lights.size(),"MultiLightPos");
	ActiveShader.SetUniform3fv(rgbdata,lights.size(),"MultiLightColor");
	ActiveShader.SetUniform1i(lights.size(),"NumLights");
	delete []xydata;
	delete []rgbdata;
	
	}
Light * LightSystem::GetLight(int channel){
		return &lights[channel];
	}
void LightSystem::SetLight(Light light, int channel){
	lights[channel] = light;
	}