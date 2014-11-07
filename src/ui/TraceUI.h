//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_constantAttenuationSlider;
	Fl_Slider*			m_linearAttenuationSlider;
	Fl_Slider*			m_quadraticAttenuationSlider;

	Fl_Slider*			m_thresholdSlider;
	Fl_Slider*			m_subpixelSlider;
	Fl_Slider*			m_ambientLightSlider;


	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	double		getConstantAttenuationCoeff();
	double		getLinearAttenuationCoeff();
	double		getQuadraticAttenuationCoeff();
	double		getThreshold();
	double		getAmbientLightValue();

	void		setConstantAttenuationCoeff(double coeff);
	void		setLinearAttenuationCoeff(double coeff);
	void		setQuadraticAttenuationCoeff(double coeff);
	void		setAmbientLightValue(double intensity);

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double		m_nConstantAttenuationCoeff;
	double		m_nLinearAttenuationCoeff;
	double		m_nQuadraticAttenuationCoeff;

	double		m_threshold;
	int			m_subpixel;

	double		m_nAmbientLight;


// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_constantAttenuationSlides(Fl_Widget* o, void* v);
	static void cb_linearAttenuationSlides(Fl_Widget* o, void* v);
	static void cb_quadraticAttenuationSlides(Fl_Widget* o, void* v);
	static void cb_thresholdSlides(Fl_Widget* o, void* v);
	static void cb_subpixelSliders(Fl_Widget* o, void* v);
	static void cb_ambientLightSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
