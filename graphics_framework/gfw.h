#include <list>
#include <string>
#include <vector>
class Texture{
	public:
	Texture(char* data, int width, int height, int colorChannels, int inputSize);
	
	unsigned int GetGLTexture();
	private:
	float * data;
	int width;
	int height;
	int ColorChannels;
	unsigned int InputSize;
	unsigned int gltex;
	
};

class Drawable{
	public:
	Drawable();
	void ActivateTextures();
	virtual void Draw();
	void AddTexture(Texture * tex,int textureChannel);
	private:
	Texture* boundTextures[2];
		
};

class DrawableTest: public Drawable{
		public:
		DrawableTest();
		void Draw();
		
		unsigned int testVBO;
};
#define POLYGON GL_POLYGON
#define LINES GL_LINES
class Polygon:public Drawable{
	public:
	Polygon(std::vector<float> vertexes, std::vector<unsigned int> indices, std::vector<float> colors, std::vector<float> uvs,unsigned int vertType,unsigned int uvType);
	Polygon(char * rawdata_verts,unsigned int lv,char* rawdata_indices, unsigned int li, char * rawdata_color, unsigned int lc, char* rawdata_uvs, unsigned int luv,unsigned int vertType,unsigned int uvType);
	Polygon();
	
	void Draw();
	void SetDrawType(unsigned int);
	void LoadUV(std::vector<float> uvVector, int drawType);
	void ReloadUV(std::vector<float> uvVector,unsigned int offset = 0);
	void LoadVerts(std::vector<float> vertVector, int drawType);
	void ReloadVerts(std::vector<float> vertVector,unsigned int offset = 0);
	
	
	bool usingColor,usingUV;
	
	std::vector<float> vertexes;
	std::vector <unsigned int>  indices;
	std::vector<float> colors;
	std::vector<float> uvs;
	unsigned int vertVbo;
	unsigned int indiceVbo;
	unsigned int colorVbo;
	
	unsigned int uvVbo;
	unsigned int uvLoadedSize;
	unsigned int uvLoadedType;
	private:
	void refreshVbos();
	unsigned int drawType;
	
};

class Text:public Drawable{
	public:
	Text(Texture * FontTex,float fromx, float tox, float fromy, float toy, int lines, int charsPerLine, int textStart);
	void SetText(std::string text);
	void Draw();
	private:
	std::string text;
	float fromx;
	float tox;
	float fromy;
	float toy;
	int lines;
	int charsPerLine;
	int textStart;
	Polygon Quad;
};


/*
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
	void SetUniform1i(int value, const char * uniformname);
	void SetUniform1fv(float * data, unsigned int count, const char * uniformname);
	void SetUniform2fv(float * data,unsigned int count, const char * uniformname);
	void SetUniform3fv(float * data,unsigned int count, const char * uniformname);
	void SetUniform1iv(int * data, unsigned int count, const char * uniformname);
	
	unsigned int ShaderProgram;
};

struct KeyEvent{
	int key;
	char charKey;
	int action;
};

struct MouseEvent{
	int button;
	int action;
};

class Vec{
	public:
	Vec();
	Vec(float x,float y);
 float X;
 float Y;	
};

Vec ScreenToWorldCoordinates(Vec in);
Vec WorldToScreenCoordinates(Vec in);
void Init(int width,int height, bool fullscreen, int FSAASamples = 0);
void DeInit();
void Refresh();
void Draw(float x, float y,float rotation, Drawable * poly);
std::list<KeyEvent> GetKeyEvents();
std::list<MouseEvent> GetMouseEvents();
Vec GetMousePos();
void Zoom(float x,float y);

void SetActiveShader(Shader);
//void SetActiveFramebuffer(Framebuffer*);
class Light{
			public:
				Light(float X, float Y, float R, float G, float B, float intensity);
				float R,G,B;
				float X,Y;
				float intensity;
};

class LightSystem{
		private:
		std::vector<Light> lights;
		
		public:
		LightSystem(int nrChannels);
		void Activate();
		Light * GetLight(int channel);
		void SetLight(Light light, int channel);
};



