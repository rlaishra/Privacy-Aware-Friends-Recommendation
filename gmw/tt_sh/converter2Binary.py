#!/usr/bin/env python

# count the number of common SNPs in two files

import sys
import operator
import argparse   


def calcHamm(args):
	mHamm = {}
	x=0
      	int1= open("input1.txt",'w')
	with open(args.input1, 'r') as f:
		for l in f:
			l = l.strip()
			r = l.split()
			s=""
			k=""
			if(len(r[2])>1):
				k=r[2][2:]
			if r[3] is 'A':
				k+=str(1)
			elif r[3] is 'T':
				k+=str(2)
			elif r[3] is 'G':
				k+=str(3)
			else :
			        k+=str(4)
			num=long(k)
			temp=""
			while num!=0:
				z=num%2
				num=num/2
				temp += str(z) + "\t"
			while len(temp) < 70:
				temp+="0\t"			
			if not r[0] in mHamm:
				mHamm[r[0]]=x
				x += 1	
			s += "{0}{1}".format(mHamm[r[0]],r[1])
			a=long(s)
			t=""
			while a!=0:
				z = a%2
				a = a/2
				t += str(z) + "\t"
			while len(t) < 80:
				t += "0\t"
			t+=temp
			t += "\n"
                       	int1.write(t)
		int1.close()
	mHamm={}	
	x=0
        int2= open("input2.txt",'w')
	with open(args.input2, 'r') as f:
		for l in f:
			l = l.strip()
			r = l.split()
			s =""
			k=""
			if(len(r[2])>1):
				k=r[2][2:]
			if r[3] is 'A':
				k+=str(1)
			elif r[3] is 'T':
				k+=str(2)
			elif r[3] is 'G':
				k+=str(3)
			else :
			        k+=str(4)
			num=long(k)
			temp=""
			while num!=0:
				z=num%2
				num=num/2
				temp += str(z) + "\t"
			while len(temp) < 70:
				temp+="0\t"
		        if not r[0] in mHamm:
                                mHamm[r[0]]=x
                                x +=1 
                        s += "{0}{1}".format(mHamm[r[0]],r[1])
                        a=long(s)
                        t=""
                        while a!=0:
                                z = a%2
                                a = a/2
                                t += str(z) + "\t"
                        while len(t) < 80:
                                t += "0\t"
			t+=temp
               		t += "\n"	
                        int2.write(t)
                int2.close()



if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='count # of common SNPs')

	parser.add_argument('-i1', '--input1', help='data 1')
	parser.add_argument('-i2', '--input2', help='data 2')
	parser.add_argument('--v', help='print records', action='store_true')
	parser.set_defaults(func=calcHamm)

	args = parser.parse_args()
	args.func(args)


