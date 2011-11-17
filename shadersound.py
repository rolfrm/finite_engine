import gfw
import pyaudio
import time
import utils

vertexShader = """
attribute vec2 pos;
attribute vec2 uv;
uniform float phase;
varying vec2 vuv;
varying vec2 uvs;
uniform float SamplingSpeed;
uniform float BlockSize;

#define M_PI_2 6.28
void main(){
	vuv = uv;
	uvs = uv;
	vuv.x += phase;	
	vuv.x *=(M_PI_2*BlockSize/(SamplingSpeed));
	gl_Position = vec4(pos,0,1);
}

"""

sineShader = """
uniform float freq;
uniform float phase;
uniform float SamplingSpeed;
uniform float BlockSize;
varying vec2 vuv;
varying vec2 uvs;
uniform float amplitude;
#define M_PI_2 6.28
void main(){
	//gl_FragColor = vec4(sin((cos(vuv.x)*freq))*sin(vuv.x*freq*10))*amplitude;//*sin(uvs.x*3.14);
	gl_FragColor = vec4(sin(vuv.x*freq))*amplitude;
}

"""

waveTableTest = """
uniform float freq;
uniform float phase;
uniform float SamplingSpeed;
uniform float BlockSize;
varying vec2 vuv;
varying vec2 uvs;
uniform float amplitude;
#define M_PI_2 6.28
uniform sampler2D tex1;
void main(){
	gl_FragColor = vec4(texture2D(tex1,vec2(vuv.x*freq,0)))*amplitude;//*sin(uvs.x*3.14);
}


"""

visVertex = """
attribute vec2 pos;
attribute vec2 uv;
varying vec2 vuv;
#define M_PI_2 6.28
void main(){
	vuv = uv;
	gl_Position = vec4(pos,0,1);
}

"""

visShader = """
uniform sampler2D tex0;
varying vec2 vuv;
void main(){
	float c = texture2D(tex0,vec2(vuv.x,0)).x;
	
	c = 0.05/abs(c - (vuv.y-0.5));
	
	gl_FragColor = vec4(c, c+0.1, c+0.5,1)*0.2 ;
}
"""

waveTableFrag = """
uniform sampler2D tex0;
uniform int tableheight;
uniform int tablewidth;
varying vec2 vuv;
void main(){
	int y = vuv.y*tableheight;
	if(y == 0){
		gl_FragColor = sin(vuv.x*3.14);
	}else if(y == 1){
		gl_FragColor = round(sin(vuv.x*3.14))/2+ 0.2;	
	}else{
		gl_FragColor =1;
	}
}

"""

slen = 700
sspeed = 44100
screenh = 500
gfw.Init(slen,screenh,False)
poly = utils.MakePolygon([-1,-1,-1,1,1,1,1,-1],[],[],[0,0, 0,1, 1,1, 1,0])
s1 = gfw.Shader(vertexShader,sineShader);
s2 = gfw.Shader(visVertex,visShader);
waveTableTest = gfw.Shader(vertexShader,waveTableTest)
waveTableGenerator = gfw.Shader(visVertex,waveTableFrag)
gfw.SetActiveShader(waveTableGenerator)
wavetable = gfw.Texture(1024,4,4,1)
waveTableFramebuffer = gfw.FrameBuffer(wavetable)
waveTableGenerator.SetUniform1i(4,"tableheight");
waveTableGenerator.SetUniform1i(1024,"tablewidth");
waveTableFramebuffer.Bind()
poly.Draw(0,0,0)
waveTableFramebuffer.UnBind()

gfw.SetActiveShader(s1)
s1.SetUniform1f(sspeed,"SamplingSpeed")
s1.SetUniform1f(slen,"BlockSize")

gfw.SetActiveShader(waveTableTest)
s1.SetUniform1f(sspeed,"SamplingSpeed")
s1.SetUniform1f(slen,"BlockSize")


t1 = gfw.Texture(slen,1,1,1,1)
fb = gfw.FrameBuffer(t1)
p = pyaudio.PyAudio()
stream = p.open(format = pyaudio.paFloat32,channels = 1,rate = 44100,frames_per_buffer = slen,output = True)
poly.AddTexture(t1,0)
poly.AddTexture(wavetable,1)
f = 100
import math
for i in range(1000000):
	
	gfw.SetActiveShader(s1)
	#gfw.SetActiveShader(waveTableTest)
	s1.SetUniform1f(f*20,"freq")
	s1.SetUniform1f(i,"phase")
	s1.SetUniform1f(gfw.GetMousePos().Y/screenh,"amplitude")
	
	fb.Bind()
	poly.Draw(0,0,0)
	bim = fb.GetBufferImage()
	fv = bim.AsFloatVector()
	stream.write(gfw.GetFloatVectorStr(fv),num_frames=slen	)

	fb.UnBind()

	gfw.SetActiveShader(s2)
	poly.Draw(0,0,0)
	gfw.Refresh()
	f = gfw.GetMousePos().X
	mev = gfw.GetMouseEvents()
	if mev.size() > 0:
		break
	#time.sleep(0.05)
time.sleep(1)

p.terminate()
