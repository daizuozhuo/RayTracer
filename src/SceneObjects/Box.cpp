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

	//find tmin and tmax
	vec3f tmin;
	vec3f tmax;
	double min;
	double max;
	for(int i=0; i<3; i++) {
		min = (bounds.min[i] - p[i]) / d[i];
		max = (bounds.max[i] - p[i]) / d[i];
		tmin[i] = std::min(min, max);
		tmax[i] = std::max(min, max);
	}

	//min of tmax, max of tmin
	min = std::min( std::min(tmax[0], tmax[1]), tmax[2]);
	max = std::max( std::max(tmin[0], tmin[1]), tmin[2]);
	if(max > min) return false;
	i.obj = this;
	vec3f N(0, 0, 0);
	if(max > 0) {
		i.t = max;
		for(int i=0; i<3; i++) {
			if(tmin[i] == max) { N[i] = -1; break; }
		}
	} else {
		i.t = min;
		for(int i=0; i<3; i++) {
			if(tmin[i] == max) { N[i] = 1; break; }
		}
	}
	i.N = N;
	return true;
}
