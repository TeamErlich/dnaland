# ersa
`ersa` estimates the combined number of generations between pairs of individuals using a [Germline](http://www1.cs.columbia.edu/~gusev/germline/) matchfile as input.  It is an implementation of [Huff et. al. (2011) Maximum-Likelihood estimation of recent shared ancenstry (ERSA)](http://genome.cshlp.org/content/21/5/768.full) and [Li et. al. (2014) Relationship Estimation from Whole-Genome Sequence Data](http://journals.plos.org/plosgenetics/article?id=10.1371/journal.pgen.1004144). ERSA receives as input sets of matching segments between pairs of individuals, and estimates whether these segments are shared IBD or IBS, as well as the maximum likelihood degree of relatedness between each pair.

For use in DNA.Land, the code has been rewritten to greatly improve runtime performance, and includes more extensive documentation. A few changes to probability calculations were also included: the correction for avuncular calculations in Li et al. (Eq. 9) has been included. In DNA.Land, a PLINK IBS calculation is run to distinguish self-matches from full-sibling matches. Masking of spurious IBD regions in Li et al. is also included. 

Lastly, the output set of matching relatives and segments can be optionally written to an SQLite database for fast querying of individuals. Two tables are generated: one storing pairs of matching individuals and attributes such as the number and length of matching segments, and another for storing the locations in bp of the matching segments.



Copyright (c) 2015 by
- Richard Muñoz (rmunoz@nygenome.org)
- Jie Yuan (jyuan@nygenome.org)
- Yaniv Erlich (yaniv@nygenome.org)

License: GNU GPL v3 (see LICENSE.txt)

## Requirements
`ersa` requires Python 3.4 or greater.  In addition, the following packages for python3 must be installed prior to the installation process:

- `numpy`
- `setuptools`

## Install
First, install python3, setuptools, and numpy.  Then clone from github:

    git clone https://github.com/TeamErlich/dnaland.git
    cd dnaland/improved-ersa-relative-matching
    sudo python3 setup.py install

This will add `ersa` to your path and download additional required python packages.

## Example
Requires a Germline matchfile as input.  By default, summary results are sent to stdout, but can also be directed to a file.  Alternatively, full results can be directed to a database, as in the example below.

    $ ersa -D "sqlite:///ersa_results.db" input.match

This creates `ersa_results.db` in current directory.

For additional options, use

    $ ersa -h

## Notes
On inserting results into a database, if a comparison between a pair of individuals exists, `ersa` will mark the old result as deleted (i.e., soft delete the result).  To physically delete these old results from the database, a utlity `ersa_delete_rows` is also provided:

    $ ersa_delete_rows "sqlite:///ersa_results.db"

