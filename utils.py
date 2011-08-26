import numpy
import gfw
import physics
import core
from copy import copy
import random
import math
import Image
def LoadImageAsTexture(path):
	img = Image.open(path)
	texdata = img.tostring()
	tex = gfw.Texture(img.tostring(),img.size[0],img.size[1],4,1)
	return tex

def MakePolygon(verts, indices,colors,uvs):
	return gfw.Polygon( gfw.FloatVector(verts), gfw.UIntVector(indices), gfw.FloatVector(colors), gfw.FloatVector(uvs))

def makeGfxBox(sizex,sizey,color = [0,0,0]):
	sx = sizex/2
	sy = sizey/2
	col = []
	for i in range(0,4):
		col.extend(color)
	return MakePolygon([-sx,-sy, sx,-sy, sx,sy, -sx,sy],[0,1,2,3],col,[0,0, 1,0, 1,-1, 0,-1])

def MakePhysicsBox(sizex, sizey,mass,position):
	sx = sizex/2
	sy = sizey/2
	o1 = physics.PhysicsObject()
	p1 = physics.Polygon()
	o1.setMass(mass)
	p1.AddVertex(-sx,-sy)
	p1.AddVertex(sx,-sy)
	p1.AddVertex(sx,sy)
	p1.AddVertex(-sx,sy)
	o1.LoadPolygon(p1)
	o1.setFriction(0.6)
	return o1

def MakeCompleteObject(verts,color = [],uvs = []):
	o1 = physics.PhysicsObject()
	p1 = physics.Polygon()
	
	for i in range(0,len(verts),2):
		p1.AddVertex(verts[i],verts[i+1])
	if len(color) == 3:
		begincolor = color[:]
		for i in range(1,len(verts)):
			color.extend(begincolor)
	o1.LoadPolygon(p1)
	o1.CalculateMomentofInertia()
	p2 = o1.GetPosition()
	o1.setMass(0)
	for i in range(0,len(verts),2):
		verts[i] -= p2.x
		verts[i+1] -= p2.y
	gfx = MakePolygon(verts,range(0,len(verts)),color,uvs)
	return core.GameObject(gfx,o1)
	
	
	
def MakePhysicsPolygon(vertexes):
	o1 = physics.PhysicsObject()
	p1 = physics.Polygon()
	for i in range(0,len(vertexes),2):
		p1.AddVertex(vertexes[i],vertexes[i+1])
	return p1
def MakeSurfaceFromHeights(heights,distance,depth):
	gameObjects = []
	d = depth
	distance = float(distance)
	for i in range(0,len(heights)-1):
		y1 = heights[i]
		y2 = heights[i+1]
		ymid = (y1 + y2 + y1-d + y2-d)/4
		x1 = float(distance)*float(i)+1
		verts = [-distance/2 -0.001, y1-ymid, distance/2+0.001,y2-ymid,distance/2+0.001,y2-ymid-depth,-distance/2-0.001,y1-ymid-depth]
		indices = [0,1,2,3]
		colors = [0.5,0.7,0.5, 0.5,0.6,0.5, 0.1,0.1,0.1, 0.1,0.1,0.1]
		colors = [0,0,0, 0,0,0, 0,0,0, 0,0,0]
		
		uvs = [0,0, 1,0, 1,1, 0,1]
		pverts =copy(verts)
		for j in range(0,len(pverts),2):
			pverts[j] +=float(i+0.5)*float(distance)
			pverts[j+1] += ymid
		p = MakePhysicsPolygon(pverts)
		o = physics.PhysicsObject()
		
		o.setFriction(0.6)
		o.LoadPolygon(p)
		o.CalculateMomentofInertia()
		p2 = o.GetPosition()
		g = MakePolygon(verts,indices,colors,uvs)
		gameObjects.append(core.GameObject(g,o))
	return gameObjects

def GenLevel(nodes, connections):
	split_chance = 0.1
	def mutateNodes(n1,n2):
		
		dist = sum(map(lambda x,y: (x-y)**2,n1[0],n2[0]))**0.5
		npos = map(lambda x,y: (x+y)/2.0+random.gauss(0,0.25)*dist,n1[0],n2[0])
		ntype = "cave"
		n3 = [npos,ntype,[]]
		roll_split = random.random()
		n4 = 0
		if roll_split < split_chance:
			npos = map(lambda x: x + random.gauss(0,0.25)*dist,npos)
			n4 = [npos,ntype,[]]
		return n3,n4
	for i in connections[:]:
		n1 = nodes[i[0]]
		n2 = nodes[i[1]]
		n3,n4 = mutateNodes(n1,n2)
		i3 = len(nodes)
		nodes.append(n3)
		connections.remove(i)
		connections.append([i[0],i3])
		connections.append([i[1],i3])
		
		if n4 is not 0:
			nodes.append(n4)
			connections.append([i3,i3+1])
def GenHeightMap(x1, x2,steps,roughness = 1):
	nodes = [x1,x2]
	def Iterate(nodeList):
		newNodes = [nodeList[0]]
		for i in range(0,len(nodeList)-1):
			last = nodeList[i]
			now = nodeList[i+1]
			dist = ((now-last)**2)**0.5
			rand = (random.random()-0.5)*dist/2*roughness
			newNodes.append((now+last)/2 +rand)
			newNodes.append(now)
			
		return newNodes
	for i in range(0,steps):
		nodes = Iterate(nodes)	
	return nodes

def getDist(x1,x2):
	return sum(map(lambda x,y: (x-y)**2,x1,x2))**0.5 

def GenWorld(PathList,portals,nodes,gc):
	levels = []
	levelSplitPlaces = []
	scaling = 25
	for path in PathList:
		walkway = []
		splits = []
		levelSplitPlaces.append(splits)
		for i in range(0,len(path)-1):
			nodeIndex = path[i]
			node = nodes[nodeIndex]
			nextnode = nodes[path[i+1]]
			pos = node[0]
			nextpos = nextnode[0]
			distance = getDist(pos,nextpos)
			n = int(math.log(distance,2))
			hmap = GenHeightMap(pos[1],nextpos[1],n/2,2)
			walkway.extend(hmap)
			splits.append([len(walkway),hmap[0]])
		surf = MakeSurfaceFromHeights(walkway,scaling,500)

		nlv = core.Level([],surf,[])
		levels.append(nlv)
	for levelIndex in range(0,len(levels)):
		level = levels[levelIndex]
		for j in portals:
			
			if levelIndex == j[0] and j[0] is not -1:
				otherlevel = levels[j[1]]
				px1 = levelSplitPlaces[levelIndex][j[2]]
				px2 = levelSplitPlaces[j[1]][j[3]]
				print px1,px2
				port1 = MakeCompleteObject([0.0,0.0, 0.0,50.0, 50.0,50.0, 50.0,0.0])
				p1 = core.Portal(level,0,port1.Visual,port1.Body,gc)
				p1.SetPos(px1[0]*scaling,px1[1]+50)
				port2 = MakeCompleteObject([0.0,0.0, 0.0,50.0, 50.0,50.0, 50.0,0.0])
				p2 = core.Portal(otherlevel,p1,port2.Visual,port2.Body,gc)
				p2.SetPos(px2[0]*scaling,px2[1] + 50)
				p1.ExitPortal = p1
				level.Portals.append(p1)
				if j[0] is not j[1]:
					p1.ExitPortal = p2
					otherlevel.Portals.append(p2)
				
				for k in portals:
					if j[0] == k[1] and j[1] == k[0]:
						
						k[0] = -1
						k[1] = -1
						break
				j[0] = -1
				j[1] = -1
				
				
	return levels
def GenPathList(nodes,connections):
	
	def GetPath(nodes,connections):
		path = []
		cnode = 0
		#1: Find unused
		for i in range(0,len(connections)):
			if len(connections[i]) == 2: #Unused
				cnode = connections[i]
				cnode.append(True)
				break
		if cnode == 0:
			return []
		path = [cnode[0], cnode[1]]
		#2: Find unused, that matches
		lookFor = cnode[1]
		while True:
			changed = False
			for i in connections:
				if len(i) == 2 and (i[0] == lookFor or i[1] == lookFor):
					v = -1
					i.append(True)
					if i[0] == lookFor:
						v = 1
					if i[1] == lookFor:
						v = 0
					changed = True
					path.append(i[v])
					lookFor = i[v] 
			if not changed:
				break
		
		return path
	
	pathList = []
	while True:
		newPath = GetPath(nodes,connections)
		if newPath == []:
			break
		pathList.append(newPath)
	
	return pathList	
def GetPortalList(pathList):
	portals = []
	for path1 in pathList:
		for path2 in pathList:
			if path2 == path1:
				continue
			for x in path1: 
				for y in path2: 
					if x == y:
						portals.append([pathList.index(path1),pathList.index(path2),path1.index(x),path2.index(y)])
	return portals

def GenTestWorld(gameCore):
	nodes = [ [[-2000.0,0.0,0.0],'cave'],[[2000.0,0.0,0.0],'forest']]
	connections = [[0,1]]
	for i in range(0,5):
		GenLevel(nodes,connections)
	pathList = GenPathList(nodes,connections)
	portalList = GetPortalList(pathList)
	if len(portalList) == 0:
		portalList.append([0,0,0,0]) #Start portal
	
	world = GenWorld(pathList,portalList,nodes,gameCore)
	
	return world


from copy import deepcopy
import time
if __name__=="__main__":
	random.seed(10)
	import gfw
	import shaders
	gfw.Init(800,600,False)
	s1 = gfw.Shader(shaders.ObjectLightning[0],shaders.ObjectLightning[1])
	gfw.SetActiveShader(s1);
	gfw.Zoom(200,200)
	GenTestWorld(0)
	quit()
	verts = []
	indices = []
	colors = []
	zmin = 100000
	zmax = -100000
	
	for i in nodes:
		z =i[0][2] 
		if z < zmin:
			zmin = z
		if z > zmax:
			zmax = z
	
	for i in nodes:
		verts.extend(i[0][0:2])
		z = i[0][2]
		col = (z-zmin)/(zmax-zmin)
		colors.extend([col,col,col])
	
	for i in connections:
		indices.extend(i)
	tree = MakePolygon(verts,indices,colors,[])
	tree.SetDrawType(0x0001)
	print(len(nodes))
	while True:
		try:
			gfw.Draw(0,0,0,tree)
			gfw.Refresh()
			time.sleep(1)
		except KeyboardInterrupt as ke:
			break
		
