// Individual.cpp: An individual with genetic data

#include "Individual.h"
#include "Cache.h"
using namespace std;

// Individual(): default constructor
Individual::Individual()
{
	if ( HAPLOID ) h = new Chromosome[1]; else h = new Chromosome[2];
	numeric_id = 0;
        all_ms_cached = false;
}

Individual::~Individual()
{
	delete[] h;
}

void Individual::freeMatches()
{
        // special iterator increment to not invalidate via erase
	for ( auto it=all_matches.begin(); it!=all_matches.end(); ) {
		deleteMatch( (it++)->first );
        }
}

Match * Individual::getMatch( const Individual* ind )
{
        auto it = all_matches.find(ind);
        if (it!=all_matches.end()) {
                return it->second;
        } else {
                return NULL;
        }
}

void Individual::assertHomozygous()
{
	if ( all_matches.count(this) ) {
		// increment this match
		all_matches[ this ]->end_ms = position_ms;
	} else {
		// this is a new match
		Match * m = new Match();
		m->end_ms = m->start_ms = position_ms;
		m->node[0] = m->node[1] = this;
		m->extendBack();
		all_matches[ this ] = m;
	}
}

void Individual::assertShares()
{
	// try to extend previous matches that did not match currently
	for( auto it=all_matches.begin(); it!=all_matches.end(); )
	{
                // Avoid invalidation from erase
                auto iter = (it++);
		if ( iter->second && iter->second->approxEqual() ) {
                        iter->second->end_ms = position_ms;
                } else {
                        deleteMatch( iter->first );
                }
	}
}

void Individual::clearMatch( const Individual* ind )
{
	all_matches.erase(ind);
}

void Individual::deleteMatch( const Individual* ind )
{
	// try to print it
	all_matches[ ind ]->print( MATCH_FILE );
	delete all_matches[ ind ];

	// erase from the list
	clearMatch( ind );
}

void Individual::addMatch( const Individual* ind , Match * m)
{
	all_matches[ ind ] = m;
}

void Individual::print(ostream& out, long start, long end)
{
	short tot;
	if ( HAPLOID ) tot=1; else tot=2;
	for(int i=0;i<tot;i++)
	{
		out << getID() << '\t';
		h[i].print(out,start,end);
		out << endl;
	}
}

int Individual::numHet()
{
	if ( HAPLOID ) return 0;
	else return countDifferences(h[0].getMarkerSet(), h[1].getMarkerSet());
}

bool Individual::isHeterozygous()
{
	if ( HAPLOID ) return false;
	else return !( h[0].getMarkerSet()->equal( h[1].getMarkerSet() ) );
}

bool Individual::isHeterozygous(int i)
{
	if ( HAPLOID ) return false;
	else return h[0].getMarkerSet()->getMarker(i) != h[1].getMarkerSet()->getMarker(i);
}

void Individual::setOffset(streamoff o)
{
	offset = o;
}

streamoff Individual::getOffset()
{
	return offset;
}

// getID(): accessor for ID
string Individual::getID() const
{
	return ID;
}

Chromosome * Individual::getAlternateChromosome( Chromosome * c)
{
	if ( HAPLOID ) return &(h[0]);
	else
	{
		if( &(h[0]) == c ) return &(h[1]); else return &(h[0]);
	}
}

Chromosome * Individual::getChromosome(int ct)
{
	if ( HAPLOID ) ct = 0;

	return &(h[ct]);
}

unsigned int Individual::getNumericID()
{
	return numeric_id;
}

void Individual::setNumericID( unsigned int id )
{
	numeric_id = id;
}

// setID(): mutator for ID.
void Individual::setID(string id)
{
	ID = id;

        // See if this id has its markersets cached
        string key = "all_cached_"+ID;
        string val = Cache::get(key);
        all_ms_cached = (val=="1");

        // Tell the chromosomes who they are
        typedef boost::dynamic_bitset<>::block_type Block;
        int keylen = id.length()+sizeof(position_ms)+1;
        key="";
        key.resize(keylen);
        memcpy(&key[0], id.data(), id.length());
        for (char i=0; i<(HAPLOID?1:2) ; i++) {
                key[id.length()]=i;
                h[i].setKey(key);
        }
}

void Individual::clearMarkers()
{
	h[0].clear();
	if ( !HAPLOID ) h[1].clear();
}

// addMarkerSet(): adds MarkerSet to a chromosome
void Individual::addMarkerSet(int ct, MarkerSet * ms)
{
	if ( HAPLOID ) ct = 0;
	h[ct].addMarkerSet(ms);
}

bool Individual::checkAllMsCached() {
  return all_ms_cached;
}

void Individual::markAllMsCached() {
  string key = "all_cached_"+ID;
  Cache::put(key, "1");
  all_ms_cached = true;
}

void Individual::saveIfNeeded() {
        for (int i=0; i<(HAPLOID?1:2); i++) {
                h[i].saveIfNeeded();
        }
}


// operator<<(): overloaded stream insertion operator
ostream& operator<<(ostream &fout, Individual& ind)
{
	fout << ind.getID() << endl;
	fout << ind.getChromosome(0) << endl;
	fout << ind.getChromosome(1) << endl;
	return fout;
}


// end Individual.cpp
