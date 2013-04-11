//==================================================================================================
// Name        : NoSuspectTwoUnknownsLikelihoodSolver.cpp
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================


#include "CachingSolver.h"
#include "../utils/LikelihoodUtil.h"
#include "../utils/ProbabilityUtil.h"
#include <iostream>
#include <algorithm>
#include <cassert>


using namespace std;
namespace LabRetriever {
    class NoSuspectTwoUnknownsLikelihoodSolver: public CachingSolver {
        public:
            double calculateLogLikelihood(const Configuration& config);
        private:
            static bool cmpFn(const Configuration& left, const Configuration& right);
            NoSuspectTwoUnknownsLikelihoodSolver();


            const static NoSuspectTwoUnknownsLikelihoodSolver EXEMPLAR;
    };

    START_COMPARE_FUNCTION(NoSuspectTwoUnknownsLikelihoodSolver, cmpFn) {
        COMPARE_CONFIGURATION_ELEMENT(suspectProfile);
        COMPARE_CONFIGURATION_ELEMENT(data);
        COMPARE_CONFIGURATION_ELEMENT(alleleProportions);
        COMPARE_CONFIGURATION_ELEMENT(identicalByDescentProbability);
        COMPARE_CONFIGURATION_ELEMENT(dropoutRate);
        COMPARE_CONFIGURATION_ELEMENT(dropinRate);
        COMPARE_CONFIGURATION_ELEMENT(alpha);
    } END_COMPARE_FUNCTION;

    NoSuspectTwoUnknownsLikelihoodSolver::NoSuspectTwoUnknownsLikelihoodSolver() :
            CachingSolver(NO_SUSPECT_TWO_UNKNOWNS, "No-Suspect_Two-Unknowns", cmpFn) {}

    double NoSuspectTwoUnknownsLikelihoodSolver::calculateLogLikelihood(
            const Configuration& config) {
        double dropinRate = config.dropinRate;
        double noDropinProb = calculateNoDropinProbability(config);
        const map<string, double>& alleleProportions = config.alleleProportions;
        const IdenticalByDescentProbability& ibdProbability =
                config.identicalByDescentProbability;
        const vector<ReplicateData>& data = config.data;
        const AlleleProfile& suspectProfile = config.suspectProfile;
        double alpha = config.alpha;
        double dropoutRate = config.dropoutRate;
        double numAlleles = alleleProportions.size();
        numComplete = 0; totalToComplete =
                pow(numAlleles, 3) * (2 + numAlleles) + numAlleles * numAlleles;


        double zeroIBDLogLikelihood = LOG_ZERO;
        if (ibdProbability.zeroAllelesInCommonProb != 0) {
            BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele1, logProbRandomAllele1, allele2,
                    logProbRandomAllele2) {
                BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele3, logProbRandomAllele3,
                        allele4, logProbRandomAllele4) {
                    double logProbHavingTheseAlleles =
                            logProbRandomAllele1 + logProbRandomAllele2 +
                            logProbRandomAllele3 + logProbRandomAllele4;
                    if (allele1 != allele2) {
                        logProbHavingTheseAlleles += LOG_TWO;
                    }
                    if (allele3 != allele4) {
                        logProbHavingTheseAlleles += LOG_TWO;
                    }

                    // Create a new suspect profile.
                    AlleleProfile newSuspectProfile;
                    newSuspectProfile.addAllele(allele1).addAllele(allele2)
                            .addAllele(allele3).addAllele(allele4);

                    double logProbGivenAlleles = calculateLogProbability(newSuspectProfile, data,
                            alleleProportions, alpha, dropoutRate, dropinRate, noDropinProb);
                    double partialZeroIBDLogLikelihood = logProbGivenAlleles +
                            logProbHavingTheseAlleles;

                    zeroIBDLogLikelihood = addLogProbability(zeroIBDLogLikelihood,
                            partialZeroIBDLogLikelihood);
                    numComplete += 1;
                } END_CHOOSE_RANDOM_ALLELES;
            } END_CHOOSE_RANDOM_ALLELES;
        }
        numComplete = pow(numAlleles, 4);

        double oneIBDLogLikelihood = LOG_ZERO;
        if (ibdProbability.oneAlleleInCommonProb != 0) {
            // Choose one allele from the suspect:
            const set<string>& suspectAlleles = suspectProfile.getAlleles();
            BEGIN_CHOOSE_ONE_RANDOM_ALLELE(alleleProportions, suspectAlleles, suspectAllele,
                    otherAllele, logProbHavingRandomAllele) {
                BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele1, logProbRandomAllele1,
                        allele2, logProbRandomAllele2) {
                    double logProbHavingTheseAlleles =
                            logProbHavingRandomAllele + logProbRandomAllele1 +
                            logProbRandomAllele2;
                    if (allele1 == allele2) {
                        logProbHavingTheseAlleles += LOG_TWO;
                    }

                    AlleleProfile relatedSuspectProfile;
                    relatedSuspectProfile.addAllele(suspectAllele).addAllele(otherAllele)
                            .addAllele(allele1).addAllele(allele2);

                    double logProbGivenAllele = calculateLogProbability(relatedSuspectProfile, data,
                            alleleProportions, alpha, dropoutRate, dropinRate, noDropinProb);
                    double partialOneIBDLogLikelihood = logProbGivenAllele +
                            logProbHavingRandomAllele;

                    // TODO: original code divides probability by 2. Check if correct.
                    oneIBDLogLikelihood = addLogProbability(oneIBDLogLikelihood,
                            partialOneIBDLogLikelihood);
                    numComplete += 1;
                } END_CHOOSE_RANDOM_ALLELES;
            } END_CHOOSE_ONE_RANDOM_ALLELE;
        }
        // Divide by two for the two alleles you can choose from the suspect.
        oneIBDLogLikelihood -= log(2.0);
        numComplete = totalToComplete - pow(numAlleles, 2);

        double bothIBDLogLikelihood = (ibdProbability.bothAllelesInCommonProb == 0) ?
                LOG_ZERO :
                LikelihoodSolver::getSolver(ONE_SUSPECT_ONE_UNKNOWN)->getLogLikelihood(config);

        double logLikelihood = addLogProbability(
                log(ibdProbability.zeroAllelesInCommonProb) + zeroIBDLogLikelihood,
                addLogProbability(
                        log(ibdProbability.oneAlleleInCommonProb) + oneIBDLogLikelihood,
                        log(ibdProbability.bothAllelesInCommonProb) + bothIBDLogLikelihood));
        numComplete = totalToComplete;
        return logLikelihood;
    }

    const NoSuspectTwoUnknownsLikelihoodSolver NoSuspectTwoUnknownsLikelihoodSolver::EXEMPLAR;
} /* namespace LabRetriever */

