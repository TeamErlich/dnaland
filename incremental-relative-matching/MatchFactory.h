// MatchFactory.h: Generates matches from individuals

#ifndef MATCHFACTORY_H
#define MATCHFACTORY_H

#include "MarkerSet.h"
#include "Individual.h"
#include <map>
#include <vector>
//Tris: adding in unordered set to keep track of the new Ids to reduce shared matches check
#include <unordered_set>

using namespace std;

class MatchFactory
{
	
public:

	// MatchFactory(): default constructor
	// Precondition: None.
	// Postcondition: segments and matches are empty and position is -1.
	MatchFactory();

	int size();

    // initialize(): initializes object
	// Precondition:  None.
	// Postcondition: If 0=<pos, then position is set to pos and map is empty;
	//  otherwise an error message is printed.
	void initialize();

	void addAndAssertIfNecessary(Individual *, const unordered_set<string>& indivSet);
	void assertShares(unordered_set<string> indivSet);

private:

	// stores data to check for matches
	map < boost::dynamic_bitset<> , Share > segments;
	map < boost::dynamic_bitset<> , Share >::iterator iter;
};

#endif

// end MatchFactory.h
