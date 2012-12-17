//==================================================================================================
// Name        : LikelihoodUtil.h
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================

#ifndef LIKELIHOODUTIL_H_
#define LIKELIHOODUTIL_H_

#include "Configuration.h"
#include <vector>
#include <map>

using namespace std;

/* Choose two alleles for a random person. */
#define BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele1, logProbRandomAllele1,   \
        allele2, logProbRandomAllele2)                                                  \
    for (map<string, double>::const_iterator allele1##iter = alleleProportions.begin(); \
            allele1##iter != alleleProportions.end(); allele1##iter++) {                \
        const string& allele1 = allele1##iter->first;                                   \
        double logProbRandomAllele1 = log(allele1##iter->second);                       \
                                                                                        \
        /* If the proportion is 0, then no need to calculate. */                        \
        if (logProbRandomAllele1 == 0) continue;                                        \
        for (map<string, double>::const_iterator allele2##iter = allele1##iter;         \
                allele2##iter != alleleProportions.end(); allele2##iter++) {            \
            const string& allele2 = allele2##iter->first;                               \
            double logProbRandomAllele2 = log(allele2##iter->second);                   \
            if (logProbRandomAllele2 == 0) continue;                                    \
            do

#define END_CHOOSE_RANDOM_ALLELES                                                       \
            while (false);                                                              \
        }                                                                               \
    }

/* Choose one allele from a set of the suspects alleles, and one randomly. */
#define BEGIN_CHOOSE_ONE_RANDOM_ALLELE(alleleProportions, suspectAlleles, allele1,          \
        allele2, logProbHavingRandomAllele)                                                 \
    for (set<string>::const_iterator allele1##iter = suspectAlleles.begin();                \
        allele1##iter != suspectAlleles.end(); allele1##iter++) {                           \
        const string& allele1 = *(allele1##iter);                                           \
                                                                                            \
        /* Choose another allele randomly: */                                               \
        for (map<string, double>::const_iterator allele2##iter = alleleProportions.begin(); \
                allele2##iter != alleleProportions.end(); allele2##iter++) {                \
            const string& allele2 = allele2##iter->first;                                   \
            double logProbHavingRandomAllele = allele2##iter->second;                       \
            if (logProbHavingRandomAllele == 0) continue;                                   \
            do

#define END_CHOOSE_ONE_RANDOM_ALLELE                                                        \
            while (false);                                                                  \
        }                                                                                   \
    }

namespace LabRetriever {

    /**
     * Calculates the probability that the a source (a person or a group of people) with the given
     * AlleleProfile explains the results of the replicated DNA.
     *
     * alleleProfile - The AlleleProfile containing the alleles from the suspected source.
     * data - A list of ReplicateData objects, each holding the detected alleles from its
     *     corresponding replication. Note that if an allele is in the masked set, then it was
     *     detected in the replicate, but explained by a known source (as opposed to not being in
     *     the masked set, meaning it was not detected in the replicate, regardless if a known
     *     source had that allele).
     * alleleProportions - A mapping of alleles to the frequency of which that allele appears in the
     *     surrounding population.
     * alpha - A term used to help calculate multiple alleles dropping out. For more explanation,
     *     check out 'Interpreting low template DNA profiles' by Balding and Buckleton.
     * dropoutRate - The (estimated) rate at which alleles that should be present do not show up in
     *     a low template DNA test.
     * dropinRate - The (estimated) rate at which alleles that should not be present somehow show up
     *     in a low template DNA test. That is, this is the probability at which an allele from a
     *     (seemingly) random person's DNA could have been picked up on the LTDNA test.
     * noDropinProb - The calculated probability at which absolutely no drop-in occurs.
     */
    double calculateLogProbability(const AlleleProfile& alleleProfile,
            const vector<ReplicateData>& data, const map<string, double>& alleleProportions,
            double alpha, double dropoutRate, double dropinRate, double noDropinProb);

    double calculateProbability(const AlleleProfile& alleleProfile,
            const vector<ReplicateData>& data, const map<string, double>& alleleProportions,
            double alpha, double dropoutRate, double dropinRate, double noDropinProb);
}


#endif /* LIKELIHOODUTIL_H_ */
