#!/usr/bin/env python2
import numpy as np
from matplotlib import pylab as pl
import sys
import struct

if len(sys.argv) != 3:
    print "Usage : convert_pic.py img output"
    exit()


file=sys.argv[1]

pic = pl.imread(file)
pic = pic*1.0 / pic.max()
size = [3,3,2]
p = []
dd = np.zeros(pic.shape[:-1]).astype(np.int8)
for i in range(3):
    p.append(np.minimum((pic[:,:,i]*(1<<size[i])).astype(np.int32), (1<<size[i])-1))
    dd = (dd << size[i]) | p[i]

data = struct.pack('>I', pic.shape[0])+struct.pack('>I', pic.shape[1])
for val in dd.flat:
    data += struct.pack('>B', val)

print "dump size:", len(data)
open(sys.argv[2], 'w').write(data)
