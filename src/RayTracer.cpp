// The main ray tracer.
//Author:	LIE, Andrianto (alie@connect.ust.hk) - 20020408
//			ZENG, Xiong (zxengac@connect.ust.hk) - 20025056

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
	productKr = 1.0; productKt = 1.0;
	double thresh = traceUI->getThreshold();
	return traceRay( scene, r, vec3f(thresh, thresh, thresh), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	// terminate after certain depth
	if (depth > traceUI->getDepth()){
		return vec3f(0.0, 0.0, 0.0);
	}
	
	isect i;
	if (scene->intersect(r, i)) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.


		const Material& m = i.getMaterial();
		
		productKr = m.kr[0] ? productKr*m.kr[0]:productKr; //global variable for storing product of coefficients
		productKt = m.kt[0] ? productKt*m.kt[0]:productKt;


		vec3f intensity = vec3f(0.0, 0.0, 0.0);
		vec3f normal = i.N;
		vec3f rayDirection = r.getDirection();
		// dot product between ray and normal is positive when a ray is entering object
		bool enteringObject = (normal.dot((-1)*rayDirection)) > RAY_EPSILON;
		// if it is light going out from the object, reverse the normal
		if (!enteringObject) normal = i.N *= -1;

		// add shadow ray into account
		intensity += m.shade(scene, r, i);
		// reflection is computed using (2*(N.(-d))*N) - (-d)
		vec3f reflectedDirection = (2 * normal * normal.dot((-1)*rayDirection)) - (-1)*rayDirection;
		ray reflectionRay = ray(r.at(i.t), reflectedDirection.normalize());
		// add reflection ray into account
		if (productKr > thresh[0]) {
			vec3f reflectionIntensity = traceRay(scene, reflectionRay, thresh, depth + 1);
			intensity[0] += m.kr[0] * reflectionIntensity[0];
			intensity[1] += m.kr[1] * reflectionIntensity[1];
			intensity[2] += m.kr[2] * reflectionIntensity[2];
		}

		if (productKt > thresh[0]) {
			// check if the object is transparent
			if (m.kt != vec3f(0.0, 0.0, 0.0)){
				// dot product between ray and normal is positive when a ray is entering object
				bool enteringObject = (normal.dot((-1)*rayDirection)) >= 0.0;
				double n_i; // incoming refraction index
				double n_t; // outgoing refraction index

				if (enteringObject){
					n_i = 1.000293; // air refractive index
					n_t = m.index;
				}
				else{
					n_i = m.index;
					n_t = 1.000293; // air refractive index
					normal = (-1)* normal; // reverse the normal if it is going out from the object
				}
				// check total internal refraction
				bool tir = false;
				double n_r = n_i / n_t;
				double normalDotIncomingLight = normal.dot((-1)*rayDirection);
				double innerSquareRoot = 1 - n_r*n_r*(1 - normalDotIncomingLight*normalDotIncomingLight);
				if (innerSquareRoot <= 0.0) tir = true;

				// add refraction ray into account
				if (!tir){
					// use formula 16.33
					vec3f refractedDirection = (n_r*normalDotIncomingLight - sqrt(1 - n_r*n_r*(1 - normalDotIncomingLight*normalDotIncomingLight)))*normal - n_r*(-1)*rayDirection;
					ray refractionRay = ray(r.at(i.t), refractedDirection.normalize());
					vec3f refractionIntensity = traceRay(scene, refractionRay, thresh, depth + 1);
					intensity[0] += m.kt[0] * refractionIntensity[0];
					intensity[1] += m.kt[1] * refractionIntensity[1];
					intensity[2] += m.kt[2] * refractionIntensity[2];
				}
			}
		}

		return intensity;
	
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
	
	m_bSceneLoaded = true;

	return true;
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
	
	for (int j = start; j < stop; ++j)
		for (int i = 0; i < buffer_width; ++i)
			tracePixel(i, j);

}

void RayTracer::tracePixel( int i, int j )
{
	int subpixel = traceUI->getAA();
	if (subpixel != 1) {
		tracePixelAA(i, j, subpixel);
		return;
	}

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

void RayTracer::tracePixelAA(int i, int j, int n) {
	vec3f col;
	double coef = 1.0 / (n*n);
	unsigned char *pixel = buffer + (i + j * buffer_width) * 3;
	double pixelAvg[3] = { 0.0, 0.0, 0.0 };

	if (!scene) {
		return;
	}

	for (double fragmentx = i; fragmentx < i + 1.0f - RAY_EPSILON; fragmentx += 1.0f / n) {
		for (double fragmenty = j; fragmenty < j + 1.0f - RAY_EPSILON; fragmenty += 1.0f / n) {
			double x = double(fragmentx) / double(buffer_width);
			double y = double(fragmenty) / double(buffer_height);
			col = trace(scene, x, y);

			pixelAvg[0] += coef * (255.0 * col[0]);
			pixelAvg[1] += coef * (255.0 * col[1]);
			pixelAvg[2] += coef * (255.0 * col[2]);
		}
	}
	
	pixel[0] = (int) pixelAvg[0];
	pixel[1] = (int) pixelAvg[1];
	pixel[2] = (int) pixelAvg[2];
	  
}