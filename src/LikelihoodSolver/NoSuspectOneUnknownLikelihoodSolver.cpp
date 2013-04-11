//==================================================================================================
// Name        : NoSuspectOneUnknownLikelihoodSolver.cpp
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
    class NoSuspectOneUnknownLikelihoodSolver: public CachingSolver {
        public:
            double calculateLogLikelihood(const Configuration& config);
        private:
            static bool cmpFn(const Configuration& left, const Configuration& right);
            NoSuspectOneUnknownLikelihoodSolver();


            const static NoSuspectOneUnknownLikelihoodSolver EXEMPLAR;
    };

    START_COMPARE_FUNCTION(NoSuspectOneUnknownLikelihoodSolver, cmpFn) {
        COMPARE_CONFIGURATION_ELEMENT(suspectProfile);
        COMPARE_CONFIGURATION_ELEMENT(data);
        COMPARE_CONFIGURATION_ELEMENT(alleleProportions);
        COMPARE_CONFIGURATION_ELEMENT(identicalByDescentProbability);
        COMPARE_CONFIGURATION_ELEMENT(dropoutRate);
        COMPARE_CONFIGURATION_ELEMENT(dropinRate);
        COMPARE_CONFIGURATION_ELEMENT(alpha);
    } END_COMPARE_FUNCTION;

    NoSuspectOneUnknownLikelihoodSolver::NoSuspectOneUnknownLikelihoodSolver() :
            CachingSolver(NO_SUSPECT_ONE_UNKNOWN, "No-Suspect_One-Unknown", cmpFn) {}

    double NoSuspectOneUnknownLikelihoodSolver::calculateLogLikelihood(
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
        numComplete = 0; totalToComplete = numAlleles * (numAlleles + 2) + 1;

        double zeroIBDLogLikelihood = LOG_ZERO;
        if (ibdProbability.zeroAllelesInCommonProb != 0) {
            BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele1, logProbRandomAllele1, allele2,
                    logProbRandomAllele2) {
                double logProbHavingTheseAlleles = logProbRandomAllele1 + logProbRandomAllele2;
                if (allele1 != allele2) {
                    logProbHavingTheseAlleles += LOG_TWO;
                }

                // Create a new suspect profile.
                AlleleProfile newSuspectProfile;
                newSuspectProfile.addAllele(allele1).addAllele(allele2);

                double logProbGivenAlleles = calculateLogProbability(newSuspectProfile, data,
                        alleleProportions, alpha, dropoutRate, dropinRate, noDropinProb);
                double partialZeroIBDLogLikelihood = logProbGivenAlleles +
                        logProbHavingTheseAlleles;

                zeroIBDLogLikelihood = addLogProbability(zeroIBDLogLikelihood,
                        partialZeroIBDLogLikelihood);
                numComplete += 1;
            } END_CHOOSE_RANDOM_ALLELES;
        }
        numComplete = numAlleles * numAlleles;

        double oneIBDLogLikelihood = LOG_ZERO;
        if (ibdProbability.oneAlleleInCommonProb != 0) {
            // Choose one allele from the suspect:
            const set<string>& suspectAlleles = suspectProfile.getAlleles();
            BEGIN_CHOOSE_ONE_RANDOM_ALLELE(alleleProportions, suspectAlleles, allele1, allele2,
                    logProbHavingRandomAllele) {
                AlleleProfile relatedSuspectProfile;
                relatedSuspectProfile.addAllele(allele1).addAllele(allele2);

                double logProbGivenAllele = calculateLogProbability(relatedSuspectProfile, data,
                        alleleProportions, alpha, dropoutRate, dropinRate, noDropinProb);
                double partialOneIBDLogLikelihood = logProbGivenAllele +
                        logProbHavingRandomAllele;

                oneIBDLogLikelihood = addLogProbability(oneIBDLogLikelihood,
                        partialOneIBDLogLikelihood);
                numComplete += 1;
            } END_CHOOSE_ONE_RANDOM_ALLELE;
        }
        // Divide by two for the two alleles you can choose from the suspect.
        oneIBDLogLikelihood -= log(2.0);
        numComplete = numAlleles * (2 + numAlleles);

        double bothIBDLogLikelihood = (ibdProbability.bothAllelesInCommonProb == 0) ?
                LOG_ZERO :
                LikelihoodSolver::getSolver(ONE_SUSPECT_NO_UNKNOWNS)->getLogLikelihood(config);
        numComplete = totalToComplete;

        double logLikelihood = addLogProbability(
                log(ibdProbability.zeroAllelesInCommonProb) + zeroIBDLogLikelihood,
                addLogProbability(
                        log(ibdProbability.oneAlleleInCommonProb) + oneIBDLogLikelihood,
                        log(ibdProbability.bothAllelesInCommonProb) + bothIBDLogLikelihood));

        return logLikelihood;
    }

    const NoSuspectOneUnknownLikelihoodSolver NoSuspectOneUnknownLikelihoodSolver::EXEMPLAR;
} /* namespace LabRetriever */


