#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    vec3f d = getDirection(P);
	vec3f col(1.0, 1.0, 1.0);
	ray r(P, d);
	priority_queue<isect> i;
	if(scene->intersectAll(r, i)) {
		while(!i.empty()) {
			isect t = i.top();
			col = col.multiply(t.getMaterial().kt.clamp());
			if(col.iszero()) {
				break;
			}
			r = ray(r.at(t.t), d);
			i.pop();
		}
	}
	return col;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	float distance = (position - P).length() * pow(10, scene->getScale());
	return min(1.0, 1.0 / (atten_coeff.dot(vec3f(1, distance, distance * distance))));
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    vec3f d = getDirection(P);
	float dis = (position - P).length();
	vec3f col(1.0, 1.0, 1.0);
	ray r(P, d);
	priority_queue<isect> i;
	if(dis >= RAY_EPSILON && scene->intersectAll(r, i)) {
		while(!i.empty()) {
			isect t = i.top();
			col = col.multiply(t.getMaterial().kt.clamp());
			if(col.iszero()) {
				break;
			}
			dis -= t.t;
			r = ray(r.at(t.t), d);
			i.pop();
		}
	}
	return col;
}

double AmbientLight::distanceAttenuation( const vec3f& P ) const
{
	// Never Used
	return 1.0;
}

vec3f AmbientLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f AmbientLight::getDirection( const vec3f& P ) const
{
	// Never Used
	return vec3f(1.0, 1.0, 1.0);
}


vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	// Never Used
    return vec3f(1.0, 1.0, 1.0);
}
