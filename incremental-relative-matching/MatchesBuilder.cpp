// MatchesBuilder.cpp: builds matches from individuals

#include <sstream>
#include <err.h>

#include "std_string_trim.h"
#include "MatchesBuilder.h"
#include "Cache.h"

unsigned int position_ms;
unsigned int num_sets;

int ALLOWED_MASKED;



// MatchesBuilder(): default constructor
MatchesBuilder::MatchesBuilder( PolymorphicIndividualsExtractor * pie )
{
	individualsP = & ALL_SAMPLES;
	pieP = pie;
	if (STATUS_STREAMABLE) {
	  if (!PED_FILENAME.empty()) {
			status_prefix = "\n[" + PED_FILENAME + "] ";
		} else {
			status_prefix = "\n";
		}
	} else {
		status_prefix = "\r";
	}
}



//Tris: gets the new individuals to find shared matches
void  MatchesBuilder::readNewIndivIds( )
{
	string line;
	size_t linenum=0;
	ifstream myfile(INDIV_FILE);
	if (myfile.is_open()) {
		if (!SILENT)
			cout << "reading "
			     << (INDIV_IS_WHITELIST?"NEW":"EXISTING")
			     << " individual IDs from '"
			     << INDIV_FILE << "'" << endl;

		while ( getline (myfile,line) ) {
			++linenum;
			line = trim(line);
			if (line.empty())
				continue;
			if (line[0] == '#')
				continue;

			/* Ensure each line is in the correct format:
			     Family-ID <whitespace> Sample-ID
			   This is required due to internal hackish
			   method of storing IDs as a concatenated string
			   of FamID+IndivID (see 'getIndividuals()' methods) */
			stringstream ss(line);
			string famID, indvID;
			ss >> famID >> indvID;
			if (!ss)
				errx(1,"input error in Individial-ID list file "
				     "%s:%zu: expecting two values " \
				     "(Family-ID, Indiv-ID), found: '%s'",
				     INDIV_FILE.c_str(), linenum, line.c_str());

			const string id = famID + " " + indvID;
			indivSet.insert(id);
			if (!SILENT)
				cout << "  '" << id << "'\n";
		}
		myfile.close();
		if (indivSet.size()==0)
			errx(1,"no individual IDs read from file '%s'",
			     INDIV_FILE.c_str());

		if (!SILENT)
			cout << "Loaded " << indivSet.size()
			     << (INDIV_IS_WHITELIST?" NEW":" EXISTING")
			     << " individual IDs from '"
			     << INDIV_FILE << "'" << endl;
	} else {
		err(1,"failed to open Individual-IDs file '%s'",
		    INDIV_FILE.c_str());
	}
}



// buildMatches(): builds matches from individuals
void MatchesBuilder::buildMatches()
{
        if ( !SILENT ) cout << "Read " << (Cache::isInit() ? "New " : "") << "Markers" << endl;
	ms_start = 0; ms_end = num_sets;
	readAllMarkers();

	//Tris: modifying to only match new set
	//First check if we just have one new person, otherwise run on list of new individuals

	if (!SINGLE_INDIV.empty()){
	    indivSet.insert(SINGLE_INDIV);
	    if (!SILENT) cout<<"Updated mode, running on a single new individual: "<<SINGLE_INDIV<<endl;
	    validate_individual_set (indivSet);
	} else if (!INDIV_FILE.empty()){
	    readNewIndivIds();
	    validate_individual_set (indivSet);
	}

	if ( !SILENT ) cout << "Match Markers" << endl;
	matchAllMarkers();
}

/* Ensure each Individual ID in the given set appears in the matches database.
   prints a warning to STDERR if an ID is not found.
   returns TRUE if all individual-IDs were found in the matches database,
   or FALSE otherwise;

   It doesn't matter if the set represents new-ids or old-ids - they must
   exist in the matches dataset before starting the matching process. */
void MatchesBuilder::validate_individual_set(const unordered_set<string>& s) const
{
	bool ok = true;
	unordered_set<string> db_ids;

	// collect list of IDs in a <set> for quicker searching
	if (!individualsP)
		errx(1,"internal error: individualsP is NULL at %s:%d",
		     __FILE__,__LINE__);

	for(individualsP->begin(); individualsP->more();)
	{
		const string id = individualsP->next()->getID();
		db_ids.insert(id);
	}

	// Search each ID
	for (const auto indv_id: s) {
		auto it = db_ids.find(indv_id);
		if (it == db_ids.end()) {
			cerr << "ERROR: Individual ID '" << indv_id
			     << "' not found in dataset/PED/HAPS files"
			     << endl;
			ok = false ;
		}
	}

	//TODO: in the future, perhaps add '--ignore-missing' option.
	if (!ok)
		exit(1);
}



void MatchesBuilder::printHaplotypes(string fout_name)
{
	ofstream fout( fout_name.c_str() );

	for(individualsP->begin();individualsP->more();)
	{
		individualsP->next()->print(fout,0,num_sets);
	}
	fout.close();
}

// matchAllMarkers(): builds matches for individuals considering all markers
void MatchesBuilder::matchAllMarkers()
{
	static int prevval = -1;
	for (position_ms = ms_start; position_ms < ms_end - 1; position_ms++)
	{
		if ( !SILENT ) {
			int val = (position_ms*100) / (ms_end - 1);
			if (val != prevval) {
				cerr << status_prefix << "Matching Markers - " <<  val << "%" << flush;
				prevval = val;
			}
		}
		matchMarkerSet();
	}
	for(individualsP->begin();individualsP->more();)
        {
                individualsP->next()->markAllMsCached();
	}

	if ( !SILENT ) cerr << status_prefix << "Matching Markers Complete" << endl;
}

void MatchesBuilder::readAllMarkers()
{
	static int prevval = -1;
	for (position_ms = ms_start; position_ms < ms_end; position_ms++)
	{
		if ( !SILENT ) {
			int val = position_ms*100/ms_end;
			if (val != prevval) {
				cerr << status_prefix << "Reading Markers - " << val  << "%" << flush;
				prevval = val;
			}
		}
		if ( HAPLOID ) readHaploidMarkerSet(); else readMarkerSet();
	}
	Individual * i;
	for(individualsP->begin();individualsP->more();)
	{
		i = individualsP->next();
		i->saveIfNeeded();
	}
	if ( !SILENT ) cerr << status_prefix << "Reading Markers Complete" << endl;
}

void MatchesBuilder::readHaploidMarkerSet()
{
	// Read the individuals two at a time

	Individual * i[2];
	for(individualsP->begin();individualsP->more();)
	{
		i[0] = individualsP->next();
		i[1] = individualsP->next();
                if ( ! (i[0]->checkAllMsCached() && i[1]->checkAllMsCached()) ) {
                        pieP->getCompleteMarkerSet( i[0] , i[1] );
                }
	}
}

void MatchesBuilder::readMarkerSet()
{
	Individual * i;
	for(individualsP->begin();individualsP->more();)
	{
		i = individualsP->next();
                if (!i->checkAllMsCached()) {
                        pieP->getCompleteMarkerSet(i);
                }
	}
}

// matchMarkerSet(): builds matches for individuals considering markers in marker set.
void MatchesBuilder::matchMarkerSet()
{
	// Match:
        for(individualsP->begin(); individualsP->more(); ) {
		matchFactory.addAndAssertIfNecessary( individualsP->next(), indivSet );
        }

	// Extend:
	for(individualsP->begin(); individualsP->more(); ) {
		individualsP->next()->assertShares();
        }

	matchFactory.initialize();
}

// end MatchesBuilder.cpp
