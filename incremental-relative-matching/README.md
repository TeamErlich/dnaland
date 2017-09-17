This is a modification of the software package [GERMLINE](http://www.cs.columbia.edu/~gusev/germline/) by Gusev et al. GERMLINE accepts genotype data from multiple individuals in PED/MAP or HapMap format, and identifies runs of shared segments between pairs of individuals.

We have adapted GERMLINE for use in DNA.Land [(website)](https://dna.land)[(preprint)](http://www.biorxiv.org/content/early/2017/05/09/135715) in our relative matching feature: the sets of shared segments serve as input to our implementation of the ERSA algorithm, in order to identify close and distant relatives among pairs of DNA.Land users. In DNA.Land, we have exclusively used the PED/MAP format for input. Also, some changes have been made to the code to improve runtime efficiency.

# New Functionality

The original implementation performs N^2 comparisons between all pairs of individuals in an input PED file. As this software needs to be executed for each of a continuous stream of new DNA.Land users, we adapted this code so that comparisons between only new users and the N existing users are performed, leading to a significant improvement in runtime. (Previously, the entire database had to be rerun for every new batch of users.) To perform this calculation, the following command line options have been added (only one of these options is to be provided for a given execution):

>`-new_ids`: a list of new individuals to run GERMLINE on
>`-single_id`: a single new individual to run GERMLINE on
>`-old_ids`: specifies a list of old individuals to ignore

All specified files are in PED format. Additionally, the new individuals should already have been added to the existing database.

# Install

```
sudo apt-get update
sudo apt-get g++ openssl libssl-dev libleveldb-dev
make
```

Run `germline` with the `-h` option to see a complete list of arguments.

# Example Usage

An example execution is shown below using files included in the /test folder. Here `CEU.22.ped` contains all individuals, and `CEU.22.ped.newonly` contains the "new" individuals to use for germline comparisons to all other individuals.

>germline -snpmap_file test/CEU.22.map -ped_file test/CEU.22.ped -new_ids test/CEU.22.ped.newonly -out_file_prefix outputfile
