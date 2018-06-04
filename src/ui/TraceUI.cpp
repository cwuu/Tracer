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
void TraceUI::cb_load_background_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);
	char* newfile = fl_file_chooser("Load Background?", "*.bmp", NULL);
		if (newfile != NULL) {
			//char buf[256];
			//if (pUI->raytracer->loadBackground(newfile)) {
			//	sprintf(buf, "Ray <%s>", newfile);
			//	done = true;	// terminate the previous rendering
			pUI->raytracer->loadBackground(newfile);
		}
			//else {
			//	sprintf(buf, "Ray <Not Loaded>");
			//}
			//pUI->m_mainWindow->label(buf);
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
// call back functions for attenuation 
void TraceUI::cb_attenuationConstantSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAttenuationConstant = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_attenuationLinearSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAttenuationLinear = double(((Fl_Slider *)o)->value());
}
void TraceUI::cb_attenuationQuadraticSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAttenuationQuadratic = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_antialising(Fl_Widget* o, void* v)
{
	//TODO: add code here
	((TraceUI*)(o->user_data()))->m_nAntiAlising = int(((Fl_Slider *)o)->value());

}
void TraceUI::cb_ambientSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAmbientLight = double(((Fl_Slider *)o)->value());
}
void TraceUI::cb_fresnelSwitch(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bIsEnableFresnel ^= true;
}
void TraceUI::cb_motionBlur(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bIsMotionBlur ^= true;
}

void TraceUI::cb_adaptiveTerminationSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAdaptiveTermination = double(((Fl_Slider *)o)->value());
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
void TraceUI::cb_glossyreflection(Fl_Widget* o, void* v)
{
	//TODO: add code here
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	pUI->raytracer->isGlossyReflection = true ;
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

double TraceUI::getAttenuationConstant()
{
	return m_nAttenuationConstant;
}

double TraceUI::getAttenuationLinear()
{
	return m_nAttenuationLinear;
}
double TraceUI::getAttenuationQuadratic()
{
	return m_nAttenuationQuadratic;
}
double TraceUI::getAmbientLight()
{
	return m_nAmbientLight;
}
int TraceUI::getAntiAlising() {
	return m_nAntiAlising;
}
double TraceUI::getAdaptiveTermination() {
	return m_nAdaptiveTermination;
}

bool TraceUI::IsEnableFresnel()
{
	return m_bIsEnableFresnel;
}
void TraceUI::cb_load_texture_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);
	char* newfile = fl_file_chooser("Load Texture Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		pUI->raytracer->loadtextureMappingImage(newfile);
	}
}
bool TraceUI::isEnableTextureMapping()
{
	return m_bIsEnableTextureMapping;
}
void TraceUI::cb_textureMappingSwitch(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bIsEnableTextureMapping ^= true;
}

bool TraceUI::isEnableBumpMapping()
{
	return m_bIsEnableBumpMapping;
}
bool TraceUI::isEnableMotionBlur()
{
	return m_bIsMotionBlur;
}

void TraceUI::cb_bumpMappingSwitch(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bIsEnableBumpMapping ^= true;
}
void TraceUI::cb_jittering(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bIsEnableJittering ^= true;
}

void TraceUI::cb_softShadowing(Fl_Widget* o, void* v) {
	((TraceUI*)(o->user_data()))->m_bIsEnableSoftShadowing ^= true;
}
// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Load Background Image", FL_ALT + 'b', (Fl_Callback *)TraceUI::cb_load_background_image },
		{ "&Load Texture Image",	FL_ALT + 't', (Fl_Callback *)TraceUI::cb_load_texture_image },
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
	m_nAttenuationConstant = 0.5;
	m_nAttenuationLinear = 0;
	m_nAttenuationQuadratic = 0;
	m_nAmbientLight = 0.5;
	m_nAntiAlising = 0;
	m_bIsEnableFresnel = false;
	m_bIsMotionBlur = false;
	m_nAdaptiveTermination = 1.0;
	m_mainWindow = new Fl_Window(100, 40, 350, 350, "Ray <Not Loaded>"); // last parameter for height
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 350, 25);
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
		m_bIsEnableTextureMapping = false;
		m_bIsEnableBumpMapping = false;
		m_bIsEnableJittering = false;
		m_bIsEnableSoftShadowing = false; 
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

		//sliders for atttentuation: Linear, Quadratics, Constant
		m_attenConstantSlider = new Fl_Value_Slider(10, 80, 180, 20, "Attenuation, Constant");
		m_attenConstantSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenConstantSlider->type(FL_HOR_NICE_SLIDER);
		m_attenConstantSlider->labelfont(FL_COURIER);
		m_attenConstantSlider->labelsize(12);
		m_attenConstantSlider->minimum(0.00);
		m_attenConstantSlider->maximum(1.00);
		m_attenConstantSlider->step(0.01);
		m_attenConstantSlider->value(m_nAttenuationConstant);
		m_attenConstantSlider->align(FL_ALIGN_RIGHT);
		m_attenConstantSlider->callback(cb_attenuationConstantSlides);


		m_adaptiveTerminationSlider = new Fl_Value_Slider(10, 230, 180, 20, "Adaptive Termination");
		m_adaptiveTerminationSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_adaptiveTerminationSlider->type(FL_HOR_NICE_SLIDER);
		m_adaptiveTerminationSlider->labelfont(FL_COURIER);
		m_adaptiveTerminationSlider->labelsize(12);
		m_adaptiveTerminationSlider->minimum(0.0);
		m_adaptiveTerminationSlider->maximum(1.00);
		m_adaptiveTerminationSlider->step(0.01);
		m_adaptiveTerminationSlider->value(m_nAdaptiveTermination);
		m_adaptiveTerminationSlider->align(FL_ALIGN_RIGHT);
		m_adaptiveTerminationSlider->callback(cb_adaptiveTerminationSlides);

		m_attenLinearSlider = new Fl_Value_Slider(10, 105, 180, 20, "Attenuation, Linear");
		m_attenLinearSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenLinearSlider->type(FL_HOR_NICE_SLIDER);
		m_attenLinearSlider->labelfont(FL_COURIER);
		m_attenLinearSlider->labelsize(12);
		m_attenLinearSlider->minimum(0.00);
		m_attenLinearSlider->maximum(1.00);
		m_attenLinearSlider->step(0.01);
		m_attenLinearSlider->value(m_nAttenuationLinear);
		m_attenLinearSlider->align(FL_ALIGN_RIGHT);
		m_attenLinearSlider->callback(cb_attenuationLinearSlides);

		m_attenQuadraticSlider = new Fl_Value_Slider(10, 130, 180, 20, "Attenuation, Quadratic");
		m_attenQuadraticSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenQuadraticSlider->type(FL_HOR_NICE_SLIDER);
		m_attenQuadraticSlider->labelfont(FL_COURIER);
		m_attenQuadraticSlider->labelsize(12);
		m_attenQuadraticSlider->minimum(0.00);
		m_attenQuadraticSlider->maximum(1.00);
		m_attenQuadraticSlider->step(0.01);
		m_attenQuadraticSlider->value(m_nAttenuationQuadratic);
		m_attenQuadraticSlider->align(FL_ALIGN_RIGHT);
		m_attenQuadraticSlider->callback(cb_attenuationQuadraticSlides);

		m_ambientSlider = new Fl_Value_Slider(10, 155, 180, 20, "Ambient Light");
		m_ambientSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientSlider->labelfont(FL_COURIER);
		m_ambientSlider->labelsize(12);
		m_ambientSlider->minimum(0.00);
		m_ambientSlider->maximum(1.00);
		m_ambientSlider->step(0.01);
		m_ambientSlider->value(m_nAmbientLight);
		m_ambientSlider->align(FL_ALIGN_RIGHT);
		m_ambientSlider->callback(cb_ambientSlides );


		m_antiAlisingSlider = new Fl_Value_Slider(10, 180, 180, 20, "Anti-Alising");
		m_antiAlisingSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_antiAlisingSlider->type(FL_HOR_NICE_SLIDER);
		m_antiAlisingSlider->labelfont(FL_COURIER);
		m_antiAlisingSlider->labelsize(12);
		m_antiAlisingSlider->minimum(0);
		m_antiAlisingSlider->maximum(4);
		m_antiAlisingSlider->step(1);
		m_antiAlisingSlider->value(m_nAntiAlising);
		m_antiAlisingSlider->align(FL_ALIGN_RIGHT);
		m_antiAlisingSlider->callback(cb_antialising);




		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);
		/*m_antialising = new Fl_Button(100, 180, 70, 20, "&Anti-alising");
		m_antialising->user_data((void*)(this));
		m_antialising->callback(cb_antialising);
		*/
		m_glossyreflection = new Fl_Button(10, 205, 70, 20, "&Glossy");
		m_glossyreflection->user_data((void*)(this));
		m_glossyreflection->callback(cb_glossyreflection);

		m_textureMappingSwitch = new Fl_Light_Button(10, 280, 70, 25, "Texture");
		m_textureMappingSwitch->user_data((void*)(this));
		m_textureMappingSwitch->value(0);
		m_textureMappingSwitch->callback(cb_textureMappingSwitch);

		m_bumpMappingSwitch = new Fl_Light_Button(150, 280, 70, 25, "Bump");
		m_bumpMappingSwitch->user_data((void*)(this));
		m_bumpMappingSwitch->value(0);
		m_bumpMappingSwitch->callback(cb_textureMappingSwitch);

		m_fresnelSwitch = new Fl_Light_Button(80, 280, 70, 25, "Fresnel");
		m_fresnelSwitch->user_data((void*)(this));
		m_fresnelSwitch->value(0);
		m_fresnelSwitch->callback(cb_fresnelSwitch);

		m_motionBlurSwitch = new Fl_Light_Button(220, 280, 70, 25, "MotionBlur");
		m_motionBlurSwitch->user_data((void*)(this));
		m_motionBlurSwitch->value(0);
		m_motionBlurSwitch->callback(cb_motionBlur);


		// add jitter 
		m_jitterSwitch = new Fl_Light_Button(90, 205, 70, 20, "&Jittering");
		m_jitterSwitch->user_data((void*)(this));
		m_jitterSwitch->value(0);
		m_jitterSwitch->callback(cb_jittering);

		m_softShadowSwitch = new Fl_Light_Button(170, 205, 70, 20, "&Soft shadow");
		m_softShadowSwitch->user_data((void*)(this));
		m_softShadowSwitch->value(0);
		m_softShadowSwitch->callback(cb_softShadowing);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
		m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}
