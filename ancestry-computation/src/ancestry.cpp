/*
 * ancestry.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: jkpickrell
 */
#include "FreqData.h"
#include "Data23.h"

void printv(){
	cout << "\nancestry v. 0.03\n";
	cout << "Joe Pickrell (joepickrell@gmail.com)\n\n";
}

void printopts(){
	cout << "Options:\n";
	cout << "-i [file name] reference input file\n";
	cout << "-g23 [file name] sample input file, in 23andMe format\n";
	cout << "-gp [file name] sample input file, in pileup format\n";
	cout << "-gvcf [file name] sample input file, in VCF/BCF format\n";
	cout << "-o [string] stem for output files (ancestry)\n";
	cout << "-tol [float] tolerance from calling convergence (1e-5)\n";
	cout << "-skip [string] chromosome to exclude from analysis (none)\n";
	cout << "-e [float] ancestry proportion below which population is dropped (1e-3)\n";
	cout << "-b [int] run bootstrap iterations (0)\n";
	cout << "-ran [float] proportion of SNPs in the input file to keep (1)\n";
	cout << "-ranN [int] randomly sample approximately N SNPs\n";
	cout << "-s [int] set the random seed [time(0)]\n";
	cout << "-noP don't print the P matrix\n";
	cout << "\n";
}


int main(int argc, char *argv[]){
	printv();
    CCmdLine cmdline;
    Ancestry_params p;
    if (cmdline.SplitLine(argc, argv) < 1){
    		printopts();
    		exit(1);
    }
    if (cmdline.HasSwitch("-i")) p.infile = cmdline.GetArgument("-i", 0);
    else{
    		printopts();
    		exit(1);
    }
    if (cmdline.HasSwitch("-o")) p.outstem = cmdline.GetArgument("-o", 0);
    if (cmdline.HasSwitch("-noP")) p.noP = true;
    if (cmdline.HasSwitch("-skip"))	{
      	p.skip = true;
      	p.skipchr = cmdline.GetArgument("-skip", 0);
      }

    if (cmdline.HasSwitch("-g23"))	{
    		p.input23 = true;
    		p.inputVCF = false;
    		p.sampfile = cmdline.GetArgument("-g23", 0);
    }
    if (cmdline.HasSwitch("-EM"))	{
     	p.EM= true;
     }
    if (cmdline.HasSwitch("-tol")){
    		p.tol = atof(cmdline.GetArgument("-tol", 0).c_str());
    }
    if (cmdline.HasSwitch("-e")){
     	p.dropepsilon = atof(cmdline.GetArgument("-e", 0).c_str());
     }
    if (cmdline.HasSwitch("-gp"))	{
      	p.input23 = false;
      	p.sampfile = cmdline.GetArgument("-gp", 0);
    }
    if (cmdline.HasSwitch("-gvcf"))	{
      	p.inputVCF = true;
      	p.input23 = false;
      	p.sampfile = cmdline.GetArgument("-gvcf", 0);
    }
    if (cmdline.HasSwitch("-b")) {
    		p.bootstrap = true;
    		p.nboot = atoi(cmdline.GetArgument("-b", 0).c_str());
    }
    if (cmdline.HasSwitch("-ran")) {
    		p.ransnp = true;
    		p.ranfrac = atof(cmdline.GetArgument("-ran", 0).c_str());
    }
    if (cmdline.HasSwitch("-ranN")) {
      	p.doranN = true;
      	p.ranN = atoi(cmdline.GetArgument("-ranN", 0).c_str());
      }

    if (!cmdline.HasSwitch("-g23") and !cmdline.HasSwitch("-gp") and !cmdline.HasSwitch("-gvcf")){
    		cerr << "ERROR: need an input genotype file (-g23, -gp, or -gvcf)\n";
    		printopts();
    		exit(1);
    }
    if (cmdline.HasSwitch("-s")){
      	p.seed = atoi(cmdline.GetArgument("-s", 0).c_str());
    }
    else{
        p.seed = (int) time(0);
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

    if (!p.bootstrap){
    		// read the data for the test individual
    		Data23 f23(&p, &cd, r);
    		// run the inferences
    		if (p.doranN) f23.sampleN(p.ranN, r);
    		if (p.EM)f23.EM();
    		else f23.squarem2();

    		// print the estimated parameters
    		string llkfile = p.outstem+".llk";
    		ofstream outlk(llkfile.c_str());
    		outlk << f23.llk() << "\n";
    		if (!p.noP) f23.printP(p.outstem+".P");
    		f23.printQ(p.outstem+".Q");
    		//f23.print();
    }
    else{
    		for (int i = 0; i < p.nboot; i++){

    		/*read in the data for every bootstrap iteration
    	  	    this could be skipped but is a small amount of time anyways.
    	  	    note that using -b and -ran or -ranN means we randomly sample snps, then boostrap on those

    	  	*/
    			Data23 f23(&p, &cd, r);
    			if (p.doranN) f23.sampleN(p.ranN, r);
    			f23.bootstrap(r);
    			// run the inferences
    			if (p.EM)f23.EM();
    			else f23.squarem2();

    			stringstream ss;
    			ss << i;

    			// print the estimated parameters
    			string llkfile = p.outstem+"."+ss.str()+".llk";
    			ofstream outlk(llkfile.c_str());
    			outlk << f23.llk() << "\n";
    			if (!p.noP) f23.printP(p.outstem+"."+ss.str()+".P");
    			f23.printQ(p.outstem+"."+ss.str()+".Q");
    		}
    }

    return 0;

}
