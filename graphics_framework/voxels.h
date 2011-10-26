#include<vector>
class Texture3D;
class Texture;
typedef int Voxel;
class VoxelGrid{
	public:
	VoxelGrid(int sizex, int sizey, int sizez);
	Voxel * Grid;
	int& At(int x,int y,int z);
	void Set(int x, int y, int z, int value);
	Texture3D * AsTexture3D();
	int SizeX,SizeY,SizeZ;
};

struct Ray{	
	//Inputs:
	float dx,dy,dz;
	float sx,sy,sz;
	
	//outputs:
	float x,y,z; //Traveled distance compared to LOD
	int color;
	int Iterations;
	char VoxelNormal; //0: x, 1: y, 2:z
	char LOD;
};

Ray MakeRay(float sx,float sy, float sz,float dx,float dy,float dz);
struct RayLight{
	float x,y,z;
	float r,g,b;
	float fade;
	char LOD;
};
RayLight MakeRayLight(float x,float y,float z,float r,float g,float b,float fade,char LOD);
struct Camera{
	float x,y,z;
	float rx,ry,rz;
	float aspect;
};

Camera MakeCamera(float x,float y,float z,float rx,float ry,float rz,float aspect);

class NTree;
class Scene{
	public:
	Scene(NTree * tree);
	void SetRayLight(RayLight light,unsigned int nr);
	void RemoveRayLight(unsigned int nr);
	void SetCamera(Camera cam);
	void SetTree(NTree * tree);
	void RenderToTexture(Texture * tex);
	NTree * GetStartNode(float &x,float &y, float &z);
	void SetAmbient(float r,float g,float b);
	static const int MaxLights = 5;
	std::vector<RayLight> Lights;
	Camera cam;
	NTree * tree;
	float AmbientB;
	float AmbientR;
	float AmbientG;
	bool DoLightning;	
};


class NTree{
	public:
	NTree();
	int At(float x, float y, float z);
	void Set(int x, int y, int z, int value);
	void Split(int N = 2);
	void SetData(int value);
	void UpdateTree();
	NTree * MakeParent();
	//void MakeLODAtPoint(float x,float y,float z, int LOD);
	void InsertPoint(float x,float y,float z, int LOD,int color);
	void MakeLODAtPoint(float x,float y,float z, int LOD);
	int GetData();
	NTree * GetChild(int x, int y, int z);
	NTree * GetChild(float x, float y, float z);
	NTree * GetChild(int n);
	
	Ray CastRay(Ray in);
	Voxel Data;
	int NChild;
	int Type;
	int nLights;
	NTree * Children;
	NTree * Parent;
	char LOD; //Only needs +-128 since this gives 2^128 precision
	
};

void RunTest(NTree *tree);
NTree GenTestNTree();
