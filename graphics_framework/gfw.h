#include <list>
#include <string>
#include <vector>
#include <GL/gl.h>
#define MAXTEXTURES 2
class Texture{
	public:
	
	enum INTERPOLATION{
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		/*LINEAR_MIPMAP = GL_LINEAR_MIPMAP,
		NEAREST_MIPMAP = GL_NEAREST_MIPMAP*/
	};
	
  	Texture(char* data, int width, int height, int colorChannels, int inputSize,int IncrasedRange);
  	Texture(int width, int height, int colorChannels, int inputSize,int IncrasedRange); //2d tex
  	Texture(int width,  int colorChannels, int inputSize,int IncrasedRange); //1d tex
  	Texture(int width, int height, int depth, int colorChannels, int inputSize,int IncrasedRange);//3d tex
	Texture();
	Texture(const Texture& copy);
	Texture & operator=(const Texture & other);
	~Texture();
	unsigned int GetGLTexture();
	void LoadData(char * data,int width, int height, int depth);
	void GenMipmaps();
	void SetMagnifyingInterpolation(INTERPOLATION ipol);
	void SetMinifyingInterpolation(INTERPOLATION ipol);
	void UpdateTexture(char * data, int width, int height, int depth, int colorChannels, int inputSize,int IncreasedRange);
	unsigned int gltex;
	unsigned int texdim;
	unsigned int glCol;
	unsigned int intype;
	unsigned int Channels;
	int range;
	
	
	int *ref;
	int width;
	int height;
	int depth;
	int channels;
	int typesize;
	
};

class Drawable{
	public:
	Drawable();
	virtual ~Drawable();
	void ActivateTextures();
	virtual void Draw(float x, float y, float a);
	void AddTexture(Texture tex,int textureChannel);
	private:
	Texture boundTextures[2];
		
};

#define POLYGON GL_POLYGON
#define LINES GL_LINES
class Polygon:public Drawable{
	/*
	 * Do a kind of reference counting.
	 * */
	public:
	Polygon(std::vector<float> vertexes, std::vector<unsigned int> indices, std::vector<float> colors, std::vector<float> uvs,unsigned int vertType = 0,unsigned int uvType = 0);
	Polygon();
	~Polygon();
	Polygon(const Polygon& other);
	Polygon & operator=(const Polygon& other);
	
	void Draw(float x=0, float y=0, float rotation=0);
	void SetDrawType(unsigned int);
	void LoadUV(std::vector<float> uvVector, int drawType);
	void ReloadUV(std::vector<float> uvVector,unsigned int offset = 0);
	void LoadVerts(std::vector<float> vertVector, int drawType);
	void ReloadVerts(std::vector<float> vertVector,unsigned int offset = 0);
	void Unload();
	
	bool usingColor,usingUV;
	
	std::vector <unsigned int>  indices;
	unsigned int vertVbo;
	unsigned int indiceVbo;
	unsigned int colorVbo;
	
	unsigned int uvVbo;
	unsigned int uvLoadedSize;
	unsigned int uvLoadedType;
	private:
	void Init(std::vector<float> vertexes, std::vector<unsigned int> indices, std::vector<float> colors, std::vector<float> uvs,unsigned int vertType,unsigned int uvType);
	unsigned int drawType;
	unsigned int * ref;
	
};

class Text:public Drawable{
	public:
	Text(Texture FontTex,float fromx, float tox, float fromy, float toy, int lines, int charsPerLine, int textStart,float fontsize);
	void SetText(std::string text);
	void Draw(float x, float y, float rotation);
	private:
	std::string text;
	float fromx;
	float tox;
	float fromy;
	float toy;
	int lines;
	int charsPerLine;
	int textStart;
	float FontSize;
	Polygon Quad;
};

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
	void SetUniform3fv(std::vector<float> fvec,int count, const char * uniformname);
	void SetUniform2fv(std::vector<float> fvec,int count, const char * uniformname);
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
void ClearBuffer();
void Draw(float x, float y,float rotation, Drawable * poly);
std::list<KeyEvent> GetKeyEvents();
std::list<MouseEvent> GetMouseEvents();
Vec GetMousePos();
void Zoom(float x,float y);
void SetBGColor(float r, float g, float b);
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

class Image{
	public:
	Image(int width, int height, int channels, int dataType);
	int Width, Height,Channels,DataType;
	float At(int x, int y, int channel);
	std::vector<float> AsFloatVector();
	std::vector<float> dataf;
};

class FrameBuffer{
	public:
	FrameBuffer(Texture outputTexture);
	void Bind();
	void UnBind();
	Image GetBufferImage();
	Texture tex;
	unsigned int fboId;
};

class FrameDoubleBuffer{
 public:
  FrameDoubleBuffer(Texture read, Texture write);
  void SwapBuffers();
  Image GetCurrentWriteBufferImage();
  void Bind();
  void Unbind();
  Texture GetReadTexture();
  Texture GetWriteTexture();
 private:
  int currentBuffer;
  Texture readBuffer;
  Texture writeBuffer;
  unsigned int fboId;
  unsigned int pbo;

};

class Texture3D{ //Deprecated
	public:
	Texture3D(int width, int height, int depth, int type, char * data);
	int Width;
	int Height;
	int Depth;
	unsigned int tex;
	void Bind(unsigned int loc);
};


float SumFloatVector(std::vector<float>,int offset ,int step);
const char * GetFloatVectorPtr(std::vector<float> * vec);
std::string GetFloatVectorStr(std::vector<float> * vec);
