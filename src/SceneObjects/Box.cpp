#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();


	double tMin = -1.0e308;
	double tMax = 1.0308;

	for (int currentaxis = 0; currentaxis < 3; currentaxis++) {

		double vd = d[currentaxis];
		double vp = p[currentaxis];

		if (vd == 0) {
			if (vp < min[currentaxis] || vp > max[currentaxis])
				continue;
		}

		double v1 = min[currentaxis] - p[currentaxis];
		double v2 = max[currentaxis] - p[currentaxis];
		double t1 = v1 / vd;
		double t2 = v2 / vd;

		if (t1 > t2) { // swap t1 & t2
			double ttemp = t1;
			t1 = t2;
			t2 = ttemp;
		}
		if (t1 > tMin)
			tMin = t1;
		if (t2 < tMax)
			tMax = t2;

		if (tMin > tMax) // box is missed
			return false;
		if (tMax < 0.0) // box is behind ray
			return false;
	}
	return true;
}