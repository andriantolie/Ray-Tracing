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
	int isectSurface;

	double tMin = -1.0e100;
	double tMax = 1.0e100;

	for (int currentaxis = 0; currentaxis < 3; currentaxis++) {

		double vd = d[currentaxis];
		double vp = p[currentaxis];

		if (vd == 0.0) {
			if (vp < min[currentaxis] || vp > max[currentaxis])
				return false;
		}
		else {
			double v1 = min[currentaxis] - p[currentaxis];
			double v2 = max[currentaxis] - p[currentaxis];
			double t1 = v1 / vd;
			double t2 = v2 / vd;

			if (t1 > t2) { // swap t1 & t2
				double ttemp = t1;
				t1 = t2;
				t2 = ttemp;
			}
			if (t1 > tMin) {
				tMin = t1;
				isectSurface = currentaxis;
			}
			if (t2 < tMax)
				tMax = t2;

			if (tMin > tMax) // box is missed
				return false;
			if (tMax < 0.0) // box is behind ray
				return false;
			if (tMin <= RAY_EPSILON)
				return false;
		}
	}

	i.obj = this;
	i.t = tMin;
	if (isectSurface == 0) {
		if (d[isectSurface] < 0)
			i.N = vec3f(1.0, 0.0, 0.0);
		else
			i.N = vec3f(-1.0, 0.0, 0.0);
	}
	else if (isectSurface == 1) {
		if (d[isectSurface] < 0)
			i.N = vec3f(0.0, 1.0, 0.0);
		else
			i.N = vec3f(0.0, -1.0, 0.0);
	}
	else {
		if (d[isectSurface] < 0)
			i.N = vec3f(0.0, 0.0, 1.0);
		else
			i.N = vec3f(0.0, 0.0, -1.0);
	}
	return true;
}