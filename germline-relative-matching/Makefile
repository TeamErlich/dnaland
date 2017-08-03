CC=	g++
# Uncomment exactly one of the following:
OPT=	-O3 -I include -std=c++11 # Production
#OPT=	-O2 -I include -std=c++11 -g -fno-omit-frame-pointer # Profiling
#OPT=	-O2 -I include -std=c++11 -g -fno-omit-frame-pointer -fno-inline # Extreme profiling
LDFLAGS= -lleveldb -lcrypto
SRCS=	GERMLINE_0001.cpp GERMLINE.cpp Share.cpp Chromosome.cpp ChromosomePair.cpp HMIndividualsExtractor.cpp MarkerSet.cpp Individual.cpp Individuals.cpp InputManager.cpp MatchFactory.cpp MatchesBuilder.cpp NucleotideMap.cpp PEDIndividualsExtractor.cpp Match.cpp PolymorphicIndividualsExtractor.cpp SNP.cpp SNPPositionMap.cpp SNPs.cpp
OBJS=	GERMLINE_0001.o GERMLINE.o Chromosome.o Share.o ChromosomePair.o HMIndividualsExtractor.o MarkerSet.o Individual.o Individuals.o InputManager.o MatchFactory.o MatchesBuilder.o NucleotideMap.o PEDIndividualsExtractor.o Match.o PolymorphicIndividualsExtractor.o SNP.o SNPPositionMap.o SNPs.o Cache.o
HEADERS= BasicDefinitions.h Chromosome.h ChromosomeIndividualPair.h ChromosomePair.h GERMLINE.h HMIndividualsExtractor.h Individual.h Individuals.h InputManager.h MarkerSet.h MatchesBuilder.h MatchFactory.h Match.h NucleotideMap.h PEDIndividualsExtractor.h PolymorphicIndividualsExtractor.h SharedSegmentPointer.h SNP.h SNPPositionMap.h SNPs.h Cache.h
MAIN=	germline
BMATCH=	parse_bmatch

all: germline bmatch test

bmatch:
	$(CC) $(BMATCH).cpp -o $(BMATCH)

%.o: %.cpp $(HEADERS)
	$(CC) $(OPT) -c $< -o $@

germline: $(OBJS)
	$(CC) $(OPT) -o $(MAIN) $(OBJS) $(LDFLAGS)

clean:
	-rm -f *.o $(MAIN) $(BMATCH) test/generated.match test/generated.log test/generated.err test/generated.out

test: test_plink test_blacklist test_caching

test_plink: test/expected.match test/CEU.22.map test/CEU.22.ped
	@printf -- "---\nRunning All At Once Test Case\n---\n"
	@rm -f test/generated.match test/generated.log test/generated.err \
	       test/generated.out test/generated.match.sorted
	./$(MAIN) -silent -bits 50 -min_m 1 -err_hom 2 -err_het 0 \
                  -ped_file test/CEU.22.ped -snpmap_file test/CEU.22.map \
                  -out_file_prefix test/generated > test/generated.out
	@sort < test/generated.match > test/generated.match.sorted
	diff -q -s test/expected.match test/generated.match.sorted

test_blacklist: test/expected.old.match test/expected.AddedIds test/test.old.run test/CEU.22.map test/CEU.22.ped
	@printf -- "---\nRunning Blacklist Test Case\n---\n"
	@rm -f test/generated.old.match test/generated.old.log \
	       test/generated.old.err test/generated.old.out \
	       test/generated.old.match.sorted
	./$(MAIN) -silent -bits 50 -min_m 1 -err_hom 2 -err_het 0 \
                  -old_ids test/oldIndivIds.txt \
	          -out_ids test/generated.AddedIds \
		  -ped_file test/CEU.22.ped -snpmap_file test/CEU.22.map \
		  -out_file_prefix test/generated.old > test/generated.old.out
	@sort < test/generated.old.match > test/generated.old.match.sorted
	@sort < test/generated.AddedIds > test/generated.AddedIds.sorted
	diff -q -s test/expected.old.match test/generated.old.match.sorted
	diff -q -s test/expected.AddedIds test/generated.AddedIds.sorted

test_caching: test/expected.old.match test/expected.AddedIds test/test.old.run test/CEU.22.map test/CEU.22.ped
	@printf -- "---\nRunning Test Case With Cache\n---\n"
	@rm -rf test/cachetest
	@mkdir test/cachetest
	@head -n 5 test/CEU.22.ped > test/cachetest/CEU.22.ped.5
	./$(MAIN) -silent -bits 50 -min_m 1 -err_hom 2 -err_het 0 \
	          -ped_file test/cachetest/CEU.22.ped.5 \
	          -snpmap_file test/CEU.22.map \
	          -cache_dir test/cachetest/cachedir \
		  -out_file_prefix test/cachetest/firstrun > /dev/null
	./$(MAIN) -silent -bits 50 -min_m 1 -err_hom 2 -err_het 0 \
                  -old_ids test/oldIndivIds.txt \
	          -ped_file test/CEU.22.ped \
	          -snpmap_file test/CEU.22.map \
	          -cache_dir test/cachetest/cachedir \
		  -out_file_prefix test/cachetest/generated > /dev/null
	@sort < test/cachetest/generated.match > test/cachetest/generated.match.sorted
	diff -q -s test/expected.old.match test/cachetest/generated.match.sorted
