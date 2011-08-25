import gfw
import numpy
import numpy.random
import math
import time
vs = """
uniform vec2 Zoom;
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
 gl_Position=vec4((npos+vec2(Xoff,Yoff))/Zoom,0,1);
 }
"""

fs = """
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform int tex0Active;
varying vec3 vColor;
varying vec2 vuv;
void main(){
	vec4 tex = texture2D(tex0,vuv);
	vec3 col = tex;// + vColor*0.1;
	col.x -=vuv.x;
	if(tex0Active == 1){
		gl_FragColor= vec4(tex);
	}else{
		gl_FragColor = vec4(vColor,1);
	}
}
"""
ar2 = numpy.array([-0.5,-0.5, -0.5,0.5, 0.5,0.5, 0.5,-0.5],dtype=numpy.float32)
indices = numpy.array([0,1,2,3],dtype=numpy.uint32)
colors = numpy.array([1,0,0, 0,1,0, 0,0,1, 1,1,0],dtype=numpy.float32)
uvs =numpy.array([0,0, 1,0, 1,1, 0,1],dtype=numpy.float32);

noise = (numpy.random.random(128*8)*255).astype(numpy.uint8)
import Image
noise = Image.open('UV_Test_Map.jpg')

gfw.Init(600,600,False)
s1 = gfw.Shader(vs,fs)

a = gfw.Polygon(ar2.tostring(),len(ar2),indices.tostring(),len(indices),colors.tostring(),len(colors),uvs.tostring(),len(uvs),0,0)
gfw.Zoom(1,1)
gfw.SetActiveShader(s1);
print noise.size
tex = gfw.Texture(noise.tostring(),noise.size[0],noise.size[1],3,1)
#a.AddTexture(tex,0);
i = 0.0001
Text = gfw.Text(tex,0,1,0,1,10,10,32)
Text.SetText("Hello fonts")


while True:
	try:
		a.ReloadUV(gfw.FloatVector([0.0,0.0, 0,1 ,1,1, 1.0,0]))
		i += 0.001
		t = time.time()
		s1.SetUniform3f(1.0,1.0,1.0,"Color")
		a.ActivateTextures()
		gfw.Draw(0,0,i,a)
		gfw.Draw(-1,0,i,a)
		gfw.Draw(0,-1,i,a)
		gfw.Draw(0,1,i,a)
		gfw.Draw(-0.5,-0.5,0,Text);
		#print i
		print time.time()-t
		gfw.Refresh()
		time.sleep(0.01)
		
	except KeyboardInterrupt as ke:
		break
gfw.DeInit()

