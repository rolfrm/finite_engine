#include "gfw.h"

typedef int Voxel;
class Texture3D;
class VoxelGrid{
	public:
	VoxelGrid(int sizex, int sizey, int sizez);
	Voxel * Grid;
	Voxel& At(int x,int y,int z);
	Texture3D AsTexture3D();
	int SizeX,SizeY,SizeZ;
	
	
};
