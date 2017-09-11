A selection of source code for DNA.Land (https://dna.land). Further information can be found in README files in each folder.

## DNA.Land Frontend
The HTML, Javascript, and CSS components for the the following reports on the website: ancestry composition, relative matching, and relatives-of-relatives (the individuals your relatives are related to, as detected by germline+ERSA). DNA.Land runs on Python Flask, and a dummy Flask app is included with demo data to generate the reports from their templates. 

To view the pages, run "python run.py" on your local machine or a server, and in a web browser, visit:

(your server ip addess):5000/[feature]

where [feature] is one of the following: "ancestry", "relative-finder", "relatives-of-relatives". 


## Ancestry Inference
The algorithm developed by Joe Pickrell to compute ancestry composition from input genotype files on DNA.Land. This is based on STRUCTURE [1] by Pritchard et al. The output .Q file consists of a list of ancestry groups and percentages. In addition to the provided instructions, you may need to install the following packages:

sudo apt-get install gsl-bin libgsl-dev

(installing htslib)

wget https://github.com/samtools/htslib/releases/download/1.5/htslib-1.5.tar.bz2

tar -xjvf htslib-1.5.tar.bz2

cd htslib-1.5

make

sudo make install

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:[your htslib path]


## Germline modification for DNA.Land relative matching
A modification of the Germline algorithm [2] by Gusev et al. to iteratively process individual users as they arrive to DNA.Land. For each user, O(N) comparisons to the existing user set are performed, rather than rerunning O(N^2) comparisons across all individuals. In addition to the instructions provided by Gusev et al., you may need to install the following packages:

sudo apt-get install openssl

sudo apt-get install libssl-dev

sudo apt-get install libleveldb-dev


## ERSA modification for DNA.Land relative matching
A modification of the ERSA algorithm [3] by Huff et al. for identifying close relatives by IBD segments. This includes some improvements in code efficiency and features like identification of self-matches, or users who uploaded their genome to DNA.Land twice, and improved differentation between avuncular and first degree relationships such as full siblings.


## References

[1] Pritchard, Jonathan K., Matthew Stephens, and Peter Donnelly. "Inference of population structure using multilocus genotype data." Genetics 155.2 (2000): 945-959.

[2] Gusev, Alexander, et al. "Whole population, genome-wide mapping of hidden relatedness." Genome research 19.2 (2009): 318-326.

[3] Huff, Chad D., et al. "Maximum-likelihood estimation of recent shared ancestry (ERSA)." Genome research 21.5 (2011): 768-774.
