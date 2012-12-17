//==================================================================================================
// Name        : LikelihoodUtil.cpp
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================

#include "LikelihoodUtil.h"
#include "ProbabilityUtil.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>

namespace LabRetriever {
    double calculateLogProbability(const AlleleProfile& alleleProfile,
            const vector<ReplicateData>& data, const map<string, double>& alleleProportions,
            double alpha, double dropoutRate, double dropinRate, double noDropinProb) {
        const map<string, unsigned int>& profileAlleleCounts = alleleProfile.getAlleleCounts();
        const double noDropinLogProb = log(noDropinProb);
        const double dropinLogRate = log(dropinRate);

        map<string, double> kDropoutsLogProbabilities;
        for (map<string, unsigned int>::const_iterator iter = profileAlleleCounts.begin();
                iter != profileAlleleCounts.end(); iter++) {
            kDropoutsLogProbabilities[iter->first] = calculateKDropoutsLogProbability(alpha,
                    dropoutRate, iter->second);
        }

        double logLikelihood = 0;
        // For each replicate...
        for (unsigned int i = 0; i < data.size(); i++) {
            const map<string, unsigned int>& suspectAlleleCounts = alleleProfile.getAlleleCounts();
            const set<string>& maskedAlleles = data[i].maskedAlleles;
            const set<string>& unattributedAlleles = data[i].unattributedAlleles;

            // For each allele in the suspect's profile...
            for (map<string, unsigned int>::const_iterator iter = suspectAlleleCounts.begin();
                    iter != suspectAlleleCounts.end(); iter++) {
                const string& allele = iter->first;
                if (maskedAlleles.count(allele) == 0) {
                    // If the allele is uncounted for (not masked by known profiles), then determine
                    // what must have happened to the suspect's profile in order to be consistent
                    // with the data.
                    double dropoutLogProbability = kDropoutsLogProbabilities.find(allele)->second;

                    if (unattributedAlleles.count(allele) == 0) {
                        // If the allele is not found in the crime scene profile, then drop-out
                        // must have occurred.
                        logLikelihood += dropoutLogProbability;
                    } else {
                        // Otherwise, no drop-out occurred.
                        logLikelihood += complementLogProbability(dropoutLogProbability);
                    }
                }
            }
            // If there are any other alleles detected in the replicate, but is not explained by
            // the suspect, then drop-in has occurred. First, calculate which alleles have dropped
            // in.
            const set<string>& suspectAlleles = alleleProfile.getAlleles();
//            debugToken(unattributedAlleles);
//            cout << " ";
//            debugToken(alleleProfile.getAlleleCounts());
//            cout << endl;
//            flush(cout);
            vector<string> droppedInAlleles;
            set_difference(unattributedAlleles.begin(), unattributedAlleles.end(),
                    suspectAlleles.begin(), suspectAlleles.end(), back_inserter(droppedInAlleles));

            if (droppedInAlleles.size() == 0) {
                // If there is no drop-in, then just multiply by the probability of no drop-in.
                logLikelihood += noDropinLogProb;
            } else {
                // Otherwise, we need to figure out the probability of allele dropping in. First,
                // calculate the renormalization factor of the allele proportions, not counting
                // masked alleles.
                double renormalizationFactor = 0;
                for (map<string, double>::const_iterator iter = alleleProportions.begin();
                        iter != alleleProportions.end(); iter++) {
                    const string& allele = iter->first;
                    if (data[i].maskedAlleles.count(allele) == 0) {
                        renormalizationFactor += iter->second;
                    }
                }

                // Normalization factor better be nonzero!
                assert(renormalizationFactor != 0);

                // For every dropped in allele, we need to multiply by the probability that drop-in
                // occurred, and it's frequency in the population.
                for (vector<string>::const_iterator iter = droppedInAlleles.begin();
                        iter != droppedInAlleles.end(); iter++) {
                    const string& allele = *iter;

                    // Awkward workaround to get data from a const map;
                    double renormalizedLogProbability = log(alleleProportions.find(allele)->second
                            / renormalizationFactor);
                    logLikelihood += (dropinLogRate + renormalizedLogProbability);
                }
            }
        }
        return logLikelihood;
    }

    double calculateProbability(const AlleleProfile& alleleProfile,
            const vector<ReplicateData>& data, const map<string, double>& alleleProportions,
            double alpha, double dropoutRate, double dropinRate, double noDropinProb) {
        const map<string, unsigned int>& profileAlleleCounts = alleleProfile.getAlleleCounts();

        map<string, double> kDropoutsProbabilities;
        for (map<string, unsigned int>::const_iterator iter = profileAlleleCounts.begin();
                iter != profileAlleleCounts.end(); iter++) {
            kDropoutsProbabilities[iter->first] = calculateKDropoutsProbability(alpha,
                    dropoutRate, iter->second);
        }

        double likelihood = 1;
        // For each replicate...
        for (unsigned int i = 0; i < data.size(); i++) {
            const map<string, unsigned int>& suspectAlleleCounts = alleleProfile.getAlleleCounts();
            const set<string>& maskedAlleles = data[i].maskedAlleles;
            const set<string>& unattributedAlleles = data[i].unattributedAlleles;

            // For each allele in the suspect's profile...
            for (map<string, unsigned int>::const_iterator iter = suspectAlleleCounts.begin();
                    iter != suspectAlleleCounts.end(); iter++) {
                const string& allele = iter->first;
                if (maskedAlleles.count(allele) == 0) {
                    // If the allele is uncounted for (not masked by known profiles), then determine
                    // what must have happened to the suspect's profile in order to be consistent
                    // with the data.
                    double dropoutProbability = kDropoutsProbabilities.find(allele)->second;

                    if (unattributedAlleles.count(allele) == 0) {
                        // If the allele is not found in the crime scene profile, then drop-out
                        // must have occurred.
                        likelihood *= dropoutProbability;
                    } else {
                        // Otherwise, no drop-out occurred.
                        likelihood *= (1 - dropoutProbability);
                    }
                }
            }
            // If there are any other alleles detected in the replicate, but is not explained by
            // the suspect, then drop-in has occurred. First, calculate which alleles have dropped
            // in.
            const set<string>& suspectAlleles = alleleProfile.getAlleles();

            vector<string> droppedInAlleles;
            set_difference(unattributedAlleles.begin(), unattributedAlleles.end(),
                    suspectAlleles.begin(), suspectAlleles.end(), back_inserter(droppedInAlleles));

            if (droppedInAlleles.size() == 0) {
                // If there is no drop-in, then just multiply by the probability of no drop-in.
                likelihood *= noDropinProb;
            } else {
                // Otherwise, we need to figure out the probability of allele dropping in. First,
                // calculate the renormalization factor of the allele proportions, not counting
                // masked alleles.
                double renormalizationFactor = 0;
                for (map<string, double>::const_iterator iter = alleleProportions.begin();
                        iter != alleleProportions.end(); iter++) {
                    const string& allele = iter->first;
                    if (data[i].maskedAlleles.count(allele) == 0) {
                        renormalizationFactor += iter->second;
                    }
                }

                // Normalization factor better be nonzero!
                assert(renormalizationFactor != 0);

                // For every dropped in allele, we need to multiply by the probability that drop-in
                // occurred, and it's frequency in the population.
                for (vector<string>::const_iterator iter = droppedInAlleles.begin();
                        iter != droppedInAlleles.end(); iter++) {
                    const string& allele = *iter;

                    // Awkward workaround to get data from a const map;
                    double renormalizedProbability = alleleProportions.find(allele)->second /
                            renormalizationFactor;
                    likelihood *= (dropinRate + renormalizedProbability);
                }
            }
        }
        return likelihood;
    }

}


