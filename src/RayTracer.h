#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"
#include <vector>

using std::vector;

enum TraceMode {
	TRACE_NORMAL = 0,
	TRACE_ANTIALIAS_NORMAL,
	TRACE_JITTER,
	TRACE_ADAPTIVE_ANTIALIAS,
	NUM_TRACE_MODE
};

#define SAMPLE_DELTA	0.01f

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
    vec3f trace( Scene *scene, double x, double y, isect& i );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth, vector<const SceneObject*>& stack );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth, isect& i, vector<const SceneObject*>& stack );

	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h, int d, float scale);
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	vec3f adaptiveSample( double x, double y, double w, double h, int depth, 
							vec3f& LB_col, isect& LB, vec3f& RB_col, isect& RB,
							vec3f& RT_col, isect& RT, vec3f& LT_col, isect& LT);

	bool DeltaChange(vec3f a, vec3f b, vec3f c, vec3f d);
	bool DeltaChange(vec3f a, vec3f b);

	bool loadScene( char* fn );

	void setMode(enum TraceMode m);
	void setSampleSize(int size);
	void setDisp(bool visual);

	void setSpotP(int p);
	void setCutoff(float c);
	bool sceneLoaded();

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	int depth;
	float dis_scale;
	Scene *scene;
	enum TraceMode mode;
	int sampleSize;
	bool ray_visual;

	bool m_bSceneLoaded;

	vec3f n_ray;
};

#endif // __RAYTRACER_H__
