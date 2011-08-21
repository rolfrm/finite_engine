import numpy
import gfw
import physics
import core
def MakePolygon(verts, indices,colors,uvs):
	nvertexes = numpy.array(verts,dtype=numpy.float32)
	nindices = numpy.array(indices,dtype=numpy.uint32)
	ncolor = numpy.array(colors,dtype=numpy.float32)
	nuv = numpy.array(uvs,dtype=numpy.float32)
	return gfw.Polygon(nvertexes.tostring(),len(nvertexes), nindices.tostring(),len(nindices),ncolor.tostring(),len(ncolor),nuv.tostring(),len(nuv))

def makeGfxBox(sizex,sizey):
	sx = sizex/2
	sy = sizey/2
	return MakePolygon([-sx,-sy, sx,-sy, sx,sy, -sx,sy],[0,1,2,3],[1,1,1, 1,1,1, 1,1,1, 1,1,1],[0,-0.9, 0.2,-0.9, 0.2,-1, 0,-1])

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
	p1.Move(position[0],position[1])
	o1.LoadPolygon(p1)
	o1.setFriction(0.6)
	return o1

def MakePhysicsPolygon(vertexes):
	o1 = physics.PhysicsObject()
	p1 = physics.Polygon()
	for i in range(0,len(vertexes),2):
		p1.AddVertex(vertexes[i],vertexes[i+1])
	#o1.LoadPolygon(p1)
	return p1
def MakeSurfaceFromHeights(heights,distance,depth):
	gameObjects = []
	d = depth
	for i in range(0,len(heights)-1):
		y1 = heights[i]
		y2 = heights[i+1]
		ymid = (y1+y2-d)/2
		verts = [-distance/2 -0.01, y1-ymid, distance/2 +0.01,y2-ymid,distance/2 + 0.01,y2-ymid-depth,-distance/2 - 0.01,y1-ymid-depth]
		indices = [0,1,2,3]
		colors = [0.5,0.6,0.5, 0.5,0.6,0.5, 0.1,0.1,0.1, 0.1,0.1,0.1]
		uvs = [0,0, 1,0, 1,1, 0,1]
		p = MakePhysicsPolygon(verts)
		p.Move((i+0.5)*distance,ymid)
		o = physics.PhysicsObject()
		o.setFriction(0.6)
		o.LoadPolygon(p)
		g = MakePolygon(verts,indices,colors,uvs)
		gameObjects.append(core.GameObject(g,o))
	return gameObjects
import random
import math
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
def GenHeightMap(x1, x2,steps):
	nodes = [x1,x2]
	def Iterate(nodeList):
		newNodes = [nodeList[0]]
		for i in range(0,len(nodeList)-1):
			last = nodeList[i]
			now = nodeList[i+1]
			dist = ((now-last)**2)**0.5
			rand = (random.random()-0.5)*dist/2
			newNodes.append((now+last)/2 +rand)
			newNodes.append(now)
			
		return newNodes
	for i in range(0,steps):
		nodes = Iterate(nodes)	
	return nodes

def getDist(x1,x2):
	return sum(map(lambda x,y: (x-y)**2,x1,x2))**0.5 

def GenLevel2(PathList,portals,nodes):
	print "Generating level, Data:"
	print PathList
	print portals
	print nodes
	levels = []
	for path in PathList:
		walkway = []
		portalList = []
		pathPortals = []
		for j in portals:
			if j[0] == PathList.index(path):
				pathPortals.append(j)
		for i in range(0,len(path)):
			nodeIndex = path[i]
			for j in pathPortals:
				if j[2] == i:
					portalList.append([len(walkway),j])
			if i == len(path)-1:
				break
			node = nodes[nodeIndex]
			nextnode = nodes[path[i+1]]
			pos = node[0]
			nextpos = nextnode[0]
			distance = getDist(pos,nextpos)
			n = int(math.log(distance,2))
			hmap = GenHeightMap(pos[1],pos[2],n)
			walkway.extend(hmap)
		surf = MakeSurfaceFromHeights(walkway,10,10)
		localPortals =[]
		for i in portalList:
			portal = core.GameObject(makeGfxBox(5.0,5.0),MakePhysicsBox(5.0,1.0,1,(i[0]*10,walkway[min(i[0],len(walkway)-1)]+40)),(0,2))
			portal = core.Portal(levels,(i[0]*10,walkway[min(i[0],len(walkway)-1)]+40),i[1][1],i[1][3])
			localPortals.append(portal)
			portal.conn = [i[1][1],i[1][3]]
		levels.append([localPortals,surf])
		
	def PortalGetLevel(path,portal,levellist):
		level = levellist[path]
		arrivePortal = level[0][portal]
		gamedata = level[0]
		gamedata.extend(level[1])
		return gamedata,arrivePortal
		
	for level in levels:
		for portal in level[0]:
			portal.levels = level
			portal.GetLevel = PortalGetLevel
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
	
from copy import deepcopy
import time
if __name__=="__main__":
	random.seed(10)
	import gfw
	import shaders
	print GenHeightMap(1.0,2.0,10)
	gfw.Init(800,600,False)
	s1 = gfw.Shader(shaders.ObjectLightning[0],shaders.ObjectLightning[1])
	gfw.SetActiveShader(s1);
	gfw.Zoom(200,200)
	nodes = [ [[-100.0,0.0,0.0],'cave'],[[100.0,0.0,0.0],'forest']]
	connections = [[0,1]]
	for i in range(0,3):
		GenLevel(nodes,connections)
	print nodes
	print connections
	print "GEN PATH LIST:"
	pathList = GenPathList(nodes,deepcopy(connections))
	portalList = GetPortalList(pathList)
	portalList.append([0,0,0,0]) #Start portal
	print pathList,portalList
	GenLevel2(pathList,portalList,nodes)
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
		
