/*
 * Ancestry_params.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: jkpickrell
 */

#include "Ancestry_params.h"

Ancestry_params::Ancestry_params(){
	outstem = "ancestry";
	beta_prior.first = 0.5;
	beta_prior.second = 0.5;
	print_warnings = true;
	missing_filter = 0.02;
	burnin = 1000;
	sampfreq = 1;
	nsamp = 10000;
	inpops.clear();
	MCMC_gauss_SD = 0.1;
	input23 = false;
	inputVCF=false;
	run_exhaustive = false;
	epsilon = 0.01;
	maxiter = 1000;
	stepmin0=1;
	stepmax0=1;
	tol=1e-5;
	mstep = 4;
	EM =false;
	EMsteps = 50;
	kr = 1;
	dropepsilon = 1e-3;
	skipchr = "";
	skip = false;
	bootstrap = false;
	seed = 0;
	ransnp = false;
	noP = false;
	doranN = false;
	ranN = 0;
	ranfrac = 0.0;
	nboot = 0;
}


void Ancestry_params::print(){
	cout << ":: reference panel counts: "<< infile << "\n";
	cout << ":: test sample file: "<< sampfile << "\n";
	cout << ":: input format : ";
	if (input23){ cout << "23andme\n";}
	else if (inputVCF){ cout << "VCF/BCF\n";}
	else cout << "pileup\n";
	cout << ":: output stem: "<< outstem << "\n";
	//cout << ":: reference populations: ";
	//for (set<string>::iterator it = inpops.begin(); it != inpops.end(); it++) cout << *it << ",";
	//cout << "\n";
	cout << ":: algorithm: ";
	if (EM) cout << "EM\n";
	else cout << "SQUAREM\n";
	if (bootstrap) cout <<":: bootstrap: "<< nboot<< "\n";
	else cout <<":: bootstrap: no\n";
	if (skip) cout <<":: skipchr: "<< skipchr << "\n";
	cout << ":: tol: "<< tol <<"\n";
	cout << ":: seed: "<< seed <<"\n";
	if (ransnp){
		cout << ":: ransnp: "<< ranfrac <<"\n";
	}
	if (doranN){
		cout << ":: ranN: "<< ranN <<"\n";
	}
	cout << ":: noP: "<< noP <<"\n";
}
