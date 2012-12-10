//==================================================================================================
// Name        : LikelihoodSolver.cpp
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================

#include "LikelihoodSolver.h"

namespace LabRetriever {
    std::map<LikelihoodSolver::ScenarioType, LikelihoodSolver*>&
            LikelihoodSolver::getExemplarMap() {
        static std::map<LikelihoodSolver::ScenarioType, LikelihoodSolver*> EXEMPLAR_MAP;
        return EXEMPLAR_MAP;
    }

    LikelihoodSolver* LikelihoodSolver::getSolver(ScenarioType type) {
        return getExemplarMap()[type];
    }

    LikelihoodSolver::LikelihoodSolver(ScenarioType type, const string& name) : name(name) {
        getExemplarMap()[type] = this;
        numComplete = 0;
        totalToComplete = 0;
    }
}


