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
	glClearColor(0,0,0,0);
	glDisable(GL_BLEND);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_STENCIL_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_NORMALIZE);
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

void SetBGColor(float r, float g, float b){
	glClearColor(r,g,b,1);
}

void Draw2(float x,float y, float rotation, Drawable* drw){
	Draw(x,y,rotation,drw);
}

Texture::Texture(char* data, int width, int height, int colorChannels, int inputSize,int IncreasedRange){
	gltex = -1;
	UpdateTexture(data,  width,  height,0,  colorChannels,  inputSize,IncreasedRange);
	ref = new int;
	*ref = 1;
	
	this->channels = colorChannels;
	this->typesize = inputSize;
	
}

Texture::Texture( int width, int height, int colorChannels, int inputSize,int IncreasedRange){
	gltex = -1;
	
	ref = new int;
	*ref = 1;
	
	UpdateTexture(NULL,  width,  height,0,  colorChannels,  inputSize,IncreasedRange);
	this->channels = colorChannels;
	this->typesize = inputSize;	
}


Texture::Texture(int width, int colorChannels, int inputSize,int IncreasedRange){
gltex = -1;
	
	ref = new int;
	*ref = 1;
	
	UpdateTexture(NULL,  width,  0,0,  colorChannels,  inputSize,IncreasedRange);
	this->channels = colorChannels;
	this->typesize = inputSize;
}
Texture::Texture(int width, int height, int depth, int colorChannels, int inputSize,int IncreasedRange){
gltex = -1;
	UpdateTexture(NULL,  width,  height, depth,  colorChannels,  inputSize,IncreasedRange);
	ref = new int;
	*ref = 1;
	
	this->channels = colorChannels;
	this->typesize = inputSize;
}

Texture::Texture(const Texture& copy){
	gltex = copy.gltex;
	ref = copy.ref;
	width = copy.width;
	height = copy.height;
	depth = copy.depth;
	texdim = copy.texdim;
	glCol = copy.glCol;
	channels = copy.channels;
	typesize = copy.typesize;
	Channels = copy.Channels;
	*ref +=1;
}

Texture::Texture(){
	gltex = -1;
	ref = new int;
	*ref = 1;
}
	
Texture & Texture::operator=(const Texture & other){
	*ref -=1;
	if(*ref <= 0){
		delete ref;
		if(gltex != -1){
			glDeleteTextures(1,&gltex);
		}
	}

	gltex = other.gltex;
	ref = other.ref;
	width = other.width;
	height = other.height;
	depth = other.depth;
	texdim = other.texdim;
	glCol = other.glCol;
	Channels = other.Channels;
	channels = other.channels;
	typesize = other.typesize;
	
	
	*ref +=1;
	return *this;
}


Texture::~Texture(){
	*ref -=1;
	if(*ref <= 0){
		delete ref;
		if(gltex != -1){
			glDeleteTextures(1,&gltex);
		}
	}
}
void Texture::UpdateTexture(char * data, int width, int height, int depth, int colorChannels, int inputSize, int IncreasedRange){
		
		this->width = width;
		this->height = height;
		this->depth = depth;
		Channels = colorChannels;
		switch(colorChannels){
			case 1: glCol = GL_LUMINANCE;break;
			case 2: glCol = GL_LUMINANCE_ALPHA;break;
			case 3: glCol = GL_RGB;break;
			case 4: glCol = GL_RGBA;break;
		}
		
		intype;
		if (inputSize == 1){
			intype = GL_UNSIGNED_BYTE;
		}else if(inputSize == 4){
			intype = GL_FLOAT;
		}
		
		texdim = GL_TEXTURE_1D;
		if(height > 0){
			texdim = GL_TEXTURE_2D;
			if(depth > 0){
				texdim = GL_TEXTURE_3D;
			}
		}
		
		
		glGenTextures(1,&gltex);
		glBindTexture(texdim, gltex);
		glTexParameterf( texdim, GL_TEXTURE_MIN_FILTER,GL_NEAREST );
		glTexParameterf( texdim, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( texdim, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( texdim, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		
		range = GL_RGBA;
		if(IncreasedRange > 0){
		  if(IncreasedRange == 1){
		  range =GL_RGBA16F;
		  }else{
		    range = GL_RGBA32F;
		  }
		}
		if(texdim == GL_TEXTURE_1D){
			glTexImage1D(texdim,0,range,width,0,glCol,intype,data);
		}else if(texdim == GL_TEXTURE_2D){
			glTexImage2D(texdim,0,range,width,height,0,glCol,intype,data);
		}else{
			glTexImage3D(texdim,0,range,width,height,depth,0,glCol,intype,data);
		}
}

void Texture::LoadData(char * data,int width, int height, int depth){
	this->width = width;
	this->height = height;
	this->depth = depth;
	if(texdim == GL_TEXTURE_1D){
		glTexImage1D(texdim,0,range,width,0,glCol,intype,data);
	}else if(texdim == GL_TEXTURE_2D){
		glTexImage2D(texdim,0,range,width,height,0,glCol,intype,data);
	}else{
		glTexImage3D(texdim,0,range,width,height,depth,0,glCol,intype,data);
	}


}

void Texture::GenMipmaps(){
	glBindTexture(texdim,gltex);
	glGenerateMipmap(texdim);
}
unsigned int Texture::GetGLTexture(){
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
void Shader::SetUniform3fv(std::vector<float> fvec,int count, const char * uniformname){
	int loc = GetUniformLocation(uniformname);
	glUniform3fv(loc,count,&fvec[0]);
}	

void Shader::SetUniform2fv(std::vector<float> fvec,int count, const char * uniformname){
	int loc = GetUniformLocation(uniformname);
	glUniform2fv(loc,count,&fvec[0]);
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


Drawable::Drawable(){
	for(int i= 0; i < MAXTEXTURES;i++){
		boundTextures[i] = Texture();
	}
}
Drawable::~Drawable(){
	
}

void Drawable::Draw(float x, float y, float rotation){
	
}
void Drawable::AddTexture(Texture tex,int textureChannel){
	if(textureChannel >= 0 && textureChannel < MAXTEXTURES){
		boundTextures[textureChannel] = tex;		
	}
}

void Drawable::ActivateTextures(){
	for(int i = 0; i < MAXTEXTURES;i++){
		glActiveTexture(GL_TEXTURE0 + i);
		if(boundTextures[i].gltex != (unsigned int) -1 ){
			glBindTexture(GL_TEXTURE_2D, boundTextures[i].gltex);
			char buf[5];
			sprintf(buf,"tex%i",i);
			ActiveShader.SetUniform1i(i,buf);
			char buf2[11];
			sprintf(buf2,"tex%iActive",i);
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
int CreatedPolygons = 0;

Polygon::Polygon(){
	
	ref = new unsigned int;
	*ref = 1;
	CreatedPolygons++;
}
Polygon::Polygon(std::vector<float> vertexes, std::vector<unsigned int> indices, std::vector<float> colors, std::vector<float> uvs,unsigned int vertType,unsigned int uvType){
//std::cout << "Polygon constructed" << CreatedPolygons << "\n";
	Init(vertexes,indices,colors,uvs,vertType,uvType);
	vertexes.clear();
	indices.clear();
	colors.clear();
	uvs.clear();
	}

Polygon::Polygon(char * rawdata_verts,unsigned int lv,char* rawdata_indices, unsigned int li, char * rawdata_color, unsigned int lc, char* rawdata_uvs, unsigned int luv,unsigned int vertType,unsigned int uvType){
	Init(std::vector<float>((float*)rawdata_verts,((float*)rawdata_verts) + lv),std::vector<unsigned int>((unsigned int*)rawdata_indices,((unsigned int*)rawdata_indices)+li),std::vector<float>((float*)rawdata_color,((float*)rawdata_color)+lc),std::vector<float>((float*)rawdata_uvs,((float*)rawdata_uvs)+ luv),vertType,uvType);
	
	}
Polygon::~Polygon(){
	//std::cout << "Polygon destructed\n";
	*ref -=1;
	if(*ref == 0){
		delete ref;
		CreatedPolygons--;
		Unload();
	}
}

Polygon::Polygon(const Polygon& other){
	
	ref = other.ref;
	*ref += 1;
	indices = other.indices;
	usingColor = other.usingColor;
	usingUV = other.usingUV;
	vertVbo  = other.vertVbo;
	indiceVbo  = other.indiceVbo;
	colorVbo  = other.colorVbo;
	
	uvVbo  = other.uvVbo;
	uvLoadedSize  = other.uvLoadedSize;
	uvLoadedType  = other.uvLoadedType;
	drawType = other.drawType;
}

Polygon & Polygon::operator=(const Polygon& other){
	*ref -=1;
	if(*ref == 0){
		delete ref;
		CreatedPolygons--;
		Unload();
	}
	ref = other.ref;
	*ref += 1;
	usingColor = other.usingColor;
	usingUV = other.usingUV;
	vertVbo  = other.vertVbo;
	indices = other.indices;
	indiceVbo  = other.indiceVbo;
	if(usingColor){
		colorVbo  = other.colorVbo;
	}
	
	if(usingUV){
		uvVbo  = other.uvVbo;
		uvLoadedSize  = other.uvLoadedSize;
		uvLoadedType  = other.uvLoadedType;
	}
	drawType = other.drawType;
	return *this;
}

void Polygon::Init(std::vector<float> vertexes, std::vector<unsigned int> indices, std::vector<float> colors, std::vector<float> uvs,unsigned int vertType ,unsigned int uvType ){
	drawType = GL_QUADS;
	LoadVerts(vertexes,vertType);
	this->indices = indices;
	indiceVbo = GenVBO(&(this->indices[0]),indices.size()*sizeof(int),0,GL_ELEMENT_ARRAY_BUFFER_ARB);
	usingColor = false;
	if(colors.size() > 0){
			usingColor = true;
			colorVbo = GenVBO(&colors[0],colors.size()*sizeof(float),0,GL_ARRAY_BUFFER_ARB);
	}
	usingUV = false;
	if(uvs.size() > 0){
		LoadUV(uvs,uvType);
	}
	ref = new unsigned int;
	*ref = 1;
	CreatedPolygons++;
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

void Polygon::Unload(){
	glDeleteBuffers(1,&vertVbo);
	glDeleteBuffers(1,&indiceVbo);
	if(usingUV){
		glDeleteBuffers(1,&uvVbo);
	}
	if(usingColor){
		glDeleteBuffers(1,&colorVbo);
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
Text::Text(Texture FontTex,float fromx, float tox, float fromy, float toy, int lines, int charsPerLine, int textStart,float fontSize){
	this->fromx = fromx;
	this->tox = tox;
	this->fromy = fromy;
	this->toy = toy;
	this->lines = lines;
	this->charsPerLine = charsPerLine;
	this->textStart = textStart;
	FontSize = fontSize;
	float verts[] = {0,0,0,fontSize,fontSize,fontSize,fontSize,0};
	unsigned int indices[] = {0,1,2,3};
	float uvs[] = {0,1,0,0,1,0,1,1};
	Quad = Polygon(std::vector<float>(verts,verts + 8), std::vector<unsigned int>(indices, indices + 4),std::vector<float>(),std::vector<float>(uvs, uvs+8),0,2);
	Quad.AddTexture(FontTex,0);
}

void Text::SetText(std::string text){
	this->text = text;
}
void Text::Draw(float x, float y, float rotation){
	float charSizeX = (tox - fromx)/charsPerLine;
	float charSizeY = (toy - fromy)/lines;
	Vec savedZoomLv = currentZoomLevel;
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
		
		float x1 = col*charSizeX-0.005;
		float x2 = x1 + charSizeX; 
		float y1 = line*charSizeY+0.005;
		float y2 = y1 + charSizeY-0.007;;
		float newuvs[] ={x1,y2,x1,y1,x2,y1,x2,y2};
		Quad.ReloadUV(std::vector<float>(newuvs,newuvs+8));
		Draw2(x + column*0.9*FontSize,y-newlines*FontSize,0,&Quad);
		column +=1;
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
	delete []idata;
	
	}
Light * LightSystem::GetLight(int channel){
		return &lights[channel];
	}
void LightSystem::SetLight(Light light, int channel){
	lights[channel] = light;
	}


FrameBuffer::FrameBuffer(Texture outputTexture){
	tex = outputTexture;
	glGenFramebuffers(1,&fboId);
	Bind();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,tex.texdim, outputTexture.GetGLTexture(), 0);
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	
	UnBind();
}

void FrameBuffer::Bind(){
	glViewport(0,0,tex.width,tex.height);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
}
void FrameBuffer::UnBind(){
	glViewport(0,0,ScreenSize.X,ScreenSize.Y);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

Image FrameBuffer::GetBufferImage(){
	glReadBuffer(GL_FRONT);
	Image img(tex.width,tex.height,tex.Channels,1);
	
	glReadPixels(0,0,tex.width,tex.height,GL_ALPHA,GL_FLOAT,&(img.dataf[0]));
	
	return img;
}


FrameDoubleBuffer::FrameDoubleBuffer(Texture buffer1, Texture buffer2){
  buf0 = buffer1;
  buf1 = buffer2;
  currentBuffer = 0;
 
  glGenFramebuffers(1,&fboId);
  Bind();
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,buf0.texdim, buf0.GetGLTexture(),0);
  Unbind();
  glGenBuffers(1,&pbo);
  glBindBuffer(GL_PIXEL_PACK_BUFFER,pbo);
  glBufferData(GL_PIXEL_PACK_BUFFER,buf0.width*buf0.height*4*4,NULL,GL_STREAM_READ);
  glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
  
}

void FrameDoubleBuffer::SwapBuffers(){
  
  int tex;
if(currentBuffer == 1){
  currentBuffer = 0;
  tex = buf1.GetGLTexture();
 }else{
  currentBuffer = 1;
  tex = buf0.GetGLTexture();
 }
 Bind();
 glFramebufferTexture2DEXT(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, tex,0);
 Unbind();
}

void FrameDoubleBuffer::Bind(){
  glViewport(0,0,buf1.width,buf1.height);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
}
void FrameDoubleBuffer::Unbind(){
   glViewport(0,0,ScreenSize.X,ScreenSize.Y);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Image FrameDoubleBuffer::GetCurrentBufferImage(){
  glReadBuffer(GL_FRONT);
  

  Image img(buf1.width,buf1.height,4,1);
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB,pbo);  

 glReadPixels(0,0,buf1.width,buf1.height,buf1.channels,GL_FLOAT,&(img.dataf[0]));
  return img;
}

Texture FrameDoubleBuffer::GetBufferedTexture(){
	if(currentBuffer == 0){
		return buf1;
	}
	return buf0;
	
}

Texture FrameDoubleBuffer::GetActiveTexture(){
	if(currentBuffer == 0) {
		return buf0;
	}
	return buf1;
}

Image::Image( int width, int height, int channels, int dataType){
	 Width = width;
	 Height = height;
	 Channels = channels;
	 DataType = dataType;
	 dataf = std::vector<float>(width*height*channels);
}
float Image::At(int x, int y, int channel){
	
}

std::vector<float> Image::AsFloatVector(){
	return dataf;
}

float SumFloatVector(std::vector<float> fv,int offset,int step ){
	float out = 0;
	for(int i = offset; i < fv.size(); i+= step){
		out += fv[i];
	}
	return out; 
}

const char * GetFloatVectorPtr(std::vector<float> * vec){
	
	return (const char*) &((*vec)[0]);
}	

std::string GetFloatVectorStr(std::vector<float> * vec){
	
	return std::string((const char*) &((*vec)[0]),vec->size()*4);
}	

Texture3D::Texture3D(int width, int height, int depth, int type, char * data){
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, & tex);
	Width = width;
	Height = height;
	Depth = depth;
	glBindTexture(GL_TEXTURE_3D, tex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, Width, Height, Depth, 0, GL_RGBA,GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_3D);
}

void Texture3D::Bind(unsigned int loc){
	glBindTexture(GL_TEXTURE_3D, tex);
	char buf[7];
	sprintf(buf,"tex3D%i",loc);
	ActiveShader.SetUniform1i(loc,buf);	
}
