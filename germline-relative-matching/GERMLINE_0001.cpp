#include "GERMLINE.h"
#include "Cache.h"
#include <string.h>
#include <err.h>

double MIN_MATCH_LEN = 3;
int MARKER_SET_SIZE = 128;
bool PRINT_MATCH_HAPS = false;
bool ROI = false;
bool HAP_EXT = false;
bool WIN_EXT = false;
bool ALLOW_HOM = false;
bool HOM_ONLY = false;
bool HAPLOID = false;
bool SILENT = false;
bool STATUS_STREAMABLE = false;
bool DEBUG = false;
bool BINARY_OUT = false;
int MAX_ERR_HOM = 4;
int MAX_ERR_HET = 1;
FileFormat FILE_FORMAT = UNKNOWN_FORMAT;
string PED_FILENAME = "";
string MAP_FILENAME = "";
string OUT_FILENAME = "";
bool OVERRIDE_CACHE_SAFETY_CHECKS = false;

//Tris: adding new parameter for file with Ids of new individuals
string INDIV_FILE = ""; 
string SINGLE_INDIV = "";
bool INDIV_IS_WHITELIST = false;
string OUT_INDIV_FILE = "";
unordered_set<string> OUT_INDIV_SET;


// main(): runs GERMLINE
int main(int argc, char* argv[])
{
	// parse arguments
	string rs_range[2] , map; map = rs_range[0] = rs_range[1] = "";
	string params = argv[0];
	bool saw_new_ids = false, saw_old_ids = false, saw_single_id = false;
        string cache_dir;

	bool bad_param = false;
	bool show_help = false;

	for(int i=1;i<argc;i++){
		params += " " + string(argv[i]);
		if( strncmp(argv[i], "-min_m", strlen("-min_m")) == 0 && i < argc-1)			MIN_MATCH_LEN = atof(argv[++i]);
		else if( strncmp(argv[i], "-err_hom", strlen("-max_err")) == 0 && i < argc-1)		{ MAX_ERR_HOM = atoi(argv[++i]); }
		else if( strncmp(argv[i], "-err_het", strlen("-max_err")) == 0 && i < argc-1)		{ MAX_ERR_HET = atoi(argv[++i]); }
		else if( strncmp(argv[i], "-from_snp", strlen("-from_snp")) == 0 && i < argc-1 )	rs_range[0] = argv[++i];
		else if( strncmp(argv[i], "-to_snp", strlen("-to_snp")) == 0 && i < argc-1 )		rs_range[1] = argv[++i];
		else if( strncmp(argv[i], "-print", strlen("-print")) == 0 )				PRINT_MATCH_HAPS = true;
		else if( strncmp(argv[i], "-silent", strlen("-silent")) == 0 )				SILENT = true;
		else if( strncmp(argv[i], "-status_streamable", strlen("-status_streamable")) == 0 )	STATUS_STREAMABLE = true;
		else if( strncmp(argv[i], "-debug", strlen("-debug")) == 0 )				DEBUG = true;
		else if( strncmp(argv[i], "-map", strlen("-map")) == 0 && i < argc-1)			map = argv[++i];
		else if( strncmp(argv[i], "-bits", strlen("-bits")) == 0 && i < argc-1)			MARKER_SET_SIZE = atoi(argv[++i]);
		else if( strncmp(argv[i], "-homoz-only", strlen("-homoz-only")) == 0 )			{ ALLOW_HOM = true; HOM_ONLY = true; }
		else if( strncmp(argv[i], "-homoz", strlen("-homoz")) == 0 )				ALLOW_HOM = true;
		else if( strncmp(argv[i], "-bin_out", strlen("-bin_out")) == 0 )			BINARY_OUT = true;
		else if( strncmp(argv[i], "-haploid", strlen("-haploid")) == 0 )			{ HAPLOID = true; HAP_EXT = true; }
		else if( strncmp(argv[i], "-h_extend", strlen("-h_extend")) == 0 )			HAP_EXT = true;
		else if( strncmp(argv[i], "-w_extend", strlen("-w_extend")) == 0 )			WIN_EXT = true;

		//Tris: adding new parameter for file with Ids of new individuals
		else if( strncmp(argv[i], "-new_ids", strlen("-new_ids")) == 0 && i < argc-1)		{ INDIV_FILE = argv[++i]; INDIV_IS_WHITELIST = true; saw_new_ids = true; }
		else if( strncmp(argv[i], "-old_ids", strlen("-old_ids")) == 0 && i < argc-1)		{ INDIV_FILE = argv[++i]; saw_old_ids = true; }
		//This is the second mode when we want to just run on a a single new individual
		else if( strncmp(argv[i], "-single_id", strlen("-single_id")) == 0 && i < argc-1)	{ SINGLE_INDIV = argv[++i]; INDIV_IS_WHITELIST = true; saw_single_id = true; }
		else if( strncmp(argv[i], "-out_ids", strlen("-out_ids")) == 0 && i < argc-1)		OUT_INDIV_FILE = argv[++i];

		else if( strncmp(argv[i], "-ped_file", strlen("-ped_file")) == 0 && i < argc-1)		{ PED_FILENAME = argv[++i]; FILE_FORMAT = PED; }
		else if( strncmp(argv[i], "-snpmap_file", strlen("-snpmap_file")) == 0 && i < argc-1)	{ MAP_FILENAME = argv[++i]; FILE_FORMAT = PED; }
		else if( strncmp(argv[i], "-out_file_prefix", strlen("-out_file_prefix")) == 0 && i < argc-1)	OUT_FILENAME = argv[++i];
		else if( strncmp(argv[i], "-cache_dir", strlen("-cache_dir")) == 0 && i < argc-1)	cache_dir = argv[++i];
		else if( strncmp(argv[i], "-override_cache_safety_checks", strlen("-override_cache_safety_checks")) == 0)	OVERRIDE_CACHE_SAFETY_CHECKS = true;
		else if( (strcmp(argv[i], "-h")==0)
			 || (strcmp(argv[i],"-help")==0)
			 || (strcmp(argv[i],"--help")==0)) { show_help = true; }

		else {
		  cerr << "Unrecognized parameter: '" << argv[i] << "'" << endl;
		  bad_param = true;
		}
	}

	/* Hack Alert: using -new_ids/-old_ids/-single_ids can not be
	   used with '-haploids' option.
	   When loading as haploids, the IDs are internally stored as:
	   'FamID<space>IndvID.{0,1}' , and the IDs specified in the input files
	   are only 'FamID<space>IndvID' . Until this is modfied, documented
	   AND TESTED, simply disable this option.
	   See 'getIndividuals()' methods.
	*/
	if ( (!INDIV_FILE.empty() || !SINGLE_INDIV.empty()) && HAPLOID ) {
		errx(1,"usage error: -haploid is currently not supported with " \
		     "-new_ids/-old_ids/-single_id");
	}

	if ( BINARY_OUT )
	{
 	        errx(1, "Binary Output is currently unreliable.  If you want to use it "
		     "review the code in Match.cpp, find a way to test it, and then "
		     "remove this check");
	}

        if ( HAPLOID ) {
                cerr << "WARNING: Haploid format was not tested after the last round of changes.\n"
                     << "It probably still works, but proceed at your own risk.\n";
        }

	if(MIN_MATCH_LEN < 0)
	{
		cerr << "ERROR: -min_m must be non-negative" << endl << endl;
		bad_param = true;
	} else if(MAX_ERR_HOM < 0 || MAX_ERR_HET < 0 )
	{
		cerr << "ERROR: -err_hom,-err_het must be non-negative" << endl << endl;
		bad_param = true;
	} else if(HAPLOID && ALLOW_HOM)
	{
		cerr << "ERROR: cannot execute with both -haploid and -homoz/-homoz-only flags active" << endl << endl;
		bad_param = true;
	}

	if (saw_old_ids + saw_new_ids + saw_single_id > 1) {
		cerr << "ERROR: can only specify one of new_ids, old_ids, single_id" << endl << endl;
		bad_param = true;
	}

	if(bad_param || show_help)
	{
		cerr << "GERMLINE - version 1-5-2 + Erlich Lab custom modifications" << endl ;

		cerr << "usage: " << argv[0] << "<flags (optional)>" << endl
		<< "flags:" << endl
		<< '\t' << "-silent" << '\t' << "Suppress all output except for warnings and prompts." << endl
		<< '\t' << "-status_streamable" << '\t' << "Make status updates fit into output streams (does nothing if -silent is specified)." << endl
		<< '\t' << "-bin_out" << '\t' << "Output in binary format to save space (currently disabled pending testing)." << endl
		<< '\t' << "-min_m" << '\t' << "Minimum length for match to be used for imputation (in cM or MB)." << endl
		<< '\t' << "-err_hom" << '\t' << "Maximum number of mismatching homozygous markers (per slice)." << endl
		<< '\t' << "-err_het" << '\t' << "Maximum number of mismatching heterozygous markers (per slice)." << endl
		<< '\t' << "-from_snp" << '\t' << "Start SNP (rsID)." << endl
		<< '\t' << "-to_snp" << '\t' << "End SNP (rsID)." << endl
		<< '\t' << "-haps" << '\t' << "Print the resolved haplotypes in a seperate HAPS file." << endl
		<< '\t' << "-map" << '\t' << "Genetic distance map." << endl
		<< '\t' << "-bits" << '\t' << "Slice size." << endl
		<< '\t' << "-homoz" << '\t' << "Allow self matches (homozygosity)" << endl
		<< '\t' << "-homoz-only" << '\t' << "Look for autozygous/homozygous segments only, does not detect IBD" << endl
		<< '\t' << "-haploid" << '\t' << "Treat input individual as two fully phased chromosomes with no recombination\n\t\toutput IDs with 0/1 suffix for chromosome destinction" << endl
		<< '\t' << "-h_extend" << '\t' << "Extend from seeds if *haplotypes* match" << endl
		<< '\t' << "-w_extend" << '\t' << "Extend, one marker at a time, beyong the boundaries of a found match" << endl
		<< '\t' << "-cache_dir" << '\t' << "Directory in which to find cache, or 'auto' to determine from PED" << endl
		<< '\t' << "-override_cache_safety_checks" << '\t' << "Run despite a cache that doesn't match the current parameters" << endl
		// Parameters for file with Ids of new individuals
		<< '\t' << "-new_ids" << '\t' << "File that contains the new individuals Ids" << endl
		<< '\t' << "-old_ids" << '\t' << "File that contains the old individuals Ids which we should not match" << endl
		<< '\t' << "-single_id" << '\t' << "Id for a single new individuals" << endl
		<< '\t' << "-out_ids" << '\t' << "File to which to write a list of ids we did match, mostly intended for use with --old_ids" << endl
 	        // Parameters for specifying input
		<< '\t' << "-ped_file" << '\t' << "PED format file with individuals data" << endl
		<< '\t' << "-snpmap_file" << '\t' << "MAP file for understanding PED file with (may be the same as -map)" << endl
	        << '\t' << "-out_file_prefix" << '\t' << "Output will be written to this.match and this.log" << endl;


		cerr << "\n"
		     << "For -new_ids/-old_ids/-single_id, the input format is:\n"
		     <<	"   Family-ID <whitespace> Indiv-ID\n"
		     << "Both Family-ID and Indiv-ID must match the values in the input PED files.\n"
		     << endl;

		return bad_param?1:0;
	}

	if( rs_range[0] != "" && rs_range[1] != "" )
	{
		ROI = true;
		ALL_SNPS.setROI(rs_range);
	}

	if(map != "")
	{
		ALL_SNPS.loadGeneticDistanceMap( map );
	}

        if (cache_dir == "auto") {
                if (PED_FILENAME != "") {
                        cache_dir = "germline_cache_";
                        int slashpos = PED_FILENAME.rfind("/");
                        if (slashpos != string::npos) {
                                cache_dir += PED_FILENAME.substr(slashpos+1, string::npos);
                        } else {
                                cache_dir += PED_FILENAME;
                        }
                        cache_dir += "_";
                        char mss_buf[16];
                        snprintf(mss_buf, sizeof(mss_buf), "%d", MARKER_SET_SIZE);
                        cache_dir += mss_buf;
                        if (!SILENT) {
                                cout << "Using cache dir '" << cache_dir << "'\n";
                        }
                } else {
                        cerr << "PED file name required for automatic cache dir\n";
                        return 1;
                }
        }

        if (cache_dir!="") {
                Cache::init(cache_dir);
        }

	GERMLINE germline;
	germline.mine( params );

        Cache::cleanup();

	if (!OUT_INDIV_FILE.empty()) {
		ofstream fout(OUT_INDIV_FILE);
		if (!fout)
			err(1,"failed to create individual-ID list " \
			    "file '%s'", OUT_INDIV_FILE.c_str());
		for (const string &i : OUT_INDIV_SET) {
			fout << i << endl;
		}
		fout.flush();
		if (fout.bad() || fout.fail())
			err(1,"failed to write new individual-ID list to " \
			    "file '%s'", OUT_INDIV_FILE.c_str());
		fout.close();
		if (fout.bad() || fout.fail())
			err(1,"failed to write new individual-ID list to " \
			    "file '%s'", OUT_INDIV_FILE.c_str());
	}

	return 0;
}


// end GERMLINE_0001.cpp
