#ifndef LRMAIN_H
#define LRMAIN_H

#include "Configuration.h"
#include "utils/FileReaderUtil.h"
#include "LikelihoodSolver/LikelihoodSolver.h"
#include "utils/ProbabilityUtil.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <float.h>
#include <math.h>
#include <fstream>
#include <sstream>

using namespace std;
using namespace LabRetriever;

void outputData(const set<string>& lociToCheck, const vector<LikelihoodSolver*>& likelihoodSolvers,
        const map<Race, vector<map<string, double> > >& raceToSolverIndexToLocusLogProb,
        const map<Race, vector<double> >& raceToSolverIndexToLogProb,
        const vector<Race> races,
        const string& outputFileName);

map<Race, vector<double> > run(const string& inputFileName, const string& outputFileName,
        vector<LikelihoodSolver*> likelihoodSolvers);


#endif // LRMAIN_H
