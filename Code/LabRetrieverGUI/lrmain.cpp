#include "lrmain.h"
#include "utils/DebugUtil.h"

vector<double> run(const string& inputFileName, const string& outputFileName,
        vector<LikelihoodSolver*> likelihoodSolvers) {
    vector<double> solverIndexToLogProb(likelihoodSolvers.size(), 0);
    vector<map<string, double> > solverIndexToLocusLogProb(likelihoodSolvers.size());

    double alpha = 0.5;
    double dropinRate = 0.01;
    double dropoutRate = 0.05;
    Race race = AFRICAN_AMERICAN;
    IdenticalByDescentProbability identicalByDescentProbability(1, 0, 0);
    map<string, vector<string> > locusToSuspectAlleles;
    map<string, set<string> > locusToAssumedAlleles;
    map<string, vector<set<string> > > locusToUnattributedAlleles;

    vector< vector<string> > inputData = readRawCsv(inputFileName);
    unsigned int csvIndex = 0;
    for (; csvIndex < inputData.size(); csvIndex++) {
        const vector<string>& row = inputData[csvIndex];
        if (row.size() == 0) continue;

        const string& header = row[0];
        unsigned int index;
        // Hack way to detect input type.
        if ((index = header.find("-Assumed")) != string::npos) {
            string locus = header.substr(0, index);
            set<string> assumedAlleles;
            for (unsigned int i = 1; i < row.size(); i++) {
                string data = row[i];
                if (data.length() != 0) {
                    assumedAlleles.insert(data);
                }
            }
            locusToAssumedAlleles[locus] = assumedAlleles;
        } else if ((index = header.find("-Unattributed")) != string::npos) {
            string locus = header.substr(0, index);
            set<string> unattribAlleles;
            for (unsigned int i = 1; i < row.size(); i++) {
                string data = row[i];
                if (data.length() != 0) {
                    unattribAlleles.insert(data);
                }
            }

            locusToUnattributedAlleles[locus].push_back(unattribAlleles);
        } else if ((index = header.find("-Suspected")) != string::npos) {
            string locus = header.substr(0, index);
            vector<string> suspectAlleles;
            for (unsigned int i = 1; i < row.size(); i++) {
                string data = row[i];
                if (data.length() != 0) {
                    suspectAlleles.push_back(data);
                }
            }
            // If there are no suspected alleles, then there's no point of calculating this.
            if (suspectAlleles.size() == 0) continue;

            locusToSuspectAlleles[locus] = suspectAlleles;
        } else if (header == "alpha") {
            if (row.size() <= 1) continue;
            double value = atof(row[1].c_str());
            if (value != 0) {
                alpha = value;
            }
        } else if (header == "Drop-in rate") {
            if (row.size() <= 1) continue;
            double value = atof(row[1].c_str());
            if (value != 0) {
                dropinRate = value;
            }
        } else if (header == "Drop-out rate") {
            if (row.size() <= 1) continue;
            double value = atof(row[1].c_str());
            if (value != 0) {
                dropoutRate = value;
            }
        } else if (header == "Race") {
            if (row.size() <= 1) continue;
            race = raceFromString(row[1]);
        } else if (header == "IBD Probs") {
            if (row.size() <= 3 || row[1].size() == 0 ||
                    row[2].size() == 0 || row[3].size() == 0 ) continue;
            identicalByDescentProbability.zeroAllelesInCommonProb = atof(row[1].c_str());
            identicalByDescentProbability.oneAlleleInCommonProb = atof(row[2].c_str());
            identicalByDescentProbability.bothAllelesInCommonProb = atof(row[3].c_str());
        }
    }

    // TODO: check for known loci and alleles.
    // I think this todo is done.
    set<string> lociToCheck;
    for (map<string, vector<string> >::const_iterator iter = locusToSuspectAlleles.begin();
            iter != locusToSuspectAlleles.end(); iter++) {
        const string& locus = iter->first;
        if (locusToAssumedAlleles.find(locus) != locusToAssumedAlleles.end() &&
                locusToUnattributedAlleles.find(locus) != locusToUnattributedAlleles.end()) {
            lociToCheck.insert(locus);
        }
    }

    // Create configurations and run on likelihood solvers.
    for (set<string>::const_iterator iter = lociToCheck.begin();
            iter != lociToCheck.end(); iter++) {
        string locus = *iter;

        vector<set<string> > unattributedAlleles = locusToUnattributedAlleles[locus];
        set<string> assumedAlleles = locusToAssumedAlleles[locus];
        vector<string> suspectAlleles = locusToSuspectAlleles[locus];

        set<string> allAlleles;
        allAlleles.insert(assumedAlleles.begin(), assumedAlleles.end());
        allAlleles.insert(suspectAlleles.begin(), suspectAlleles.end());
        for (unsigned int i = 0; i < unattributedAlleles.size(); i++) {
            allAlleles.insert(unattributedAlleles[i].begin(), unattributedAlleles[i].end());
        }

        AlleleProfile suspectProfile;
        vector<ReplicateData> replicateDatas;

        for (unsigned int i = 0; i < suspectAlleles.size(); i++) {
            suspectProfile.addAllele(suspectAlleles[i]);
        }
        for (unsigned int unattribIndex = 0; unattribIndex < unattributedAlleles.size();
                unattribIndex++) {
            replicateDatas.push_back(ReplicateData::fromUnattributedAndMaskedAlleles(
                    unattributedAlleles[unattribIndex], assumedAlleles));
        }

        map<string, unsigned int> alleleCounts =
                getAlleleCountsFromFile("Allele Frequency Tables/" + locus + "_B.count.csv", race);

        // Edit the allele counts so that every allele is given at least 5 counts, even if the
        // allele does not appear in the table.
        for (set<string>::const_iterator iter = allAlleles.begin(); iter != allAlleles.end();
                iter++) {
            const string& allele = *iter;
            if (alleleCounts.count(allele) == 0) {
                alleleCounts[allele] = 5;
            }
        }

        for (map<string, unsigned int>::iterator iter = alleleCounts.begin();
                iter != alleleCounts.end(); iter++) {
            unsigned int count = iter->second;
            iter->second = (count > 5) ? count : 5;             // Max function
        }


        map<string, double> alleleProp =
                getAlleleProportionsFromCounts(alleleCounts, suspectProfile);

        Configuration config(suspectProfile, replicateDatas, alleleProp,
                identicalByDescentProbability, dropoutRate, dropinRate, alpha);

        for (unsigned int solverIndex = 0; solverIndex < likelihoodSolvers.size(); solverIndex++) {
            LikelihoodSolver* solver = likelihoodSolvers[solverIndex];
            double logLikelihood = solver->getLogLikelihood(config);
            solverIndexToLocusLogProb[solverIndex][locus] = logLikelihood;
            solverIndexToLogProb[solverIndex] += logLikelihood;
        }
    }

    // TODO: Output in proper format to file:
    stringstream logProbStream, regProbStream;
    logProbStream.setf(ios::scientific);
    logProbStream.precision(4);
    regProbStream.setf(ios::scientific);
    regProbStream.precision(4);

    logProbStream << "Log Probabilities, total";
    regProbStream << "Probabilities, total";
    for (set<string>::const_iterator iter = lociToCheck.begin(); iter != lociToCheck.end();
            iter++) {
        const string& locus = *iter;
        logProbStream << ", " << locus;
        regProbStream << ", " << locus;
    }
    logProbStream << endl;
    regProbStream << endl;

    for (unsigned int solverIndex = 0; solverIndex < likelihoodSolvers.size(); solverIndex++) {
        map<string, double> locusToLogProb = solverIndexToLocusLogProb[solverIndex];

        logProbStream << likelihoodSolvers[solverIndex]->name << ", "
                << solverIndexToLogProb[solverIndex];
        regProbStream << likelihoodSolvers[solverIndex]->name << ", "
                << exp(solverIndexToLogProb[solverIndex]);
        for (set<string>::const_iterator iter = lociToCheck.begin(); iter != lociToCheck.end();
                iter++) {
            const string& locus = *iter;
            double logProb = locusToLogProb[locus];
            logProbStream << ", " << logProb;
            regProbStream << ", " << exp(logProb);
        }
        logProbStream << endl;
        regProbStream << endl;
    }

    logProbStream << endl;
    regProbStream << endl;

    logProbStream << "Log Probabilities Difference, total";
    regProbStream << "Probabilities Ratios, total";
    for (set<string>::const_iterator iter = lociToCheck.begin(); iter != lociToCheck.end();
            iter++) {
        const string& locus = *iter;
        logProbStream << ", " << locus;
        regProbStream << ", " << locus;
    }
    logProbStream << endl;
    regProbStream << endl;


    for (unsigned int i = 0; i < likelihoodSolvers.size(); i++) {
        map<string, double> locusToLogProb_i = solverIndexToLocusLogProb[i];
        for (unsigned int j = i + 1; j < likelihoodSolvers.size(); j++) {
            map<string, double> locusToLogProb_j = solverIndexToLocusLogProb[j];

            string ratioName = likelihoodSolvers[i]->name + " to " + likelihoodSolvers[j]->name;
            double diff = solverIndexToLogProb[i] - solverIndexToLogProb[j];
            logProbStream << ratioName << ", " << diff;
            regProbStream << ratioName << ", " << exp(diff);


            for (set<string>::const_iterator iter = lociToCheck.begin(); iter != lociToCheck.end();
                    iter++) {
                const string& locus = *iter;
                double logProbDiff = locusToLogProb_i[locus] - locusToLogProb_j[locus];
                logProbStream << ", " << logProbDiff;
                regProbStream << ", " << exp(logProbDiff);
            }
            logProbStream << endl;
            regProbStream << endl;
        }
    }

    stringstream outputStringStream;
    outputStringStream << regProbStream.str() << endl << logProbStream.str();
    string dataToOutput = outputStringStream.str();

//    cout << dataToOutput;

    ofstream myFileStream;
    myFileStream.open(outputFileName.c_str());
    myFileStream << dataToOutput;
    myFileStream.close();

    return solverIndexToLogProb;
}
