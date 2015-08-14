#!/usr/bin/python2
import sys

if len(sys.argv) != 3:
    print "Usage : mydiff.py file1 file2"
    exit()

f1 = open(sys.argv[1]).read()
f2 = open(sys.argv[2]).read()

l1 = min(len(f1), len(f2))
l2 = max(len(f1), len(f2))

if l1 == l2 or l1+1 == l2:
    pass
else:
    print "warning: file size not equ"

print "file size:", l1
print "file check ok" if f1[:l1] == f2[:l1] else "file check failed"

