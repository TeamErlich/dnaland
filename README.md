A selection of source code for DNA.Land (https://dna.land), a website for crowdsourcing the collection of genomic data. The preprint can be found [here](http://www.biorxiv.org/content/early/2017/05/09/135715). The included features are:

-`dnaland-report-viewer`: A Flask app running a basic version of the DNA.Land Frontend, including a hypothetical user's main profile page and browsable reports displaying a user's ancestry and potential relatives on DNA.Land.

-`ancestry-computation`: A method for inferring the ancestry composition of DNA.Land users, based on the [STRUCTURE](http://www.ncbi.nlm.nih.gov/pubmed/10835412)  model by Pritchard, Stephens, and Donnelly.

-`incremental-relative-matching`: A modification of the [GERMLINE](http://www.cs.columbia.edu/~gusev/germline/) package by Gusev et al., modified to allow more efficient processing of a continuous stream of new DNA.Land users.

-'improved-ersa': A re-implementation of the [ERSA](http://genome.cshlp.org/content/21/5/768.full) model by Huff et al., with some improvements to efficiency and modifications of probabilities for some relationships.

Further information can be found in README files in each folder.

