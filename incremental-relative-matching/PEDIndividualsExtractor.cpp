// KOSIndividualsExtractor.cpp: Manages input from Plink files

#include "PEDIndividualsExtractor.h"
#include <cctype>
#include <iostream>
#include <err.h>

using namespace std;

// PEDIndividualsExtractor(): default constructor
PEDIndividualsExtractor::PEDIndividualsExtractor()
{}

void PEDIndividualsExtractor::stripWhitespace()
{
	if (stream.is_open())
	{
		char c;
		while((c=stream.peek())!=EOF && isspace(c))
		stream.get();
	}
	else
	{
		cerr << "WARNING:PolymorphicIndividualsExtractor::stripWhiteSpace():stream is not open" << endl;
		valid_flag = false;
	}
}

void PEDIndividualsExtractor::loadInput()
{
	individualsP = &ALL_SAMPLES;
	ALL_SNPS.processMAPFile();
	ALL_SNPS.beginChromosome();
	numberOfMarkers = ALL_SNPS.size();

	while (stream.good() )
	{
		getIndividuals();
		stream.seekg(numberOfMarkers*4 + 1,ios::cur);
	}
	
	stream.clear();
}

// getInput(): gets individuals from .ped file
void PEDIndividualsExtractor::getInput()
{

	if (MAP_FILENAME.empty()) {
		cout << "Please enter the MAP file name" << endl;
		cin >> map_file;
	} else {
		map_file = MAP_FILENAME;
	}

	if (PED_FILENAME.empty()) {
		cout << "Please enter the PED file name" << endl;
		cin >> ped_file;
	} else {
		ped_file = PED_FILENAME;
	}

	/* Hack Alert: this assumes 'setFile()' only calls 'ifstream.open()',
	   and errno is still valid */
	if ( !ALL_SNPS.setFile( map_file ) )
		err(1,"PEDIndividualsExtractor::getInput() failed to open " \
		      "map file '%s'", map_file.c_str());

	stream.open( ped_file.c_str() );
	if ( !stream )
		err(1,"PEDIndividualsExtractor::getInput() failed to open " \
		      "PED file '%s'", ped_file.c_str());
}

// getIndividuals(): gets the next nuclear family from stream
void PEDIndividualsExtractor::getIndividuals()
{
	string discard, ID, famID;
	stream >> famID >> ID >> discard >> discard >> discard >> discard;
	if(!stream.good()) return;
	if ( HAPLOID )
	{
		Individual * new_ind[2];
		new_ind[0] = new Individual();
		new_ind[1] = new Individual();
		new_ind[0]->setOffset( stream.tellg() );
		new_ind[1]->setOffset( stream.tellg() );
		new_ind[0]->setID(famID + " " + ID + ".0" );
		new_ind[1]->setID(famID + " " + ID + ".1" );
		
		individualsP->addIndividual( new_ind[0] );
		individualsP->addIndividual( new_ind[1] );
	} else
	{
		Individual * new_ind = new Individual;
		new_ind->setOffset(stream.tellg());
		new_ind->setID(famID + " " + ID);
		individualsP->addIndividual(new_ind);
	}
}

void PEDIndividualsExtractor::getCompleteMarkerSet(Individual * p)
{
	stream.seekg(p->getOffset() + 4*ALL_SNPS.getROIStart().getMarkerNumber() + 4*position_ms*MARKER_SET_SIZE + 1);
	MarkerSet * ms[2];
	ms[0] = new MarkerSet();
	ms[1] = new MarkerSet();

	readMarkerSet( ms );

	p->addMarkerSet(UNTRANS,ms[0]);
	p->addMarkerSet(TRANS,ms[1]);
}

void PEDIndividualsExtractor::readMarkerSet( MarkerSet ** ms )
{
	unsigned int maxsize = ALL_SNPS.currentSize();
	for (int position = 0; position < MARKER_SET_SIZE; position++)
	{
		if(position_ms*MARKER_SET_SIZE+position >= maxsize) break;
		for(int al=0;al<2;al++){
			stripWhitespace();
			char marker = stream.peek();
			if ( ALL_SNPS.mapNucleotideToBinary(marker,position_ms*MARKER_SET_SIZE+position) == 1 )
				ms[al]->set(position , true );
			stream.get();
		}
	}
}

void PEDIndividualsExtractor::getCompleteMarkerSet(Individual * p0 , Individual * p1 )
{
	stream.seekg(p0->getOffset() + 4*ALL_SNPS.getROIStart().getMarkerNumber() + 4*position_ms*MARKER_SET_SIZE + 1);
	MarkerSet * ms[2];
	ms[0] = new MarkerSet();
	ms[1] = new MarkerSet();

	readMarkerSet( ms );

	p0->addMarkerSet(TRANS,ms[0]);
	p1->addMarkerSet(TRANS,ms[1]);
}

// end PEDIndividualsExtractor.cpp

