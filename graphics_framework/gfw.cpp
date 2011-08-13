#include <GL/glew.h>
#include <GL/glfw.h>
#include <GL/gl.h>
#include "gfw.h"
#include <string>
#include <iostream>
#include <string.h>
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
Shader ActiveShader;
void SetActiveShader(Shader s){
		ActiveShader = s;
		glUseProgram(s.ShaderProgram);
}


void Init(int width,int height, bool fullscreen){
	glfwInit();
	
	if(fullscreen){
		glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_FULLSCREEN);
	}else{
		glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_WINDOW);
	}
	glewInit() ;
	SetActiveShader(Shader("attribute vec2 pos; \nvoid main(){\n gl_Position=vec4(pos,0,1);}","void main(){gl_FragColor= vec4(1,1,0,1);\n}"));
	glPointSize(3);
	glColor4f(1,1,1,1);
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
unsigned int Shader::GetUniformLocation(const char * uniformname){
	return glGetUniformLocation(ShaderProgram,uniformname);
	}
Drawable::Drawable(){
	
	}
void Drawable::Draw(){
	
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
	std::cout << vertexes.size() << "\n";
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
	glEnableVertexAttribArray(posAttribLoc);
	std::cout << colors.size() << " " << indices.size() << " " << vertexes.size() << "\n";
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertVbo);
	glVertexAttribPointer(posAttribLoc,2,GL_FLOAT,0,0,NULL);
	if(usingColor){
		glEnableVertexAttribArray(colorAttribLoc);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertVbo);
		glVertexAttribPointer(colorAttribLoc,3,GL_FLOAT,0,0,NULL);
	}
	
	
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indiceVbo);	
	glDrawElements(GL_TRIANGLE_STRIP,indices.size(),GL_UNSIGNED_INT,0);			
	//glDrawArrays(GL_POLYGON,0, vertexes.size()/2);
	glDisableVertexAttribArray(posAttribLoc);
	if(usingColor){
		glDisableVertexAttribArray(colorAttribLoc);
	}
	}
void PrintString(const char * str,int len){
	float * fstr = (float *)str;
	for(int i=0; i < len;i++){
		std::cout << (float) fstr[i] << " " ;
		}
	std::cout << "\n";
	}