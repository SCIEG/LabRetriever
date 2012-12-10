#include "VectorUtil.h"

#include <algorithm>

bool vectorFind(const vector<QString>& v, QString q) {
    return v.end() != std::find(v.begin(), v.end(), q);
}
