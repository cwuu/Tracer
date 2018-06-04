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
	// sliders for attenutation
	Fl_Slider*			m_attenConstantSlider;
	Fl_Slider*			m_attenQuadraticSlider;
	Fl_Slider*			m_attenLinearSlider;
	Fl_Slider*			m_adaptiveTerminationSlider;

	// slider for ambient light
	Fl_Slider*			m_ambientSlider;
	Fl_Slider*			m_antiAlisingSlider;
	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;


	Fl_Button*			m_glossyreflection;
	Fl_Button*			m_antialising;

	Fl_Light_Button* 	m_textureMappingSwitch;
	Fl_Light_Button* 	m_bumpMappingSwitch;
	Fl_Light_Button*	m_jitterSwitch;
	Fl_Light_Button*	m_softShadowSwitch;
	Fl_Light_Button*	m_motionBlurSwitch;
	Fl_Light_Button*	m_fresnelSwitch;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	double		getAttenuationConstant();
	double		getAttenuationLinear();
	double		getAttenuationQuadratic();
	double		getAmbientLight();
	double		getAdaptiveTermination();
	int			getAntiAlising();
	bool		IsEnableFresnel();
	bool 		isEnableTextureMapping();
	bool		isEnableJittering() { return m_bIsEnableJittering; };
	bool		isEnableSoftShadowing() { return m_bIsEnableSoftShadowing;  }
	bool 		isEnableBumpMapping();
	bool		isEnableMotionBlur();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double		m_nAttenuationConstant;
	double		m_nAttenuationLinear;
	double		m_nAttenuationQuadratic;
	double		m_nAmbientLight;
	double		m_nAdaptiveTermination;
	int			m_nAntiAlising;
	bool		m_bIsEnableFresnel;
	bool 		m_bIsEnableTextureMapping;
	bool 		m_bIsEnableBumpMapping;
	bool		m_bIsEnableJittering;
	bool		m_bIsEnableSoftShadowing; 
	bool		m_bIsMotionBlur;
// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_load_background_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_attenuationConstantSlides(Fl_Widget* o, void* v);
	static void cb_attenuationLinearSlides(Fl_Widget* o, void* v);
	static void cb_attenuationQuadraticSlides(Fl_Widget* o, void* v);
	static void cb_ambientSlides(Fl_Widget* o, void* v);
	static void cb_adaptiveTerminationSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
	static void cb_glossyreflection(Fl_Widget* o, void* v);
	static void cb_antialising(Fl_Widget* o, void* v);
	static void cb_jittering(Fl_Widget* o, void* v);
	static void cb_softShadowing(Fl_Widget* o, void* v);

	static void cb_fresnelSwitch(Fl_Widget* o, void* v);
	static void cb_motionBlur(Fl_Widget* o, void* v);
	static void cb_load_texture_image(Fl_Menu_* o, void* v);
	static void cb_textureMappingSwitch(Fl_Widget* o, void* v);
	static void cb_bumpMappingSwitch(Fl_Widget* o, void* v);
};

#endif
