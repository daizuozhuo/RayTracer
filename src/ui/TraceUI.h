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
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Choice*			m_modeChooser;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_disscaleSlider;
	Fl_Slider*			m_spotpSlider;
	Fl_Slider*			m_cutoffSlider;
	Fl_Slider*			m_sampleSlider;
	Fl_Slider*			m_threshSlider;

	Fl_Check_Button*	m_rayVisualButton;
	Fl_Check_Button*	m_bspAccelButton;
	Fl_Check_Button*	m_useBGButton;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	float		getDistScale();
	float		getThresh();
	int			getSampleSize();
	bool		getRayVisual();
	bool		getBSPAccel();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	int			m_nSampleSize;
	int			m_nSpotP;
	float		m_fDisScale;
	float		m_fCutoff;
	float		m_fThresh;
	bool		m_bRayVisual;
	bool		m_bBSPAccel;

// static class members
	static Fl_Menu_Item menuitems[];
	static Fl_Menu_Item traceModeMenu[NUM_TRACE_MODE+1];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_load_bg(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_modeChoice(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_disscaleSlides(Fl_Widget* o, void* v);
	static void cb_spotpSlides(Fl_Widget* o, void* v);
	static void cb_cutoffSlides(Fl_Widget* o, void* v);
	static void cb_sampleSizeSlides(Fl_Widget* o, void* v);
	static void cb_threshSlides(Fl_Widget* o, void *v);

	static void cb_rayVisualCheck(Fl_Widget* o, void* v);
	static void cb_BSPAccelCheck(Fl_Widget* o, void* v);
	static void cb_useBGCheck(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
