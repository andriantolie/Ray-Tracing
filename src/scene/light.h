#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;
protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light( scene, color ), position( pos ) {
		constantAttenuationCoeff = traceUI->getConstantAttenuationCoeff();
		linearAttenuationCoeff = traceUI->getLinearAttenuationCoeff();
		quadraticAttenuationCoeff = traceUI->getQuadraticAttenuationCoeff();
	}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
	void setConstantAttenuationCoeff(const double coeff){ constantAttenuationCoeff = coeff * traceUI->getConstantAttenuationCoeff(); }
	void setLinearAttenuationCoeff(const double coeff){ linearAttenuationCoeff = coeff * traceUI->getLinearAttenuationCoeff(); }
	void setQuadraticAttenuationCoeff(const double coeff){ quadraticAttenuationCoeff = coeff * traceUI->getQuadraticAttenuationCoeff(); }
	double getConstantAttenuationCoeff() { return constantAttenuationCoeff; }
	double getLinearAttenuationCoeff() { return linearAttenuationCoeff; }
	double getQuadraticAttenuationCoeff() { return quadraticAttenuationCoeff; }

protected:
	vec3f position;
	double constantAttenuationCoeff;
	double linearAttenuationCoeff;
	double quadraticAttenuationCoeff;
};

class AmbientLight
	: public Light
{
public:
	AmbientLight(Scene *scene, const vec3f& color)
		: Light(scene, color) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

};
#endif // __LIGHT_H__
