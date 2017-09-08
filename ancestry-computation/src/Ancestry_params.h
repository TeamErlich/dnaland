/*
 * Ancestry_params.h
 *
 *  Created on: Sep 16, 2014
 *      Author: jkpickrell
 */

#ifndef ANCESTRY_PARAMS_H_
#define ANCESTRY_PARAMS_H_
#include <string>
#include <map>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>
#include <cstdlib>
#include "gzstream.h"
#include "CmdLine.h"
#include <sys/stat.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/math/special_functions/binomial.hpp>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_nan.h>
#include <gsl/gsl_sys.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_multimin.h>
#include <algorithm>
#include <htslib/synced_bcf_reader.h>
#include <htslib/hts.h>

using std::string;
using std::vector;
using std::list;
using std::stack;
using std::map;
using std::set;
using std::multiset;
using std::cout;
using std::cin;
using std::endl;
using std::ostream;
using std::ofstream;
using std::stringstream;
using std::pair;
using std::iterator;
using std::pair;
using std::make_pair;
using std::fstream;
using std::ifstream;
using boost::math::binomial_coefficient;



class Ancestry_params{
public:
	Ancestry_params();
	string infile;
	string outstem;
	string sampfile;
	bool print_warnings;
	float missing_filter;
	int burnin, sampfreq, nsamp;
	double MCMC_gauss_SD;
	set<string> inpops;
	pair<double, double> beta_prior; //prior on allele frequency distribution
	void print();
	bool input23;
	bool inputVCF;

	bool bootstrap; //resample sites
	int nboot;

	bool run_exhaustive;
	double epsilon;
	int maxiter;
	bool skip;
	string skipchr;
	double dropepsilon;
	int seed;
	double stepmax0, stepmin0, mstep, tol, kr; //for squarem
	bool EM; //run EM (not squarem) default to false
	int EMsteps; //run some basic EM steps to start

	bool ransnp;
	double ranfrac; // if we're randomly sampling SNPs, this is the fraction to keep
	bool noP; //don't print the P matrix


	// randomly sample N
	int ranN;
	bool doranN;
};


#endif /* ANCESTRY_PARAMS_H_ */
