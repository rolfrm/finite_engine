typedef int Voxel;

class VoxelGrid{
	public:
	VoxelGrid(int sizex, int sizey, int sizez);
	Voxel * Grid;
	Voxel& At(int x,int y,int z);
	
	int SizeX,SizeY,SizeZ;
	
};
