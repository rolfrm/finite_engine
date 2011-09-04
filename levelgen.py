import numpy
import math
import random
import gfw
import shaders
import utils
import time
from game_object import GameObject
def MakeSurfLevel(constraints,iterations ):	
	for i in constraints:
		i[0] = float(i[0])
		i[1] = float(i[1])
	#1. interpolate constraints
	
	for i in range(0,iterations):
		newconstraints = []
		for i in range(0,len(constraints)-1):
			o1 = constraints[i]
			
			o2 = constraints[i+1]
			dx = o1[0] - o2[0]
			dy = o1[1] - o2[1]
			d = (dx**2+dy**2)**0.5
			
			mid = [(o1[0] + o2[0])/2 + random.gauss(0,d/10),(o1[1] + o2[1])/2 + random.gauss(0,d/10)]
			newconstraints.append(o1)
			t = o1[2]
			
			if random.random() < 0.5:
				t = o2[2]
			newconstraints.append([mid[0],mid[1],t,[]])
		newconstraints.append(constraints[-1])
		constraints = newconstraints
	return constraints



import Image
import math

class MapCell:
	def __init__(self,x,y):
		self.x = x
		self.y = y

def MakeChunk(c1,c2,featureList):
		pass
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

def MakeMSquares(scale):
	sx = scale[0]/2
	sy = scale[1]/2
	simples = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
	quad = utils.MakePolygon([-sx,-sy, -sx,sy,sx,sy,sx,-sy],[],[],[0,1, 0,0, 1,0, 1,1])
	simples[15] = quad
	
	q1 = utils.MakePolygon([-sx,-sy,-sx,0,0,-sy],[],[],[1,1, 1,1, 1,1])
	q1.SetDrawType(0x0005)
	q3 = utils.MakePolygon([-sx,-sy,-sx,0,sx,0,sx,-sy],[],[],[-0.5,-0.5, -0.5,0, 0.5,0, 0.5,-0.5])
	q3.SetDrawType(0x0007)
	
	q4 = utils.MakePolygon([sx,0, sx,sy, 0,sy],[], [],[0,0.5,0.5,0.5, 0.5,0])
	q4.SetDrawType(0x0005)
	
	q6 = utils.MakePolygon([sx,-sy, sx,sy,0,-sy,0,sy],[],[],[0,0,0,0,0,0])
	q6.SetDrawType(0x0005)
	
	q9 = utils.MakePolygon([ -sx,sy,-sx,-sy, 0,sy, 0,-sy],[],[],[-0.5,-0.5,-0.5,0.5,0,0.5,0,-0.5])
	q9.SetDrawType(0x0005)
	
	q11 = utils.MakePolygon([-sx,sy,-sx,-sy, 0,sy, sx,-sy, sx,0],[],[],[-0.5,-0.5,-0.5,0.5, 0.5,0.5, 0.5,0.5 ,0.5,0.5])
	q11.SetDrawType(0x0005)
	
	q12 = utils.MakePolygon([-sx,0,-sx,sy,sx,sy,sx,0],[],[],[-0.5,-0.5, -0.5,0, 0.5,0, 0.5,-0.5])
	q12.SetDrawType(0x0007)
	
	q14 = utils.MakePolygon([sx,sy, -sx,sy, sx,-sy, -sx,0, 0,-sy ],[],[],[-0.5,0, -0.5,0.5,0,-0.5, 0.5,0.5, 0.5,-0.5])
	q14.SetDrawType(0x0005)
	
	simples[1] = q1
	simples[3] = q3
	simples[4] = q4
	simples[6] = q6
	simples[9] = q9
	simples[11] = q11
	simples[12] = q12
	simples[14] = q14
		
	def marchingSquares(index):
		out = simples[index]
		if out == 0:
			print "not impl:",index
			return simples[15]
		return out
		
	
	return marchingSquares

from copy import deepcopy
class Chunk:#Test chunk generator, inputs constraints, outputs game object list
	def __init__(self,c1,c2, preProcess = 0,postProcess= 0):
		self.c1 = c1
		self.c2 = c2
		self.PostProcess = postProcess
		self.PreProcess = preProcess
	def Make(self):
		def minMax(x):
			return min(x),max(x)
		points = map(lambda x: map(float,x),NNInterpolate(self.c1,self.c2)) #Make float matrix to int matrix
		xex = minMax(map(lambda x: x[0],points)) #Extremes
		yex = minMax(map(lambda x: x[1],points))
		
		xyrange = [xex[1]-xex[0]+1,yex[1]-yex[0]+1]
		scale = 10
		valueScaling = [scale/(xyrange[0]),scale/(xyrange[1])]
		pointsInImage = map( lambda x: map(int,[(x[0]-xex[0])*valueScaling[0],(x[1]-yex[0])*valueScaling[1]]), points)
		
		
		ipoints = map( lambda x,y: NNInterpolate(x,y), pointsInImage[0:-1],pointsInImage[1:])
		
		npoints = []
		for i in ipoints:
			npoints.extend(i)
		img = numpy.zeros((scale,scale),dtype=numpy.int)
		
		for i in npoints:
			img[i[0],i[1]] = 1
		if self.PreProcess is not 0:
			self.PreProcess(img)
		objsize = (xyrange[0]/scale,xyrange[1]/scale)
		msq = MakeMSquares((objsize[0],objsize[1]))
		tiles = []
		for i in range(0,scale-1):
			for j in range(0,scale-1):
				 msqIndex = img[i,j] + img[i+1,j]*2 + img[i+1,j+1]*4 + img[i,j+1]*8
				 #if msqIndex > 0:
				 #print "Cell:", msqIndex
				 #print img[i,j+1],img[i+1,j+1]
				 #print img[i,j],img[i+1,j]
				 #print
				 if msqIndex == 0:
					 continue
				 ngo = GameObject(gfw.Polygon(msq(msqIndex)),0,i*objsize[0]+xex[0] - 150,j*objsize[1] + yex[0] )
				 tiles.append(ngo)
		
		if self.PostProcess is not 0:
			self.PostProcess(tiles)
		return tiles


def cave(nodes,index):
	obj = nodes[index]
	#obj[1][1] -= 100
	k1 = index-1
	k2 = index+1
	while(math.fabs(nodes[k1][0]- nodes[k2][0]) < 2 or math.fabs(nodes[k1][1]- nodes[k2][1]) < 2):
		k1-=1
		k2+=1
	medy = 0
	for i in range(k1,k2):
		medy += nodes[i][1]
	medy /=(k2-k1)
	for i in range(k1,k2):
		nodes[i][1] = medy
		
if __name__ =="__main__":
	gfw.Init(800,600,False)
	s1 = gfw.Shader(shaders.ObjectLightning[0],shaders.ObjectLightning[1])
	gfw.SetActiveShader(s1);
	gfw.Zoom(100,100)
	tex1 = utils.LoadImageAsTexture("grasstex.png")
	tex2 = utils.LoadImageAsTexture("grasstex2.png")
	tex3 = utils.LoadImageAsTexture("grassborder.png")
	def PP2(tiles):
		j = False
		for i in tiles:
			j = not j
			#print i.Visual
			if j == True:
				i.Visual.AddTexture(tex1,0)
			else:
				i.Visual.AddTexture(tex2,0)
		tiles[-1].Visual.AddTexture(tex3,0)
		#tiles[0].a = math.pi
		tiles[0].Visual.AddTexture(tex3,0)
	
	def PreProcess(img):
		img[img.shape[0]/2,img.shape[1]/2] = 1
	
	c = Chunk([0,0],[100.0,50.0],0,PP2)
	d = Chunk([100.0,50],[200,0],PreProcess,PP2)
	objs = c.Make()
	objs.extend(d.Make())
	objs = d.Make()
	while True:
		try:
			for i in objs:
				i.Draw()
			gfw.Refresh()
			time.sleep(1)
		except KeyboardInterrupt as ke:
			break
	
	
	quit()
	
	pos = [[0,1,"forest",[]],[10,20,"rocks",[]],[20,10,"hills",[cave]],[40,0,"grass",[]],[60,-10,"forest",[]],[100,0,"forest",[]]]
	nodes = MakeSurfLevel(pos,10)
	for i in range(0,len(nodes)):
		if len(nodes[i]) >= 4 and len(nodes[i][3]) > 0:
			for j in nodes[i][3]:
				j(nodes,i)
	verts = []
	minx = -10
	miny = -100
	maxx = 110
	maxy = 100
	nodes.append([maxx,miny,"",[]])
	for i in nodes:
		x = i[0]
		y = i[1]
		if x < minx:
			minx = x
		if x > maxx:
			maxx = x
		if y < miny:
			miny = y
		if y > maxy:
			maxy = y
		
		verts.extend([i[0],i[1]])
	range_x = (maxx-minx)
	range_y = maxy-miny
	scale = 32
	img = numpy.zeros((scale,scale))
	dscNodes = [[0,0,[]]]
	for i in nodes:
		x = i[0]
		y = i[1]
		mapx = int((x-minx)/range_x*float(scale))-1
		mapy = int((y-miny)/range_y*float(scale))
		#img[scale-1-mapy,mapx] = 1
		if len(dscNodes) > 0:
			while True:
				last = dscNodes[-1]
				dx = float(mapx)-last[0] 
				dy = float(mapy)-last[1]
				d =  math.fabs(dx)+math.fabs(dy)
				if d >= 2:
					cx = round(dx/d*2**0.5)
					cy = round(dy/d*2**0.5)
					dscNodes.append((int(last[0]+cx),int(last[1]+cy)))
				else:
					break
		
		dscNodes.append((mapx,mapy,i[2],i[3]))
	
	for i in dscNodes:
		#print i
		col = 1
		if len(i) > 2:
			t = i[2]
			if t == 'forest':
				col = 0.8
			elif t == 'rocks':
				col = 0.7
			elif t == 'grass':
				col = 0.6
			elif t == 'hills':
				col = 0.5
			else:
				print t
		img[i[0],i[1]] = col
	
	fillpoints = [(4,4)]
	while len(fillpoints) > 0:
	
		x,y = fillpoints.pop()
		if img[x,y] > 0:
			continue
			
		img[x,y] = 1
		for i,j in [[0,1],[-1,0],[1,0],[0,-1]]:
			#print i,j
			if x+i <0 or x+i > scale-1:
				#print 1
				continue
			if y+j < 0 or y+j > scale-1:
				#print "hmmm:", y+j
				continue
			if i is 0 and j is 0:
				#print 3
				continue
			if img[x+i,y+j] == 0:
				#print "Appending,",x+i,y+j
				fillpoints.append((x+i,y+j))
	
	interp = [(4,4)]
	def Interpolate(image):
	
		for x in range(0,image.shape[0]-1):
			for y in range(0,image.shape[1]-1):
				val = img[x,y]
				
				if  val == 1:
					j = 0;
					print y+j					
					while j+y < image.shape[1]-1 and img[x,j+y] == 1:
						print "lolz:",img[x,j+y]
						j+=1
					
					ival = img[x,y+j]
					for i in range(0,j):
						img[x,y+i] =ival 
					
	Interpolate(img)
		
		
		
	im = Image.frombuffer('L',(scale,scale),(img*255).astype(numpy.uint8),'raw','L',0,1)
	im.save("test.png")
	quit()
	class drawer:
		def __init__(self,drawable,x,y,a = 0):
			self.draw = drawable
			self.x = x
			self.y = y
			self.a = a
	quad = utils.MakePolygon([0.0,0, 0.0,2.0, 2.0,2.0, 2.0,0.0],[],[],[])
	msquares = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,quad]
	
	tri = utils.MakePolygon([0,0, 1,0, 1,1],[],[],[])
	tri.SetDrawType(0x004)
	rtri = utils.MakePolygon([0,0, 0,1, 1,1],[],[],[])
	rtri.SetDrawType(0x004)
	
	c1 = utils.MakePolygon([0,0,1,0,0,1],[],[],[])
	c1.SetDrawType(0x0004)
	c2 = utils.MakePolygon([1,0,2,1,2,0],[],[],[])
	c2.SetDrawType(0x0004)
	
	c3 = utils.MakePolygon([0,0, 0,1, 2,1, 2,0],[],[],[])
	c3.SetDrawType(0x0007)
	
	c4 = utils.MakePolygon([1,2,2,2,2,1],[],[],[])
	c4.SetDrawType(0x0004)
	
	c5 = utils.MakePolygon([0,0, 0,1,1,2,2,0],[],[],[])
	c5.SetDrawType(0x0005) #Triangle strip
	
	c6 = utils.MakePolygon([1,0, 1,2, 2,2, 2,0],[],[],[])
	c6.SetDrawType(0x0007)
	
	c7 = utils.MakePolygon([0,0, 0,1, 2,0, 2,2, 1,2],[],[],[])
	c7.SetDrawType(0x0007)
	c9 = utils.MakePolygon([0,0, 0,2, 1,2,1,0],[],[],[])
	c9.SetDrawType(0x0007)
	c11 = utils.MakePolygon([0,0, 0,2, 1,2,2,0, 2,1],[],[],[])
	c11.SetDrawType(0x0005)
	
	cx = utils.MakePolygon([0,0,0,2,1,2,1,0],[],[],[])
	cx.SetDrawType(0x0005) #Triangle strip
	msquares[1] = c1
	msquares[2] = c2
	msquares[3] = c3
	msquares[4] = c4
	msquares[5] = c5
	#msquares[6] = c6
	msquares[9] = c9
	msquares[11] = c11
	msquares[7] = c7
	quad.SetDrawType(0x006)
	polys = []
	
	def GetPoly(index,x,y):
		draw = msquares[index]
		if draw is not 0:
			return drawer(msquares[index],x,y)
		elif index is not 0:
			print index
	for x in range(0,img.shape[0]-1,2):
		for y in range(0,img.shape[1]-1,2):
			val = int(img[x,y])
			val += int(img[x+1,y])<<1
			val += int(img[x+1,y+1])<<2
			val += int(img[x,y+1])<<3
			d = GetPoly(val,x,y)
			if d is not None:
				polys.append(d)
			img[x,y] = float(val)
	#print polys
	
	#quit()
	tree = utils.MakePolygon(verts,[],[],[])
	tree.SetDrawType(0x002)
	
	while True:
		try:
			for i in polys:
				gfw.Draw(i.x-50,i.y,0,i.draw)
			gfw.Refresh()
			time.sleep(1)
			gfw.Draw(-50,0,0,tree)
			
		except KeyboardInterrupt as ke:
			break
		

