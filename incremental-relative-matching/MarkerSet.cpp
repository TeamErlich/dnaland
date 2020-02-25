// MarkerSet.cpp: a marker set

#include "MarkerSet.h"
#include <iostream>
using namespace std;


// MarkerSet(): default constructor
MarkerSet::MarkerSet()
{
	markers.resize(MARKER_SET_SIZE);
}

MarkerSet::MarkerSet(const MarkerSet& copy)
{
	markers = copy.markers; // copy.getMarkerBits();
}

void MarkerSet::clear()
{
	markers.reset();
}

// getMarker(): gets marker
bool MarkerSet::getMarker(int index) const
{
	return markers.test( index );
}

void MarkerSet::set(int index , bool bit )
{
	markers.set( index , bit );
}

void MarkerSet::print(ostream& out, unsigned int start, unsigned int end)
{
	for(unsigned int i=start;i<end && (position_ms*MARKER_SET_SIZE)+i < ALL_SNPS.currentSize();i++)
	{
		out << ALL_SNPS.getSNP(position_ms*MARKER_SET_SIZE + i).getVariant( markers[i] );
	}
}

void MarkerSet::print(ostream& out)
{
	print( out , 0 , MARKER_SET_SIZE);
}

// equal(): compares another MarkerSet to see if it has the 
bool MarkerSet::equal(MarkerSet * ms)
{
	return markers == ms->markers;
}


int MarkerSet::diff(MarkerSet * ms) const
{
	return (int) ( markers ^ ms->markers ).count();
}

/*static*/ int MarkerSet::blocks() {
        int blocks = MARKER_SET_SIZE / boost::dynamic_bitset<>::bits_per_block;
        blocks += !! (MARKER_SET_SIZE % boost::dynamic_bitset<>::bits_per_block);
        return blocks;
}

// operator<<:overloaded insertion operator
ostream& operator<<(ostream& fout, const MarkerSet& m1)
{
	for (int i = 0; i < MARKER_SET_SIZE; i++)
		fout << m1.getMarker(i);
	return fout;
}

// Equivalent to (a^b).count(), but without allocation/deallocation
int countDifferences(MarkerSet* _a, MarkerSet* _b) {
        if (_a == _b) return 0;
        boost::dynamic_bitset<> &a = _a->getMarkerBits();
        boost::dynamic_bitset<> &b = _b->getMarkerBits();
        a ^= b;
        int out = a.count();
        a ^= b;
        return out;
}


// end MarkerSet.cpp
