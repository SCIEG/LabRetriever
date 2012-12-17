//==================================================================================================
// Name        : ProbabilityUtil.cpp
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================

#include "ProbabilityUtil.h"
#include <cmath>
#include <float.h>

namespace LabRetriever {
    const double LOG_ZERO = log(0);
    const double LOG_TWO = log(2);

    double addLogProbability(double logProb1, double logProb2) {
        if (logProb1 == LOG_ZERO) return logProb2;
        else if (logProb2 == LOG_ZERO) return logProb1;

        return logProb1 + log(1 + exp(logProb2 - logProb1));
    }

    double complementLogProbability(double logProb) {
//        static const double INV_E = exp(-1);
//        return -log(INV_E + exp(logProb));
        return log(1 - exp(logProb));
    }

    map<string, double> getAlleleProportionsFromCounts(
            const map<string, unsigned int>& alleleCounts, const AlleleProfile& suspectProfile,
            unsigned int samplingAdjustment, double fst) {
        const map<string, unsigned int>& suspectAlleleCounts = suspectProfile.getAlleleCounts();
        double totalCounts = 0, fstCorrection = (1-fst) / (1+fst);
        for (map<string, unsigned int>::const_iterator iter = alleleCounts.begin();
                iter != alleleCounts.end(); iter++) {
            totalCounts += iter->second;
        }

        double totalSuspectAlleleCounts = 0;
        for (map<string, unsigned int>::const_iterator iter = suspectAlleleCounts.begin();
                iter != suspectAlleleCounts.end(); iter++) {
            totalSuspectAlleleCounts += iter->second;
        }

//        // Find the total counts in the alleleCounts, plus extra per suspect allele.
//        totalCounts += totalSuspectAlleleCounts * samplingAdjustment;
        double multiplierCorrection = fstCorrection / totalCounts;
        map<string, double> alleleProportions;
        for (map<string, unsigned int>::const_iterator iter = alleleCounts.begin();
                iter != alleleCounts.end(); iter++) {
            const string& allele = iter->first;
            alleleProportions[allele] = iter->second * multiplierCorrection;
        }

        // For each suspect allele, add in the proportions that the adjustments would have added.
        for (map<string, unsigned int>::const_iterator iter = suspectAlleleCounts.begin();
                iter != suspectAlleleCounts.end(); iter++) {
            const string& allele = iter->first;
            unsigned int count = iter->second;
            alleleProportions[allele] += count *
//                    (samplingAdjustment * multiplierCorrection + (fst / (1 + fst)));
                    (fst / (1 + fst));
        }
        return alleleProportions;
    }

    double calculateKDropoutsLogProbability(double alpha, double dropoutRate, unsigned int k) {
        if (dropoutRate == 0) return -DBL_MAX;
        if (k == 0) return 0;
        double logAlpha = log(alpha);
        double logDropoutProb = log(dropoutRate);
        double logFinalProb = (k - 1) * (logAlpha + logDropoutProb) + logDropoutProb;
        return logFinalProb;
    }

    double calculateKDropoutsProbability(double alpha, double dropoutRate, unsigned int k) {
        if (k == 1 || dropoutRate == 0) return dropoutRate;
        if (k == 0) return 1;
        double logFinalProb = calculateKDropoutsLogProbability(alpha, dropoutRate, k);
        return exp(logFinalProb);
    }

    double calculateNoDropinProbability(const Configuration& config) {
       double dropinRate = config.dropinRate;
       int numOfAlleles = config.alleleProportions.size();
       return (1 - 2 * dropinRate + pow(dropinRate, numOfAlleles)) / (1 - dropinRate);
    }

    double calculateNoDropinLogProbability(const Configuration& config) {
        // Since the drop-in rate is relatively small, the regular function on normal inputs is
        // numerically stable. There is no need to do anything special to calculate the log
        // probability.
        return log(calculateNoDropinProbability(config));
    }
}


