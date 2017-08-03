#include "Individual.h"
#include "Cache.h"
#include <unordered_set>
#include <sstream>

Share::Share( Individual * cip, bool to_match )
{
	add( cip, to_match );
}



void Share::add(Individual * cip, bool to_match)
{
        for (auto i : matches) {
                if (i.first==cip) {
                        // This should never happen, but it might if we got interrupted
                        // in a previous run
                        if (to_match && ! i.second) {
                                markAsToMatch(cip);
                        }
                        return;
                }
        }
	matches.push_back( make_pair(cip, to_match) );
        if (to_match) {
                assertMatches(cip, true);
        } else if (any_to_match) {
                assertMatches(cip, false);
        }
        any_to_match |= to_match;
}

void Share::markAsToMatch(Individual* to_match) {
        list<pair<Individual*, bool> >::iterator i;
        for (i=matches.begin(); i!=matches.end(); ++i) {
                if (i->first == to_match) {
                        i->second = true;
                }
        }
        any_to_match = true;
        assertMatches(to_match, true);
}


Match * Share::createMatch(Individual * c1 , Individual * c2)
{
	Match * new_match = new Match();
	new_match->end_ms = new_match->start_ms = position_ms;

	new_match->node[0] = c1;
	new_match->node[1] = c2;

	return new_match;
}



void Share::assertMatches(Individual* to_match, bool with_all)
{
        for ( auto& i : matches ) {

                // Don't match old-to-old or self-to-self
                if (!with_all && !i.second) continue;
                if (i.first == to_match) continue;

                // Check if this pair matched in previous word (symmetrically)
                Match * m = i.first->getMatch( to_match );
                if ( m == NULL ) m = to_match->getMatch( i.first );

                if ( m != NULL ) {
                        // This match can be incremented
                        m->end_ms = position_ms;
                } else {
                        // This match must be created
                        m = createMatch( i.first , to_match );
                        // Extend the match backwards
                        m->extendBack();
                        // Mark asserted
                        i.first->addMatch( to_match , m );
                }

        }
}

string make_key(int pos, const boost::dynamic_bitset<>& ms) {
        int blocks = MARKER_SET_SIZE / boost::dynamic_bitset<>::bits_per_block;
        blocks += !! (MARKER_SET_SIZE % boost::dynamic_bitset<>::bits_per_block);
        int keylen = blocks * sizeof(boost::dynamic_bitset<>::block_type) + sizeof(int);
        string key;
        key.resize(keylen);
        memcpy(&key[0], &pos, sizeof(pos));
        to_block_range(ms, (boost::dynamic_bitset<>::block_type*)(&key[sizeof(pos)]));
        return key;
}

void Share::save(int pos, const boost::dynamic_bitset<>& ms) {
        list<pair<Individual*, bool> >::iterator i;
        string key = make_key(pos, ms);
        stringstream val;
        for (i=matches.begin(); i!=matches.end(); ++i) {
                val << i->first->getID() << endl;
        }
        Cache::put(key, val.str());
}

Share::Share(int pos, const boost::dynamic_bitset<>& ms) {
        string key = make_key(pos, ms);
        string val = Cache::get(key);
        if (val!="") {
                stringstream ss(val);
                string id;
                while (!ss.eof()) {
                        getline(ss, id);
                        if (id=="") break;
                        Individual * ind = ALL_SAMPLES.getById(id);
                        if (!ind) {
                                cerr << "Unrecognized id in cache '" << id << "' from '" << val << "'\n";
                                exit(1);
                        }
                        matches.push_back( make_pair(ind, false) );
                }
        }
}

