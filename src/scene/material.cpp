#include "ray.h"
#include "material.h"
#include "light.h"
#include "scene.h"
#include "../ui/TraceUI.h"
#include <cmath>
#include <math.h>
extern TraceUI* traceUI;
// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	// iteration 0 
	
	double ambientLight = traceUI->getAmbientLight();
	vec3f I = ke + ka * ambientLight;
	const vec3f point = r.at(i.t);
	// iteration one = ka * ia
	for (list<Light*>::const_iterator li = scene->beginLights(); li != scene->endLights(); li++) {
		vec3f atten = (*li)->distanceAttenuation(point) * (*li)->shadowAttenuation(point); // calculate for each 
		double diffuseCoefficient = (i.N).dot((*li)->getDirection(r.at(i.t)));
		diffuseCoefficient = (diffuseCoefficient > 0) ? diffuseCoefficient : 0; 
		vec3f diffuseTerm = kd * diffuseCoefficient; 
		vec3f reflection = (2 * ((*li)->getDirection(point).normalize().dot(i.N) * i.N)- (*li)->getDirection(point).normalize()).normalize();
		vec3f view = -r.getDirection().normalize();
		double specularCoefficient = reflection.dot(view);
		specularCoefficient = (specularCoefficient > 0) ? specularCoefficient : 0; 
		specularCoefficient = pow(specularCoefficient, shininess * 128);
		vec3f specularTerm = ks * specularCoefficient;
		I += prod(atten, diffuseTerm + specularTerm);
	}

	
	I = I.clamp();// TOBECHECKED
	return I;
	
}
