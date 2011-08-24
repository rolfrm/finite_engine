import numpy
import random
import Image
import numpy.random
import opencv.highgui as highgui


def saveMap(Map,name):
	min = Map.min()
	max = Map.max()
	rng = max-min
	rescaled = (Map - min)/rng*255
	rescaled = rescaled.astype(numpy.uint8)
	im = Image.fromarray(rescaled)
	im.save(name)
	
def saveMap2(Map,name,high,low):
	min = low
	max = high
	rng = max-min
	rescaled = (Map - min)/rng*255
	print (rescaled.min(),rescaled.max())
	rescaled = rescaled.astype(numpy.uint8)
	im = Image.fromarray(rescaled)
	im.save(name)


def diamondStep(curmap,scaling):
	sizey = curmap.shape[1]
	sizex = curmap.shape[0]
	for i in range(0,sizey):
		odd = i%2
		for j in range(0,(sizex-1)/2 + odd):
			x = j*2 + 1 - odd
			y = i
			nval = 0
			n = 0
			if not x == 0:
				nval += curmap[x-1][y]
				n += 1
			if not x ==sizex-1:
				nval += curmap[x+1][y]
				n +=1
			if not y == 0:
				nval += curmap[x][y-1]
				n += 1
			if not y == sizey-1:
				nval += curmap[x][y+1]
				n +=1
			nval /=n
			nval += random.random()*scaling			
			#	nval += random.gauss(0,1)*scaling
			curmap[x,y] = nval
	return curmap
	
def squareStep(curmap,scaling):
	nowshape = curmap.shape
	newshape = (nowshape[0]*2-1,nowshape[1]*2-1)
	deltas = nowshape[0]-1
	newmap = numpy.zeros(newshape)
	for i in range(0,nowshape[0]):
		for j in range(0,nowshape[1]):
			x = i*2
			y = j*2
			newmap[x][y] = curmap[i,j]
	for i in range(0,deltas):
		for j in range(0,deltas):
			x = i*2 +1
			y = j*2 +1
			nval = (newmap[x-1][y-1] + newmap[x+1][y-1] + newmap[x-1][y+1] + newmap[x+1][y+1])/4
			nval += random.random()*scaling
			#nval += random.gauss(0,1)*scaling
			newmap[x][y] = nval
			
	return newmap
	
def MakeMap():
	Map = numpy.zeros((7,7))
	#Map[2][2] = 5
	for i in range(0,4):
		scaling = 0.5**float(i)
		Map = squareStep(Map,scaling)
		Map = diamondStep(Map,scaling)
		print(Map.shape)
	return Map

def ErosionFilter(Map, iterations):
	water = numpy.ones(Map.shape)*0
	fl = numpy.zeros(water.shape)
	fr = numpy.zeros(water.shape)
	ft = numpy.zeros(water.shape)
	fb = numpy.zeros(water.shape)

	dt = 0.3

	def fixheights(h,water):
		a = h > water
		#print a
		h[a] = water[a]
		a = h < 0
		h[a] = 0

	#iterate water flow
	#water += 0.1
	g = 1
	l = 1
	A = 1			
	totalheight = Map+water

	padding = numpy.zeros(water.shape)

	saveMap(water,"h.png")
	saveMap(Map,"m.png")
	saveMap(totalheight,"th.png")
	counter = 0
	#water += 0.5
	for i in range(0,iterations):
		
		counter +=1	
		hl = padding.copy()
		hr = padding.copy()
		ht = padding.copy()
		hb = padding.copy()

		hl[1:,:] -=  totalheight[:len(totalheight)-1,:] - totalheight[1:,:]
		hr[:-1,:] -= totalheight[1:,:] - totalheight[:-1,:]
		ht[:,1:] -=  totalheight[:,:len(ht)-1] - totalheight[:,1:]
		hb[:,:-1] -= totalheight[:,1:] - totalheight[:,:-1]
		fixheights(hl,water)
		fixheights(hr,water)
		fixheights(hb,water)
		fixheights(ht,water)
		fl = fl + hl*(g/l*dt)
		fr = fr + hr*(g/l*dt)
		fb = fb + hb*(g/l*dt)
		ft = ft + ht*(g/l*dt)
	
		outflow = fl+fr+fb+ft
		outdv = dt*outflow
		k = water/outdv
		print "k:",k
		a = abs(k)<1
	
		fl[a] = fl[a]*k[a]
		fr[a] = fr[a]*k[a]
		fb[a] = fb[a]*k[a]
		ft[a] = ft[a]*k[a]
		#fl = fl*k
		#fr = fl*k
		#fb = fl*k
		#ft = fl*k

		dv = numpy.zeros(water.shape)
		#in flows:
		dv[:-1,:] += fl[1:,:]
		dv[1:,:] += fr[:-1,:]
		dv[:,:-1] += ft[:,1:]
		dv[:,1:] += fb[:,:-1]
		#out flows:
		dv -=fl+fr+ft+fb
		dv *=dt
		#print "water:",water
		water += dv
		totalheight = Map+water
		#print totalheight.max()
		if i%1 == 0:
			m = totalheight
			print (m.min(),m.max())
			nim = (m-m.min() )/(m.max()-m.min() )
			highgui.cvShowImage("oh hai",nim)
			k = highgui.cvWaitKey(1)
			if k == 'a' and counter > 10:
				water += 0.1
				counter = 0
			if k == 'b' and counter > 10:
				water[:water.shape[0]/8,:water.shape[1]/8] += 1
	return totalheight

if __name__ == "__main__":
	Map = MakeMap()
	ErosionFilter(Map,10000)
	
