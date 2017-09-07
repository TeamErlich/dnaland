/*
 * FreqData.h
 *
 *  Created on: Sep 25, 2014
 *      Author: jkpickrell
 */

#ifndef FREQDATA_H_
#define FREQDATA_H_

#include "Ancestry_params.h"


class FreqData{
public:
	FreqData();
	FreqData(Ancestry_params *);
	Ancestry_params *params;


	void read_freqs(string);
	map<string, int> pop2id;
	map<int, string> id2pop;

	map<string, map<int, int> > chr2pos2index;
	map<int, pair<string, int> > index2pos;


	vector<map<string, string> > SNPinfo;
	//gsl_matrix *alfreqs;
	vector<vector<pair<int, int> > > allele_counts;

	void print_alfreqs(string);
	pair<string, string> get_als(pair<string, int>);
	int get_npop();
	double get_f(pair<string, int>, int);
	double get_f(pair<string, int>, string);
	double get_weighted_f(pair<string, int>, map<string, float>);
	pair<int, int> get_counts(pair<string, int>, string);
	pair<int, int> get_counts_index(int, string);
	int get_index(pair<string, int>);
	double get_lk(pair<string, int>, string, double);
	double lbinom(int, double, int);
	vector<string> get_popnames();
private:
	void check_a(string);
	int npop, nsnp;
};



#endif /* FREQDATA_H_ */
