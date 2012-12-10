//==================================================================================================
// Name        : OneSuspectLikelihoodSolver.cpp
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================

//#include "DebugUtil.h"

#include "LikelihoodSolver/CachingSolver.h"
#include "utils/LikelihoodUtil.h"
#include "utils/ProbabilityUtil.h"
#include <iostream>
#include <algorithm>
#include <cassert>


using namespace std;
namespace LabRetriever {

    class OneSuspectLikelihoodSolver: public LabRetriever::CachingSolver {
        public:
            double calculateLogLikelihood(const Configuration& config);
        private:
            static bool cmpFn(const Configuration& left, const Configuration& right);
            OneSuspectLikelihoodSolver();


            const static OneSuspectLikelihoodSolver EXEMPLAR;
    };

    START_COMPARE_FUNCTION(OneSuspectLikelihoodSolver, cmpFn) {
        COMPARE_CONFIGURATION_ELEMENT(suspectProfile);
        COMPARE_CONFIGURATION_ELEMENT(data);
        COMPARE_CONFIGURATION_ELEMENT(alleleProportions);
        COMPARE_CONFIGURATION_ELEMENT(dropoutRate);
        COMPARE_CONFIGURATION_ELEMENT(dropinRate);
        COMPARE_CONFIGURATION_ELEMENT(alpha);
    } END_COMPARE_FUNCTION;

    OneSuspectLikelihoodSolver::OneSuspectLikelihoodSolver() : CachingSolver(
            ONE_SUSPECT_NO_UNKNOWNS, "One-Suspect_No-Unknowns", cmpFn) {}

    double OneSuspectLikelihoodSolver::calculateLogLikelihood(const Configuration& config) {
        numComplete = 0; totalToComplete = 1;
        double dropinRate = config.dropinRate;
        double noDropinProb = calculateNoDropinProbability(config);
        const map<string, double>& alleleProportions = config.alleleProportions;
        const vector<ReplicateData>& data = config.data;
        const AlleleProfile& suspectProfile = config.suspectProfile;
        double alpha = config.alpha;
        double dropoutRate = config.dropoutRate;

        double logLikelihood = calculateLogProbability(suspectProfile, data, alleleProportions,
                alpha, dropoutRate, dropinRate, noDropinProb);
        numComplete = 1;
        return logLikelihood;
    }

    const OneSuspectLikelihoodSolver OneSuspectLikelihoodSolver::EXEMPLAR;
} /* namespace LabRetriever */
