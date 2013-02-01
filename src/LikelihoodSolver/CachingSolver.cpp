//==================================================================================================
// Name        : CachingSolver.cpp
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

namespace LabRetriever {
    CachingSolver::CachingSolver(ScenarioType type, const string& name,
            bool(*cmpFn)(const Configuration&, const Configuration&)) :
                    LikelihoodSolver(type, name), cachingMap(cmpFn) {}

    double CachingSolver::getLogLikelihood(const Configuration& config) {
        if (cachingMap.count(config) == 0) {
            double likelihood = this->calculateLogLikelihood(config);
            cachingMap[config] = likelihood;
            return likelihood;
        }
        return cachingMap[config];
    }

    /**********************************************************************************************
     * Utility comparison functions
     **********************************************************************************************/
    int compare(double left, double right) {
        return (left < right) ? -1 :
                (left > right) ? 1 : 0;
    }

    /* Lazy implementation */
    int compare(const AlleleProfile& left, const AlleleProfile& right) {
        const map<string, unsigned int>& leftAlleleCounts = left.getAlleleCounts();
        const map<string, unsigned int>& rightAlleleCounts = right.getAlleleCounts();
        return (leftAlleleCounts < rightAlleleCounts) ? -1 :
                (leftAlleleCounts > rightAlleleCounts) ? 1 : 0;
    }

    int compare(const vector<ReplicateData>& left, const vector<ReplicateData>& right) {
        int leftSize = left.size(), rightSize = right.size();
        if (leftSize < rightSize) return -1;
        if (leftSize > rightSize) return 1;
        for (int i = 0; i < leftSize; i++) {
            int val = compare(left[i], right[i]);
            if (val != 0) {
                return val;
            }
        }
        return 0;
    }

    /* Lazy implementation. May consider more optimization. */
    int compare(const map<string, double>& left, const map<string, double>& right) {
        return (left < right) ? -1 :
                (left > right) ? 1 : 0;
    }

    int compare(const IdenticalByDescentProbability& left,
            const IdenticalByDescentProbability& right) {
        int val = compare(left.zeroAllelesInCommonProb, right.zeroAllelesInCommonProb);
        if (val != 0) return val;
        val = compare(left.oneAlleleInCommonProb, right.oneAlleleInCommonProb);
        if (val != 0) return val;
        return compare(left.bothAllelesInCommonProb, right.bothAllelesInCommonProb);
    }


    int compare(const ReplicateData& left, const ReplicateData& right) {
        int leftMaskedAlleleSize = left.maskedAlleles.size();
        int rightMaskedAlleleSize = right.maskedAlleles.size();
        if (leftMaskedAlleleSize < rightMaskedAlleleSize) return -1;
        if (leftMaskedAlleleSize > rightMaskedAlleleSize) return 1;
        int leftUnattributedAlleleSize = left.unattributedAlleles.size();
        int rightUnattributedAlleleSize = right.unattributedAlleles.size();
        if (leftUnattributedAlleleSize < rightUnattributedAlleleSize) return -1;
        if (leftUnattributedAlleleSize > rightUnattributedAlleleSize) return 1;

        int val = compare(left.maskedAlleles, right.maskedAlleles);
        if (val != 0) {
            return val;
        }
        return compare(left.unattributedAlleles, right.unattributedAlleles);
    }

    int compare(const set<string>& left, const set<string>& right) {
        set<string>::const_iterator leftIter, rightIter;
        for (leftIter = left.begin(), rightIter = right.begin();
                leftIter != left.end(); leftIter++, rightIter++) {
            const string& leftString = *leftIter;
            const string& rightString = *rightIter;
            int val = leftString.compare(rightString);
            if (val != 0) {
                return val;
            }
        }
        return 0;
    }

} /* namespace LabRetriever */
