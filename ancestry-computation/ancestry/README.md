Ancestry is a program for inferring the "genetic ancestry" of an individual from their genotype or sequence data. It requires a reference set of labeled individuals, and models the test individual as a mixture of these reference sets. The underlying model is the [Pritchard, Stephens, and Donnelly STRUCTURE model](http://www.ncbi.nlm.nih.gov/pubmed/10835412). 

More detailed documentation to come. 

#Dependencies
1. [The GNU Scientific Library](http://www.gnu.org/software/gsl/)
2. [The Boost libraries](http://www.boost.org/)

#Install:

>./configure

>make

#Arguments
-i input matrix of allele counts in the reference populations (see Downloads)

this file must be gzipped.

-g23 23andMe-style genotype file of test individual

Alternative, use -gp for a pileup file (make sure you have run pileup *without* a reference genome)

-gp gzipped pileup file

#Usage

The standard way to use this software is to estimate the ancestry of a test individual using a pre-defined reference panel. A useful (albeit not perfect) reference panel is provided in the Downloads section. Coordinates for this reference panel are relative to hg19, note we perform no checks to ensure the coordinates of the input file are also on hg19, so please double-check this before running the software. 

Ancestry inference takes two steps. First, run the ancestry inference model on (for example) a random 100,000 SNPs some number of times (here 10):

>ancestry -i bigmerge_v2_ref.gz -b 10 -ranN 100000 -g23 testing/genome_Joseph_Pickrell_Full_20140918123017.txt.gz -o test

Then summarize the boostrap replicates (using the cluster names in the file 'cluters', also provided in the Downloads section):

>python summarize_bootstrap.py clusters test*.Q

This will print the summarized ancestry inference to stdout.
