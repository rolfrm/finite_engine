import numpy
import math
import random
import gfw
import shaders
import utils
import time
import Image
import math
from physics import Vec2
from core import GameObject
from core import LevelBase

from copy import deepcopy
def NNInterpolate(x1,x2):
	xi = x1
	output = []
	def step(x1,x2):
		d = math.fabs(x1-x2)
		if d > 0:
			return (x1-x2)/d
		return 0 
	dx = [100,100,100]	
	while sum(map(math.fabs,dx)) != 0:
		output.append(xi)
		dx = map(step,x2,xi)
		xi = map(lambda x,y: x+y, dx,xi)
		
	return output

def MakeBlock(startVec, stopVec,depth = 1000):
	
	print startVec.x,startVec.y,stopVec.x,stopVec.y
	poly = utils.MakeCompleteObject([startVec.x, startVec.y, stopVec.x, stopVec.y,stopVec.x,stopVec.y-depth,startVec.x,startVec.y-depth])
	poly.Visual.SetDrawType(0x0007)
	return poly
	
def MakeBlock2(points,depth = 5000):
	outverts = []
	for i in points:
		outverts.extend([i.x,i.y,i.x,i.y-depth])
	poly = utils.MakePolygon(outverts,[],[],map(lambda x: -x/10,outverts))
	poly.SetDrawType(0x0005)
	return GameObject(poly)
		
	
def MakeChunk(places,interpolator = 0,objectInserter = 0,blockHandler = 0):
	if interpolator is not 0:
		places = interpolator(places)
	
	blocks = map(MakeBlock,places[0:-1],places[1:])
	#blocks = [MakeBlock2(places)]
	
	if blockHandler is not 0:
		blockHandler(blocks)
	
	if objectInserter:
		nobj = objectInserter(places)
		blocks.extend(nobj)
	return blocks
	

def InterPolataur(points):
	out = []
	iterations = 6
	for i in range(0,iterations):
		out = []
		for i in range(0,len(points)-1):
			cur = points[i]
			nxt = points[i+1]
			mid = (cur + nxt)/2
			mid.y += random.gauss(0,((cur.y-nxt.y)**2+(cur.x-nxt.x)**2)**0.5/8)
			out.append(points[i])
			out.append(mid)
		out.append(points[-1])
		
		points = out[:]	
		
	return out

def GetObjPosition(surfList,x):
	x = x
	y = 0
	for i in range(0,len(surfList)-1):
			v1 = surfList[i]
			v2 = surfList[i+1]
			if v1.x <= x and v2.x >= x:
				scaling = (x - v1.x)/(v2.x-v1.x)
				y =  v1.y + (v2.y-v1.y)*scaling
				break
	return x,y
	

	
	
if __name__ =="__main__":
	gfw.Init(800,600,False)
	s1 = gfw.Shader(shaders.ObjectLightning[0],shaders.ObjectLightning[1])
	gfw.SetActiveShader(s1);
	gfw.Zoom(400,400)
	treetex = utils.LoadImageAsTexture("tree.png")
	tex = utils.LoadImageAsTexture("grasstex2.png")
	tex2 = utils.LoadImageAsTexture("rock.png")
	def BlockHandler(blocks):
		for i in blocks:
			i.Visual.AddTexture(tex,0)
	def BlockHandler2(blocks):
		for i in blocks:
			i.Visual.AddTexture(tex2,0)
	def MakeTree(x,y):
		tree = GameObject(utils.MakePolygon([-16,0,-16,64,16,64,16,0],[],[],[0,1, 0,0, 1,0,1,1]),0,x,y)
		tree.Visual.AddTexture(treetex,0)
		return tree
	def MakeWater(t):
		wtex = utils.LoadImageAsTexture("water.png")
		verts = [-800.0,-800.0,-800.0,-50.0,800.0,-50.0,800.0,-800.0]
		water = GameObject(utils.MakePolygon(verts,[],[],map(lambda x: -x*0.01-t,verts)),0,0,0)
		water.Visual.AddTexture(wtex,0)
		return water
	def Extras(surfList):
		out = []
		for i in range(int(surfList[0].x),int(surfList[-1].x),10):
			if random.random() < 0.6:
				continue
			x = i
			x,y = GetObjPosition(surfList,x)
			out.append(MakeTree(x,y))
		return out
	
	gfw.SetBGColor(0.5,0.5,1)
	t = 0
	while True:
		t +=0.1
		try:
			MakeWater(t).Draw()
			for i in MakeChunk([Vec2(-400,-100),Vec2(-200,-100),Vec2(0,-100),Vec2(200,-100),Vec2(400,-100)],InterPolataur,Extras,BlockHandler):
				i.Draw()
			
			for i in MakeChunk([Vec2(-400,200),Vec2(-200,-200),Vec2(0,-250),Vec2(200,-200),Vec2(400,200)],InterPolataur,Extras,BlockHandler):
				i.Draw()
			for i in MakeChunk([Vec2(-400,200),Vec2(-200,-200),Vec2(0,-250),Vec2(200,-200),Vec2(400,200)],InterPolataur,0,BlockHandler2):
				i.Draw()
			gfw.Refresh()
			time.sleep(0.2)
		except KeyboardInterrupt as ke:
			break

