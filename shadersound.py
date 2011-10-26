import gfw
import pyaudio
import time
import utils

vertexShader = """
attribute vec2 pos;
attribute vec2 uv;
uniform float phase;
varying vec2 vuv;
varying float amplitude;
void main(){
	amplitude =0.5;
	vuv = uv;
	vuv.x += phase;	
	gl_Position = vec4(pos,0,1);
}

"""

sineShader = """
uniform float freq;
uniform float phase;
uniform float SamplingSpeed;
uniform float BlockSize;
varying vec2 vuv;
varying float amplitude;
#define M_PI_2 6.28
void main(){
	gl_FragColor = vec4(sin((vuv.x*freq)*(M_PI_2*BlockSize/(SamplingSpeed))))*amplitude;
}

"""


visShader = """
uniform sampler2D tex0;
varying vec2 vuv;
void main(){
	float c = texture2D(tex0,vec2(vuv.x,0)).x*0.3;
	
	c = 0.05/abs(c - (vuv.y-0.5));
	
	gl_FragColor = vec4(c, c+0.1, c+0.5,1)*0.2 ;
}
"""
slen = 1024
sspeed = 44100

gfw.Init(1024,100,False)
poly = utils.MakePolygon([-1,-1,-1,1,1,1,1,-1],[],[],[0,0, 0,1, 1,1, 1,0])
s1 = gfw.Shader(vertexShader,sineShader);



s2 = gfw.Shader(vertexShader,visShader);
gfw.SetActiveShader(s1)
s1.SetUniform1f(sspeed,"SamplingSpeed")
s1.SetUniform1f(slen,"BlockSize")

t1 = gfw.Texture(int(slen),1,1,4,1)
fb = gfw.FrameBuffer(t1)
p = pyaudio.PyAudio()
stream = p.open(format = pyaudio.paFloat32,channels = 1,rate = 44100,frames_per_buffer = slen,output = True)
poly.AddTexture(t1,0)
f = 100
for i in range(10000):
	
	gfw.SetActiveShader(s1)
	s1.SetUniform1f(f,"freq")
	s1.SetUniform1f(i,"phase")
	fb.Bind()
	poly.Draw(0,0,0)
	bim = fb.GetBufferImage()
	fv = bim.AsFloatVector()
	stream.write(gfw.GetFloatVectorStr(fv),num_frames=slen	)

	fb.UnBind()

	gfw.SetActiveShader(s2)
	poly.Draw(0,0,0)
	gfw.Refresh()
	f = gfw.GetMousePos().X*4
	#time.sleep(0.1)
time.sleep(5)

p.terminate()
