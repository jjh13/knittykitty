from knitcode import *
from PIL import Image
import PIL.ImageOps



def img2kc(image):
    width = len(image[0])*2
    start = 100 - width/2
    end = start + width
    # insert 0s into the patterns
    for i, r in enumerate(image):
        lcode = ""
        op = "LS" if i % 2 == 0 else "RS"
        for pix in r:
            if pix > 0:
                lcode += ("%s,N," % op) if i%2==0 else ("N,%s," % op)
            else:
                lcode +="N,N,"
            op = "RS" if op == "LS" else "LS"
        lcode = lcode[:-1] + ";"
        print "L(%d:%d) %s" % (start,end,lcode)
        print "L(%d:%d) N%d;" % (start,end,width)
        print "K(%d:%d) K%d;" % (start,end,width)


    # setup all the lefts, then setup rights then pad with an N

image = Image.open('good_design2.png')
image = image.convert('L')
pix =  image.load()

qlevels = set()
m = image.size[0]
n = image.size[1]
img = [ ([ pix[i,j] for i in xrange(m)]) for j in xrange(n)]

imgtst = [
    [0,0,0,1,0,0,0],
    [0,0,1,1,0,0,0],
    [0,0,1,1,1,0,0],
    [0,1,0,0,1,0,0],
    [0,1,0,0,0,1,0],
]

img2kc(img)
