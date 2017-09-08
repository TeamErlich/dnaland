/*
 * Data23.h
 *
 *  Created on: Sep 20, 2014
 *      Author: jkpickrell
 */

#ifndef DATA23_H_
#define DATA23_H_

#include "FreqData.h"

class Data23{
public:
	Data23();
	~Data23();
	Data23(Ancestry_params*, FreqData*, gsl_rng *);
	FreqData *ref;
	Ancestry_params *params;
	gsl_rng *r;
	gsl_vector *Q, *tmpQ;
	gsl_matrix *P, *tmpP;
	vector<pair<string, int> > info; //vector of [chr, pos]
	vector<int> refindices; //hold indices of snps in reference
	void set_refindices();
	vector<bool> data;
	map<pair<string, int>, bool> datamap;
	void set_datamap();
	vector<string> refpopnames;
	void read_file(string);
	void read_file_pileup(string);
	void read_vcf(string);
	void print();
	void printP(string);
	void printQ(string);
	void updateP();
	void updateQ();
	void EM();
	void EM(int);
	void squarem1();
	void squarem2();
	void setQ(vector<double>);

	void init(); //initialize P, Q
	void init_weights();

	void bootstrap(gsl_rng *);
	void sampleN(int, gsl_rng*);
	void set_weights();
	void set_weights(vector<int>);
	double llk();
	double llk(vector<double>);
	double llk_xv(vector<int>);
	double llk(vector<int>, int);
	void GSL_optim(vector<int>, int);
	void set_single_weight(int);
	vector<double> params2vec();
	void assignfromvec(vector<double>);
	//void test_all();
	void test_subset(string);
	void exhaustive_test(string);
	void filter_pops_missing();
	double lnvecdens(vector<double>);
	double lndgauss(double, double);
//	void MCMC(gsl_rng*);
//	int MCMC_update(gsl_rng*);
	void propose_newpi(int, gsl_rng*);
	double compute_prior(vector<double>);
	double calc_f2(int);
	double calc_f2(vector<int>);

	vector<double> pi; //fractions
	vector<double> alpha; //in exponential form

	// chromosome segments
	vector<pair<int, int> > chrsegments;
	void set_chrsegments();

	// relfinder functions
	vector<bool> relfinder_overlap(Data23 *);
	vector<pair<double, double> > relfinder(Data23 *, map<string, float>);
	double relfinder_llk(float, Data23*, vector<bool>, vector<double>);

private:
	void dropuseless();
	int nsnp;
	int npop, originalnpop;
	double current_llk;
	int nonzero_count;
};

struct GSL_params{
        Data23 *d;
        vector<int> which;
        int skip;
};

extern double GSL_llk(const gsl_vector *, void *GSL_params);
extern double GSL_f2(const gsl_vector *, void *GSL_params);


#endif /* DATA23_H_ */
