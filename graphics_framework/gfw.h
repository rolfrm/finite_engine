#include <list>
#include <string>
#include <vector>
/*class Vec2{
	public:
	Vec2(float x,float y);
};
class Vec3{
	public:
	Vec3(float x,float y, float z);
};*/


class Drawable{
	public:
	Drawable();
	virtual void Draw();
};

class DrawableTest: public Drawable{
		public:
		DrawableTest();
		void Draw();
		unsigned int testVBO;
};

class Polygon:public Drawable{
	public:
	Polygon(std::vector<float> vertexes, std::vector<int> indices, std::vector<float> colors, std::vector<float> uvs);
	Polygon(char * rawdata_verts,unsigned int lv,char* rawdata_indices, unsigned int li, char * rawdata_color, unsigned int lc, char* rawdata_uvs, unsigned int luv);
	
	
	void Draw();
	
	bool usingColor,usingUV;
	
	std::vector<float> vertexes;
	std::vector <unsigned int>  indices;
	std::vector<float> colors;
	std::vector<float> uvs;
	unsigned int vertVbo;
	unsigned int indiceVbo;
	unsigned int colorVbo;
	
	unsigned int uvVbo;
	
	private:
	void refreshVbos();
	
};

/*class Text:public Drawable{
	public:
	Text(std::string fontbase);
	void SetText(std::string text);
	
};

class Texture{
	public:
	Texture(std::string path);
};

class Framebuffer{
	public:
	Framebuffer();
	Texture GetAsTexture();
};*/

class Shader{
	public:
	Shader(const char * vertexSource, const char * fragmentSource);
	Shader();
	
	unsigned int GetUniformLocation(const char * uniformname);
	void SetUniform1f(float value,const char * uniformname);
	void SetUniform2f(float v1,float v2,const char * uniformname);
	void SetUniform3f(float v1, float v2, float v3,const char * uniformname);
	unsigned int ShaderProgram;
};

void Init(int width,int height, bool fullscreen);
void DeInit();
void Refresh();
void Draw(float x, float y,float rotation, Drawable * poly);
void PrintString(const char * str,int len);
//void Zoom(float x,float y);


void SetActiveShader(Shader);
//void SetActiveFramebuffer(Framebuffer*);
//void SetActiveTexture(Texture *);


