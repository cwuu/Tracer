#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"
#include <map>

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth );


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );
	//vec3f glossyReflection(ray R, vec3f intensity);
	bool loadScene( char* fn );
	bool loadBackground(char* fn);
	bool sceneLoaded();
	double getFresnelCoeff(isect& i, const ray& r);
	bool isGlossyReflection;
	bool isAntiAlising;
private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;
	
	unsigned char *background;
	int background_width, background_height;
	bool isBackgroundLoad; 
	
	bool m_bSceneLoaded;

	std::map<int, Material>Record;
};

#endif // __RAYTRACER_H__