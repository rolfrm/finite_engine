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
void Init(int width,int height, bool fullscreen, int FSAASamples){
	glfwInit();
	ScreenSize.X = width;
	ScreenSize.Y = height;
	if(FSAASamples > 0 && FSAASamples <= 16){
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES,FSAASamples);
	}
	if(fullscreen){
		glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_FULLSCREEN);
	}else{
		glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_WINDOW);
	}
	glfwSetKeyCallback(keycallback);
	glfwSetMouseButtonCallback(mousebuttoncallback);
	glfwSetMousePosCallback(mouseposcallback);;
	glLoadIdentity();
	glPointSize(3);
	glColor4f(1,1,1,1);
	glClearColor(0.5,0.6,0.7,1);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
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
	
	
	drw->Draw(x,y,rotation);
}

void Draw2(float x,float y, float rotation, Drawable* drw){
	Draw(x,y,rotation,drw);
}


Texture::Texture(char* data, int width, int height, int colorChannels, int inputSize){
	this->data = (float*)data;
	this->width = width;
	this->height = height;
	this->InputSize = inputSize;
	ColorChannels = colorChannels;
	gltex = -1;
	gltex = GetGLTexture();
	}
unsigned int Texture::GetGLTexture(){
	if(gltex == (unsigned int)-1){
		int glCol;
		if(ColorChannels == 3){
			glCol = GL_RGB;
		}else if(ColorChannels == 4){
			glCol = GL_RGBA;
		}else{
			glCol = GL_INTENSITY;
		}
		unsigned int intype;
		if (InputSize == 1){
			intype = GL_UNSIGNED_BYTE;
		}else if(InputSize == 4){
			intype = GL_FLOAT;
		}
		glGenTextures(1,&gltex);
		glBindTexture(GL_TEXTURE_2D, gltex);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		
		glTexImage2D(GL_TEXTURE_2D,0,ColorChannels,this->width,this->height,0,glCol,intype,data);
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

void Shader::SetUniform1iv(int * data, unsigned int count, const char * uniformname){
	int loc = GetUniformLocation(uniformname);
	glUniform1iv(loc,count,data);
	}
void Shader::SetUniform1fv(float * data, unsigned int count, const char * uniformname){
	int loc = GetUniformLocation(uniformname);
	glUniform1fv(loc,count,data);
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
void Drawable::Draw(float x, float y, float rotation){
	
	}
void Drawable::AddTexture(Texture * tex,int textureChannel){
	if(textureChannel >= 0 && textureChannel < 2){
		boundTextures[textureChannel] = tex;
		
	}
}

void Drawable::ActivateTextures(){
	for(int i = 0; i < 2;i++){
			glActiveTexture(GL_TEXTURE0 + i);
			if(boundTextures[i] != NULL){
				glBindTexture(GL_TEXTURE_2D, boundTextures[i]->GetGLTexture());
				char buf[5];
				sprintf(buf,"tex%i\0",i);
				ActiveShader.SetUniform1i(i,buf);
				char buf2[11];
				sprintf(buf2,"tex%iActive\0",i);
				ActiveShader.SetUniform1i(1,buf2);


			}else{
				char buf[11];
				sprintf(buf,"tex%iActive",i);
				ActiveShader.SetUniform1i(0,buf);
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
Polygon::Polygon(){
	
}

unsigned int GenVBO(void * data, unsigned int lenBytes, unsigned int type, unsigned int glBufferType){
	if(type == 0){
		type = GL_STATIC_DRAW_ARB;
	}else if(type == 1){
		type = GL_DYNAMIC_DRAW_ARB;
	}else{
		type = GL_STREAM_DRAW_ARB;
	}
	
	unsigned int output;
	glGenBuffersARB(1,&output);
	glBindBufferARB(glBufferType, output);
	glBufferDataARB(glBufferType,lenBytes,data,type);
	return output;
	
}

Polygon::Polygon(std::vector<float> vertexes, std::vector<unsigned int> indices, std::vector<float> colors, std::vector<float> uvs,unsigned int vertType,unsigned int uvType){
	drawType = GL_QUADS;
	LoadVerts(vertexes,vertType);
	this->indices = indices;
	if(colors.size() > 0){
			usingColor = true;
	}
	this->colors = colors;
	usingUV = false;
	if(uvs.size() > 0){
		LoadUV(uvs,uvType);
	}
	
	refreshVbos();
	}

Polygon::Polygon(char * rawdata_verts,unsigned int lv,char* rawdata_indices, unsigned int li, char * rawdata_color, unsigned int lc, char* rawdata_uvs, unsigned int luv,unsigned int vertType,unsigned int uvType){
	drawType = GL_QUADS;
	LoadVerts(std::vector<float>((float*)rawdata_verts,((float*)rawdata_verts) + lv),vertType);
	this->indices = std::vector<unsigned int>((unsigned int*)rawdata_indices,((unsigned int*)rawdata_indices)+li);
	
	if(lc > 0){
	this->colors = std::vector<float>((float*)rawdata_color,((float*)rawdata_color)+lc);
	
	usingColor = true;
	}else{
		usingColor = false;
	
	}
	usingUV = false;
	if(luv > 0){
		std::vector<float> nuvs = std::vector<float>((float*)rawdata_uvs,((float*)rawdata_uvs)+ luv);
		LoadUV(nuvs,uvType);
	}
	
	refreshVbos();
	}
	
void Polygon::LoadUV(std::vector<float> uvVector, int drawType){
	usingUV = true;
	uvVbo = GenVBO(&(uvVector[0]),uvVector.size()*sizeof(float),drawType,GL_ARRAY_BUFFER_ARB);
}

void Polygon::ReloadUV(std::vector<float> uvVector,unsigned int offset){
	glBindBuffer(GL_ARRAY_BUFFER_ARB,uvVbo);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, uvVector.size()*sizeof(float), &(uvVector[0]));
}
void Polygon::LoadVerts(std::vector<float> vertVector, int drawType){
	vertVbo = GenVBO(&(vertVector[0]),vertVector.size()*sizeof(float),drawType,GL_ARRAY_BUFFER_ARB);
}
void Polygon::ReloadVerts(std::vector<float> vertVector,unsigned int offset){
	glBindBuffer(GL_ARRAY_BUFFER_ARB,vertVbo);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, vertVector.size()*sizeof(float), &(vertVector[0]));
}

void Polygon::refreshVbos(){
	//Load in constructor instead..
	//vertVbo = GenVBO(&vertexes[0],vertexes.size()*sizeof(float),0,GL_ARRAY_BUFFER_ARB);
	indiceVbo = GenVBO(&indices[0],indices.size()*sizeof(int),0,GL_ELEMENT_ARRAY_BUFFER_ARB);
	
	if(usingColor){
		colorVbo = GenVBO(&colors[0],colors.size()*sizeof(float),0,GL_ARRAY_BUFFER_ARB);
	}
	
	}
	
void Polygon::Draw(float x, float y, float rotation){
	ActiveShader.SetUniform1f(x,"Xoff");
	ActiveShader.SetUniform1f(y,"Yoff");
	ActiveShader.SetUniform1f(rotation,"Rotation");
	int posAttribLoc = glGetAttribLocation(ActiveShader.ShaderProgram,"pos");
	int colorAttribLoc = glGetAttribLocation(ActiveShader.ShaderProgram,"color");
	int uvAttribLoc = glGetAttribLocation(ActiveShader.ShaderProgram,"uv");
	glEnableVertexAttribArray(posAttribLoc);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertVbo);
	glVertexAttribPointer(posAttribLoc,2,GL_FLOAT,0,0,NULL);
	ActivateTextures();
	if(usingColor){
		glEnableVertexAttribArray(colorAttribLoc);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorVbo);
		glVertexAttribPointer(colorAttribLoc,3,GL_FLOAT,0,0,0);
		
	}
	
	if(usingUV){
			
			glEnableVertexAttribArray(uvAttribLoc);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,uvVbo);
			glVertexAttribPointer(uvAttribLoc,2,GL_FLOAT,0,0,0);
		
	}
	
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indiceVbo);	
	glDrawElements(drawType,indices.size(),GL_UNSIGNED_INT,0);
	glDisableVertexAttribArray(posAttribLoc);
	
	if(usingColor){
		
		glDisableVertexAttribArray(colorAttribLoc);
	}
	if(usingUV){
		glDisableVertexAttribArray(uvAttribLoc);
		}
	
	}
void Polygon::SetDrawType(unsigned int i){
	drawType = i;
}
Text::Text(Texture * FontTex,float fromx, float tox, float fromy, float toy, int lines, int charsPerLine, int textStart){
	//AddTexture(FontTex,0);
	this->fromx = fromx;
	this->tox = tox;
	this->fromy = fromy;
	this->toy = toy;
	this->lines = lines;
	this->charsPerLine = charsPerLine;
	this->textStart = textStart;
	float verts[] = {0,0,0,1,1,1,1,0};
	unsigned int indices[] = {0,1,2,3};
	float uvs[] = {0,1,0,0,1,0,1,1};
	Quad = Polygon(std::vector<float>(verts,verts + 8), std::vector<unsigned int>(indices, indices + 4),std::vector<float>(),std::vector<float>(uvs,uvs+8),0,2);
	Quad.AddTexture(FontTex,0);
}
void Text::SetText(std::string text){
	this->text = text;
}
void Text::Draw(float x, float y, float rotation){
	ActivateTextures();
	float charSizeX = (tox - fromx)/charsPerLine;
	float charSizeY = (toy - fromy)/lines;
	Zoom(20,20);
	int newlines = 0;
	int column = 0;
	for(int i = 0; i < text.length(); i++){
		
		char letter = text[i];
		if(letter =='\n'){
			newlines +=1;
			column = 0;
			continue;
		}
		
		int index = letter - this->textStart;
		if( index < 0){
			index = 0;
		}
		int line = index/charsPerLine;
		int col = index - line*charsPerLine;
		
		float x1 = col*charSizeX;
		float x2 = x1 + charSizeX; 
		float y1 = line*charSizeY;
		float y2 = y1 + charSizeY;
		float newuvs[] ={x1,y2,x1,y1,x2,y1,x2,y2};
		Quad.ReloadUV(std::vector<float>(newuvs,newuvs+8));
		Draw2(x + column*0.9,y-newlines,0,&Quad);
		column +=1;
		
	}
	Zoom(1,1);
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
	float * idata = new float[lights.size()];
	for(int i= 0; i < lights.size();i++){
		xydata[i*2] = lights[i].X;
		xydata[i*2+1] = lights[i].Y;
		rgbdata[i*3] = lights[i].R;
		rgbdata[i*3+1] = lights[i].G;
		rgbdata[i*3+2] = lights[i].B;
		idata[i] = lights[i].intensity;
		}
	ActiveShader.SetUniform2fv(xydata,lights.size(),"MultiLightPos");
	ActiveShader.SetUniform3fv(rgbdata,lights.size(),"MultiLightColor");
	ActiveShader.SetUniform1fv(idata,lights.size(),"MultiLightIntensity");
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
