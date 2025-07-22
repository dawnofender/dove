#include "func_common.hpp"
#include <math.h>

namespace Dove {
    float length(Vector3 v) {
        return sqrt(dot(v, v));
    }

    float dot(Vector3 v1, Vector3 v2) {
        return (v1.x * v2.x +
                v1.y * v2.y +
                v1.z * v2.z
        );
    }
}
