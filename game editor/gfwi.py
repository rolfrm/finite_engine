import gfw
import types
import Image
Textures = {}
Polygons = {}

_channelsConv = {'RGBA':4,'RGB':3,'LA':2,'L':1}

def GetTexture(name):
    if not name in Textures:
        try:
            img = Image.open(name)
            global _channelsConv
            Textures[name] = gfw.Texture(img.tostring(),img.size[0],img.size[1],_channelsConv[img.mode],1)
        except Exception as e:
            print e
            return 0
    return Textures[name]
    
class Drawable:
    def __init__(self, position, rotation,scale,polygon):
        self.Pos = position
        self.Rotation = rotation
        self.Poly = polygon
        self.Scale = scale
        self.Texture = 0
    def _Draw(self):
        s = gfw.GetActiveShader()
        s.SetUniform2f(self.Pos[0],self.Pos[1],"offset")
        s.SetUniform2f(self.Scale[0],self.Scale[1],"scale")
        s.SetUniform1f(self.Rotation,"rotation")
        if self.Texture:
            tex = GetTexture(self.Texture)
            if tex:
                tex.Bind(0)
        self.Poly._Draw()

class Polygon:
    def __init__(self,verts,indexes=[],colors = [],uvs = []):
        self.verts = verts
        self.indexes = indexes
        if self.indexes == []:
            self.indexes = range(0,len(self.verts))
        self.uvs = uvs
        self.colors = colors
        self.Poly = 0
    def _Draw(self):
        if not self.Poly:
            if not self.Hash() in Polygons:
                print "Making polygon"
                Poly = gfw.Polygon(gfw.FloatVector(self.verts),gfw.UIntVector(self.indexes),gfw.FloatVector(self.colors),gfw.FloatVector(self.uvs))
                Polygons[self.Hash()] = Poly
            self.Poly = Polygons[self.Hash()]  
        self.Poly.Draw()
    def Hash(self):
        return hash(str(self.verts + self.indexes + self.uvs + self.colors))
    def __getstate__(self):
        return (self.verts,self.indexes,self.colors,self.uvs)
    def __setstate__(self,state):
        self.__init__(state[0],state[1],state[2],state[3])

RenderList = []

KeyHandlerList = []
ScriptList = []

class KeyEvent:
    def __init__(self,key,action,charkey):
        self.Key = key
        self.Action = action
        self.CharKey = charkey
    def __str__(self):
        return "Key: {0} Action: {1} Char rep: {2}".format(self.Key,self.Action,self.CharKey)
def LoadObject(obj):
    if isinstance(obj,KeyEventHandler):
        KeyHandlerList.append(obj)
    if isinstance(obj,Scripted):
        ScriptList.append(obj)
    if "_Draw" in dir(obj):
        print "Loading"
        RenderList.append(obj)

class KeyEventHandler:
    def KeyEvent(evt):
        pass
class Scripted:
    def ScriptUpdate(self):
        pass


def UnloadObject(obj):
    print "TRying to load",obj
    if isinstance(obj,KeyEventHandler):
        KeyHandlerList.remove(obj)
    if isinstance(obj,Scripted):
        ScriptList.remove(obj)
    if "_Draw" in dir(obj):
        RenderList.remove(obj)



def UpdateUI():
    gfw.EnableState(gfw.ALPHA_BLEND)
    gfw.ClearBuffer()
    for i in RenderList:
        i._Draw()
    for i in ScriptList:
        i.ScriptUpdate()
    gfw.Refresh()
    keyevents = gfw.GetKeyEvents()
    keyev = []
    for i in range(keyevents.size()):
        j = keyevents[i]
        kev = KeyEvent(j.key,j.action,j.charKey)
        for l in KeyHandlerList:
            l.KeyEvent(kev)
