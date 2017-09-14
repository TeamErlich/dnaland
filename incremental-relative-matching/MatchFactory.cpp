// MatchFactory.cpp: Generates matches from individuals

#include "MatchFactory.h"


// MatchFactory(): default constructor
MatchFactory::MatchFactory()
{}


int MatchFactory::size()
{
	return (int)segments.size();
}

// initialize(): initializes object
void MatchFactory::initialize()
{
        for (iter=segments.begin(); iter!=segments.end(); ++iter) {
                iter->second.save(position_ms, iter->first);
        }
	segments.clear();
}	

void MatchFactory::addAndAssertIfNecessary( Individual * i, const unordered_set<string>& indivSet )
{
        bool match_this = (indivSet.count(i->getID())==INDIV_IS_WHITELIST);

        if (match_this && !OUT_INDIV_FILE.empty()) {
                OUT_INDIV_SET.insert(i->getID());
        }


        bool in_cache = i->checkAllMsCached();

        if (!match_this && in_cache) return;

	int haps , het = i->numHet();
	
	if ( het == 0 || HAPLOID ) haps = 1;
	else haps = 2;

	if ( ALLOW_HOM && het <= MAX_ERR_HOM + MAX_ERR_HET ) i->assertHomozygous();
	if ( HOM_ONLY ) return;

	
	for ( int c = 0 ; c < haps ; c++ )
	{
		boost::dynamic_bitset<>& ms = i->getChromosome(c)->getMarkerSet()->getMarkerBits();
                iter = segments.find( ms );
		if ( iter == segments.end() ) {
                        iter = segments.insert( make_pair(ms, Share(position_ms, ms)) ).first;
                }
                if (!in_cache) {
                        iter->second.add( i, match_this );
                } else if (match_this) {
                        iter->second.markAsToMatch( i );
                }
	}
}

// end MatchFactory.cpp
