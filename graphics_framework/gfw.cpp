#include <GL/glew.h>
#include <GL/glfw.h>
#include <GL/gl.h>
#include "gfw.h"
#include <string>
#include <iostream>
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

void SetActiveShader(Shader s){
		glUseProgram(s.ShaderProgram);
}

Shader DefaultShader;
void Init(int width,int height, bool fullscreen){
	glfwInit();
	
	if(fullscreen){
		glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_FULLSCREEN);
	}else{
		glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_WINDOW);
	}
	glewInit() ;
	DefaultShader = Shader("attribute vec2 pos; \nvoid main(){\n gl_Position=vec4(pos,0,1);}","void main(){gl_FragColor= vec4(1,1,0,1);\n}");
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

void Draw(float x,float y, Drawable* drw){
	drw->Draw();
}

Shader::Shader(std::string vertexsrc, std::string fragmentsrc){
	const char* vsrc = vertexsrc.c_str();
	const char* fsrc = fragmentsrc.c_str();
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
		int posAttribLoc = glGetAttribLocation(DefaultShader.ShaderProgram,"pos");
		glEnableVertexAttribArray(posAttribLoc);
		
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, testVBO);
		glVertexAttribPointer(posAttribLoc,2,GL_FLOAT,0,0,0);
		
		glDrawArrays(GL_POINTS,0,3);
		glDisableVertexAttribArray(posAttribLoc);
}
