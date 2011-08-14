import gfw
import numpy
import numpy.random
import math
import time
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
varying vec3 vColor;
varying vec2 vuv;
void main(){
	vec3 tex = texture2D(tex0,vuv).xyz;
	vec3 col = tex;// + vColor*0.1;
	col.x -=vuv.x;
gl_FragColor= vec4(col,1);
}
"""
ar2 = numpy.array([-0.5,-0.5, -0.5,0.5, 0.5,0.5, 0.5,-0.5],dtype=numpy.float32)
indices = numpy.array([0,1,2,3],dtype=numpy.uint32)
colors = numpy.array([1,0,0, 0,1,0, 0,0,1, 1,1,0],dtype=numpy.float32)
uvs =numpy.array([0,0, 1,0, 1,1, 0,1],dtype=numpy.float32);

noise = (numpy.random.random(128*3)*255).astype(numpy.uint8)

gfw.Init(300,300,False)
s1 = gfw.Shader(vs,fs)

a = gfw.Polygon(ar2.tostring(),len(ar2),indices.tostring(),len(indices),colors.tostring(),len(colors),uvs.tostring(),len(uvs))
gfw.SetActiveShader(s1);

tex = gfw.Texture(noise.tostring(),16,16)
a.AddTexture(tex,0);
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
		
	except KeyboardInterrupt as ke:
		break
gfw.DeInit()
