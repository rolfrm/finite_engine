#include "voxels.h"
#include <iostream>
#include "gfw.h"
#include <math.h>
#include <GL/gl.h>
#include <vector>
VoxelGrid::VoxelGrid(int sizex,int sizey,int sizez){
  Grid = new Voxel[sizex*sizey*sizez];
  SizeX = sizex;
  SizeY = sizey;
  SizeZ = sizez;
  for(int x = 0; x < sizex;x++){
    for(int y = 0; y < sizey;y++){
      for(int z = 0; z < sizez;z++){
	if(x == 0 || x == sizex-1){
	  Grid[x + y*SizeX + z*SizeY*SizeX] = 0xFFFFFFFF;
	}else if(z == sizez - 1){
	  Grid[x + y*SizeX + z*SizeY*SizeX] = 0xFFFFFFFF;
	}else if(y == 0|| y == SizeY -1){
	  Grid[x + y*SizeX + z*SizeY*SizeX] = 0xFFFFFFFF;
	}else{
	  Grid[x + y*SizeX + z*SizeY*SizeX] = 0;
	}
      }
    }	
  }
}	

int& VoxelGrid::At(int x, int y, int z){
  return Grid[x + y*SizeX + z*SizeY*SizeX];
}

Texture3D *VoxelGrid::AsTexture3D(){
  return new Texture3D(SizeX,SizeY,SizeZ,0,(char *) Grid);
}

void VoxelGrid::Set(int x, int y, int z, int value){
  At(x,y,z) = value;
}

NTree::NTree(){
  Children = NULL;
  Parent = NULL;
  Data = 0;
  LOD = 0;
  Type = 0;
  NChild = 0;
	
}

void NTree::Split(int N){
  Children = new NTree[N*N*N];
  for(int i = 0; i < N*N*N; i++){
    Children[i].Data = Data;
    Children[i].Parent = this;
    Children[i].LOD =LOD - 1;
    Children[i].NChild = i;
  }
}
 
int NTree::At(float x, float y, float z){
  int cellx = 2*x;
  int celly = y*2;
  int cellz = z*2;
  if(Children == NULL){
    return Data; 
  }else{
		
    return Children[cellx + celly*2 + cellz*2*2].At(x*2-(float)cellx, y*2-(float)celly, z*2-(float)cellz);
  }
}

NTree * NTree::GetChild(int x, int y, int z){
  return Children+(x + y*2 + z*4);
}

NTree * NTree::GetChild(float x, float y, float z){
  int cellx = 2*x;
  int celly = y*2;
  int cellz = z*2;
  return Children+cellx + celly*2 + cellz*4;
}

void NTree::Set(int x, int y, int z, int value){
	
}

void NTree::SetData(int value){
  Data = value;
}

int NTree::GetData(){
  return Data;
}

NTree * NTree::GetChild(int n){
  if(n < 8){
    return Children + n;
  }
  return Children;
}


#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
inline float sign(float f){
  if (f < 0) return -1;
  return 1;
}

	

Ray NTree::CastRay(Ray ray){
  NTree * currentTree = this;
  std::vector<Ray> rays;
  rays.reserve(8);
  int whileit = 0;
  do{
    if((ray.sx <= 0 && ray.dx < 0) || (ray.sx >= 1 && ray.dx > 0) || (ray.sy <= 0 && ray.dy < 0)  || (ray.sy >=1 && ray.dy > 0) || (ray.sz <= 0 && ray.dz < 0) || (ray.sz >= 1 && ray.dz > 0)){ //Check boundaries
      if( currentTree->Parent == NULL){
	break;
      }
			
      if(rays.size() == 0){
	//Cast to parent
	//std::cout << "Casting up, from:" << ray.sx << " " << ray.sy << " " << ray.sz << "\n";
	//std::cout << "Cell: " << NChild << "\n";
	ray.sx =  (currentTree->NChild & 1)*0.5 + ray.sx/2;
	ray.sy = ((currentTree->NChild & 2) >> 1)*0.5 + ray.sy/2;
	ray.sz = ((currentTree->NChild & 4) >> 2)*0.5  + ray.sz/2;
	/*
	  int cellx = ray.sx*2;
	  int celly = ray.sy*2;
	  int cellz = ray.sz*2;
			
	  float xt = ray.dx == 0?100000 :(((float)cellx+(ray.dx > 0? 1:0))/2-ray.sx)/ray.dx;
	  float yt = ray.dy == 0?100000 :(((float)celly+(ray.dy > 0? 1:0))/2-ray.sy)/ray.dy;
	  float zt = ray.dz == 0?100000 :(((float)cellz+(ray.dz > 0? 1:0))/2-ray.sz)/ray.dz;
		
	  float dmax = MIN(xt,MIN(yt,zt))*1.1;
				
	  ray.sx = ray.sx + ray.dx*dmax;
	  ray.sy = ray.sy + ray.dy*dmax;
	  ray.sz = ray.sz + ray.dz*dmax;
	*/
	//std::cout << "Casting up, to:" << ray.sx << " " << ray.sy << " " << ray.sz << "\n";
	currentTree = currentTree->Parent;
	continue;
      }
			
      ray = rays.back();
      rays.pop_back();
      currentTree = currentTree->Parent;
      continue;
			
    }
	
		
    int cellx = ray.sx*2;
    int celly = ray.sy*2;
    int cellz = ray.sz*2;
		
    float xt = ray.dx == 0?100000 :(((float)cellx+(ray.dx > 0? 1:0))/2-ray.sx)/ray.dx;
    float yt = ray.dy == 0?100000 :(((float)celly+(ray.dy > 0? 1:0))/2-ray.sy)/ray.dy;
    float zt = ray.dz == 0?100000 :(((float)cellz+(ray.dz > 0? 1:0))/2-ray.sz)/ray.dz;
		
    float dmax = MIN(xt,MIN(yt,zt))*1.1;
    Ray nextray = ray;
    nextray.sx = ray.sx + ray.dx*dmax;
    nextray.sy = ray.sy + ray.dy*dmax;
    nextray.sz = ray.sz + ray.dz*dmax;
		
    if(currentTree->Children != NULL){
      rays.push_back(nextray);
      currentTree = currentTree->Children + cellx +2*celly + cellz*4;
      nextray.sx = ray.sx*2 - cellx; //Transform coordinates to local
      nextray.sy = ray.sy*2 - celly;
      nextray.sz = ray.sz*2 - cellz;
      float scaling = 1 << ray.Iterations;
      nextray.x = ray.x + floor(ray.sx*2)/2/scaling;
      nextray.y = ray.y + floor(ray.sy*2)/2/scaling;
      nextray.z = ray.z + floor(ray.sz*2)/2/scaling;
      nextray.Iterations = ray.Iterations + 1;
				
    }else if(currentTree->Data !=0){
      ray.color = currentTree->Data;
      float scaling = 1 << ray.Iterations;
      ray.x = ray.x + floor(ray.sx*2)/2/scaling;
      ray.y = ray.y + floor(ray.sy*2)/2/scaling;
      ray.z = ray.z + floor(ray.sz*2)/2/scaling;
      float closest = 1;
      char normal = 0;
			
      ray.sx < 1 - ray.sx ? ray.sx : 1 - ray.sx;
			
      if(ray.sx < closest){
	closest = ray.sx;
	normal = 0;
      }
			
      if(1 - ray.sx < closest){
	closest = 1 - ray.sx;
	normal = 1;
      }
			
      if(ray.sy < closest){
	closest = ray.sy;
	normal = 2;
      }
			
      if(1 - ray.sy < closest){
	closest = 1 - ray.sy;
	normal = 3;
      }
			
      if( ray.sz < closest){
	normal = 4;
	closest = ray.sz;
      }
			
      if(1-ray.sz < closest) {
	normal = 5;
      }
			
      ray.VoxelNormal = normal;
      return ray;
    };
    ray = nextray;
  }while( whileit++ < 200);
	
  //std::cout << " " << whileit << "\n";
	
  return ray;
	
}
	
	
	
struct _color{
  unsigned char r,g,b,a;
};

void RotateVector(float *x, float *y, float *z, float rx, float ry,float rz){
  float sx = *x;
  float sy = *y;
  float sz = *z;
 
  //*x = sx*cos(rz) - sy*sin(rz) + sz*sin(ry) + sx*cos(ry);;
 
  *x = sx*cos(rz) - sy*sin(rz);
  *y = sx*sin(rz) + sy*cos(rz);
  *z = sz;
  
  sx = *x;
  sy = *y;
  sz = *z;
  *y = sy*cos(rx) - sz*sin(rx);
  *z = sy*sin(rx) + sz*cos(rx);
 
  sx = *x;
  sy = *y;
  sz = *z;
 
  *z = sz*cos(ry) - sx*sin(ry);
  *x = sz*sin(ry) + sx*cos(ry);
 
 
 
}


void NTree::InsertPoint(float x,float y,float z, int LOD,int color){
  int clod = 0;
  NTree * curcell = this;
  while(clod < LOD){
	  
	  
    if(curcell->Children == NULL){
      curcell->Split(2);
    }
	   
    int cellx = x*2;
    int celly = y*2;
    int cellz = z*2;
    x = x*2 - cellx;
    y = y*2 - celly;
    z = z*2 - cellz;
    curcell = curcell->Children + cellx + celly*2 + cellz*4;
	 
    clod +=1;
  }
  curcell->Data = color;
}

void NTree::MakeLODAtPoint(float x,float y,float z, int LOD){
  int clod = 0;
  NTree * curcell = this;
  while(clod < LOD){
    int cellx = x/2;
    int celly = y/2;
    int cellz = z/2;
    if(curcell->Children == NULL){
      curcell->Split(2);
    }
	  
    curcell = curcell->Children + cellx + celly*2 + cellz*4;
    clod +=1;
  }
}

NTree * NTree::MakeParent(){
  if(Parent != NULL){
    return Parent;
  }
  NTree *  output = new NTree;
  output->LOD = LOD +1;
  output->Split();
  Parent = output;
  *(output->Children + NChild) = *this;
  return output;
	
}

void NTree::UpdateTree(){
  if(Children == NULL){
    return;
  }
  int mean = 0;
  for(unsigned int i= 0; i < 8;i++){
    Children[i].UpdateTree();
    mean += Children[i].Data;
  }
  mean /=4;
  Data = mean;
}


NTree GenTestNTree(){
  NTree output;
  output.Type = 1;
  output.Split();
  return output;
}

void RunTest(NTree *tree){
  Ray ray;
  int col;
  for(float i = 0; i < 1000;i++){
    for(float j = 0; j < 1000; j++){
      float x = ((float) j-1000/2)/1000 ;
      float y = ((float)1000-i-1000/2)/1000 ;
      float z = 0.5;
			
      float l = sqrt(x*x+y*y+z*z);
      x = x/l;
      y = y/l;
      z = z/l;
			
      float nx = cos(0)*x +sin(0)*z;
      float ny = y;
      float nz = -sin(0)*x + cos(0)*z;
			
      ray = MakeRay(0,0,0,nx,ny,nz);
      //col = tree->CastRay(-0.5,0,-0.5,ray.dx,ray.dy,ray.dz,&ray);
			
    }
  }
}

RayLight MakeRayLight(float x,float y,float z,float r,float g,float b,float fade,char LOD){
  RayLight out;
  out.x = x;
  out.y = y;
  out.z = z;
  out.r = r;
  out.g = g;
  out.b = b;
  out.fade = fade;
  out.LOD = LOD;
  return out;

}

Scene::Scene(NTree * tree){
  for(int i = 0; i < MaxLights;i++){
    Lights.push_back(MakeRayLight(0,0,0,0,0,0,0,0));
  }
  this->tree = tree;
  AmbientR = 0;
  AmbientG = 0;
  AmbientB = 0;
  DoLightning = true;
	
}
void Scene::SetRayLight(RayLight light,unsigned int nr){
  Lights[nr] = light;
}
void Scene::RemoveRayLight(unsigned int nr){
  Lights[nr] = MakeRayLight(0,0,0,0,0,0,0,0);
}
void Scene::SetCamera(Camera cam){
  this->cam = cam;
}
void Scene::SetTree(NTree * tree){
  this->tree = tree;
}


NTree* Scene::GetStartNode(float &x,float &y, float &z){
  NTree * TreeIterator = tree;
  if(TreeIterator->Children == NULL){
    return TreeIterator;
  }  
  while(true){
    NTree * nextLv = TreeIterator->GetChild(x,y,z);
    if(nextLv->Data == 0 || nextLv->Children == NULL){
      std::cout << "Breaking off \n";
      break;
    }
    std::cout << "Going one down" << x << " " << y << " " << z << "\n";
    TreeIterator = nextLv;
    x = x*2 - floor(x*2);
    y = y*2 - floor(y*2);
    z = z*2 - floor(z*2);
  }
  return TreeIterator;


}

void Scene::RenderToTexture(Texture * tex){
  int width = tex == NULL? 512 : tex->width;
  int height = tex == NULL?512 : tex->height;
  int * buffer = new int[width*height];
  NTree * renderTree = 	tree;//GetStartNode(cam.x,cam.y,cam.z);
  #pragma omp parallel for
  for(int i = 0; i < width; i++){
	
    float nzz = 1*1;
    int halfWidth = width >> 2;
    int halfHeight = height >> 2;
	
    float px;
    float py;
    float pz;
    float scale,nscale;
    float x,y,z,l;
    float dx,dy,dz,raydistance;
    _color * col;
    Ray ray;
    Ray sray;
    float nx = ((float)(i-halfWidth))/width;
    float nxx = nx*nx;
    for(int j = 0; j < width;j++){
		
      px = cam.x;
      py = cam.y;
      pz = cam.z;
		
      x = nx;
      y = ((float)(j-halfHeight))/height;
      z = 0.5;
      l = sqrt(nxx+y*y+nzz);
      x = x/l;
      y = y/l;
      z = z/l;
      RotateVector(&x,&y,&z,cam.rx,cam.ry,cam.rz);
      scale = 0;
      if( z > 0 && pz < 0 ){
	scale = (0-pz)/z;
      }else if( z < 0 && pz > 1 ){
	scale = (1-pz)/z;
      }
			
      if(y > 0 && py < 0){
	nscale = -py/y;
	if(nscale > scale){
	  scale = nscale;
	}
      }else if(y < 0 && py > 1){
	nscale = (1-py)/y;
	if(nscale > scale){
	  scale = nscale;
	}
      }
			
      if(x > 0 && px < 0){
	nscale = -px/x;
	if(nscale > scale){
	  scale = nscale;
	}
      }else if(x < 0 && px > 1){
	nscale = (1-px)/x;
	if(nscale > scale){
	  scale = nscale;
	}
      }
      px += x*scale;
      py += y*scale;
      pz += z*scale;
      if(px < 0 || px > 1 || pz < 0 || pz > 1 || py < 0 || py > 1){
	continue;
      }
      ray = MakeRay(px,py,pz,x,y,z);
      ray = renderTree->CastRay(ray);
      if(ray.color == 0){
	buffer[i + j*width] = 0;
	continue;
      }
      float r= AmbientR,g= AmbientG,b = AmbientB;
			
      if(DoLightning == false){
	col = (_color *) &(ray.color);
	col->r = col->r*(MIN(1,r/(1+ray.z))) ;
	col->g = col->g*(MIN(1,g/(1+ray.z)));
	col->b = col->b*(MIN(1,b/(1+ray.z)));
	col->a = 255;
	buffer[i + j*width] = ray.color;
	continue;
      }
			
      for(int k = 0; k < Lights.size();k++){	
	RayLight light = Lights[k];
	if(light.fade == 0){
	  continue;
	}
	Ray sray = MakeRay(ray.x + 0.001,ray.y+0.01,ray.z+0.001,  light.x - ray.x , light.y  - ray.y ,light.z  - ray.z);
				
	sray = tree->CastRay(sray);
	dx = sray.dx;
	dy = sray.dy;
	dz = sray.dz;
				
	raydistance = sqrt(dx*dx + dy*dy + dz*dz);
	dx /= raydistance;
	dy /= raydistance;
	dz /= raydistance;
				
	float falloff = raydistance*raydistance/light.fade;
	if(sray.color == 0){
	  float n;
	  switch (ray.VoxelNormal){
	  case 0: n = -dx; break;
	  case 1: n = dx;break;
	  case 2: n = -dy;break;
	  case 3: n = dy;break;
	  case 4: n = -dz;break;
	  case 5: n = dz;break;
	  }
	  if(n > 0){
	    r += light.r/(falloff)*n;
	    g += light.g/(falloff)*n;
	    b += light.b/(falloff)*n;
	  }
					
					
	}
			
      }
			
      col = (_color *) &(ray.color);
      col->r = col->r*(MIN(1,r)) ;
      col->g = col->g*(MIN(1,g));
      col->b = col->b*(MIN(1,b));
      col->a = 255;
      buffer[i + j*width] = (ray.color);
    }		
  }
#pragma omp barrier
  if(tex != NULL){
    glBindTexture(GL_TEXTURE_2D, tex->gltex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,(void*) buffer);
  }
	
  delete  []buffer;
}

void Scene::SetAmbient(float r,float g,float b){
  AmbientB = b;
  AmbientR = r;
  AmbientG = g;	
}
	
	
	
Ray MakeRay(float sx,float sy, float sz,float dx,float dy,float dz){
  Ray out;
  out.sx = sx;
  out.sy = sy;
  out.sz = sz;
  out.dx =dx;
  out.dy = dy;
  out.dz =dz;
  out.x = 0;
  out.y = 0;
  out.z = 0;
  out.color = 0;
  out.Iterations = 0;
  out.LOD = 0;
  out.VoxelNormal = 0;
  return out;
}

Camera MakeCamera(float x,float y,float z,float rx,float ry,float rz,float aspect){
  Camera out;
  out.x = x;
  out.y = y;
  out.z = z;
  out.rx = rx;
  out.ry = ry;
  out.rz = rz;
  out.aspect = aspect;
  return out;
}

