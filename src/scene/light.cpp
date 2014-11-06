#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	// ---Implemented---
	vec3f direction = getDirection(P).normalize();
	ray shadowRay = ray(P, direction);
	isect i;
	vec3f attenuation = vec3f(1.0, 1.0, 1.0);
	bool intersectOtherObject = scene->intersect(shadowRay, i);
	while (intersectOtherObject) {
		//handles transparency
		vec3f transparencyIndex = i.getMaterial().kt;
		if (transparencyIndex == vec3f(0.0, 0.0, 0.0)){
			return vec3f(0, 0, 0);
		}
		else{
			attenuation[0] *= transparencyIndex[0];
			attenuation[1] *= transparencyIndex[1];
			attenuation[2] *= transparencyIndex[2];
		}
		shadowRay = ray(shadowRay.at(i.t), direction);
		intersectOtherObject = scene->intersect(shadowRay, i);
	}
	
		return attenuation;
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
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double distance = (P - position).length();
	double attenuation = 1.0 / (constantAttenuationCoeff + linearAttenuationCoeff*distance + quadraticAttenuationCoeff*distance*distance);
	if (attenuation > 1.0) attenuation = 1.0;
	return attenuation;
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
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	// ---Implemented---
	vec3f direction = getDirection(P).normalize();
	ray shadowRay = ray(P, direction);
	isect i;
	vec3f attenuation = vec3f(1.0, 1.0, 1.0);
	bool intersectOtherObject = scene->intersect(shadowRay, i);
	while (intersectOtherObject) {
		//handles transparency
		vec3f transparencyIndex = i.getMaterial().kt;
		if (transparencyIndex == vec3f(0.0, 0.0, 0.0)){
			return vec3f(0, 0, 0);
		}
		else{
			attenuation[0] *= transparencyIndex[0];
			attenuation[1] *= transparencyIndex[1];
			attenuation[2] *= transparencyIndex[2];
		}
		shadowRay = ray(shadowRay.at(i.t), direction);
		intersectOtherObject = scene->intersect(shadowRay, i);
	}

	return attenuation;
}

//----- Ambient light------
double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	// no attenuation here
	return 1.0;
}

vec3f AmbientLight::getColor(const vec3f& P) const
{
	return color;
}

vec3f AmbientLight::getDirection(const vec3f& P) const
{
	// ambient light illuminates surrounding, return 0
	return vec3f(0.0,0.0,0.0);
}


vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	// ambient light illuminates surrounding, return 1
	return vec3f(1, 1, 1);
}
