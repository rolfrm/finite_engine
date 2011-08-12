import numpy
import random
import Image
import numpy.random
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
			nval += random.gauss(0,1)*scaling
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
			nval += random.gauss(0,1)*scaling
			newmap[x][y] = nval
			
	return newmap
	

Map = numpy.zeros((5,5))
Map[2][2] = 5
for i in range(0,7):
	scaling = 0.5**float(i)
	Map = squareStep(Map,scaling)
	Map = diamondStep(Map,scaling)

water = numpy.zeros(Map.shape)


	
min = Map.min()
max = Map.max()
rng = max-min
rescaled = (Map - min)/rng*255
rescaled = rescaled.astype(numpy.uint8)
im = Image.fromarray(rescaled)
im.save('test.png')