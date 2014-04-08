#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	BoundingBox bounds = ComputeLocalBoundingBox();
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();
	//find tmin and tmax
	vec3f tmin;
	vec3f tmax;
	vec3f nmin(0, 0, 0);
	vec3f nmax(0, 0, 0);
	double min;
	double max;
	for(int j=0; j<3; j++) {
		if(d[j]>=0) {
			tmin[j] = (bounds.min[j] - p[j]) / d[j];
			tmax[j] = (bounds.max[j] - p[j]) / d[j];
			nmin[j] = -1;	
			nmax[j] = 1;
		} else {
			tmin[j] = (bounds.max[j] - p[j]) / d[j];
			tmax[j] = (bounds.min[j] - p[j]) / d[j];
			nmin[j] = 1;
			nmax[j] = -1;
		}
	}

	//min of tmax, max of tmin
	max = std::min( std::min(tmax[0], tmax[1]), tmax[2]);
	min = std::max( std::max(tmin[0], tmin[1]), tmin[2]);
	if(min > max || max < RAY_EPSILON) return false;
	i.obj = this;
	vec3f N(0, 0, 0);
	if(min >= RAY_EPSILON) {
		i.t = min;
		for(int i=0; i<3; i++) {
			if(tmin[i] == min) { N[i] = nmin[i]; break; }
		}
	} else {
		i.t = max;
		for(int i=0; i<3; i++) {
			if(tmax[i] == max) { N[i] = nmax[i]; break; }
		}
	}
	i.N = N;
	return true;
}

