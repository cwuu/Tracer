#include <cmath>
#include <assert.h>
#include <limits>
#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	/*i.obj = this;
	double Tfar = std::numeric_limits<double>::infinity();
	printf("%lf", Tfar);
	double Tnear = -Tfar;
	vec3f &Xdir = r.getDirection(); // Xd: the ray s direction
	vec3f &Xori = r.getPosition(); //Xo: the origin
	const double Xlower = -0.5;
	const double Xupper = 0.5; // the higher & lower edge of box
	vec3f N1, N2, Nnear, Nfar; // normal set
	for(int i = 0; i < 3 ; i ++){
		if (abs(Xdir[i]) < RAY_EPSILON) {// the ray uis parallel to the x planes 
			if (Xori[i]< Xlower || Xori[i] > Xupper) return false;
		}
		double T1 = (Xlower - Xori[i]) / Xdir[i];
		double T2 = (Xupper - Xori[i]) / Xdir[i];
		N1[i] = -1;
		N2[i] = 1; 
		if (T1 > T2) { // swap T1 & T2 
			double temp = T1; 
			T1 = T2; 
			T2 = temp;
			double temp1 = N1[i];
			N1[i] = N2[i];
			N2[i] = temp1; 
		}
		if (T1 > Tnear) { 
			Tnear = T1;
			Nnear = N1;
			}
		if (T2 < Tfar) {
			Tfar = T2;
			Nfar = N2;
		}
		if (Tnear > Tfar ||Tfar < RAY_EPSILON ) return false;
		//if (Tfar < 0) return false; 			
	}
	i.setT(Tnear);
	i.setN(Nnear);
	return true;	
	*/
	// The Box Intersection algorithm addressed here: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
	i.obj = this;

	numeric_limits<double> doubleLimit;
	double Tfar = doubleLimit.infinity(), Tnear = -Tfar;
	vec3f Nnear, Nfar;

	vec3f &dir = r.getDirection();
	vec3f &ori = r.getPosition();
	const double lBound = -0.5, uBound = 0.5;

	for (int axis = 0; axis < 3; ++axis) {
		if (abs(dir[axis]) < RAY_EPSILON) {
			if (ori[axis] < lBound || ori[axis] > uBound) return false;
		}
		vec3f N1, N2;
		double t1 = (lBound - ori[axis]) / dir[axis];
		N1[axis] = -1;
		double t2 = (uBound - ori[axis]) / dir[axis];
		N2[axis] = 1;
		if (t1 > t2) {
			double t = t1;
			t1 = t2;
			t2 = t;
			double n = N1[axis];
			N1[axis] = N2[axis];
			N2[axis] = n;
		}

		if (t1 > Tnear) {
			Tnear = t1;
			Nnear = N1;
		}
		//the same for Tfar
		if (t2 < Tfar) {
			Tfar = t2;
			Nfar = N2;
		}
		//return false if the ray misses the box or is behind the source
		if (Tfar < Tnear || Tfar < RAY_EPSILON) {
			return false;
		}
	}
	i.setT(Tnear);
	i.setN(Nnear);
	return true;
	
}