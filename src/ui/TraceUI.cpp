//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_constantAttenuationSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nConstantAttenuationCoeff = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_linearAttenuationSlides(Fl_Widget* o, void* v)
{

	((TraceUI*)(o->user_data()))->m_nLinearAttenuationCoeff = double (((Fl_Slider *)o)->value());

}

void TraceUI::cb_quadraticAttenuationSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nQuadraticAttenuationCoeff = double (((Fl_Slider *)o)->value());
}

void TraceUI::cb_thresholdSlides(Fl_Widget* o, void* v) {
	((TraceUI*)(o->user_data()))->m_threshold = double (((Fl_Slider *)o)->value());
}

void TraceUI::cb_subpixelSliders(Fl_Widget* o, void* v) {
	((TraceUI*)(o->user_data()))->m_subpixel = int(((Fl_Slider *)o)->value());
}

void TraceUI::cb_ambientLightSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAmbientLight = double(((Fl_Slider *)o)->value());

}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

double TraceUI::getConstantAttenuationCoeff()
{
	return m_nConstantAttenuationCoeff;
}

double TraceUI::getLinearAttenuationCoeff()
{
	return m_nLinearAttenuationCoeff;
}

double TraceUI::getQuadraticAttenuationCoeff()
{
	return m_nQuadraticAttenuationCoeff;
}


double TraceUI::getThreshold() {
	return m_threshold;
}

double TraceUI::getAmbientLightValue()
{
	return m_nAmbientLight;
}

int TraceUI::getAA() {
	return m_subpixel;
}


void TraceUI::setConstantAttenuationCoeff(double coeff){
	m_nConstantAttenuationCoeff = coeff;
}

void TraceUI::setLinearAttenuationCoeff(double coeff){
	m_nLinearAttenuationCoeff = coeff;
}

void TraceUI::setQuadraticAttenuationCoeff(double coeff){
	m_nQuadraticAttenuationCoeff = coeff;
}

void TraceUI::setAmbientLightValue(double intensity){
	m_nAmbientLight = intensity;
}
// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_nConstantAttenuationCoeff = 1.0;
	m_nLinearAttenuationCoeff = 0.0;
	m_nQuadraticAttenuationCoeff = 0.0;

	m_threshold = 0.0;
	m_subpixel = 1;
	m_nAmbientLight = 0.0;
	m_mainWindow = new Fl_Window(100, 40, 400, 250, "Ray <Not Loaded>");

	

		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 400, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		// install constant attenuation size
		m_constantAttenuationSlider = new Fl_Value_Slider(10, 75, 180, 20, "Attenuation, Constant");
		m_constantAttenuationSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_constantAttenuationSlider->type(FL_HOR_NICE_SLIDER);
		m_constantAttenuationSlider->labelfont(FL_COURIER);
		m_constantAttenuationSlider->labelsize(12);
		m_constantAttenuationSlider->minimum(0.00);
		m_constantAttenuationSlider->maximum(1.00);
		m_constantAttenuationSlider->step(0.01);
		m_constantAttenuationSlider->align(FL_ALIGN_RIGHT);
		m_constantAttenuationSlider->callback(cb_constantAttenuationSlides);

		// install linear attenuation size
		m_linearAttenuationSlider = new Fl_Value_Slider(10, 95, 180, 20, "Attenuation, Linear");
		m_linearAttenuationSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_linearAttenuationSlider->type(FL_HOR_NICE_SLIDER);
		m_linearAttenuationSlider->labelfont(FL_COURIER);
		m_linearAttenuationSlider->labelsize(12);
		m_linearAttenuationSlider->minimum(0.00);
		m_linearAttenuationSlider->maximum(1.00);
		m_linearAttenuationSlider->step(0.01);
		m_linearAttenuationSlider->align(FL_ALIGN_RIGHT);
		m_linearAttenuationSlider->callback(cb_linearAttenuationSlides);

		// install quadratic attenuation size
		m_quadraticAttenuationSlider = new Fl_Value_Slider(10, 115, 180, 20, "Attenuation, Quadratic");
		m_quadraticAttenuationSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_quadraticAttenuationSlider->type(FL_HOR_NICE_SLIDER);
		m_quadraticAttenuationSlider->labelfont(FL_COURIER);
		m_quadraticAttenuationSlider->labelsize(12);
		m_quadraticAttenuationSlider->minimum(0.00);
		m_quadraticAttenuationSlider->maximum(1.00);
		m_quadraticAttenuationSlider->step(0.01);
		m_quadraticAttenuationSlider->align(FL_ALIGN_RIGHT);
		m_quadraticAttenuationSlider->callback(cb_quadraticAttenuationSlides);

		// install threshold slider
		m_thresholdSlider = new Fl_Value_Slider(10, 135, 180, 20, "Threshold");
		m_thresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_thresholdSlider->type(FL_HOR_NICE_SLIDER);
		m_thresholdSlider->labelfont(FL_COURIER);
		m_thresholdSlider->labelsize(12);
		m_thresholdSlider->minimum(0.00);
		m_thresholdSlider->maximum(1.00);
		m_thresholdSlider->step(0.01);
		m_thresholdSlider->value(m_threshold);
		m_thresholdSlider->align(FL_ALIGN_RIGHT);
		m_thresholdSlider->callback(cb_thresholdSlides);

		// install subpixel slider
		m_aaSlider = new Fl_Value_Slider(10, 155, 180, 20, "No. of Subpixel");
		m_aaSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_aaSlider->type(FL_HOR_NICE_SLIDER);
		m_aaSlider->labelfont(FL_COURIER);
		m_aaSlider->labelsize(12);
		m_aaSlider->minimum(1);
		m_aaSlider->maximum(5);
		m_aaSlider->step(1);
		m_aaSlider->value(m_subpixel);
		m_aaSlider->align(FL_ALIGN_RIGHT);
		m_aaSlider->callback(cb_subpixelSliders);

		// install ambient light slider
		m_ambientLightSlider = new Fl_Value_Slider(10, 175, 180, 20, "Ambient Light");
		m_ambientLightSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightSlider->value(m_nAmbientLight);
		m_ambientLightSlider->labelfont(FL_COURIER);
		m_ambientLightSlider->labelsize(12);
		m_ambientLightSlider->minimum(0.00);
		m_ambientLightSlider->maximum(1.00);
		m_ambientLightSlider->step(0.01);
		m_ambientLightSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightSlider->callback(cb_ambientLightSlides);

		m_ambientLightSlider = new Fl_Value_Slider(10, 175, 180, 20, "Ambient Light");
		m_ambientLightSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightSlider->value(m_nAmbientLight);
		m_ambientLightSlider->labelfont(FL_COURIER);
		m_ambientLightSlider->labelsize(12);
		m_ambientLightSlider->minimum(0.00);
		m_ambientLightSlider->maximum(1.00);
		m_ambientLightSlider->step(0.01);
		m_ambientLightSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightSlider->callback(cb_ambientLightSlides);

		m_renderButton = new Fl_Button(280, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(280, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}