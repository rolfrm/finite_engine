#include "voxels.h"
#include <iostream>
VoxelGrid::VoxelGrid(int sizex,int sizey,int sizez){
	Grid = new Voxel[sizex*sizey*sizez];
	SizeX = sizex;
	SizeY = sizey;
	SizeZ = sizez;
}	

Voxel& VoxelGrid::At(int x, int y, int z){
	return Grid[x + y*SizeX + z*SizeY*SizeX];
}



int main(){
	VoxelGrid a(10,10,10);
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++){
			for( int k = 0; k < 10; k++){
			a.At(i,j,k) = i*j*k;
			}
		}
	}
	std::cout << a.At(9,9,9) << "\n";
}
	
