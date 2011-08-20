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
	return MakePolygon([-sx,-sy, sx,-sy, sx,sy, -sx,sy],[0,1,2,3],[1,1,1, 1,1,1, 1,1,1, 0,1,1],[0,-0.9, 0.2,-0.9, 0.2,-1, 0,-1])

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
		verts = [-distance/2, y1-ymid, distance/2,y2-ymid,distance/2,y2-ymid-depth,-distance/2,y1-ymid-depth]
		indices = [0,1,2,3]
		colors = [1,1,1, 1,1,1, 1,1,1, 1,1,1]
		uvs = [0,0, 1,0, 1,1, 0,1]
		p = MakePhysicsPolygon(verts)
		p.Move((i+0.5)*distance,ymid)
		o = physics.PhysicsObject()
		o.setFriction(0.6)
		o.LoadPolygon(p)
		g = MakePolygon(verts,indices,colors,uvs)
		gameObjects.append(core.GameObject(g,o))
	return gameObjects
