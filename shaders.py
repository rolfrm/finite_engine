ObjectLightning =["""
uniform vec2 Zoom;
attribute vec2 pos; 
attribute vec3 color;
attribute vec2 uv;
uniform float Xoff;
uniform float Yoff;
uniform float Rotation;
uniform int tex0Active;
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
vuv = uv;
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
 //gl_Position=vec4((npos - CameraPosition)/Zoom,0.0,1.0);
 gl_Position=vec4((npos)/Zoom,0.0,1.0);
 }
""", """
uniform sampler2D tex0;
uniform int tex0Active;
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
	vec3 globalColor = vec3(1,1,1);
	vec3 col = vColor;
	vec4 tex;
	if( tex0Active == 1){
		 tex= texture2D(tex0,vuv);	
		col +=  tex.xyz;
	}
	vec3 ncol = vec3(0.0,0.0,0.0);
	for(int i =0; i< NumLights;i++){
		 float namb = 1.0/(length(vMultiLightDir[i]));
		 float ndiff = max(dot(normalize(normal),normalize(vMultiLightDir[i])),0.0)*1.0/(length(vMultiLightDir[i])/5.0);
		 ncol += col*(min(1.0,namb) + ndiff*0.5)*MultiLightColor[i]*MultiLightIntensity[i]; 
	}
	ncol += col*globalColor;
	if(tex0Active == 1){
		gl_FragColor= vec4(tex.rgb,tex.a);
	}else{
		gl_FragColor= vec4(ncol,1);

	}
}
"""]
