import gfw
import numpy
import numpy.random
import math
import time
vs = """
attribute vec2 pos; 
attribute vec3 color;
uniform float Xoff;
uniform float Yoff;
uniform float Rotation;
uniform vec3 Color;
varying vec3 vColor;

void main(){
vColor = color;
 vec2 npos = vec2(pos.x*cos(Rotation) - pos.y*sin(Rotation), pos.y*cos(Rotation)+ pos.x*sin(Rotation));
 gl_Position=vec4(npos+vec2(Xoff,Yoff),0,1);
 }
"""

fs = """
varying vec3 vColor;
void main(){
gl_FragColor= vec4(vColor,1);
}
"""
ar2 = numpy.sin(numpy.arange(0,20,dtype=numpy.float32))
indices = numpy.arange(0,10,dtype=numpy.uint32)

#colors = numpy.random.random((len(indices))).astype(numpy.float32)
colors = numpy.ones((len(ar2)*1.5,)).astype(numpy.float32)
print "color", colors
s = ar2.tostring()
gfw.PrintString(s,len(ar2))
ar = ar2.tolist() #[-0.5,0.5 ,0,0.6,0.5,0.5,0.5,-0.5,-0.5,-0.5 ]
gfw.Init(200,300,False)
s1 = gfw.Shader(vs,fs)

b = gfw.FloatVector(ar)
#a = gfw.Polygon(b,b,b,b)
a = gfw.Polygon(ar2.tostring(),len(ar2),indices.tostring(),len(indices),colors.tostring(),len(colors),"",0)
gfw.SetActiveShader(s1);

i = 0

while True:
	try:
		i +=0.001
		t = time.time()
		s1.SetUniform3f(1.0,0.0,1.0,"Color")
		gfw.Draw(math.sin(i),math.cos(i),i,a)
		s1.SetUniform3f(1.0,1.0,0.0,"Color")
		gfw.Draw(math.sin(-i),math.cos(i),-i,a)
		s1.SetUniform3f(0.0,1.0,1.0,"Color")
		gfw.Draw(math.sin(-i),math.cos(i+math.pi/2),-i,a)
		
		
		gfw.Refresh()
		time.sleep(0.02)
		print str(1/(time.time()-t)) + " fps"
		
	except KeyboardInterrupt as ke:
		break
gfw.DeInit()
