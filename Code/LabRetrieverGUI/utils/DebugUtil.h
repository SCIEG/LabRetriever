//==================================================================================================
// Name        : DebugUtil.h
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================

#ifndef DEBUGUTIL_H_
#define DEBUGUTIL_H_

#include "Configuration.h"
#include <map>
#include <vector>
#include <set>

using namespace std;

namespace LabRetriever {
    template <class A>
    void debug(const A& a);

    template <class A, class B>
    void debugToken(const map<A, B>& m);
    template <class A>
    void debugToken(const vector<A>& v);
    template <class A>
    void debugToken(const set<A>& s);
    void debugToken(const AlleleProfile& a);
    void debugToken(const ReplicateData& r);
    void debugToken(const Configuration& c);

    // Default.
    template <class A>
    void debugToken(const A& a);
}

#endif /* DEBUGUTIL_H_ */
