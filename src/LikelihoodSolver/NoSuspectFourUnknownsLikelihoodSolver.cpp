//==================================================================================================
// Name        : NoSuspectFourUnknownsLikelihoodSolver.cpp
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
    class NoSuspectFourUnknownsLikelihoodSolver: public CachingSolver {
        public:
            double calculateLogLikelihood(const Configuration& config);
        private:
            static bool cmpFn(const Configuration& left, const Configuration& right);
            NoSuspectFourUnknownsLikelihoodSolver();


            const static NoSuspectFourUnknownsLikelihoodSolver EXEMPLAR;
    };

    START_COMPARE_FUNCTION(NoSuspectFourUnknownsLikelihoodSolver, cmpFn) {
        COMPARE_CONFIGURATION_ELEMENT(suspectProfile);
        COMPARE_CONFIGURATION_ELEMENT(data);
        COMPARE_CONFIGURATION_ELEMENT(alleleProportions);
        COMPARE_CONFIGURATION_ELEMENT(identicalByDescentProbability);
        COMPARE_CONFIGURATION_ELEMENT(dropoutRate);
        COMPARE_CONFIGURATION_ELEMENT(dropinRate);
        COMPARE_CONFIGURATION_ELEMENT(alpha);
    } END_COMPARE_FUNCTION;

    NoSuspectFourUnknownsLikelihoodSolver::NoSuspectFourUnknownsLikelihoodSolver() :
            CachingSolver(NO_SUSPECT_FOUR_UNKNOWNS, "No-Suspect_Four-Unknowns", cmpFn) {}

    double NoSuspectFourUnknownsLikelihoodSolver::calculateLogLikelihood(
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
        numComplete = 0;
        // TODO: not correct.
        totalToComplete =  pow(numAlleles, 4) *
            (2 * numAlleles + numAlleles * numAlleles + 1);

        double zeroIBDLogLikelihood = LOG_ZERO;
        if (ibdProbability.zeroAllelesInCommonProb != 0) {
            BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele1, logProbRandomAllele1, allele2,
                    logProbRandomAllele2) {
                BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele3, logProbRandomAllele3,
                        allele4, logProbRandomAllele4) {
                    BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele5, logProbRandomAllele5,
                            allele6, logProbRandomAllele6) {
                        BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele7,
                                logProbRandomAllele7, allele8, logProbRandomAllele8) {
                            double logProbHavingTheseAlleles =
                                    logProbRandomAllele1 + logProbRandomAllele2 +
                                    logProbRandomAllele3 + logProbRandomAllele4 +
                                    logProbRandomAllele5 + logProbRandomAllele6 +
                                    logProbRandomAllele7 + logProbRandomAllele8;
                            if (allele1 != allele2) {
                                logProbHavingTheseAlleles += LOG_TWO;
                            }
                            if (allele3 != allele4) {
                                logProbHavingTheseAlleles += LOG_TWO;
                            }
                            if (allele5 != allele6) {
                                logProbHavingTheseAlleles += LOG_TWO;
                            }
                            if (allele7 != allele8) {
                                logProbHavingTheseAlleles += LOG_TWO;
                            }

                            // Create a new suspect profile.
                            AlleleProfile newSuspectProfile;
                            newSuspectProfile.addAllele(allele1).addAllele(allele2)
                                    .addAllele(allele3).addAllele(allele4)
                                    .addAllele(allele5).addAllele(allele6)
                                    .addAllele(allele7).addAllele(allele8);

                            double logProbGivenAlleles = calculateLogProbability(newSuspectProfile,
                                    data, alleleProportions, alpha, dropoutRate, dropinRate,
                                    noDropinProb);
                            double partialZeroIBDLogLikelihood = logProbGivenAlleles +
                                    logProbHavingTheseAlleles;

                            zeroIBDLogLikelihood = addLogProbability(zeroIBDLogLikelihood,
                                    partialZeroIBDLogLikelihood);
                            numComplete += 1;
                        } END_CHOOSE_RANDOM_ALLELES;
                    } END_CHOOSE_RANDOM_ALLELES;
                } END_CHOOSE_RANDOM_ALLELES;
            } END_CHOOSE_RANDOM_ALLELES;
        }
        numComplete = pow(numAlleles, 6);

        double oneIBDLogLikelihood = LOG_ZERO;
        if (ibdProbability.oneAlleleInCommonProb != 0) {
            // Choose one allele from the suspect:
            const set<string>& suspectAlleles = suspectProfile.getAlleles();
            BEGIN_CHOOSE_ONE_RANDOM_ALLELE(alleleProportions, suspectAlleles, suspectAllele,
                    otherAllele, logProbHavingRandomAllele) {
                BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele1, logProbRandomAllele1,
                        allele2, logProbRandomAllele2) {
                    BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele3, logProbRandomAllele3,
                            allele4, logProbRandomAllele4) {
                        BEGIN_CHOOSE_RANDOM_ALLELES(alleleProportions, allele5,
                                logProbRandomAllele5, allele6, logProbRandomAllele6) {
                            double logProbHavingTheseAlleles =
                                    logProbHavingRandomAllele + logProbRandomAllele1 +
                                    logProbRandomAllele2 + logProbRandomAllele3 +
                                    logProbRandomAllele4 + logProbRandomAllele5 +
                                    logProbRandomAllele6;
                            if (allele1 == allele2) {
                                logProbHavingTheseAlleles += LOG_TWO;
                            }
                            if (allele3 == allele4) {
                                logProbHavingTheseAlleles += LOG_TWO;
                            }
                            if (allele5 == allele6) {
                                logProbHavingTheseAlleles += LOG_TWO;
                            }

                            AlleleProfile relatedSuspectProfile;
                            relatedSuspectProfile.addAllele(suspectAllele).addAllele(otherAllele)
                                    .addAllele(allele1).addAllele(allele2)
                                    .addAllele(allele3).addAllele(allele4)
                                    .addAllele(allele5).addAllele(allele6);

                            double logProbGivenAllele = calculateLogProbability(relatedSuspectProfile,
                                    data, alleleProportions, alpha, dropoutRate, dropinRate,
                                    noDropinProb);
                            double partialOneIBDLogLikelihood = logProbGivenAllele +
                                    logProbHavingRandomAllele;

                            // TODO: original code divides probability by 2. Check if correct.
                            oneIBDLogLikelihood = addLogProbability(oneIBDLogLikelihood,
                                    partialOneIBDLogLikelihood);
                            numComplete += 1;
                        } END_CHOOSE_RANDOM_ALLELES;
                    } END_CHOOSE_RANDOM_ALLELES;
                } END_CHOOSE_RANDOM_ALLELES;
            } END_CHOOSE_ONE_RANDOM_ALLELE;
        }
        // TODO: not correct.
        numComplete = totalToComplete - pow(numAlleles, 4);

        double bothIBDLogLikelihood = (ibdProbability.bothAllelesInCommonProb == 0) ?
                LOG_ZERO :
                LikelihoodSolver::getSolver(ONE_SUSPECT_THREE_UNKNOWNS)->getLogLikelihood(config);

        double logLikelihood = addLogProbability(
                log(ibdProbability.zeroAllelesInCommonProb) + zeroIBDLogLikelihood,
                addLogProbability(
                        log(ibdProbability.oneAlleleInCommonProb) + oneIBDLogLikelihood,
                        log(ibdProbability.bothAllelesInCommonProb) + bothIBDLogLikelihood));
        numComplete = totalToComplete;
        return logLikelihood;
    }

    const NoSuspectFourUnknownsLikelihoodSolver NoSuspectFourUnknownsLikelihoodSolver::EXEMPLAR;
} /* namespace LabRetriever */


