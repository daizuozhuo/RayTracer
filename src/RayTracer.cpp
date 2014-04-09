// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	//Judge if the starting point is in the air or in an object
	isect i;
	const SceneObject* obj;
	bool inside = false;
	if(scene->intersect(r, i)) {
		obj = i.obj;
		if(scene->intersect(r, i) && i.obj == obj) {
			inside = true;
		}
	}
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), depth ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth, bool inside )
{
	isect i;
	if( depth>=0 && scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.
		
		const Material& m = i.getMaterial();
		vec3f color = m.shade(scene, r, i);
		//calculate the reflected ray
		vec3f d = r.getDirection();
		vec3f position = r.at(i.t);
		vec3f direction = d - 2 * i.N * d.dot(i.N);
		ray newray(position, direction);
		vec3f reflect = m.kr.multiply(traceRay(scene, newray, thresh, depth-1, inside).clamp());
		color += reflect;

		//calculate the refracted ray
		float ref_ratio;
		float sin_ang = d.cross(i.N).length();
		vec3f N = i.N;
		if(inside) {
			//Material to air
			ref_ratio = m.index / 1.0;
			N = -N;
		}
		else {
			ref_ratio = 1.0 / m.index;
		}

		if(!m.kt.iszero() && (ref_ratio < 1.0 + RAY_EPSILON || sin_ang < 1.0 / ref_ratio + RAY_EPSILON)) {
			//No total internal reflection
			//We do refraction now
			float c = N.dot(-d);
			direction = (ref_ratio * c - sqrtf(1 - ref_ratio * ref_ratio * (1 - c * c))) * N + ref_ratio * d;
			newray = ray(position, direction);
			vec3f refraction = m.kt.multiply(traceRay(scene, newray, thresh, depth-1, !inside).clamp());
			color += refraction;
		}

		return color;

	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	//add a spot light to scene
	Light *spot = new SpotLight(scene, vec3f(0, 0, 0), vec3f(1.0, 1.0, 1.0), vec3f(0.1, 0.1, 0.1), vec3f(1.0,1.0,1.0)); 
	scene->add(spot);
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h, int d, float scale )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
	depth = d;
	if(scene) {
		scene->setScale(scale);
	}
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}