/*
 * FreqData.cpp
 *
 *  Created on: Sep 25, 2014
 *      Author: jkpickrell
 */

#include "FreqData.h"


FreqData::FreqData(){

}

FreqData::FreqData(Ancestry_params* p){
	params = p;
	read_freqs(p->infile);
}


int FreqData::get_npop(){
	return npop;
}


double FreqData::get_f(pair<string, int> cpos, int popindex){
	string chr = cpos.first;
	int pos = cpos.second;
	if (chr2pos2index.find(chr) != chr2pos2index.end() and chr2pos2index[chr].find(pos) != chr2pos2index[chr].end()){
		int snpindex = chr2pos2index[chr][pos];
		int count1 = allele_counts[snpindex][popindex].first;
		int count2 = allele_counts[snpindex][popindex].second;
		return (double) count1/ ( (double) count1 + (double) count2);
		//return gsl_matrix_get(alfreqs, snpindex, popindex);
	}
	else{
		cerr << "ERROR: can't find "<< chr << ":"<< pos << "\n";
		exit(1);
	}
}

vector<string> FreqData::get_popnames(){
	vector<string> toreturn;
	for (map<string, int>::iterator it = pop2id.begin(); it != pop2id.end(); it++){
		toreturn.push_back(it->first);
	}
	return toreturn;

}

pair<int, int> FreqData::get_counts_index(int snpindex, string pop){
	int popindex = pop2id[pop];
	return allele_counts[snpindex][popindex];
}


int FreqData::get_index(pair<string, int> cpos){
	string chr = cpos.first;
	int pos = cpos.second;
	if (chr2pos2index.find(chr) != chr2pos2index.end() and chr2pos2index[chr].find(pos) != chr2pos2index[chr].end()){
		int snpindex = chr2pos2index[chr][pos];
		return snpindex;
	}
	else{
		cerr << "ERROR: can't find "<< chr << ":"<< pos << "\n";
		exit(1);
	}
}


pair<int, int> FreqData::get_counts(pair<string, int> cpos, string pop){
	string chr = cpos.first;
	int pos = cpos.second;
	//if (chr2pos2index.find(chr) != chr2pos2index.end() and chr2pos2index[chr].find(pos) != chr2pos2index[chr].end()){
	int snpindex = chr2pos2index[chr][pos];
	int popindex = pop2id[pop];
	return allele_counts[snpindex][popindex];
	//}
	//else{
	//	cerr << "ERROR: can't find "<< chr << ":"<< pos << "\n";
	//	exit(1);
	//}
}

double FreqData::get_weighted_f(pair<string, int> cpos, map<string, float> ancestry){
	double toreturn = 0;
	for (map<string, float>::iterator it = ancestry.begin(); it != ancestry.end(); it++){
		double f = get_f(cpos, it->first);
		double wf = f*it->second;
		toreturn +=wf;
	}
	return toreturn;
}
double FreqData::get_f(pair<string, int> cpos, string pop){
	string chr = cpos.first;
	int pos = cpos.second;
	if (chr2pos2index.find(chr) != chr2pos2index.end() and chr2pos2index[chr].find(pos) != chr2pos2index[chr].end()){
		int snpindex = chr2pos2index[chr][pos];
		int popindex = pop2id[pop];
		int count1 = allele_counts[snpindex][popindex].first;
		int count2 = allele_counts[snpindex][popindex].second;
		return (double) count1/ ( (double) count1 + (double) count2);
	}
	else{
		cerr << "ERROR: can't find "<< chr << ":"<< pos << "\n";
		exit(1);
	}
}

double FreqData::lbinom(int k, double p, int n){
	double toreturn = log(binomial_coefficient<double>(n, k));
	toreturn += k*log(p) + (n-k)*log(1-p);
	return toreturn;
}


double FreqData::get_lk(pair<string, int> cpos, string pop, double f){
	string chr = cpos.first;
	int pos = cpos.second;
	if (chr2pos2index.find(chr) != chr2pos2index.end() and chr2pos2index[chr].find(pos) != chr2pos2index[chr].end()){
		int snpindex = chr2pos2index[chr][pos];
		int popindex = pop2id[pop];
		int count1 = allele_counts[snpindex][popindex].first;
		int count2 = allele_counts[snpindex][popindex].second;
		return lbinom(count1, f, count1+count2);
	}
	else{
		cerr << "ERROR: can't find "<< chr << ":"<< pos << "\n";
		exit(1);
	}
}

void FreqData::read_freqs(string infile){
    pop2id.clear();
    id2pop.clear();
    npop = 0;
    nsnp = 0;
    SNPinfo.clear();
    chr2pos2index.clear();
    index2pos.clear();
    vector<vector<double> > fs;

    //make sure infile is gzipped
    string ext = infile.substr(infile.size()-3, 3);
    if (ext != ".gz"){
    	std::cerr << infile << " is not gzipped (only .gz files accepted)\n";
    	exit(1);
    }
	igzstream in(infile.c_str()); //only gzipped files
    vector<string> line;
    struct stat stFileInfo;
    int intStat;
    string st, buf;

    intStat = stat(infile.c_str(), &stFileInfo);
    if (intStat !=0){
            std::cerr<< "ERROR: cannot open file " << infile << "\n";
            exit(1);
    }

    /*
     * header contains population names
     */
    getline(in, st);
    stringstream ss(st);
    line.clear();
    while (ss>> buf){
    	line.push_back(buf);
     }
    /*
     * make map from header, number populations according to order
     */
    int start = 0;
    start = 6;
    for(int i = start; i < line.size(); i++) {
    	pop2id.insert(make_pair(line[i], i-start));
    	id2pop.insert(make_pair(i-start, line[i]));
    	npop ++;
    }
    int headsize = line.size();
    /*
     * read counts, store in allele_counts
     */
    while(getline(in, st)){
            buf.clear();
            stringstream ss(st);
            line.clear();
            while (ss>> buf){
                    line.push_back(buf);
            }
            map<string, string> tmp;

            // chromosome is column 0
            string chr = line[0];

            // id is column 1
            tmp.insert(make_pair("rs", line[1]));
            tmp.insert(make_pair("chr", chr));

            // genetic position is column 2 and is not used, can be set to 0
            // physical position is column 3
            tmp.insert(make_pair("pos", line[3]));

            // alleles are in columns 4 and 5
            // check to make sure they're in [A, T, C, G]
            // allele counts will be in format [#a1],[#a2]
            check_a(line[4]);
            check_a(line[5]);
            tmp.insert(make_pair("a1", line[4]));
            tmp.insert(make_pair("a2", line[5]));

            if (line.size() != headsize){
            	cerr << "ERROR: Line "<< nsnp <<" has "<< line.size() << " entries. Header has "<< headsize <<"\n";
            	exit(1);
            }
            vector<pair<int, int> > topush;
            for ( int i = start; i < line.size(); i++) {

            	//split on ","
            	typedef boost::tokenizer<boost::char_separator<char> >
            	tokenizer;
            	boost::char_separator<char> sep(",");
            	tokenizer tokens(line[i], sep);
            	vector<int> tmpcounts;
            	for (tokenizer::iterator tok_iter = tokens.begin();  tok_iter != tokens.end(); ++tok_iter){
            		int tmp = atoi(tok_iter->c_str());
            		tmpcounts.push_back(tmp);
            	}
            	if (tmpcounts.size() != 2){
            		std::cerr << "ERROR: "<< line[i] << " does not have two alleles (expecting SNP data)\n";
            		exit(1);
            	}
            	topush.push_back(make_pair(tmpcounts[0], tmpcounts[1]));
            }
            //if there's a population with entirely missing data, skip the SNP
            bool snpok = true;
            for( vector<pair<int, int> >::iterator it= topush.begin(); it !=topush.end(); it++){
            	if (it->first  == 0 and it->second == 0) snpok = false;
            }

            // otherwise SNP is ok, put it in the reference panel
            if (snpok){
            	allele_counts.push_back(topush);
            	SNPinfo.push_back(tmp);

            	//edit chr2pos2index
            	int pp = atoi(line[3].c_str());
            	if (chr2pos2index.find(chr) == chr2pos2index.end()){
            		map<int, int> tmpmap;
            		chr2pos2index.insert(make_pair(chr, tmpmap));
            	}
            	chr2pos2index[chr].insert(make_pair(pp, nsnp));

            	//edit index2pos
            	pair<string, int> cp;
            	cp.first = chr;
            	cp.second = pp;
            	index2pos.insert(make_pair(nsnp, cp));

            	nsnp++;
            }
    }
}

pair<string, string> FreqData::get_als(pair<string, int> info){
	if (chr2pos2index.find(info.first) != chr2pos2index.end() && chr2pos2index[info.first].find(info.second) != chr2pos2index[info.first].end() ){
		int index = chr2pos2index[info.first][info.second];
		string a1 = SNPinfo[index]["a1"];
		string a2 = SNPinfo[index]["a2"];
		return(make_pair(a1, a2));
	}
	else{
		cerr << "ERROR: no such SNP chr"<< info.first << ":"<< info.second << "\n";
		exit(1);
	}
}

void FreqData::check_a(string a){
	if (a == "A" or a == "T" or a == "C" or a == "G") return;
	else{
		cerr << "Allele is: "<< a << ". Not a valid base\n";
		exit(1);
	}
}



void FreqData::print_alfreqs(string outfile){
	ogzstream out(outfile.c_str());

	out << "rs chr pos a1 a2 ";
	for(map<string, int>::iterator it = pop2id.begin(); it != pop2id.end(); it++)	out << it->first << " ";
	out << "\n";
	for (int i = 0; i < nsnp ; i++){
		map<string, string> info = SNPinfo[i];
		out << info["rs"]<< " "<< info["chr"]<< " "<< info["pos"]<< " "<< info["a1"] << " "<< info["a2"];
		for(map<string, int>::iterator it2 = pop2id.begin(); it2 != pop2id.end(); it2++)	 out << " "<< allele_counts[i][it2->second].first <<","<< allele_counts[i][it2->second].second;
		out << "\n";
	}
}


