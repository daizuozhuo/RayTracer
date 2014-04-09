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
	vec3f col = getColor(P);
	ray r(P, d);
	isect i;
	while(scene->intersect(r, i)) {
		col = col.multiply(i.getMaterial().kt.clamp());
		if(col.iszero()) {
			break;
		}
		r = ray(r.at(i.t), d);
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
	float distance = (position - P).length();
	return min(1.0,  pow(10, scene->getScale()) / (atten_coeff.dot(vec3f(1, distance, distance * distance))));
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
	vec3f col = getColor(P);
	ray r(P, d);
	isect i;
	while(dis >= RAY_EPSILON && !col.iszero() && scene->intersect(r, i)) {
		dis -= i.t;
		if(dis < RAY_EPSILON) {
			break;
		}
		col = col.multiply(i.getMaterial().kt.clamp());
		r = ray(r.at(i.t), d);
	}
	return col;
}

vec3f SpotLight::getColor(const vec3f& P) const
{
	//Splot light color depend on p
	vec3f L = direction ;
	vec3f tL=getDirection(P);
	return color * pow(L.dot(tL), 16);
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
