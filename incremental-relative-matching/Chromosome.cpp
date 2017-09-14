// Chromosome.cpp.haplotyped markers for a chromosome

#include "Chromosome.h"
#include "Cache.h"
#include <iostream>
#include <cstdlib>
using namespace std;

static const size_t IOAtOnce = 32;

// Chromosome(): default constructor
Chromosome::Chromosome()
{}

MarkerSet * Chromosome::getMarkerSet()
{
	return getMarkerSet(position_ms);
}

MarkerSet * Chromosome::getMarkerSet(unsigned int pos)
{
        if (pos<chromosome.size() && chromosome[pos]) {
                return chromosome[pos];
        }

        unsigned int pos_r = (pos/IOAtOnce)*IOAtOnce;
        memcpy(&key[key.size()-sizeof(pos_r)], &pos_r, sizeof(pos_r));
        string val = Cache::get(key);
        if (val=="") {
                cerr << "Failed to lazy-load " << key
                     << " position " << pos << endl;
                exit(1);
        }
        MarkerSet::Block* data = (MarkerSet::Block*)&val[0];
        int nread = val.length() /
                ( MarkerSet::blocks() * sizeof(MarkerSet::Block) );
        if (pos_r+nread-1 < pos) {
                cerr << "Failed to lazy-load " << key
                     << " position " << pos << endl;
                cerr << "Only read " << nread << " Markersets starting at "
                     << pos_r << endl;
                exit(1);
        }
        chromosome.reserve(pos_r+nread);
        for (int i=0; i<nread; i++) {
                unsigned int pos_i = pos_r + i;
                if (pos_i>=chromosome.size()) {
                        chromosome.resize(pos_i+1);
                }
                chromosome[pos_i] = new MarkerSet();
                from_block_range(data+(i*MarkerSet::blocks()),
                                 data+((i+1)*MarkerSet::blocks()),
                                 chromosome[pos_i]->getMarkerBits());
        }

        return chromosome[pos];
}

void Chromosome::saveIfNeeded()
{
        if (!dirty) return;
        for (unsigned int pos_r=0; pos_r<chromosome.size(); pos_r+=IOAtOnce) {
                memcpy(&key[key.size()-sizeof(pos_r)], &pos_r, sizeof(pos_r));
                string val;
                int ntowrite = min(IOAtOnce, chromosome.size()-pos_r);
                val.resize(ntowrite * MarkerSet::blocks() * sizeof(MarkerSet::Block));
                MarkerSet::Block* data = (MarkerSet::Block*)val.data();
                for (int i=0; i<ntowrite; i++) {
                        to_block_range(chromosome[pos_r+i]->getMarkerBits(),
                                       &data[MarkerSet::blocks()*i]);
                }
                Cache::put(key, val);
        }
        dirty=false;
}

void Chromosome::clear()
{
	for ( size_t i = 0 ; i < chromosome.size() ; i++ ) { delete chromosome[i]; }
	chromosome.clear();
}

// addMarkerSet(): adds a MarkerSet
void Chromosome::addMarkerSet(MarkerSet * ms)
{
        dirty=true;
	chromosome.push_back(ms);
}

void Chromosome::print_snps(ostream& out, unsigned int start, unsigned int end)
{
	unsigned int p_ms = position_ms;

	unsigned int ms_start = start / MARKER_SET_SIZE;
	unsigned int ms_end = end / MARKER_SET_SIZE;
	if( start % MARKER_SET_SIZE != 0 ) { position_ms = ms_start; chromosome[ms_start++]->print(out,start % MARKER_SET_SIZE,MARKER_SET_SIZE); out << ' '; }
	print(out,ms_start,ms_end);
	if( end % MARKER_SET_SIZE != 0 ) { out << ' '; chromosome[ms_end]->print(out,0,end % MARKER_SET_SIZE); }

	position_ms = p_ms;
}

void Chromosome::print(ostream& out,unsigned int start,unsigned int end)
{
	for(position_ms=start;position_ms<end;position_ms++)
	{
		if( position_ms > start ) out << ' ';
		chromosome[position_ms]->print(out);
	}
}

void Chromosome::setKey(const string& _key)
{
        key = _key;
}

ostream& operator<<(ostream &fout, Chromosome& c)
{
	fout << c.getMarkerSet();
	return fout;
}

// end Chromosome.cpp
