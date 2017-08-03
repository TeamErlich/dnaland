// Individuals.h: A collection of individuals

#ifndef INDIVIDUALS_H
#define INDIVIDUALS_H

#include "BasicDefinitions.h"
#include "Chromosome.h"
#include "Individual.h"
#include <map>
#include <ostream>
using namespace std;

class Individual;
class Individuals
{
public:

	// Individuals(): default constructor
	// Precondition: None.
	// Postcondition: individuals is empty.
	Individuals();
	~Individuals();

	// addIndividual(): adds an Individual object
	// Precondition: None.
    // Postcondition: ind has been added to individuals
	void addIndividual( Individual * ind );
        Individual * getById( string id ) { return data[id]; }

	bool more();
	Individual* next();
	void begin();
	size_t size() { return data.size(); }
	void print( ostream& );
	
	void freeMatches();
	void freeMarkers();

private:

	void permuteMarkerSet(Chromosome *, int, MarkerSet);

        map< string, Individual* > data;
        map< string, Individual* >::iterator iter;
};

#endif

// end Individuals.h
