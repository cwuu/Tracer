#include <cmath>
#include "../ui/TraceUI.h"
#include "light.h"
extern TraceUI* traceUI;
double DirectionalLight::distanceAttenuation(const vec3f& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	//vec3f d = (getDirection(P) - P).normalize();
	//ray r(P, dir);
	vec3f d = getDirection(P);
	vec3f curP = P;
	isect isecP; // update the intersect 
	ray r = ray(curP, d);
	vec3f result = getColor(P);
	while (scene->intersect(r, isecP))
	{
		if (isecP.getMaterial().kt.iszero()) return vec3f(0, 0, 0); // if black stop & return 0
		curP = r.at(isecP.t);
		r = ray(curP, d);// updae
		result = prod(result, isecP.getMaterial().kt);
	}
	return result;
}

vec3f DirectionalLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection(const vec3f& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	//return 1.0;
	double constant = traceUI->getAttenuationConstant();
	double linear = traceUI->getAttenuationLinear();
	double qua = traceUI->getAttenuationQuadratic();
	double distance = (P - position).length();
	double atten = 1.0 / (constant + linear *distance + qua *(distance * distance));
	if (atten < 1.0) return atten;
	else
		return 1.0;
	
}

vec3f PointLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	//return vec3f(1,1,1);


	vec3f d = getDirection(P);
	double Dlen = (position - P).length();
	ray r = ray(P, getDirection(P));
	vec3f curP = r.getPosition();
	isect isecP;
	vec3f result = vec3f(0,0,0);
	if (traceUI->isEnableSoftShadowing()) {
		result = vec3f(1.0, 1.0, 1.0);
		for (unsigned m = 0; m < 6; m++) {
			for (unsigned n = 0; n < 6; n++) {
				for (unsigned p = 0; p < 6; p++) {
					//samples
					vec3f nearpos = vec3f(position[0] - 0.5 + (double)m / 6 - P[0], position[1] - 0.5 + (double)n / 6 - P[1], position[2] - 0.5 + (double)p / 6 - P[2]);
					//ray newRay(P, (nearpos - P).normalize());
					//isect isecNewP;
					//vec3f newP = newRay.at(isecNewP.t);
					ray newRay(P, nearpos.normalize());
					result += softShadowAttenuation(P, newRay) / 6 / 6 / 6;
					//if ((Dlen -= isecNewP.t) < RAY_EPSILON) {}
					//if (isecNewP.getMaterial().kt.iszero()) {} // if black;
					//else result += prod(result, isecNewP.getMaterial().kt)/6/6/6;

				}
			}
		}
		return result;
	}
		result = getColor(P);
		while (scene->intersect(ray(curP, d), isecP)) {
			if ((Dlen -= isecP.t) < RAY_EPSILON) return result;
			if (isecP.getMaterial().kt.iszero()) return vec3f(0, 0, 0); // if black;
			curP = r.at(isecP.t);
			result = prod(result, isecP.getMaterial().kt);
		}
	return result;

}

vec3f PointLight::softShadowAttenuation(const vec3f& P,const ray& r) const {
	vec3f d = r.getDirection();
	double Dlen = (position - P).length();
	vec3f curP = r.getPosition();
	vec3f result = getColor(P);
	isect isecP;
	ray newRay = ray(curP, d);
	//vec3f result = vec3f(1, 1, 1);
	while (scene->intersect(ray(curP, d), isecP)) {
		if ((Dlen -= isecP.t) < RAY_EPSILON) return result;
		if (isecP.getMaterial().kt.iszero()) return vec3f(0, 0, 0); // if black;

		//curP = r.at(isecP.t);
		result = prod(result, isecP.getMaterial().kt);
	}
	return result;
}
double SpotLight::distanceAttenuation(const vec3f& P) const {
	double constant = traceUI->getAttenuationConstant();
	double linear = traceUI->getAttenuationLinear();
	double qua = traceUI->getAttenuationQuadratic();
	double distance = (P - position).length();
	double atten = 1.0 / (constant + linear *distance + qua *(distance * distance));
	if (atten < 1.0) return atten;
	else return 1.0;
}
vec3f SpotLight::getColor(const vec3f& P) const
{

	return color;
}

vec3f SpotLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}



vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
	vec3f d = getDirection(P);
	double Dlen = (position - P).length();
	//double intensity = d.dot(orientation.normalize());
	//set the edge place 
	//double length = (position - edgeplace).length(); // range of spotlight

	vec3f dir = (position - P).normalize();
	vec3f toFrag = position - edgeplace;
	// cone angle calculation

	double coneAngle = direction.dot(toFrag) / (direction.length() * toFrag.length());
	if (coneAngle != 0 && dir * direction < coneAngle*3.1415926 ) return vec3f(0, 0, 0);

	ray r = ray(P, getDirection(P));
	vec3f curP = r.getPosition();
	isect isecP;

	double tuner = dir* direction > 0 ? dir*direction : 0;
	vec3f result = tuner * getColor(P);
	if (dir * direction < 0)
		while (scene->intersect(ray(curP, d), isecP)) {


			if ((Dlen -= isecP.t) < RAY_EPSILON) return result;
			if (isecP.getMaterial().kt.iszero()) return vec3f(0, 0, 0); // if black;
			curP = r.at(isecP.t);
			result = prod(result, isecP.getMaterial().kt);

		}
	return result;

}