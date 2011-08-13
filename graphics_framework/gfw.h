#include <list>
#include <string>

class Vec2{
	public:
	Vec2(float x,float y);
};
class Vec3{
	public:
	Vec3(float x,float y, float z);
};


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
	Polygon(std::list<Vec3> vertexes, std::list<int> indices, std::list<Vec3> colors, std::list<Vec2> uvs);
};

class Text:public Drawable{
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
};

class Shader{
	public:
	Shader(std::string vertexSource, std::string fragmentSource);
	Shader();
	
	unsigned int ShaderProgram;
};

void Init(int width,int height, bool fullscreen);
void DeInit();
void Refresh();
void Draw(float x, float y, Drawable * poly);
void Zoom(float x,float y);
class Shader;
class Framebuffer;

extern Shader DefaultShader;
extern Framebuffer DefaultFramebuffer;

void SetActiveShader(Shader);
void SetActiveFramebuffer(Framebuffer*);
void SetActiveTexture(Texture *);


