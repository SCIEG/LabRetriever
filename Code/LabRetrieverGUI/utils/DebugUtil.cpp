//==================================================================================================
// Name        : DebugUtil.cpp
// Author      : Ken Cheng
// Copyright   : This program is free software: you can redistribute it and/or modify it under the
//   terms of the GNU General Public License as published by the Free Software Foundation, either
//   version 3 of the License, or (at your option) any later version.  This program is distributed
//   in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//   more details see <http://www.gnu.org/licenses/>.
// Description : 
//==================================================================================================

#include "DebugUtil.h"

#include <iostream>

using namespace std;

namespace LabRetriever {
    template <class A>
    void debug(const A& a) {
        debugToken(a);
        cout << endl;
    }

    template <class A, class B>
    void debugToken(const map<A, B>& m) {
        typedef typename map<A, B>::const_iterator Iterator;
        cout << "{" << endl;
        for (Iterator iter = m.begin(); iter != m.end(); iter++) {
            debugToken(iter->first);
            cout << "\t:\t";
            debugToken(iter->second);
            cout << endl;
        }
        cout << "}";
    }

    template <class A>
    void debugToken(const vector<A>& v) {
        cout << "[ ";
        for (unsigned int i = 0; i < v.size(); i++) {
            debugToken(v[i]);
            cout << " ";
        }
        cout << "]";
    }

    template <class A>
    void debugToken(const set<A>& s) {
        cout << "{ ";
        if (s.size() != 0) {
            typedef typename set<A>::const_iterator Iterator;
            for (Iterator iter = s.begin(); iter != s.end(); iter++) {
                debugToken(*iter);
                cout << " ";
            }
        }
        cout << "}";
    }

    void debugToken(const AlleleProfile& a) {
        cout << "Allele Profile: ";
        debugToken(a.getAlleleCounts());
    }

    void debugToken(const ReplicateData& r) {
        cout << "Assumed Alleles: ";
        debugToken(r.maskedAlleles);
        cout << endl << "Unattributed Alleles: ";
        debugToken(r.unattributedAlleles);
    }

    void debugToken(const Configuration& c) {
        cout << "Configuration: " << endl;
        debug(c.suspectProfile);
        debug(c.data);
        cout << "Allele Proportions: ";
        debug(c.alleleProportions);
        cout << "Alpha: ";
        debug(c.alpha);
        cout << "Drop-in rate: ";
        debug(c.dropinRate);
        cout << "Drop-out rate: ";
        debugToken(c.dropoutRate);
    }


    template <class A>
    void debugToken(const A& a) {
        cout << a;
    }

}


