#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;
	static void setSpotP(int p) { spotP = p; }
	static void setCutoff(double cut) { cutoff = cut; }

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}
	static int spotP;
	static float cutoff;
	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color, const vec3f& coeff )
		: Light( scene, color ), position( pos ), atten_coeff(coeff) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f position;
	vec3f atten_coeff;
};

class SpotLight 
	: public PointLight
{
public:
	SpotLight( Scene *scene, const vec3f& pos, const vec3f& color, const vec3f& coeff, const vec3f& direct, float cutoff, float shine )
		:PointLight(scene, pos, color, coeff ), direction(direct.normalize()), cutoff_ang(cosf(cutoff)), shiness(shine) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
protected:
	vec3f direction;
	float cutoff_ang;
	float shiness;
};

class AmbientLight
	: public Light
{
public:
	AmbientLight( Scene *scene, const vec3f& color )
		: Light( scene, color ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
};

#endif // __LIGHT_H__
