//==================================================================================================
// Name        : CachingSolver.h
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : This abstract subclass of LikelihoodSolver caches the result of a previously
//     calculated likelihood. Each subclass is responsible for providing a comparison function for
//     Configuration objects, as some solvers may disregard certain configuration values.
//==================================================================================================

#ifndef CACHINGSOLVER_H_
#define CACHINGSOLVER_H_

#include "LikelihoodSolver.h"

/*
 * For ease of creating Configuration comparison functions. Assumes that there is a compare function
 * for the element of the Configuration that you want. Usage:
 *
 *     START_COMPARE_FUNCTION(CachingSolverSubclass, cmpFn) {
 *         COMPARE_CONFIGURATION_ELEMENT(suspectProfile);
 *         COMPARE_CONFIGURATION_ELEMENT(data);
 *         ...
 *     } END_COMPARE_FUNCTION;
 *
 * Note that the last semicolon is optional.
 */
#define START_COMPARE_FUNCTION(className, fnName)                               \
    bool className::fnName (const Configuration& left,                          \
            const Configuration& right) {                                       \
        int val;                                                                \
        do

#define COMPARE_CONFIGURATION_ELEMENT(element)                                  \
            val = compare(left.element , right.element);                        \
            if (val != 0) return (val < 0)

#define END_COMPARE_FUNCTION                                                    \
          while (false);                                                        \
        return 0;                                                               \
    }

namespace LabRetriever {

    class CachingSolver: public LabRetriever::LikelihoodSolver {
        public:
            CachingSolver(ScenarioType type, const string& name,
                    bool(*cmpFn)(const Configuration&, const Configuration&));

            double getLogLikelihood(const Configuration& config);
            virtual double calculateLogLikelihood(const Configuration& config) = 0;
        private:
            map<Configuration, double, bool(*)(const Configuration&, const Configuration&)>
                cachingMap;
    };

    /*
     * The following functions are comparison functions. They return a number that is negative if
     * the left comes before the right, positive if the left comes after the right, and 0 if the
     * two arguments are equal.
     */

    int compare(double left, double right);

    /* Defines a lexicographic ordering for SuspectProfiles. */
    int compare(const AlleleProfile& left, const AlleleProfile& right);

    /* Defines arbitrary orderings, but guarantees equality. */
    int compare(const vector<ReplicateData>& left, const vector<ReplicateData>& right);
    int compare(const map<string, double>& left, const map<string, double>& right);
    int compare(const IdenticalByDescentProbability& left,
            const IdenticalByDescentProbability& right);
    int compare(const ReplicateData& left, const ReplicateData& right);
    int compare(const set<string>& left, const set<string>& right);
} /* namespace LabRetriever */
#endif /* CACHINGSOLVER_H_ */
