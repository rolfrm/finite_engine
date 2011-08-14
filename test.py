import physics
import gfw
import time
import numpy
vs = """
uniform sampler2D tex0;
attribute vec2 pos; 
attribute vec3 color;
attribute vec2 uv;
uniform float Xoff;
uniform float Yoff;
uniform float Rotation;
uniform vec3 Color;
varying vec3 vColor;
varying vec2 vuv;
void main(){
vColor = color;
vuv = uv;//vec2(0.5,0.5);
 vec2 npos = vec2(pos.x*cos(Rotation) - pos.y*sin(Rotation), pos.y*cos(Rotation)+ pos.x*sin(Rotation));
 gl_Position=vec4(npos+vec2(Xoff,Yoff),0,1);
 }
"""

fs = """
uniform sampler2D tex0;
uniform sampler2D tex1;
varying vec3 vColor;
varying vec2 vuv;
void main(){
	vec3 tex = texture2D(tex0,vuv).xyz;
	vec3 col = tex + vColor + vec3(0.5,0.5,0.5);
	col.x -=vuv.x;
gl_FragColor= vec4(col,1);
}
"""
class GameObject:
	def __init__(self,physicsObject,graphicsobject):
		self.PhysicsObject = physicsObject
		self.GraphicsObject = graphicsobject
		
	def Draw(self):
		
		pos = self.PhysicsObject.GetPosition()
		print pos.x,pos.y
		angle = self.PhysicsObject.GetAngle()
		gfw.Draw(pos.x,pos.y,angle,self.GraphicsObject)
	

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
	go = GameObject(o1 ,gfw.Polygon(v.tostring(),len(v),indices.tostring(),len(indices),color.tostring(),len(color),"",0))
	go.o1 = o1
	go.p1 = p1
	return go
gfw.Init(400,400,False)

s1 = gfw.Shader(vs,fs)
gfw.SetActiveShader(s1);

pc = physics.Core(100)
pc.setGravity(0.0,-0.0004)
b1 = MakeBox(1.0,0.2,1.0,(0.0,0.0))
b2 = MakeBox(1.0,1.0,0,(0.0,-1.0))
b1.PhysicsObject.CalculateMomentofInertia()
pc.LoadObject(b1.PhysicsObject)
pc.LoadObject(b2.PhysicsObject)
i = 0
print 2
while True:
	try:
		b1.Draw()
		b2.Draw()
		i +=0.001
		t = time.time()
		gfw.Refresh()
		time.sleep(0.01)
		pc.Run()
		
	except KeyboardInterrupt as ke:
		break