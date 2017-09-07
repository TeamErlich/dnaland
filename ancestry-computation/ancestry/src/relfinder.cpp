/*
 * relfinder.cpp
 *
 *  Created on: Oct 16, 2016
 *      Author: jkpickrell
 */

#include "FreqData.h"
#include "Data23.h"

void printv(){
	cout << "\nrelfinder v. 0.02\n";
	cout << "Joe Pickrell (joepickrell@gmail.com)\n\n";
}

void printopts(){
	cout << "Options:\n";
	cout << "-i [file name] reference input file\n";
	cout << "-gp [file name] first sample input file, in pileup format\n";
	cout << "-gp2 [file name] second sample input file, in pileup format\n";
	cout << "-a2 [file name] second sample ancestry, in format output from ancestry\n";
	cout << "-o [string] stem for output files (relfinder)\n";
	cout << "\n";
}


int main(int argc, char *argv[]){
	printv();
    CCmdLine cmdline;
    Ancestry_params p;
    Ancestry_params p2;
    string a2_file;
    string outstem = "relfinder";
    if (cmdline.SplitLine(argc, argv) < 1){
    	printopts();
    	exit(1);
    }
    if (cmdline.HasSwitch("-i")) p.infile = cmdline.GetArgument("-i", 0);
    else{
    	printopts();
    	exit(1);
    }
    if (cmdline.HasSwitch("-o")) outstem = cmdline.GetArgument("-o", 0);

    if (cmdline.HasSwitch("-gp"))	{
      	p.input23 = false;
      	p.sampfile = cmdline.GetArgument("-gp", 0);
    }
    else{
      	printopts();
      	exit(1);
      }

    if (cmdline.HasSwitch("-gp2"))	{
    	p2.input23= false;
      	p2.sampfile = cmdline.GetArgument("-gp2", 0);
    }
    else{
      	printopts();
      	exit(1);
    }
    if (cmdline.HasSwitch("-a2"))	{
       	a2_file = cmdline.GetArgument("-a2", 0);
     }
     else{
       	printopts();
       	exit(1);
     }


    //random number generator
    const gsl_rng_type * T;
    gsl_rng * r;
    gsl_rng_env_setup();
    T = gsl_rng_ranlxs2;
    r = gsl_rng_alloc(T);
    gsl_rng_set(r, p.seed);

    // print the parameter settings
    p.print();

    // read the reference panel
    FreqData cd(&p);

    // read the data for the test individual
    Data23 f23(&p, &cd, r);

    // read the data for the second individual
    Data23 f23_2(&p2, &cd, r);

    // read the ancestry file
    map<string, float> a2;
    ifstream in(a2_file.c_str());
    vector<string> line;
    struct stat stFileInfo;
    int intStat;
    string st, buf;

    intStat = stat(a2_file.c_str(), &stFileInfo);
    if (intStat !=0){
    	std::cerr<< "ERROR: cannot open file " << a2_file << "\n";
    	exit(1);
    }

    while(getline(in, st)){
    	buf.clear();
    	stringstream ss(st);
    	line.clear();
    	while (ss>> buf){
    		line.push_back(buf);
    	}
    	string p = line[0];
    	float f = atof(line[1].c_str());
    	//cout << p << " "<< f << "\n";
    	a2.insert(make_pair(p, f));
    }

    // run the inference
    vector<pair<double, double> > rel = f23.relfinder(&f23_2, a2);
    string oflk = outstem+".llk";
    ofstream outf(oflk.c_str());
    for (vector<pair<double, double> >::iterator it = rel.begin(); it != rel.end(); it++){
    	outf << it->first << " "<< it->second << "\n";
    }

    // get maximum
    int maxi = 0;
    int maxlk = rel[0].second;
    for (int i = 0; i < rel.size(); i++){
    	if (rel[i].second > maxlk){
    		maxi = i;
    		maxlk = rel[i].second;
    	}
    }
    string ofsummary = outstem+".summary";
    ofstream outf2(ofsummary.c_str());
    outf2 << "est: "<< rel[maxi].first << "\n";
    outf2 << "maxlk: "<< rel[maxi].second <<"\n";
    outf2 << "lk0: "<< rel[0].second << "\n";

    return 0;

}
