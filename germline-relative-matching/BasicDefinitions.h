// BasicDefinitions.h: constants and enumerations for GERMLINE

#ifndef BASICDEFINITIONS_H
#define BASICDEFINITIONS_H
// #define VERBOSE
#include <vector>
#include <fstream>
#include <iostream>
#include <string.h>
#include <unordered_set>

using namespace std;

// where we are in the sequence (markerset & physical)
extern unsigned int position_ms;
extern unsigned int num_sets;
extern size_t num_samples;

extern unsigned long num_matches;

// type for file format
enum FileFormat{HAPS,PED,HM,UNKNOWN_FORMAT};
// type for families
enum FamilyType{NOPARENT,SINGLEPARENT,TWOPARENT};
// type for sex
enum Sex{MALE,FEMALE,UNKNOWN};
// type for nucleotides
enum Nucleotide{A=1,C=2,G=3,T=4};
// type for chromosomes types
enum ChromosomeType{TRANS=0,UNTRANS=1,MISSING=-1};
// type for chromosome ids
enum ChromosomeID{ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN, TWELVE,
                THIRTEEN, FOURTEEN, FIFTEEN, SIXTEEN, SEVENTEEN, EIGHTEEN, NINETEEN, TWENTY,
				TWENTYONE, TWENTYTWO, X, Y};

// parameters
extern double MIN_MATCH_LEN;
extern int MARKER_SET_SIZE;
extern int MAX_ERR_HOM;
extern int MAX_ERR_HET;
extern bool PRINT_MATCH_HAPS;
extern bool ROI;
extern bool HAPLOID;
extern bool HAP_EXT;
extern bool WIN_EXT;
extern bool ALLOW_HOM;
extern bool HOM_ONLY;
extern bool SILENT;
extern bool STATUS_STREAMABLE;
extern bool DEBUG;
extern bool BINARY_OUT;
extern FileFormat FILE_FORMAT;
extern string PED_FILENAME;
extern string MAP_FILENAME;
extern string OUT_FILENAME;
extern bool OVERRIDE_CACHE_SAFETY_CHECKS;


// Parameters relevant to individual lists
extern string INDIV_FILE; // Name of file with a list of individuals
extern string SINGLE_INDIV; // One individual of interest
extern bool INDIV_IS_WHITELIST; // Are the individual(s) in the preceeding variables those *to* match (as opposed to those *not* to match)
extern string OUT_INDIV_FILE; // When we're done, dump the list of individuals we did try to match to this file
extern unordered_set<string> OUT_INDIV_SET; // The data that will go in the preceeding file

// If we specify newIndivFile or singleNewId, then we have listed what *to* match and this will
// be true.  If not, then we have a list of what *not to* match and this will be false.


class SNPs;
class Individuals;
extern SNPs ALL_SNPS;
extern Individuals ALL_SAMPLES;
extern ofstream MATCH_FILE;

enum ErrorType{RECOMB=0,MI=1};
const int HET=2;
const int MIS=9;
const short PAR_M = 0;
const short PAR_F = 1;

#endif

// end of BasicDefinitions.h
