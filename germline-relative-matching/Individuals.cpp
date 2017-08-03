// Individuals.cpp: A collection of individuals

#include "Individuals.h"
#include <iostream>
using namespace std;


// Individuals(): default constructor
Individuals::Individuals()
{}

Individuals::~Individuals()
{}

void Individuals::freeMatches()
{
	for(begin();more();next()) iter->second->freeMatches();
}

void Individuals::freeMarkers()
{
	for(begin();more();next()) { iter->second->clearMarkers(); }
}

void Individuals::print( ostream& out )
{
	for(begin();more();next())
		out << iter->first << endl;
}

void Individuals::begin()
{
        iter = data.begin();
}

bool Individuals::more()
{
	return iter != data.end();
}

Individual * Individuals::next()
{
	return ( iter++ )->second;
}

// addIndividual(): adds an Individual object
void Individuals::addIndividual(Individual * ind)
{
	ind->setNumericID( (unsigned int) num_samples++ );
        data[ind->getID()] = ind;
}

// end Individuals.cpp
