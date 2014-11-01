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
		result[0] += ka[0] * ambient_light->getColor(isectCoord)[0];
		result[1] += ka[1] * ambient_light->getColor(isectCoord)[1];
		result[2] += ka[2] * ambient_light->getColor(isectCoord)[2];
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

		// diffuse reflection angle
		double diffuseReflectionAngle = direction.dot(normal);
		//add difuse light
		result[0] += intensity[0] * kd[0] * diffuseReflectionAngle * distanceAttenuation;
		result[1] += intensity[1] * kd[1] * diffuseReflectionAngle * distanceAttenuation;
		result[2] += intensity[2] * kd[2] * diffuseReflectionAngle * distanceAttenuation;

		//specular reflection angle
	
	}
	return result;
}
