
all: LikelihoodSolver utils
	g++ -I ../lib/boost_1_53_0 -g lrmain.cpp Configuration.o LikelihoodSolver/LikelihoodSolver.o \
		LikelihoodSolver/CachingSolver.o \
		LikelihoodSolver/NoSuspectFourUnknownsLikelihoodSolver.o \
		LikelihoodSolver/NoSuspectOneUnknownLikelihoodSolver.o \
		LikelihoodSolver/NoSuspectThreeUnknownsLikelihoodSolver.o \
		LikelihoodSolver/NoSuspectTwoUnknownsLikelihoodSolver.o \
		LikelihoodSolver/OneSuspectNoUnknownsLikelihoodSolver.o \
		LikelihoodSolver/OneSuspectOneUnknownLikelihoodSolver.o \
		LikelihoodSolver/OneSuspectThreeUnknownsLikelihoodSolver.o \
		LikelihoodSolver/OneSuspectTwoUnknownsLikelihoodSolver.o \
		utils/LikelihoodUtil.o utils/FileReaderUtil.o utils/ProbabilityUtil.o \
		-o Tide/labr

Configuration:
	g++ -g -c Configuration.cpp

LikelihoodSolver: Configuration
	$(MAKE) -C LikelihoodSolver

utils: Configuration
	$(MAKE) -C utils

clean:
	rm -rf *.o */*.o Tide/labr*

