#include "ray.h"
#include "material.h"
#include "light.h"

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
	vec3f result = vec3f(0.0, 0.0, 0.0);
	vec3f isectCoord = r.at(i.t);


	//add emissive light
	result += ke;

	//add ambient light
	Light* ambient_light = scene->getAmbient();
	if (ambient_light != NULL){
		result[0] += (1 - kt[0]) * ka[0] * ambient_light->getColor(isectCoord)[0];
		result[1] += (1 - kt[1]) * ka[1] * ambient_light->getColor(isectCoord)[1];
		result[2] += (1 - kt[2]) * ka[2] * ambient_light->getColor(isectCoord)[2];
	}

	list<Light*>::const_iterator light_start = scene->beginLights();
	list<Light*>::const_iterator light_end = scene->endLights();
	list<Light*>::const_iterator light_iterator;
	for (light_iterator = light_start; light_iterator != light_end; light_iterator++){
		// Light intensity
		vec3f intensity = (*light_iterator)->getColor(isectCoord);
		// Light direction
		vec3f direction = (*light_iterator)->getDirection(isectCoord);
        // Intersection normal
		vec3f normal = i.N;
		// distance attenuation
		double distanceAttenuation = (*light_iterator)->distanceAttenuation(isectCoord);
		// shadow attenuation
		vec3f shadowAttenuation = (*light_iterator)->shadowAttenuation(isectCoord);

		// diffuse reflection angle
		double diffuseReflectionAngle = normal.dot(direction);
		if (diffuseReflectionAngle < NORMAL_EPSILON) diffuseReflectionAngle = 0.0;
		//add difuse light
		result[0] += (1 - kt[0]) * intensity[0] * kd[0] * diffuseReflectionAngle * shadowAttenuation[0] * distanceAttenuation;
		result[1] += (1 - kt[1]) * intensity[1] * kd[1] * diffuseReflectionAngle * shadowAttenuation[1] * distanceAttenuation;
		result[2] += (1 - kt[2]) * intensity[2] * kd[2] * diffuseReflectionAngle * shadowAttenuation[2] * distanceAttenuation;

		// specular reflection angle
		// based on equation 16.17 R.V = (2N(N.L) - L).V
		double specularReflectionAngle = ((2 * normal * normal.dot(direction)) - direction).dot(((-1)*r.getDirection()).normalize());
		if (specularReflectionAngle < NORMAL_EPSILON) specularReflectionAngle = 0.0;
		double specularReflectionAnglePowered = pow(specularReflectionAngle, shininess * 128);
		// add specular reflection
		result[0] += intensity[0] * ks[0] * specularReflectionAnglePowered * shadowAttenuation[0] * distanceAttenuation;
		result[1] += intensity[1] * ks[1] * specularReflectionAnglePowered * shadowAttenuation[1] * distanceAttenuation;
		result[2] += intensity[2] * ks[2] * specularReflectionAnglePowered * shadowAttenuation[2] * distanceAttenuation;
	
	}
	return result;
}
