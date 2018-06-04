// The main ray tracer.

#include <Fl/fl_ask.h>
#include <iostream>
#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "scene/scene.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "fileio/bitmap.h"
#include "ui/TraceUI.h"
extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	isect i;

	if( scene->intersect( r, i ) ) 
	{
		
		
		
		if (traceUI->isEnableTextureMapping() && i.obj->hasUVmapping())
		{
			//x=r*cos(deta)*cos(phi)
			//z=r*sin(deta)*cos(phi)
			//y=r*sin(phi)
			//phi=arcsin(y/r)
			//deta=arctan2(z,x)
			//u=deta/pi
			//v=phi/(2*pi)
			              
			//coordinate transform
			//UV Mapping
			vec3f Sp = vec3f(0, 0, 1);
			vec3f Se = vec3f(-1, 0, 0);
			vec3f Sn = i.N.normalize();
			double pi = 3.1415926535;
			double phi = acos(-Sn*Sp);
			double v = phi / pi;
			double u;
			if (abs(v) < RAY_EPSILON || abs(v - 1) < RAY_EPSILON)
			{
				u = 0;
			}
			else
			{
				double mini = min(1.0, (Se*Sn) / sin(phi));
				if (mini > 1.0)
					mini = 1.0;
				if (mini < -1.0)
					mini = -1.0;
				double theta = acos(mini) / (2 * pi);
				u = theta;
				if(Sp.cross(Se)*Sn <= RAY_EPSILON)
					u = 1-theta;
					
			}
			
			/*
			vec3f Sp = vec3f(0, 1, 0);
			vec3f Se = vec3f(1, 0, 0);
			vec3f Sn = i.N.normalize();
			double pipipi = 3.1415926535;
			double phi = acos(-Sn.dot(Sp));
			double v = phi / pipipi;
			double theta = acos((Se.dot(Sn)) / sin(phi)) / 2 / pipipi;
			double u = theta;
			if (Sp.cross(Se).dot(Sn) <= 0)
			{
				u = 1 - theta;
			}
			*/
			//color

			if (textureMappingImage && u >= 0 && u <= 1 && v >= 0 && v <= 1)
			{
				int pixelx = min(texture_width - 1, int(u*texture_width));
				int pixely = min(texture_height - 1, int(v*texture_height));

				double r = textureMappingImage[(pixelx+pixely*texture_width) * 3];
				double g = textureMappingImage[(pixelx + pixely*texture_width) * 3 + 1];
				double b = textureMappingImage[(pixelx + pixely*texture_width) * 3 + 2];


				return vec3f(r / 255.0, g / 255.0, b / 255.0).clamp();
			}
			else
				return vec3f(0.0, 0.0, 0.0);



		}
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		

		const Material& m = i.getMaterial();
		

		vec3f Intensity = m.shade(scene, r, i); 
		vec3f source = -r.getDirection();
		vec3f normal = i.N;
		vec3f reflectionDir = 2 * (source.dot(normal))*(normal) - source;
		vec3f conPoint = r.at(i.t);
		ray R = ray(conPoint, reflectionDir);

		if (depth >= traceUI->getDepth())
			return Intensity;
		
		
	
		//const double fresnel_coeff = getFresnelCoeff(i, r);
		// Reflection 

		double fresnel_coeff = FresnelShade(i, r) * 10;
		if (fresnel_coeff > 1.0)
			fresnel_coeff = 0.95;
		cout << fresnel_coeff << endl;
		if (!i.getMaterial().kr.iszero())
		{
			//TODO: adaptive & glossy conflict
			if (isGlossyReflection) {
				//printf("glossy");
				// put the ray in glossy reflection
				vec3f reflection = 2 * (-r.getDirection().dot(i.N)*i.N) + r.getDirection();
				if (reflection[2] > RAY_EPSILON || reflection[2] < -RAY_EPSILON) {
					for (int g = 0; g < 9; g++) {
						double eps1 = double(std::rand()) / RAND_MAX;
						double eps2 = double(std::rand()) / RAND_MAX;
						double eps3 = double(std::rand()) / RAND_MAX;
						vec3f dir = reflectionDir;
						vec3f glossy;
						glossy[1] = dir[1] + eps1/20 ;
						glossy[0] = dir[0] + eps2/20 ;
						glossy[2] = dir[2] + eps3/20;
						Intensity += 0.1 * prod(i.getMaterial().kr, traceRay(scene, ray(r.at(i.t), glossy.normalize()), thresh, depth +1 ));
						//glossy[0] = dir[0]+eps1*10;
						//glossy[1] = dir[1] + eps2*10;
						//glossy[2] = dir[2] +eps3*5;
						//Intensity +=  0.2  * prod(i.getMaterial().kr, traceRay(scene, ray(conPoint, glossy.normalize()), thresh, depth + 1));
					}
					Intensity += (0.1 * prod(i.getMaterial().kr, traceRay(scene, R, thresh, depth + 1)));
					//vec3f glossyReflection4 = vec3f(source[0], source[1], source[2] + eps3).normalize();
					//ray ray1 = ray(conPoint, glossyReflection1);
					//ray ray2 = ray(conPoint, glossyReflection2);
					//ray ray3 = ray(conPoint, glossyReflection3);
					//ray ray4 = ray(conPoint, glossyReflection4);
				}
			}
			else
			Intensity += (fresnel_coeff  *prod(i.getMaterial().kr*(1 - traceUI->getAdaptiveTermination()), traceRay(scene, R, thresh, depth - 1)));

			
		}

		// Refraction, maintained by a map, whcih is actually an extended stack	  
		if (!i.getMaterial().kt.iszero()) 
		{
			// take account total refraction effect
			bool TotalRefraction = false;
			// opposite ray
			ray oppR(conPoint, r.getDirection()); //without refraction

			 // flag for map/ stack
			bool toAdd = false;
			bool toErase = false;

			//hardcode interior, always assume it is true 
			//should replace true to i.obj->hasInterior()

			/*

			pseudocode for refration

			if (ray is entering object ) then
				n_t = material.index
				n_i = air.index
			else
				n_i = material.index
				n_t = air.index

			*/
			if (true) 
			{
				// refractive index eg.air&water
				double indexA;
				double indexB;

				// For ray go out of an object
				//record the index of last object so that we can use it when ray enter the obj
				if (normal*r.getDirection() > RAY_EPSILON)
				{
					if (Record.empty())
						indexA = 1.0;
					else
						// return the refractive index of last object
						indexA = Record.rbegin()->second.index;

					Record.erase(i.obj->getOrder());
					toAdd = true;

					if (Record.empty())
						indexB = 1.0;
					else
						indexB = Record.rbegin()->second.index;

					normal = -i.N;
				}
				// For ray get in the object
				else
				{
					if (Record.empty())
						indexA = 1.0;
					else
						indexA = Record.rbegin()->second.index;

					Record.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
					toErase = true;
					indexB = Record.rbegin()->second.index;
					normal = i.N;
				}

				double indexRatio = indexA / indexB;
				//min(x, 1.0) to prevent cos_i becomes bigger than 1
				double cos_i = max(min(normal*((-r.getDirection()).normalize()), 1.0), -1.0); 
				double sin_i = sqrt(1 - cos_i*cos_i);
				double sin_t = sin_i * indexRatio;
				
				//check totalrefraction
				if (sin_t > 1.0)
					TotalRefraction = true;
				else
				{
					TotalRefraction = false;
					double cos_t = sqrt(1 - sin_t*sin_t);
					vec3f Tdir = (indexRatio*cos_i - cos_t)*normal - indexRatio*-r.getDirection();
					oppR = ray(conPoint, Tdir);

					if (!traceUI->IsEnableFresnel()) {
						Intensity += prod(i.getMaterial().kt, traceRay(scene, oppR, thresh, depth + 1));
					}
					else
					{
						Intensity += ((1 - fresnel_coeff)*prod(i.getMaterial().kt, traceRay(scene, oppR, thresh, depth + 1)));
					}
				}
			}

			//naintain stack
			if (toAdd)
				Record.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
			if (toErase)
				Record.erase(i.obj->getOrder());
		}
	
		//intensity range from 0.0 to 1.0
		Intensity = Intensity.clamp();
		return Intensity;
	
	
	} 
	else
	{
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		if (background) {
			printf("background loading ");
			double bg_x = scene->getCamera()->getU();
			double bg_y = scene->getCamera()->getV();
			vec3f backgroundByPixel = vec3f(0, 0, 0);
			//double x = (bg_x / bg_z) + 0.5;
			//double y = bg_y / bg_z + 0.5;
			int xPixel = bg_x*background_width;
			int yPixel = bg_y*background_height;
			if (xPixel < 0 || xPixel >= background_width || yPixel < 0 || yPixel >= background_height) return vec3f(0, 0, 0);
			else{
				double v1 = background[(yPixel*background_width + xPixel) * 3] / 255.0;
				double v2 = background[(yPixel*background_width + xPixel) * 3 + 1] / 255.0;
				double v3 = background[(yPixel*background_width + xPixel) * 3 + 2] / 255.0;
				return vec3f(v1, v2, v3);
				//return vec3f(0,0,0);
			}
			return backgroundByPixel;
		}
	}
		return vec3f(0,0,0);
}
double RayTracer::FresnelShade(isect& i, const ray& r)
{
	//fresnel equation
	//reflectance R of a dielectric dpends on the refractive index n of the material and the angle of incidence deta
	//R0 = (n-1)^2/(n+1)^2 for deta = 0


	if (!traceUI->IsEnableFresnel())
	{
		return 1.0;
	}
	vec3f normal = i.N;

	//assume it has interior

	double indexA;
	double indexB;
	// For ray get out the object
	if (normal*r.getDirection() > RAY_EPSILON)
	{
		if (Record.empty())
			indexA = 1.0;
		else
			// return the refractive index of last object
			indexA = Record.rbegin()->second.index;

		Record.erase(i.obj->getOrder());

		if (Record.empty())
			indexB = 1.0;
		else
			indexB = Record.rbegin()->second.index;

		normal = -i.N;
		Record.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
	}
	// For ray get in the object
	else
	{
		if (Record.empty())
			indexA = 1.0;
		else
			indexA = Record.rbegin()->second.index;

		normal = i.N;
		Record.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
		indexB = Record.rbegin()->second.index;
		Record.erase(i.obj->getOrder());
	}

	double r0 = ((indexA - indexB) / (indexA + indexB))* ((indexA - indexB) / (indexA + indexB));


	const double cos_i = max(min(i.N.dot(-r.getDirection().normalize()), 1.0), -1.0);
	//const double cos_i = i.N.dot(-r.getDirection().normalize());
	if (indexA <= indexB)
	{
		double x = 1 - cos_i;
		return r0 + (1 - r0)*x*x*x*x*x;
	}
	else
	{
		const double ratio = indexA / indexB;
		const double sinT = ratio*ratio*(1 - cos_i*cos_i);

		if (sinT>1.0)
		{
			return 1.0;
		}
		else
		{
			//const double cos_t = sqrt(1 - ratio);
			double x = 1 - sqrt(1 - sinT*sinT);
			return r0 + (1 - r0)*x*x*x*x*x;
		}
	}

}
RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	textureMappingImage = NULL;
	background = NULL;
	isBackgroundLoad = false;
	isGlossyReflection = false; 
	isAntiAlising = false;
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
void RayTracer::loadtextureMappingImage(char* fn) 
{
	unsigned char* data = NULL;
	data = readBMP(fn, texture_width, texture_height);
	if (data)
	{
		if (textureMappingImage)
		{
			delete[]textureMappingImage;
		}
		textureMappingImage = data;
	}
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
	
	m_bSceneLoaded = true;

	return true;
}

bool RayTracer::loadBackground(char* fn)
{
	unsigned char* data = NULL; 
	data = readBMP(fn, background_width, background_height);
	if (data!=NULL) {
		isBackgroundLoad = true;
		if (background) {
			// if the bmp file is not empty
			delete[] background;
			background = data;
		

		}
		background = data;
		return true;
	}
	else {
		isBackgroundLoad = false;
		return false;
	}
}

void RayTracer::traceSetup( int w, int h )
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

	if (traceUI->isEnableMotionBlur()) {
		col =0.2*trace(scene, x + 0.007, y-0.007)+ 0.1*trace(scene, x + 0.008, y - 0.008)
			  + 0.2*trace(scene, x - 0.007, y + 0.007) + 0.1*trace(scene, x - 0.008, y + 0.008)+0.4*trace(scene,x,y);
		//col =col + trace(scene, x, y)+ trace(scene, x, y)+ trace(scene, x, y)+ trace(scene, x, y);
		//col[0] = col[0] / 6;
		//col[1] = col[1] / 6;
		//col[2] = col[2] / 6;
	}

	if (traceUI->isEnableJittering()) {
		double xJitter = double(rand() % 10 - 5) / 10.0;
		double yJitter = double(rand() % 10 - 5) / 10.0;
		double x = double(i + xJitter) / double(buffer_width);
		double y = double(j + yJitter) / double(buffer_height);
	}
	else if (traceUI->getAntiAlising() >0) {

		int sampling = traceUI->getAntiAlising() +1;
		for (int m = 0; m < sampling; m++) { 
			for (int n = 0; n < sampling; n++) {
				x = double(i - 0.5 + m/double(sampling)) / double(buffer_width);
				y = double(j - 0.5 + n/double(sampling)) / double(buffer_height);
				col += trace(scene, x, y);
			}
		}
		col = col / sampling/sampling;
	}
	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;



	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}
