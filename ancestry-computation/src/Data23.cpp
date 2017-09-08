/*
 * Data23.cpp
 *
 *  Created on: Sep 20, 2014
 *      Author: jkpickrell
 *
 *
 *      The squarem1() and squarem2() functions are modified from https://github.com/daichaoxing/cxxSQUAREM
 *
 *
 */
#include "Data23.h"

Data23::Data23(){

}


Data23::Data23(Ancestry_params *p, FreqData *rf, gsl_rng *ran){
	ref = rf;
	params = p;
	r = ran;
	data.clear();
	refpopnames.clear();
	//read the sample data
	if (p->input23) read_file(p->sampfile);
	else if (p->inputVCF) read_vcf(p->sampfile);
	else read_file_pileup(p->sampfile);

	set_chrsegments();
	refpopnames = rf->get_popnames();
	npop = refpopnames.size();
	originalnpop = npop;
	nsnp = data.size();
	set_refindices();
	cout << npop << " reference populations, "<< nsnp << " overlapping SNPs on " << chrsegments.size() << " chromosomes\n";

	if (p->bootstrap) {
		cout << "[bootstrapping]\n";
		bootstrap(ran);

	}

	Q = gsl_vector_alloc(npop);
	P = gsl_matrix_alloc(nsnp, npop);
	tmpQ = gsl_vector_alloc(npop);
	tmpP = gsl_matrix_alloc(nsnp, npop);
	init();

}

void Data23::set_datamap(){
	datamap.clear();
	for (int i = 0; i < nsnp; i++){
		//cout << info.at(i).first << " "<< info.at(i).second << " "<< data.at(i) << " dmap\n";
		datamap.insert(make_pair(info.at(i), data.at(i)));
	}
}
vector<bool> Data23::relfinder_overlap(Data23 * d){
	vector<bool> toreturn;
	for (vector<pair<string, int> >::iterator it = info.begin(); it !=info.end(); it++){
		if ( d->datamap.find(*it) != d->datamap.end() ) toreturn.push_back(true);
		else toreturn.push_back(false);
	}
	return toreturn;
}

vector<pair<double, double> > Data23::relfinder(Data23 * d, map<string, float> a2){
	vector<pair<double, double> > toreturn;
	d->set_datamap();
	vector<bool> overlap = relfinder_overlap(d);
	int nover = 0;
	for (vector<bool>::iterator it = overlap.begin(); it != overlap.end(); it++){
		if (*it) nover++;
	}
	vector<double> weighted_alfreq2;
	cout << "### relfinder ###" <<"\n";
	cout << "### overlap of " << nover<< " SNPs\n";
	cout <<"###\n";
	for (int i = 0; i < nsnp; i++){
		if (overlap.at(i)){
			double f = ref->get_weighted_f(info.at(i), a2);
			weighted_alfreq2.push_back(f);
		}
		else{
			weighted_alfreq2.push_back(-1);
		}
	}
	for (double i = 0; i <=1.001; i+=0.01){
		double llk = relfinder_llk(i, d, overlap, weighted_alfreq2);
		toreturn.push_back(make_pair(i, llk));
	}
	return toreturn;
}

double Data23::relfinder_llk(float lambda, Data23* d, vector<bool> overlap, vector<double> f2){
	double toreturn = -10000;
	for (int i = 0; i < nsnp ;i++){
		if (overlap.at(i)){
			assert(f2.at(i) >=0);
			bool g1 = data.at(i);
			bool g2 = d->datamap[info.at(i)];
			bool eq = false;
			double f = f2.at(i);
			if (!g1) f = 1-f;
			if (g1 == g2) eq = true;
			double pmatch = lambda *(0.5 + 0.5* f) + (1-lambda) *f;
			if (pmatch < 1e-10) pmatch = 1e-10;
			if (pmatch > 1-1e-10) pmatch = 1-1e-10;
			if (eq) toreturn += log(pmatch);
			else toreturn+= log(1.0-pmatch);
		}
	}
	return toreturn;

}


Data23::~Data23(){
	gsl_vector_free(Q);
	gsl_matrix_free(P);
	gsl_vector_free(tmpQ);
	gsl_matrix_free(tmpP);
}
void Data23::set_refindices(){
	refindices.clear();
	for (int i = 0;i < nsnp; i++){
		refindices.push_back(ref->get_index(info[i]));
	}
}

void Data23::bootstrap(gsl_rng *r){
	//run resampling
	//note this is going to mean all SNPs are out of genomic order
	//not going to fix this since all SNPs are treated independently anyways

	vector<bool> newdata;
	vector<pair<string, int> > newinfo;
	for (int i = 0; i < nsnp; i++){
		int rr = gsl_rng_uniform_int(r, nsnp);
		//cout << rr <<"\n";
		newdata.push_back(data.at(rr));
		newinfo.push_back(info.at(rr));
	}
	for (int i = 0; i <nsnp ;i++){
		data[i] = newdata[i];
		info[i] = newinfo[i];
	}
	set_refindices();
}



void Data23::sampleN(int N, gsl_rng *r){
	// randomly sample SNPs to get approximately N
	double f = (float) N/ float(nsnp);
	if (f > 1) return;
	if (f < 1e-6){
		cerr << "ERROR: sampling a zero or negative fraction "<< f << "\n";
		exit(1);
	}
	cout << "SAMPLING: random "<< f << " fraction of SNPs\n";
	vector<bool> newdata;
	vector<pair<string, int> > newinfo;
	for (int i = 0; i < nsnp; i++){
		double rr = gsl_rng_uniform(r);
		if (rr < f){
			newdata.push_back(data.at(i));
			newinfo.push_back(info.at(i));
		}
	}
	data.clear();
	info.clear();
	for (int i = 0; i <newdata.size() ;i++){
		data.push_back( newdata[i] );
		info.push_back( newinfo[i] );
	}
	nsnp = data.size();
	if (nsnp < 1){
		cerr << "ERROR: no SNPs left\n";
		exit(1);
	}
	cout << "SAMPLING: now "<< nsnp << " SNPs\n";
	set_refindices();
}


void Data23::set_chrsegments(){
	chrsegments.clear();
	int i = 0;
	int start = i;
	int startpos = info[i].second;
	string startchr = info[i].first;
	while (i < info.size()){
		int tmppos = info[i].second;
		string tmpchr = info[i].first;
		if (tmpchr != startchr){
			int end = i;
			chrsegments.push_back(make_pair(start, end));
			start = i;
			startpos = info[i].second;
			startchr = info[i].first;
		}
		i++;
	}
	int end = i;
	chrsegments.push_back(make_pair(start, end));
}

void Data23::read_file(string infile){
	// read a 23andme-style file
	// format is: [rsid] [chromosome] [position] [genotype]
	// not checking if genome build is the same as in the reference frequencies
	// checking to make sure the allele matches one of the reference alleles

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

    int nwarning = 0;
    while(getline(in, st)){
            buf.clear();
            stringstream ss(st);
            line.clear();
            while (ss>> buf){
                    line.push_back(buf);
            }
            if (line[0] == "#") continue;

            //if randomly sampling, skip some snps
            if (params->ransnp){
            	double rrr = gsl_rng_uniform(r);
            	if (rrr > params->ranfrac) continue;
            }
            string rs = line[0];
            string chr = line[1];
            if (params->skip and chr == params->skipchr) continue;
            int pos = atoi(line[2].c_str());
            string geno = line[3];
            if (ref->chr2pos2index.find(chr) != ref->chr2pos2index.end() and ref->chr2pos2index[chr].find(pos) != ref->chr2pos2index[chr].end()){
            	//cout << line[0] << " "<< geno << "\n";
            	double rn = gsl_rng_uniform(r);
            	string a = geno.substr(0, 1);
            	if (rn > 0.5){
            		a = geno.substr(1,1);
            	}

            	// test if ref or alternate
            	pair<string, string> refals = ref->get_als(make_pair(chr, pos));
            	//cout << rn << " "<< a << " "<< refals.first << " "<< refals.second << "\n";
            	if (a == refals.first) {
            		data.push_back(true); //true = matches the minor allele
            		info.push_back(make_pair(chr, pos));
            	}
            	else if (a == refals.second){
            		data.push_back(false);
            		info.push_back(make_pair(chr, pos));
            	}
            	else{
            		if (params->print_warnings) cerr << "WARNING: skipping "<<chr <<":"<< pos <<". Allele is "<< a << ". Reference is "<< refals.first << " or "<< refals.second << "\n";
            		nwarning++;
            	}

            }
    }
    cout << "WARNING: skipped "<< nwarning << " SNPs\n";
}


void Data23::read_vcf(string infile){
	// read a VCF/BCF file
	//
	// not checking if genome build is the same as in the reference frequencies
	// checking to make sure the allele matches one of the reference alleles
	cout << "Reading "<< infile << "\n";
    int nwarning = 0;
	//using htslib
	bcf_srs_t *sr =  bcf_sr_init();
	bcf_sr_add_reader (sr, infile.c_str() );
	bcf_hdr_t *header = sr->readers->header;
	bcf1_t *line; //VCF line gets put in here
	int Nsamp=bcf_hdr_nsamples(header);
	if (Nsamp > 1){
		cerr << "ERROR: should only be one sample per file, this file has "<< Nsamp << " samples\n";
		exit(1);
	}
	int index = 0;
	while(bcf_sr_next_line (sr)) { //loop through file
	   line =  bcf_sr_get_line(sr, 0);  //read a line
	   //if randomly sampling, skip some snps
	   if (params->ransnp){
		   double rrr = gsl_rng_uniform(r);
		   if (rrr > params->ranfrac) continue;
	   }
	   bcf_unpack(line, BCF_UN_ALL);
	   // to do: first check filters, make sure it's not monomorphic, is bi-allelic, etc.
	   // should be running on filtered 1000 Genomes data for now so not a huge priority
	   string chr =  bcf_hdr_id2name(header, line->rid);
	   int pos = line->pos+1;
	   //cout << chr << " "<< pos << "\n"; cout.flush();
       if (params->skip and chr == params->skipchr) continue;
	   //string refal = line->d.allele[0];
	  // cout << refal << "\n"; cout.flush();
	  // string altal = "N";
	   //cout << altal << "\n"; cout.flush();
	   int *gt = NULL;
	   int32_t n = 0;

	   int ngt = bcf_get_genotypes(header, line, &gt, &n);
	   //cout << ngt << " "<< Nsamp << "\n"; cout.flush();
	   // number of alleles should be 2x number of samples
	   if (ngt != Nsamp*2.0){
		   cerr <<"at "<< chr << " "<< pos << " number of individuals and genotypes is not consistent\n";
		   exit(1);
	   }
	   vector<int> alleles;
	   bool ismissing = false;
	   for (int i = 0; i < ngt; i++){
		   int32_t *ptr = gt+i;
		   if ( bcf_gt_is_missing(ptr[0]) ) {ismissing = true;}
		   else{
			   int idx = bcf_gt_allele(ptr[0]);
			   alleles.push_back(idx);
		   }
	   }
	   if (ismissing) continue;

       if (ref->chr2pos2index.find(chr) != ref->chr2pos2index.end() and ref->chr2pos2index[chr].find(pos) != ref->chr2pos2index[chr].end()){
    	   	   //cout << line[0] << " "<< geno << "\n";
    	   	   double rn = gsl_rng_uniform(r);
    	   	   string a =  line->d.allele[alleles[0]];
    	   	   if (rn > 0.5){
    	   		   a = line->d.allele[alleles[1]];
    	   	   }
    	   	  // string a = refal;
    	   	   //if (aa == true){
    	   		//   a = altal;
    	   	  // }

    	   	   // test if ref or alternate
    	   	   pair<string, string> refals = ref->get_als(make_pair(chr, pos));
    	   	   //cout << rn << " "<< a << " "<< refals.first << " "<< refals.second << "\n";
    	   	   if (a == refals.first) {
    	   		   data.push_back(true); //true = matches the minor allele
    	   		   info.push_back(make_pair(chr, pos));
    	   	   }
    	   	   else if (a == refals.second){
    	   		   data.push_back(false);
    	   		   info.push_back(make_pair(chr, pos));
    	   	   }
    	   	   else{
    	   		   if (params->print_warnings) cerr << "WARNING: skipping "<<chr <<":"<< pos <<". Allele is "<< a << ". Reference is "<< refals.first << " or "<< refals.second << "\n";
    	   		   nwarning++;
    	   	   }

       }
       //cout << "ok\n"; cout.flush();
	   free(gt);

	}
    cout << "WARNING: skipped "<< nwarning << " SNPs\n";
}

void Data23::dropuseless(){
	//
	// find all populations in the Q vector less than epislon, drop them
	//
	// need to update npop, refpopnames, P, Q
	int index = 0;
	double EPSILON = params->dropepsilon;
	vector<string> newrefpopnames;
	map<int, int> old2newindex;
	for (int i = 0; i < npop; i++){
		double tmp = gsl_vector_get(Q, i);
		if (tmp > EPSILON){

			newrefpopnames.push_back(refpopnames[i]);
			old2newindex.insert(make_pair(i, index));
			index++;
		}
		else{
			cout << "DROPPING: "<< refpopnames[i] << "\n";
		}
	}
	int newnpop = newrefpopnames.size();

	if (newrefpopnames.size() < refpopnames.size()){
		cout << "DROPPING: now "<< newnpop << " populations\n"; cout.flush();
		gsl_matrix_memcpy(tmpP, P);
		gsl_vector_memcpy(tmpQ, Q);
		for (int i  = 0; i < nsnp; i++){
			for (int j = 0; j < npop; j++){
				if (old2newindex.find(j) != old2newindex.end()){
					int newindex = old2newindex[j];
					gsl_matrix_set(P, i, newindex, gsl_matrix_get(tmpP, i, j));
				}
			}
		}
		for (int i = 0; i < npop; i++){
			if (old2newindex.find(i) != old2newindex.end()){
				int newindex = old2newindex[i];
				gsl_vector_set(Q, newindex, gsl_vector_get(tmpQ, i));
			}
		}
		//gsl_matrix_free(tmpP);
		//gsl_vector_free(tmpQ);
		npop = newnpop;
		refpopnames.clear();
		for (int i = 0; i < npop; i++) refpopnames.push_back(newrefpopnames[i]);
	}
	else return;
}

void Data23::read_file_pileup(string infile){
	// read a pileup-style file
	// format is: [chromosome] [position] [ref] [count] [alleles]
	// not checking if genome build is the same as in the reference frequencies
	// checking to make sure the allele matches one of the reference alleles

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

    int nwarning = 0;
    while(getline(in, st)){
            buf.clear();
            stringstream ss(st);
            line.clear();
            while (ss>> buf){
                    line.push_back(buf);
            }



            if (line[0] == "#") continue;
            if (line.size() < 5) continue;

            //if randomly sampling, skip some snps
            if (params->ransnp){
            	double rrr = gsl_rng_uniform(r);
            	if (rrr > params->ranfrac) continue;
            }
            //chr
            string chr = line[0];
            if (params->skip and chr == params->skipchr) continue;
            //pos
            int pos = atoi(line[1].c_str());

            // get the first allele
            string geno = line[4];
            //if (pos == 6107136) cout << pos << " "<< geno << "\n";
            geno = geno.substr(0, 1);
            boost::to_upper(geno);

            if (ref->chr2pos2index.find(chr) != ref->chr2pos2index.end() and ref->chr2pos2index[chr].find(pos) != ref->chr2pos2index[chr].end()){
            	//cout << line[0] << " "<< geno << "\n";
            	// test if ref or alternate
            	pair<string, string> refals = ref->get_als(make_pair(chr, pos));
            	//cout << rn << " "<< a << " "<< refals.first << " "<< refals.second << "\n";
            	if (geno == refals.first) {
            		data.push_back(true); //true = matches the minor allele
            		info.push_back(make_pair(chr, pos));
            	}
            	else if (geno == refals.second){
            		data.push_back(false);
            		info.push_back(make_pair(chr, pos));
            	}
            	else{
            		if (params->print_warnings) cerr << "WARNING: skipping "<<chr <<":"<< pos <<". Allele is "<< geno << ". Reference is "<< refals.first << " or "<< refals.second << "\n";
            		nwarning++;
            	}

            }
    }
    cout << "WARNING: skipped "<< nwarning << " SNPs\n";
}

void Data23::setQ(vector<double> q){
	for(int i = 0; i < npop; i++){
		gsl_vector_set(Q, i, q[i]);
	}
}

void Data23::print(){
	for (int i = 0; i < info.size(); i++){
		cout << info[i].first << " "<< info[i].second << " "<< data[i] << "\n";
	}
}

void Data23::printP(string outfile){
	ofstream out(outfile.c_str());
	out << "chr pos samp";
	for (int i = 0; i < npop; i++) out << " "<< refpopnames[i];
	out << "\n";
	for (int i = 0; i < nsnp; i++){
		out << info[i].first<< " "<< info[i].second << " "<< data[i];
		for (int j = 0; j < npop; j++) out << " "<< gsl_matrix_get(P, i, j);
		out << "\n";
	}
}



void Data23::printQ(string outfile){
	ofstream out(outfile.c_str());

	for (int i = 0; i < npop; i++){
		out << refpopnames[i] << " "<< gsl_vector_get(Q, i) << "\n";
	}
}

void Data23::init(){
	for (int i = 0; i < npop; i++) gsl_vector_set(Q, i, 1.0 /(double) npop);
	for (int i = 0; i < nsnp; i++){
		for (int j = 0; j < npop; j++){
			double f = ref->get_f(info[i], refpopnames[j]);
			if (f < 1e-6) f = 1e-6;
			if (f > 1-1e-6) f = 1-1e-6;
			gsl_matrix_set(P, i, j, f);
		}
	}
}

void Data23::init_weights(){
	pi.clear();
	alpha.clear();
	int npop = ref->get_npop();
	for (int i = 0; i < npop; i++){
		pi.push_back(0.0);
		alpha.push_back(0.0);
	}
	set_weights();
}

void Data23::filter_pops_missing(){
	vector<int> nmiss;
	for (int i = 0; i < npop; i++){
		int m = 0;
		for (int j = 0; j < nsnp; j++){
			double f = ref->get_f(info[j], refpopnames[i]);
			if (isnan(f)) m++;
		}
		nmiss.push_back(m);
	}
	vector<string> keptpops;
	for (int i = 0; i < npop; i++){
		float fmiss = (float) nmiss[i] / (float) nsnp;
		if (fmiss > params->missing_filter){
			cout << "Filtering "<< refpopnames[i] <<" missingness: "<< fmiss << "\n";
		}
		else	keptpops.push_back(refpopnames[i]);
	}
	refpopnames.clear();
	for (vector<string>::iterator it = keptpops.begin(); it != keptpops.end(); it++) refpopnames.push_back(*it);
	npop = refpopnames.size();

}

void Data23::set_weights(){
	double sum = 0;
	for (int i = 0; i< alpha.size();i++) sum+= exp(alpha[i]);
	for (int i = 0; i < alpha.size(); i++) {

		pi[i] = exp(alpha[i])/sum;
		//cout << i << " "<< alpha[i]<< " "<< sum << " "<< pi[i]<< "\n";
	}
}

void Data23::set_weights(vector<int> pop2keep){
	double sum = 0;
	for (vector<int>::iterator it = pop2keep.begin(); it != pop2keep.end(); it++) sum+= exp(alpha[*it]);
	for (vector<int>::iterator it = pop2keep.begin(); it != pop2keep.end(); it++) pi[*it] = exp(alpha[*it])/sum;

}

double Data23::llk(vector<double> d){
	int index = 0;
	for (int i = 0; i < nsnp; i++){
		for (int j = 0; j < npop; j++){
			gsl_matrix_set(P, i, j, d[index]);
			index++;
		}
	}
	for (int i = 0; i < npop; i++){
		gsl_vector_set(Q, i, d[index]);
		index++;
	}
	return llk();
}

double Data23::llk(){
	double lk = -10000;
	for (int i = 0; i < nsnp ; i++){
		bool g = data[i];
		double tmpf = 0;
		for (int j=  0; j < npop; j++){
			double w = gsl_vector_get(Q, j);
			double f = gsl_matrix_get(P, i, j);
			tmpf += w*f;
			lk += ref->get_lk(info[i], refpopnames[j], f);
		}
		if (g) lk+= log(tmpf);
		else lk+= log(1-tmpf);

	}
	return lk;
}


void Data23::updateP(){
	for (int i = 0; i < nsnp; i++){
		vector<double> tmpfs;
		double s1 = 0;
		double s2 = 0;
		for (int j = 0; j < npop; j++) {
			tmpfs.push_back(gsl_matrix_get(P, i, j));
			s1+= gsl_vector_get(Q, j)* gsl_matrix_get(P, i, j);
			s2+= gsl_vector_get(Q, j)* (1-gsl_matrix_get(P, i, j));
		}
		for (int j = 0; j < npop; j++){
			pair<int, int> counts = ref->get_counts_index(refindices[i], refpopnames[j]);
			//pair<int, int> counts = ref->get_counts(info[i], refpopnames[j]);
			double num = (double) counts.first;
			double denom = (double) counts.first + (double) counts.second;
			if (data[i]) {
				num +=  gsl_vector_get(Q, j)* tmpfs[j] / s1;
				denom +=  gsl_vector_get(Q, j)* tmpfs[j] / s1;
			}
			else{
				denom +=  gsl_vector_get(Q, j)* (1-tmpfs[j]) / s2;
			}
			double tmpf = num/denom;
			if (tmpf > 1-1e-6) tmpf = 1-1e-6;
			if (tmpf < 1e-6) tmpf = 1e-6;
			gsl_matrix_set(P, i, j, tmpf);
		}
	}
}

void Data23::updateQ(){
	vector<double> tmpQ;
	for (int i = 0; i < npop; i++) tmpQ.push_back(0.0);
	for (int i = 0; i < nsnp; i++){
		if (data[i]){
			double s1 = 0;
			for (int j = 0; j < npop; j++) s1+= gsl_vector_get(Q, j)* gsl_matrix_get(P, i, j);
			for (int j = 0; j < npop; j++){
				double toadd = gsl_vector_get(Q, j)* gsl_matrix_get(P, i, j) / s1;
				tmpQ[j] += toadd;
			}
		}
		else{
			double s2 = 0;
			for (int j = 0; j < npop; j++) s2+= gsl_vector_get(Q, j)* (1-gsl_matrix_get(P, i, j));
			for (int j = 0; j < npop; j++){
				double toadd = gsl_vector_get(Q, j)* (1-gsl_matrix_get(P, i, j)) / s2;
				tmpQ[j] += toadd;
			}
		}
	}
	for (int i = 0; i < npop; i++){
		double toplace = tmpQ[i]/(double) nsnp;
		if (toplace < 1e-6) toplace = 1e-6;
		if (toplace > 1-1e-6) toplace = 1-1e-6;
		gsl_vector_set(Q, i, toplace);
	}
}

void Data23::EM(int nit){
	double oldllk = llk();
	updateP();
	updateQ();
	double newllk = llk();
	cout << oldllk << " "<< newllk << "\n";

	double delta = newllk - oldllk;
	//cout << nit << " "<< delta<< " "<< newllk << "\n";
	for (int i = 0; i < nit ;i++){
		updateP();
		updateQ();
		oldllk = newllk;
		newllk = llk();
		delta = newllk - oldllk;
		cout << "nit: "<<i << " delta: "<< delta<< " "<< newllk << "\n";
	}
}

void Data23::EM(){
	double oldllk = llk();
	updateP();
	updateQ();
	double newllk = llk();
	cout << oldllk << " "<< newllk << "\n";
	int nit = 0;
	double delta = newllk - oldllk;
	cout << nit << " "<< delta<< " "<< newllk << "\n";
	while ( delta > params->epsilon and  nit < params->maxiter){
		updateP();
		updateQ();
		oldllk = newllk;
		newllk = llk();
		delta = newllk - oldllk;
		nit ++;
		cout << "nit: "<<nit << " delta: "<< delta<< " "<< newllk << "\n";

	}
}

vector<double> Data23::params2vec(){
	vector<double> toreturn (npop*nsnp+npop, 0);
	int index = 0;
	for (int i = 0; i < nsnp; i++){
		for (int j = 0; j < npop; j++){
			//cout << i << " "<< j << "\n";
			toreturn[index] = gsl_matrix_get(P, i, j);
			index++;
		}
	}
	for (int i = 0; i < npop; i++) {
		toreturn[index] = gsl_vector_get(Q, i);
		index++;
	}
	return toreturn;
}


void Data23::assignfromvec(vector<double> d){
	int index = 0;
	for (int i = 0; i < nsnp; i++){
		for (int j = 0; j < npop; j++){
			gsl_matrix_set(P, i, j, d[index]);
			index++;
		}
	}
	for (int i = 0; i < npop; i++) {
		gsl_vector_set(Q, i, d[index]);
		index++;
	}
}
//actual acceleration function
void  Data23::squarem1(){
	EM(params->EMsteps);
    //std::vector<double> p,p1,p2;//R data types
    double loldcpp,lnewcpp;
    std::vector<double> pcpp,p1cpp,p2cpp,pnew;
    std::vector<double> q1,q2,sr2,sq2,sv2,srv;
    double sr2_scalar,sq2_scalar,sv2_scalar,srv_scalar,alpha,stepmin,stepmax;
    int iter,feval,leval;
    bool conv,extrap;
    stepmin=params->stepmin0;
    stepmax=params->stepmax0;
    //if(SquaremDefault.trace){std::cout<<"Squarem-1"<<std::endl;}

    iter=1;

    //cout << "here\n"; cout.flush();
    pcpp = params2vec();
    //cout << "here1\n"; cout.flush();
    pnew = params2vec();
    loldcpp=llk(pcpp);
    //cout <<"here?\n"; cout.flush();
    leval=1;

    lnewcpp=loldcpp;
    cout<<"Objective fn: "<<loldcpp<<std::endl;
    feval=0;conv=true;

    const long int parvectorlength=pcpp.size();

    while(feval<params->maxiter){
        //Step 1
        extrap = true;
        updateP();
        updateQ();
        p1cpp=params2vec();
        feval++;


        sr2_scalar=0;
        for (int i=0;i<parvectorlength;i++){sr2_scalar+=pow(p1cpp[i]-pcpp[i],2);}
        if(sqrt(sr2_scalar)<params->tol){break;}

        //Step 2
        updateP();
        updateQ();
        p2cpp=params2vec();
       // p2cpp=fixptfn(p1cpp);
        feval++;

        sq2_scalar=0;
        for (int i=0;i<parvectorlength;i++){sq2_scalar+=pow(p2cpp[i]-p1cpp[i],2);}
        //sq2_scalar=sqrt(sq2_scalar);
        if (sqrt(sq2_scalar)<params->tol){break;}
        sv2_scalar=0;
        for (int i=0;i<parvectorlength;i++){sv2_scalar+=pow(p2cpp[i]-2*p1cpp[i]+pcpp[i],2);}
        srv_scalar=0;
        for (int i=0;i<parvectorlength;i++){srv_scalar+=(p2cpp[i]-2*p1cpp[i]+pcpp[i])*(p1cpp[i]-pcpp[i]);}
        //std::cout<<"sr2,sv2,srv="<<sr2_scalar<<","<<sv2_scalar<<","<<srv_scalar<<std::endl;//debugging

        //Step 3 Proposing new value
       // switch (SquaremDefault.method){
        //    case 1: alpha= -srv_scalar/sv2_scalar;
       //     case 2: alpha= -sr2_scalar/srv_scalar;
       //     case 3: alpha= sqrt(sr2_scalar/sv2_scalar);
      //  }
        alpha= sqrt(sr2_scalar/sv2_scalar);
        alpha=std::max(stepmin,std::min(stepmax,alpha));
        //std::cout<<"alpha="<<alpha<<std::endl;//debugging
        for (int i=0;i<parvectorlength;i++){pnew[i]=pcpp[i]+2.0*alpha*(p1cpp[i]-pcpp[i])+pow(alpha,2)*(p2cpp[i]-2*p1cpp[i]+pcpp[i]);}
        //pnew = pcpp + 2.0*alpha*q1 + alpha*alpha*(q2-q1);

        //Step 4 stabilization
        if(std::abs(alpha-1)>0.01){
            try{
            	assignfromvec(pnew);
            	updateP();
            	updateQ();
            	pnew = params2vec();
            	//pnew=fixptfn(pnew);
            	feval++;
            }
            catch(...){
                pnew=p2cpp;
                try{
                	lnewcpp=llk(pnew);
                	leval++;
                }
                catch(...){
                    lnewcpp=loldcpp;
                }
                if(alpha==stepmax){
                    stepmax=std::max(params->stepmax0,stepmax/params->mstep);
                }
                alpha=1;
                extrap=false;
                if(alpha==stepmax){stepmax=params->mstep*stepmax;}
                if(stepmin<0 && alpha==stepmin){stepmin=params->mstep*stepmin;}
                pcpp=pnew;
                if(!std::isnan(lnewcpp)){loldcpp=lnewcpp;}
                std::cout<<"Objective fn: "<<lnewcpp<<"  Extrapolation: "<<extrap<<"  Steplength: "<<alpha<<std::endl;
                iter++;
                continue;//next round in while loop
            }


            try{
            	lnewcpp=llk(pnew);
            	leval++;
            }
            catch(...){
            	pnew=p2cpp;
            	try{
            		lnewcpp=llk(pnew);
            		leval++;
            	}
            	catch(...){
            		std::cout<<"Error in objfn function evaluation";
            		exit(1);
            	}
            	if(alpha==stepmax){
            		stepmax=std::max(params->stepmax0,stepmax/params->mstep);
            	}
            	alpha=1;
            	extrap=false;
            }

            if (lnewcpp>loldcpp+1) {
                pnew=p2cpp;
                try{
                	lnewcpp=llk(pnew);
                	leval++;
                }
                catch(...){
                    std::cout<<"Error in objfn function evaluation";
                    exit(1);
                }
                if(alpha==stepmax){
                    stepmax=std::max(params->stepmax0,stepmax/params->mstep);
                }
                alpha=1;
                extrap=false;
            }
        }else{//same as above, when stablization is not performed.
        	try{lnewcpp=llk(pnew);leval++;}
        	catch(...){
        		pnew=p2cpp;
        		try{lnewcpp=llk(pnew);leval++;}
        		catch(...){
        			std::cout<<"Error in objfn function evaluation";
        			exit(1);
        		}
        		if(alpha==stepmax){
        			stepmax=std::max(params->stepmax0,stepmax/params->mstep);
        		}
        		alpha=1;
        		extrap=false;
        	}

            if (lnewcpp>loldcpp+1) {
                pnew=p2cpp;
                try{lnewcpp=llk(pnew);leval++;}
                catch(...){
                    std::cout<<"Error in objfn function evaluation";
                    exit(1);
                }
                if(alpha==stepmax){
                    stepmax=std::max(params->stepmax0,stepmax/params->mstep);
                }
                alpha=1;
                extrap=false;
            }
        }
        if(alpha==stepmax){stepmax=params->mstep*stepmax;}
        if(stepmin<0 && alpha==stepmin){stepmin=params->mstep*stepmin;}

        pcpp=pnew;
        double delta = lnewcpp - loldcpp;
        if(!std::isnan(lnewcpp)){loldcpp=lnewcpp;}
        cout<<"delta: "<<delta<<"  Extrapolation: "<<extrap<<"  Steplength: "<<alpha<<std::endl;
        iter++;
        //std::cout<<"leval="<<leval<<std::endl;//debugging
    }

    if (feval >= params->maxiter){conv=false;}
    //if (!isfinite(SquaremDefault.objfninc)){loldcpp=objfn(pcpp);leval++;}

    //assigning values
    assignfromvec(pcpp);

    //sqobj.par=pcpp;
    //sqobj.valueobjfn=loldcpp;
    //sqobj.iter=iter;
    //sqobj.pfevals=feval;
    //sqobj.objfevals=leval;
    //sqobj.convergence=conv;
    //return(sqobj);
}


void Data23::squarem2(){
	// without calls to the likelihood
    double res,parnorm,kres;
    std::vector<double> pcpp,p1cpp,p2cpp,pnew,ptmp;
    std::vector<double> q1,q2,sr2,sq2,sv2,srv;
    double sr2_scalar,sq2_scalar,sv2_scalar,srv_scalar,alpha,stepmin,stepmax;
    int iter,feval;
    bool conv,extrap;
    stepmin=params->stepmin0;
    stepmax=params->stepmax0;
    //if(SquaremDefault.trace){std::cout<<"Squarem-1"<<std::endl;}
    pcpp = params2vec();
    //cout << "here1\n"; cout.flush();
    pnew = params2vec();
    iter=1;
    feval=0;conv=true;

    long int parvectorlength=pcpp.size();

    int completeit = 0;
    while(completeit<params->maxiter){

        //Step 1
        extrap = true;
        try{
        	updateP();
        	updateQ();
        	p1cpp = params2vec();
        	feval++;
        }
        catch(...){
            std::cout<<"Error in fixptfn function evaluation";
            exit(1);
        }

        sr2_scalar=0;
        for (int i=0;i<parvectorlength;i++){sr2_scalar+=pow(p1cpp[i]-pcpp[i],2);}
        if(sqrt(sr2_scalar)<params->tol){break;}

        //Step 2
        try{
        	updateP();
        	updateQ();
        	p2cpp = params2vec();
        	feval++;
        }
        catch(...){
            std::cout<<"Error in fixptfn function evaluation";
            exit(1);
        }
        sq2_scalar=0;
        for (int i=0;i<parvectorlength;i++){sq2_scalar+=pow(p2cpp[i]-p1cpp[i],2);}
        sq2_scalar=sqrt(sq2_scalar);
        if (sq2_scalar<params->tol){break;}
        res=sq2_scalar;

        sv2_scalar=0;
        for (int i=0;i<parvectorlength;i++){sv2_scalar+=pow(p2cpp[i]-2*p1cpp[i]+pcpp[i],2);}
        srv_scalar=0;
        for (int i=0;i<parvectorlength;i++){srv_scalar+=(p2cpp[i]-2*p1cpp[i]+pcpp[i])*(p1cpp[i]-pcpp[i]);}
        //std::cout<<"sr2,sv2,srv="<<sr2_scalar<<","<<sv2_scalar<<","<<srv_scalar<<std::endl;//debugging

        //Step 3 Proposing new value
       // switch (SquaremDefault.method){
       //     case 1: alpha= -srv_scalar/sv2_scalar;
       //     case 2: alpha= -sr2_scalar/srv_scalar;
        //    case 3: alpha= sqrt(sr2_scalar/sv2_scalar);
       // }
        alpha= sqrt(sr2_scalar/sv2_scalar);
        alpha=std::max(stepmin,std::min(stepmax,alpha));
        //std::cout<<"alpha="<<alpha<<std::endl;//debugging
        for (int i=0;i<parvectorlength;i++){pnew[i]=pcpp[i]+2.0*alpha*(p1cpp[i]-pcpp[i])+pow(alpha,2)*(p2cpp[i]-2*p1cpp[i]+pcpp[i]);}
        //pnew = pcpp + 2.0*alpha*q1 + alpha*alpha*(q2-q1);

        //Step 4 stabilization
        if(std::abs(alpha-1)>0.01){
            try{
            	assignfromvec(pnew);
            	updateP();
            	updateQ();
            	ptmp= params2vec();
            	feval++;
            }
            catch(...){
                pnew=p2cpp;
                if(alpha==stepmax){
                    stepmax=std::max(params->stepmax0,stepmax/params->mstep);
                }
                alpha=1;
                extrap=false;
                if(alpha==stepmax){stepmax=params->mstep*stepmax;}
                if(stepmin<0 && alpha==stepmin){stepmin=params->mstep*stepmin;}
                pcpp=pnew;
                std::cout<<"Residual: "<<res<<"  Extrapolation: "<<extrap<<"  Steplength: "<<alpha<<std::endl;
                iter++;
                continue;//next round in while loop
            }
            res=0;
            for (int i=0;i<parvectorlength;i++){res+=pow(ptmp[i]-pnew[i],2);}
            res=sqrt(res);
            parnorm=0;
            for (int i=0;i<parvectorlength;i++){parnorm+=pow(p2cpp[i],2);}
            parnorm=sqrt(parnorm/parvectorlength);
            kres=params->kr*(1+parnorm)+sq2_scalar;
            if(res <= kres){
                pnew=ptmp;
            }else{
                pnew=p2cpp;
                if(alpha==stepmax){stepmax=params->mstep*stepmax;}
                alpha=1;
                extrap=false;
            }
        }

        if(alpha==stepmax){stepmax=params->mstep*stepmax;}
        if(stepmin<0 && alpha==stepmin){stepmin=params->mstep*stepmin;}

        pcpp=pnew;
        std::cout<<completeit<< "  Residual: "<<res<<"  Extrapolation: "<<extrap<<"  Steplength: "<<alpha<< "  param_diffs: "<< sq2_scalar << std::endl;
        iter++;
        if (completeit % 20 ==0) {
        	dropuseless();
        	pcpp = params2vec();
        	parvectorlength=pcpp.size();
        }
        completeit++;
    }

    if (completeit >= params->maxiter){conv=false;}

    //assigning values
    cout << "EXITING: convergence = "<< conv << "\n";
    assignfromvec(pcpp);
    //sqobj.par=pcpp;
   // sqobj.valueobjfn=NAN;
   // sqobj.iter=iter;
    //sqobj.pfevals=feval;
   // sqobj.objfevals=0;
   // sqobj.convergence=conv;
   // return(sqobj);
}

void Data23::GSL_optim(vector<int> pop2keep, int skip){
	if (pop2keep.size() < 2){
		set_single_weight(pop2keep[0]);
		current_llk = llk(pop2keep, skip);
		return;
	}
	set_weights(pop2keep);
	int nparam = pop2keep.size()-1;
	size_t iter = 0;
	double size;
    int status;
    const gsl_multimin_fminimizer_type *T =
    		gsl_multimin_fminimizer_nmsimplex2;
    gsl_multimin_fminimizer *s;
    gsl_vector *x;
    gsl_vector *ss;
    gsl_multimin_function lm;
    lm.n = nparam;
    lm.f = &GSL_llk;
    struct GSL_params p;
    p.d = this;
    p.which = pop2keep;
    p.skip = skip;
    lm.params = &p;
    //cout << llk()<< "\n"; cout.flush();
    //
    // initialize parameters
    //
    x = gsl_vector_alloc (nparam);
    for (int i = 0; i < nparam; i++)   gsl_vector_set(x, i, 0);

    // set initial step sizes to 1
    ss = gsl_vector_alloc(nparam);
    gsl_vector_set_all(ss, 1.0);
    s = gsl_multimin_fminimizer_alloc (T, nparam);

    gsl_multimin_fminimizer_set (s, &lm, x, ss);
    bool fixed = false;
    do
     {

             iter++;
             status = gsl_multimin_fminimizer_iterate (s);

             if (status){
                     printf ("error: %s\n", gsl_strerror (status));
                     break;
             }
             size = gsl_multimin_fminimizer_size(s);
             status = gsl_multimin_test_size (size, 0.0001);
             //cout << iter << " "<< iter %10 << "\n";
             if (iter % 20 < 1 || iter < 2){
            	 for(vector<int>::iterator it = pop2keep.begin(); it != pop2keep.end(); it++) cout << " "<< pi[*it];
            	 //for(vector<int>::iterator it = pop2keep.begin(); it != pop2keep.end(); it++) cout << " "<< alpha[*it];
            	 cerr << " "<< s->fval << " "<< size <<  "\n";

             }
     }
     while (status == GSL_CONTINUE && iter <5000);
     if (iter > 4999) {
             cerr << "WARNING: failed to converge\n";
             //exit(1);
     }
     for (int i = 0; i < nparam; i++) alpha[pop2keep[i]] = gsl_vector_get(s->x, i);
     set_weights(pop2keep);
     cout << "# held out fold "<< skip;
     for(vector<int>::iterator it = pop2keep.begin(); it != pop2keep.end(); it++) cout << " "<< pi[*it];
     cout << "\n";
     current_llk = llk(pop2keep, skip);
     gsl_multimin_fminimizer_free (s);
     gsl_vector_free (x);
     gsl_vector_free(ss);

}

double Data23::llk_xv(vector<int> pop2keep){
	double lk = -10000;
	for (int j = 0; j < chrsegments.size(); j++){
		GSL_optim(pop2keep, j);
		pair<int, int> seg  = chrsegments[j];
		for (int i = seg.first;i < seg.second ; i++){
			//cout << info[i].first << " "<< info[i].second << " ";
			bool g = data[i];
			double ff = 0;
			//cout << g <<  " ";
			for (vector<int>::iterator it2 = pop2keep.begin(); it2 != pop2keep.end(); it2++){
				double w = pi[*it2];
				double f = ref->get_f(info[i], refpopnames[*it2]);
						//cout << refpopnames[j]<< "("<< f<<") ";
				if (!isnan(f)) ff += w*f;
			}
			if (g && ff> 0) lk+= log(ff);
			else if (ff>00) lk+= log(1-ff);

		}
	}
	return lk;

}
double Data23::llk(vector<int> pop2keep, int skip){
	double lk = -10000;
	for (int j = 0; j < chrsegments.size(); j++){
		if (j == skip) continue;
		pair<int, int> seg  = chrsegments[j];
		for (int i = seg.first;i < seg.second ; i++){
			//cout << info[i].first << " "<< info[i].second << " ";
			bool g = data[i];
			double ff = 0;
			//cout << g <<  " ";
			for (vector<int>::iterator it2 = pop2keep.begin(); it2 != pop2keep.end(); it2++){
				double w = pi[*it2];
				double f = ref->get_f(info[i], refpopnames[*it2]);
				//cout << refpopnames[j]<< "("<< f<<") ";
				if (!isnan(f)) ff += w*f;
			}
			if (g && ff> 0) lk+= log(ff);
			else if (ff>00) lk+= log(1-ff);
		//cout <<  "\n";
		}
	}
	lk += lnvecdens(alpha);
	return lk;
}


double Data23::lndgauss(double dif, double se){
        double toreturn = 0;
        toreturn += -log (se * sqrt(2.0*M_PI));
        toreturn += -(dif*dif) /(2*se*se);
        return toreturn;
}

/*
void Data23::test_all(){
	vector<int> kept;
	for (int i = 0; i< npop; i++){
		kept.push_back(i);
		//set_single_weight(i);
		//cout << "1POP: "<< llk(kept)<< " "<< refpopnames[i]<<"\n";
	}
	GSL_optim(kept);
	for (int i = 0; i < npop; i++){
		cout << refpopnames[i] << " "<< pi[i]<<"\n";
	}
}

void Data23::test_all_single(){
	for (int i = 0; i< npop; i++){
		vector<int> kept;
		kept.push_back(i);
		set_single_weight(i);
		cout << "1POP: "<< llk(kept)<< " "<< refpopnames[i]<<"\n";
	}
}

void Data23::test_all_single_f2(){
	for (int i = 0; i< npop; i++){
		double f2 = calc_f2(i);
		cout << "1POP: "<<f2<< " "<< refpopnames[i]<<"\n";
	}
}

void Data23::test_all_double(){
	for (int i = 0; i < npop; i++){
		for(int j = i+1; j < npop; j++){
			vector<int> pops;
			pops.push_back(i); pops.push_back(j);
			GSL_optim(pops);
			cout << "2POP: "<< current_llk << " "<< refpopnames[i]<< " "<< refpopnames[j] << " "<< pi[i] << " "<< pi[j]<< "\n";
		}
	}
}
*/
void Data23::test_subset(string outfile){
	ofstream out(outfile.c_str());
	vector<int> pops;
	for (int i = 0; i< npop; i++){
		string tpop = refpopnames[i];
		if (params->inpops.find(tpop) != params->inpops.end()){
			pops.push_back(i);
		}
	}
	if (pops.size() < 1){
		cerr<< "ERROR: reference population names not found in reference file\n";
		exit(1);
	}
	double lkxv = llk_xv(pops);
	GSL_optim(pops, -9);
	cout << "LLK: "<<lkxv;
	for (int i = 0; i < pops.size(); i++) cout << " "<< refpopnames[pops[i]];
	for (int i = 0; i < pops.size(); i++) cout << " "<< pi[pops[i]];
	cout <<"\n";

	out << "LLK: "<< lkxv<< " "<< pops.size();
	for (int i = 0; i < pops.size(); i++) out << " "<< refpopnames[pops[i]];
	for (int i = 0; i < pops.size(); i++) out << " "<< pi[pops[i]];
	out <<"\n";
}

void Data23::exhaustive_test(string outfile){
	ofstream out(outfile.c_str());
	vector<int> pops;
	for (int i = 0; i< npop; i++){
		string tpop = refpopnames[i];
		if (params->inpops.find(tpop) != params->inpops.end()){
			pops.push_back(i);
		}
	}
	if (pops.size() < 1){
		cerr<< "ERROR: reference population names not found in reference file\n";
		exit(1);
	}

	//likelihood with all populations
	double lkxv = llk_xv(pops);
	GSL_optim(pops, -9);
	cout << "LLK: "<<lkxv;
	for (int i = 0; i < pops.size(); i++) cout << " "<< refpopnames[pops[i]];
	for (int i = 0; i < pops.size(); i++) cout << " "<< pi[pops[i]];
	cout <<"\n";

	out << "LLK: "<< lkxv<< " "<< pops.size();
	for (int i = 0; i < pops.size(); i++) out << " "<< refpopnames[pops[i]];
	for (int i = 0; i < pops.size(); i++) out << " "<< pi[pops[i]];
	out <<"\n";

	//keep track of best model
	bool done = false;
	vector<int> bestpops;
	vector<int> testpops;
	double bestlk = lkxv;
	double prevbest = lkxv;
	for (vector<int>::iterator it = pops.begin(); it != pops.end(); it ++){
		bestpops.push_back(*it);
		testpops.push_back(*it);
	}

	while (!done && testpops.size() > 1){
		double best_lk_thisiter = -1e15;

		//drop each population in turn
		for (int i = 0; i < testpops.size(); i++){
			vector<int> testpops2;
			for (int j = 0; j < testpops.size(); j++){
				if (i == j) continue;
				testpops2.push_back(testpops[j]);
			}
			double testlkxv = llk_xv(testpops2);
			if (testlkxv > best_lk_thisiter) best_lk_thisiter = testlkxv;
			if (testlkxv > bestlk){
				bestlk = testlkxv;
				bestpops.clear();
				for (vector<int>::iterator it = testpops2.begin(); it != testpops2.end(); it++) bestpops.push_back(*it);
			}
			GSL_optim(testpops2, -9);
			cout << "LLK: "<<testlkxv;
			for (int i = 0; i < testpops2.size(); i++) cout << " "<< refpopnames[testpops2[i]];
			for (int i = 0; i < testpops2.size(); i++) cout << " "<< pi[testpops2[i]];
			cout <<"\n";

			out << "LLK: "<< testlkxv<< " "<< testpops2.size();
			for (int i = 0; i < testpops2.size(); i++) out << " "<< refpopnames[testpops2[i]];
			for (int i = 0; i < testpops2.size(); i++) out << " "<< pi[testpops2[i]];
			out <<"\n";
		}
		// if dropping a population improved the model, try dropping additional populations
		if (best_lk_thisiter > prevbest){
			prevbest = best_lk_thisiter;
			testpops.clear();
			for (vector<int>::iterator it = bestpops.begin(); it != bestpops.end(); it++) testpops.push_back(*it);
		}

		//otherwise finish
		else done = true;
	}

	GSL_optim(bestpops, -9);
	cout << "BEST LLK: "<<bestlk;
	for (int i = 0; i < bestpops.size(); i++) cout << " "<< refpopnames[bestpops[i]];
	for (int i = 0; i < bestpops.size(); i++) cout << " "<< pi[bestpops[i]];
	cout <<"\n";

	out << "BEST LLK: "<< bestlk<< " "<< bestpops.size();
	for (int i = 0; i < bestpops.size(); i++) out << " "<< refpopnames[bestpops[i]];
	for (int i = 0; i < bestpops.size(); i++) out << " "<< pi[bestpops[i]];
	out <<"\n";
}
/*
void Data23::test_all_triple(){
	for (int i = 0; i < npop; i++){
		for(int j = i+1; j < npop; j++){
			for(int k = j+1; k < npop; k++){
				vector<int> pops;
				pops.push_back(i); pops.push_back(j); pops.push_back(k);
				GSL_optim(pops);
				cout << "3POP: "<< current_llk << " "<< refpopnames[i]<< " "<< refpopnames[j] << " "<< refpopnames[k]<< " "<< pi[i] << " "<< pi[j]<< " "<< pi[k] << "\n";

			}
		}
	}
}
*/
void Data23::set_single_weight(int which){
	//cout << "Setting all weight to "<< refpopnames[which]<< "\n";
	if (which < alpha.size()){
		vector<int> kept;
		kept.push_back(which);
		set_weights(kept);
	}
	else{
		cerr <<"ERROR: index "<< which <<" not found in Data23::set_single_weight\n";
		exit(1);
	}

}

double Data23::lnvecdens(vector<double> current){

	vector<double> tmp;
	for (int i = 0; i < current.size(); i++) tmp.push_back( lndgauss(current[i], 3));
	double toreturn = tmp[0];
	for (int i = 1; i < current.size(); i++) toreturn+= tmp[i];
	return toreturn;
}
/*
void Data23::MCMC(gsl_rng *r){

	cout << "iteration nonzero llk";
	for (int i = 0; i < npop; i++){
		cout << " "<< refpopnames[i];
	}
	cout << " running_accept\n";
	//burnin
    int nsamp = 0;
    int naccept = 0;
    for (int i = 0; i < params->burnin; i++) {
        naccept+= MCMC_update(r);
        nsamp+=npop;
    	if (i % params->sampfreq ==0) {
    		cout << i << " " << nonzero_count<< " ";

    		cout <<  current_llk;
    		for (int i = 0; i < npop; i++)	cout << " "<< pi[i];
    		cout << " "<< (float) naccept/ (float)nsamp<< "\n";
    	}
      }
    nsamp = 0;
    naccept = 0;
    for (int i = 0; i < params->nsamp; i++){
              naccept+= MCMC_update(r);
              nsamp+=npop;
              if (i % params->sampfreq ==0) {
            	  	cout << i << " " << nonzero_count << " ";

              		cout <<  current_llk;
              		for (int i = 0; i < npop; i++)	cout << " "<< pi[i];
              		cout << " "<< (float) naccept/ (float)nsamp<< "\n";
              }

      }
}
*/


/*
int Data23::MCMC_update(gsl_rng *r){

	int toreturn = 0;

	for (int i = 0; i < npop; i++){
		// copy current state
		vector<double> oldpi;
		vector<double> oldalpha;
		double oldllk = current_llk;
		for (int i = 0; i < npop; i++){
			oldpi.push_back(pi[i]);
			oldalpha.push_back(alpha[i]);
		}

		propose_newpi(i, r);
		double oldprior = compute_prior(oldpi);
		double newprior = compute_prior(pi);

		double num = llk()+ newprior;
		double denom = oldllk+ oldprior;
		double diff = num - denom;
		double acceptance = exp(diff);
		if (acceptance > 1) toreturn++;

		//generate random unif
		else{
			double unif = gsl_rng_uniform(r);

			if (unif < acceptance) toreturn++;
			else{
				for (int i = 0; i < npop; i++) {
					alpha[i] = oldalpha[i];
					pi[i] = oldpi[i];
				}
				current_llk = oldllk;
			}
		}
	}
    return toreturn;
}
*/
double Data23::compute_prior(vector<double> testpi){
	// prior on the proportions is a prior on the number of weights over 0 (or some epsilon)
	int count = 0;
	for (int i = 0; i < testpi.size(); i++){
		if (testpi[i] > 1e-5) count++;
	}
	nonzero_count = count;
	if (count > 5){
		int nover = npop-5;
		double f = 0.02/(double) nover;
		return(log(f));
	}
	else if (count ==1){
		return (log(0.246));
	}
	else if (count ==2) return(log(0.221));
	else if (count == 3) return(log(0.196));
	else if (count ==4) return(log(0.171));
	else if (count == 5) return(log(0.146));
	else{
		cerr << "ERROR: can't compute prior, count = "<< count << "\n";
		exit(1);
	}


}

void Data23::propose_newpi(int index, gsl_rng *r){
	cout << index <<"\n";
	double tmp = gsl_ran_gaussian(r, params->MCMC_gauss_SD);
	//cout << tmp << "\n";
	alpha[index] = alpha[index]+ tmp;
    set_weights();
}

double Data23::calc_f2(int index){
	double f2 = 0;
	int totalsnp = 0;
	for (int i = 0;i < nsnp ; i++){
		bool g = data[i];
		double f = ref->get_f(info[i], refpopnames[index]);

		if (!isnan(f)){
			if (g) f2+= (1-f)*(1-f);
			else f2+= f*f;
			totalsnp++;
		}
	}
	f2 = f2/(double) totalsnp;
	//double tm = ref->trim[refpopnames[index]];
	//f2 = f2-tm;
	return f2;
}


double Data23::calc_f2(vector<int> pop2keep){
	double f2 = 0;
	int totalsnp = 0;
	for (int i = 0;i < nsnp ; i++){
		bool g = data[i];
		double ff = 0;
		for (vector<int>::iterator it = pop2keep.begin(); it != pop2keep.end(); it++){
			double w = pi[*it];
			double f = ref->get_f(info[i], refpopnames[*it]);
			if (!isnan(f)) ff += w*f;
		}
		if (g) f2+= (1-ff)*(1-ff);
		else f2+= ff*ff;
		totalsnp++;
	}
	f2 = f2/(double) totalsnp;
	for (vector<int>::iterator it = pop2keep.begin(); it != pop2keep.end(); it++){
		double w = pi[*it];
		string popname = refpopnames[*it];
		//double tm = ref->trim[popname];
		//f2 = f2- w*w*tm;
	}
	return f2;
}

double GSL_llk(const gsl_vector *x, void *params ){

	int np = ((struct GSL_params *) params)->which.size();

	for (int i = 0; i < np-1; i++){
		int index = ((struct GSL_params *) params)->which[i];
		//cout << index << " "<< ((struct GSL_params *) params)->which[i+1] <<  " i\n";
		((struct GSL_params *) params)->d->alpha[index] = gsl_vector_get(x, i);
	}
	((struct GSL_params *) params)->d->set_weights(((struct GSL_params *) params)->which);
	return -((struct GSL_params *) params)->d->llk(((struct GSL_params *) params)->which, ((struct GSL_params *) params)->skip);
}

double GSL_f2(const gsl_vector *x, void *params ){

	int np = ((struct GSL_params *) params)->which.size();

	for (int i = 0; i < np-1; i++){
		int index = ((struct GSL_params *) params)->which[i];
		//cout << index << " "<< ((struct GSL_params *) params)->which[i+1] <<  " i\n";
		((struct GSL_params *) params)->d->alpha[index] = gsl_vector_get(x, i);
	}
	((struct GSL_params *) params)->d->set_weights(((struct GSL_params *) params)->which);
	return ((struct GSL_params *) params)->d->calc_f2(((struct GSL_params *) params)->which);
}

