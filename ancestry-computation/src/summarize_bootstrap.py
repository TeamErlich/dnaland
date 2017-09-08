#!/usr/bin/python

import sys, os

pop2clust = dict()
clust2cont = dict()
pop2cont = dict()
cont2clust = dict()

infile = open(sys.argv[1])
for line in infile.xreadlines():
	line = line.strip().split()
	pop = line[0]
	clust = line[1]
	cont = line[2]
	pop2clust[pop] = clust
	pop2cont[pop] = cont
	clust2cont[clust] = cont
	cont2clust[cont] = clust

NBOOT = 0
TRIVIAL_CUTOFF = 0.01
BOOTCUTOFF = 0.5

pop2fracs = dict()
clust2fracs = dict()
cont2fracs = dict()

bootfiles = sys.argv[2:]
for f in bootfiles:
	infile = open(f)
	p2f = dict()
	cl2f = dict()
	co2f = dict()
	for line in infile.xreadlines():
		line = line.strip().split()
		pop = line[0]
		
		clust = pop2clust[pop]
		cont = pop2cont[pop]
		frac = float(line[1])
		if frac > TRIVIAL_CUTOFF:
			p2f[pop] = frac
			if cl2f.has_key(clust)==0:
				cl2f[clust] = 0
			if co2f.has_key(cont) ==0:
				co2f[cont] = 0
			cl2f[clust] = cl2f[clust]+frac
			co2f[cont] = co2f[cont]+frac
	for pop in pop2clust.keys():
		if pop2fracs.has_key(pop)==0:
			pop2fracs[pop] = list()
		if p2f.has_key(pop):
			pop2fracs[pop].append(p2f[pop])
		else:
			pop2fracs[pop].append(0)
	for clust in clust2cont.keys():
		if clust2fracs.has_key(clust)==0:
			clust2fracs[clust] = list()
		if cl2f.has_key(clust):
			clust2fracs[clust].append(cl2f[clust])
		else:
			clust2fracs[clust].append(0)
	for cont in cont2clust.keys():
		if cont2fracs.has_key(cont)==0:
			cont2fracs[cont] = list()
		if co2f.has_key(cont):
			cont2fracs[cont].append(co2f[cont])
		else:
			cont2fracs[cont].append(0)	
	NBOOT = NBOOT+1

toprint = dict()
for clust, fracs in clust2fracs.items():
	count = 0
	count1 = 0
	sum = 0
	for f in fracs:
		if f > 0:
			sum = sum+f
			count1 = count1+1
		count = count+1
	if float(count1)/float(NBOOT) > BOOTCUTOFF:
		meanf = sum/float(count)
		toprint[clust] = meanf
for cont, fracs in cont2fracs.items():
	count = 0
	count1 = 0
        sum = 0
        for f in fracs:
                if f > 0:
                        sum = sum+f
			count1 = count1+1
                count = count+1
        if float(count1)/float(NBOOT) > BOOTCUTOFF:
                meanf = sum/float(count)
		for clust, cont2 in clust2cont.items():
			if cont == cont2 and toprint.has_key(clust):
				meanf = meanf-toprint[clust]
                if meanf > TRIVIAL_CUTOFF:
			toprint["AMBIG_"+cont] = meanf

total = 0
for p, f in toprint.items():
	total = total+f

ambig = 1-total
toprint["AMBIGUOUS"] = ambig
for l, f in toprint.items():
	print l, f
