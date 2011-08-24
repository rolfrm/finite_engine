import physics
import gfw
import time
import numpy
import random
import math
import world_generator.fractal_land as worldgen
gfw.Init(500,700,False)
vs = """
uniform sampler2D tex0;
uniform vec2 Zoom;
attribute vec2 pos; 
attribute vec3 color;
attribute vec2 uv;
uniform float Xoff;
uniform float Yoff;
uniform float Rotation;
uniform vec3 Color;
uniform vec2 lightpos;
#define MAX_LIGHTS 5
uniform vec2 MultiLightPos[MAX_LIGHTS];
uniform float MultiLightIntensity[MAX_LIGHTS];
uniform vec2 CameraPosition;
uniform int NumLights;
varying vec3 vColor;
varying vec2 vuv;
varying vec2 lightDir;
varying vec2 normal;
varying vec2 vMultiLightDir[MAX_LIGHTS];

void main(){
vColor = color;
vuv = uv;//vec2(0.5,0.5);
 vec2 npos = vec2(pos.x*cos(Rotation) - pos.y*sin(Rotation), pos.y*cos(Rotation)+ pos.x*sin(Rotation))+vec2(Xoff,Yoff);
 lightDir = npos-lightpos;
 lightDir = npos-MultiLightPos[0];
 
 for(int i= 0; i < NumLights;i++){
	vMultiLightDir[i] = npos-MultiLightPos[i];
 }
 
 normal = uv - vec2(0.5,0.5);
 normal.x *=-1.0;
 normal.y *=-1.0;
 normal = vec2(normal.x*cos(Rotation) - normal.y*sin(Rotation), normal.y*cos(Rotation)+ normal.x*sin(Rotation));
 gl_Position=vec4((npos - CameraPosition)/Zoom,0.0,1.0);
 }
"""

fs = """
uniform sampler2D tex0;
uniform sampler2D tex1;
varying vec3 vColor;
varying vec2 vuv;
varying vec2 lightDir;
#define MAX_LIGHTS 5

uniform float MultiLightIntensity[MAX_LIGHTS];
varying vec2 vMultiLightDir[MAX_LIGHTS];
uniform vec3 MultiLightColor[MAX_LIGHTS];
uniform int NumLights;
varying vec2 normal;
void main(){
	vec3 globalColor = vec3(0.7,0.6,0.5)/2.0*0;
	vec4 tex = texture2D(tex0,vuv);
	vec3 col = vColor;
	col +=  tex.xyz;
	vec3 ncol = vec3(0.0,0.0,0.0);
	for(int i =0; i< NumLights;i++){
		 float namb = 1.0/(length(vMultiLightDir[i]));
		 float ndiff = max(dot(normalize(normal),normalize(vMultiLightDir[i])),0.0)*1.0/(length(vMultiLightDir[i])/5.0);
		ncol += col*(min(1.0,namb) + ndiff*0.5)*MultiLightColor[i]*MultiLightIntensity[i]; 
	}
	ncol += globalColor;
	//if(tex != 0){
		gl_FragColor= vec4(ncol,1.0);
	//}else{
	//gl_FragColor= vec4(ncol,1.0);

	//}
}
"""

#lol?

s1 = gfw.Shader(vs,fs)
gfw.SetActiveShader(s1);
gfw.Zoom(50,50)
class GameObject:
	def __init__(self,physicsObject,graphicsobject):
		self.PhysicsObject = physicsObject
		self.GraphicsObject = graphicsobject
		
	def Draw(self):
		
		pos = self.PhysicsObject.GetPosition()
		#print pos.x,pos.y
		angle = self.PhysicsObject.GetAngle()
		gfw.Draw(pos.x,pos.y,angle,self.GraphicsObject)


def MakePhysicsBox(sizex, sizey,mass,position):
	sx = sizey/2
	sy = sizex/2
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

def MakeBox(sizex, sizey,mass,position):
	sx = sizey/2
	sy = sizex/2
	px = position[0]
	py = position[1]
	
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
	v = numpy.array([-sx,-sy, sx,-sy, sx,sy, -sx,sy],dtype =numpy.float32)
	indices = numpy.array([0,1,2,3],dtype=numpy.uint32)
	color = numpy.array([1,1,1, 1,1,1, 1,1,1, 1,1,1],dtype=numpy.float32)
	uv = numpy.array([0,0,1,0,1,1,0,1],dtype=numpy.float32)
	go = GameObject(o1 ,gfw.Polygon(v.tostring(),len(v),indices.tostring(),len(indices),color.tostring(),len(color),uv.tostring(),len(uv)))
	go.GraphicsObject.AddTexture(tex,0)

	return go

def makeGfxBox(sizex,sizey):
	sx = sizey/2
	sy = sizex/2
	v = numpy.array([-sx,-sy, sx,-sy, sx,sy, -sx,sy],dtype =numpy.float32)
	indices = numpy.array([0,1,2,3],dtype=numpy.uint32)
	color = numpy.array([1,1,1, 1,1,1, 1,1,1, 1,1,1],dtype=numpy.float32)
	uv = numpy.array([0,0,1,0,1,1,0,1],dtype=numpy.float32)
	return gfw.Polygon(v.tostring(),len(v), indices.tostring(),len(indices),color.tostring(),len(color),uv.tostring(),len(uv))

class man:
	body = makeGfxBox(3.0,3.0)
	head = makeGfxBox(2.0,2.0)
	foot = makeGfxBox(1.0,1.0)
	def __init__(self,x,y):
		self.x = x
		self.y = y
		self.pobj = MakePhysicsBox(5,5,10,(x,y))
		self.pobj.setRestitution(0)
		#print self.pobj.SetInertia(0)
	def Draw(self):
		xy = self.pobj.GetPosition()
		self.x = xy.x
		self.y = xy.y
		#self.pobj.SetInertia(0)
		gfw.Draw(self.x,self.y,self.pobj.GetAngle(),self.body)
		gfw.Draw(self.x,self.y+3,0,self.head)
		gfw.Draw(self.x+3,self.y+1,0,self.foot)
		gfw.Draw(self.x-3,self.y+1,0,self.foot)
		gfw.Draw(self.x+math.sin(self.x/math.pi)*2,self.y-2+math.cos(self.x/math.pi),0,self.foot)
		gfw.Draw(self.x+math.sin(self.x/math.pi	+ math.pi)*2,self.y-2+math.cos(self.x/math.pi+ math.pi),0,self.foot)


box1 = makeGfxBox(100,100)
man1 = man(0,0)
noise = (numpy.random.random(16*16*4)).astype(numpy.float32)
#noise = numpy.ones(16*16*4,dtype=numpy.float32)*0.5
map1 = worldgen.MakeMap().astype(numpy.float32)
print map1[[[1,2],[2,3]]]

tex = gfw.Texture(worldgen.MakeMap().tostring(),64,64,1)

pc = physics.Core(100)
objlist = []
def AddObject(obj):
	pc.LoadObject(obj.PhysicsObject)
	objlist.append(obj)
def Render():
		man1.Draw()
		for i in objlist:
			i.Draw()

pc.setGravity(0.0,-0.004)

ls = gfw.LightSystem(5)

for i in range(0,5):
	col = random.random()
	if col < 0.33 :
		print "R"
		ls.GetLight(i).R = 1 
	elif col < 0.66:
		print "G"	
		ls.GetLight(i).G = 1
	else:	
		print "B"
		ls.GetLight(i).B = 1
	ls.GetLight(i).intensity = 1#1.0/(1.0+i)

for i in range(0,3):
	AddObject(MakeBox(10.0,10.0,10.0,(0.1+0.01*i,-30.0 + 10*i)))
b1 = MakeBox(10.0,2.0,10.0,(-10,20))
b2 = MakeBox(10.0,2.0,10.0,(-1,30))
b3 = MakeBox(10.0,2.0,10.0,(10,50))
b4 = MakeBox(10.0,2.0,10.0,(0,60))
b5 = MakeBox(10.0,2.0,10.0,(10,70))


AddObject(b1)
AddObject(b2)
AddObject(b3)
AddObject(b4)
AddObject(b5)
AddObject(MakeBox(10.0,100.0,0.0,(0.0,-50.0)))
#AddObject(MakeBox(200.0,10.0,0.0,(-30.0,-15.0)))
#AddObject(MakeBox(200.0,10.0,0.0,(30.0, -15.0)))
pc.LoadObject(man1.pobj)
i = 0
while True:
	try:
		kev = gfw.GetKeyEvents()
		for j in kev:
			if j.charKey == 'A' and j.action == 1:
				man1.pobj.AddForce(-1,0)
			if j.charKey == 'W' and j.action == 1:
				man1.pobj.AddForce(0,1)
			if j.charKey == 'D' and j.action == 1:
				man1.pobj.AddForce(1,1)
		mev = gfw.GetMouseEvents()
		for j in mev:
			vpos = gfw.ScreenToWorldCoordinates(gfw.GetMousePos())
			print vpos.X,vpos.Y
			if j.action == 1:
				AddObject(MakeBox(10.0,2.0,10.0,(vpos.X,vpos.Y)))
			print j.button
			print j.action
		s1.SetUniform2f(man1.x,man1.y,"CameraPosition")
		b1pos = b1.PhysicsObject.GetPosition()
		b2pos = b2.PhysicsObject.GetPosition()
		b3pos = b3.PhysicsObject.GetPosition()
		b4pos = b4.PhysicsObject.GetPosition()
		b5pos = b5.PhysicsObject.GetPosition()
		#s1.SetUniform2f(b1pos.x,b1pos.y,"lightpos");
		ls.GetLight(0).X = b1pos.x;
		ls.GetLight(0).Y = b1pos.y;
		ls.GetLight(1).X = b2pos.x;
		ls.GetLight(1).Y = b2pos.y;
		ls.GetLight(2).X = b3pos.x;
		ls.GetLight(2).Y = b3pos.y;
		ls.GetLight(3).X = b4pos.x;
		ls.GetLight(3).Y = b4pos.y;
		ls.GetLight(4).X = b5pos.x;
		ls.GetLight(4).Y = b5pos.y;
		
		
		ls.Activate()
		t = time.time()
		Render()
		i +=0.001
		gfw.Refresh()
		
		pc.Run()
		#print 1/(time.time()-t)
		time.sleep(0.01)
		
	except KeyboardInterrupt as ke:
		break
