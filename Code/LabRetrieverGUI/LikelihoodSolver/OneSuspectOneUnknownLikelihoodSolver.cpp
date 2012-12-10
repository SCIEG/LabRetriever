//==================================================================================================
// Name        : OneSuspectOneUnkownLikelihoodSolver.cpp
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================


#include "LikelihoodSolver/CachingSolver.h"
#include "utils/LikelihoodUtil.h"
#include "utils/ProbabilityUtil.h"
#include <iostream>
#include <algorithm>
#include <cassert>


using namespace std;
namespace LabRetriever {
    class OneSuspectOneUnknownLikelihoodSolver: public CachingSolver {
        public:
            double calculateLogLikelihood(const Configuration& config);
        private:
            static bool cmpFn(const Configuration& left, const Configuration& right);
            OneSuspectOneUnknownLikelihoodSolver();


            const static OneSuspectOneUnknownLikelihoodSolver EXEMPLAR;
    };

    START_COMPARE_FUNCTION(OneSuspectOneUnknownLikelihoodSolver, cmpFn) {
        COMPARE_CONFIGURATION_ELEMENT(suspectProfile);
        COMPARE_CONFIGURATION_ELEMENT(data);
        COMPARE_CONFIGURATION_ELEMENT(alleleProportions);
        COMPARE_CONFIGURATION_ELEMENT(dropoutRate);
        COMPARE_CONFIGURATION_ELEMENT(dropinRate);
        COMPARE_CONFIGURATION_ELEMENT(alpha);
    } END_COMPARE_FUNCTION;

    OneSuspectOneUnknownLikelihoodSolver::OneSuspectOneUnknownLikelihoodSolver() :
            CachingSolver(ONE_SUSPECT_ONE_UNKNOWN, "One-Suspect_One-Unknown", cmpFn) {}

    double OneSuspectOneUnknownLikelihoodSolver::calculateLogLikelihood(
            const Configuration& config) {
        double dropinRate = config.dropinRate;
        double noDropinProb = calculateNoDropinProbability(config);
        const map<string, double>& alleleProportions = config.alleleProportions;
        const vector<ReplicateData>& data = config.data;
        const AlleleProfile& suspectProfile = config.suspectProfile;
        double alpha = config.alpha;
        double dropoutRate = config.dropoutRate;
        numComplete = 0; totalToComplete = pow((double) alleleProportions.size(), 2);

        double logLikelihood = LOG_ZERO;

        BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele1, logProbRandomAllele1, allele2,
                logProbRandomAllele2) {
            double logProbHavingTheseAlleles = logProbRandomAllele1 + logProbRandomAllele2;
            if (allele1 != allele2) {
                logProbHavingTheseAlleles += LOG_TWO;
            }

            // Create a suspect profile + unknown.
            AlleleProfile newSuspectProfile = suspectProfile;
            newSuspectProfile.addAllele(allele1).addAllele(allele2);

            double logProbGivenAlleles = calculateLogProbability(newSuspectProfile, data,
                    alleleProportions, alpha, dropoutRate, dropinRate, noDropinProb);
            double partialLogLikelihood = logProbGivenAlleles + logProbHavingTheseAlleles;

            logLikelihood = addLogProbability(logLikelihood, partialLogLikelihood);
            numComplete += 1;
        } END_CHOOSE_RANDOM_ALLELES;

        return logLikelihood;
    }

    const OneSuspectOneUnknownLikelihoodSolver OneSuspectOneUnknownLikelihoodSolver::EXEMPLAR;
} /* namespace LabRetriever */

