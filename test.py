import physics
import gfw
import time
import numpy
gfw.Init(1000,1000,False)


vs = """
uniform sampler2D tex0;
attribute vec2 pos; 
attribute vec3 color;
attribute vec2 uv;
uniform float Xoff;
uniform float Yoff;
uniform float Rotation;
uniform vec3 Color;
uniform vec2 lightpos;
varying vec3 vColor;
varying vec2 vuv;
varying vec2 lightDir;
varying vec2 normal;

void main(){
vColor = color;
vuv = uv;//vec2(0.5,0.5);
 vec2 npos = vec2(pos.x*cos(Rotation) - pos.y*sin(Rotation), pos.y*cos(Rotation)+ pos.x*sin(Rotation))+vec2(Xoff,Yoff);
 lightDir = npos-lightpos;
 normal = uv - vec2(0.5,0.5);
 normal.x *=-1;
 normal.y *=-1;
 normal = vec2(normal.x*cos(Rotation) - normal.y*sin(Rotation), normal.y*cos(Rotation)+ normal.x*sin(Rotation));
 gl_Position=vec4((npos)*0.02,0,1);
 }
"""

fs = """
uniform sampler2D tex0;
uniform sampler2D tex1;
varying vec3 vColor;
varying vec2 vuv;
varying vec2 lightDir;
varying vec2 normal;
void main(){

	vec3 tex = texture2D(tex0,vuv).xyz;
	vec3 col = vColor;
	col +=  tex;
	float amb = 1/(length(lightDir)/5);
	float diff = max(dot(normalize(normal),normalize(lightDir)),0)*1/(length(lightDir)/5);;
	vec3 diffCol = diff*col;
	
gl_FragColor= vec4(col*min(1,amb)*1 + diffCol*0.5,1);
}
"""

s1 = gfw.Shader(vs,fs)
gfw.SetActiveShader(s1);

class GameObject:
	def __init__(self,physicsObject,graphicsobject):
		self.PhysicsObject = physicsObject
		self.GraphicsObject = graphicsobject
		
	def Draw(self):
		
		pos = self.PhysicsObject.GetPosition()
		#print pos.x,pos.y
		angle = self.PhysicsObject.GetAngle()
		gfw.Draw(pos.x,pos.y,angle,self.GraphicsObject)
	

noise = (numpy.random.random(128*3)*255).astype(numpy.uint8)
tex = gfw.Texture(noise.tostring(),16,16)


def MakeBox(sizex, sizey,mass,position):
	sx = sizey/3
	sy = sizex/3
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
	color = numpy.array([0,1,1, 1,0,1, 1,1,0, 1,1,1],dtype=numpy.float32)
	uv = numpy.array([0,0,1,0,1,1,0,1],dtype=numpy.float32)
	go = GameObject(o1 ,gfw.Polygon(v.tostring(),len(v),indices.tostring(),len(indices),color.tostring(),len(color),uv.tostring(),len(uv)))
	go.GraphicsObject.AddTexture(tex,0)
	#go.o1 = o1
	#go.p1 = p1
	return go


pc = physics.Core(100)
objlist = []
def AddObject(obj):
	pc.LoadObject(obj.PhysicsObject)
	objlist.append(obj)
def Render():
		for i in objlist:
			i.Draw()

pc.setGravity(0.0,-0.0004)

for i in range(0,20):
	AddObject(MakeBox(10.0,2.0,10.0,(0.1+0.3*i,-30.0 + 10*i)))
b1 = MakeBox(10.0,2.0,10.0,(-1,20))
AddObject(b1)
AddObject(MakeBox(10.0,1000.0,0.0,(1.0,-50.0)))


i = 0
print 2
while True:
	try:
		b1pos = b1.PhysicsObject.GetPosition()
		print b1pos.x,b1pos.y
		s1.SetUniform2f(b1pos.x,b1pos.y,"lightpos");
		t = time.time()
		Render()
		i +=0.001
		gfw.Refresh()
		
		pc.Run()
		print 1/(time.time()-t)
		time.sleep(0.01)
		
	except KeyboardInterrupt as ke:
		break
