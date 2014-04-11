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

void TraceUI::cb_disscaleSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_fDisScale=float( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_sampleSizeSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nSampleSize=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_threshSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_fThresh=float( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_spotpSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=((TraceUI *)(o->user_data()));
	pUI->m_nSpotP=int( ((Fl_Slider *)o)->value() ) ;
	pUI->raytracer->setSpotP(pUI->m_nSpotP);
}

void TraceUI::cb_cutoffSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=((TraceUI *)(o->user_data()));
	pUI->m_fCutoff=float( ((Fl_Slider *)o)->value() ) ;
	pUI->raytracer->setCutoff(pUI->m_fCutoff);
}

void TraceUI::cb_rayVisualCheck(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bRayVisual=bool( ((Fl_Check_Button *)o)->value() ) ;
}

void TraceUI::cb_BSPAccelCheck(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bBSPAccel=bool( ((Fl_Check_Button *)o)->value() ) ;
}

void TraceUI::cb_modeChoice(Fl_Widget* o, void* v)
{
	TraceUI* pUI=((TraceUI *)(o->user_data()));

	int type = (int)v;
	if(type != TRACE_NORMAL) {
		pUI->m_sampleSlider->activate();
	}
	else {
		pUI->m_sampleSlider->deactivate();
	}

	pUI->raytracer->setMode((enum TraceMode)type);
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

		pUI->raytracer->traceSetup(width, height, pUI->getDepth(), pUI->getDistScale(), pUI->getThresh());
		pUI->raytracer->setSampleSize(pUI->getSampleSize());
		pUI->raytracer->setDisp(pUI->getRayVisual());
		pUI->raytracer->setAccel(pUI->getBSPAccel());
		
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

float TraceUI::getDistScale()
{
	return m_fDisScale;
}

int TraceUI::getSampleSize()
{
	return m_nSampleSize;
}

bool TraceUI::getRayVisual()
{
	return m_bRayVisual;
}

bool TraceUI::getBSPAccel()
{
	return m_bBSPAccel;
}

float TraceUI::getThresh()
{
	return m_fThresh;
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

Fl_Menu_Item TraceUI::traceModeMenu[NUM_TRACE_MODE+1] = {
	{"Normal",						FL_ALT+'n', (Fl_Callback *)TraceUI::cb_modeChoice, (void *)TRACE_NORMAL},
	{"Antialias Supersampling",	    FL_ALT+'s', (Fl_Callback *)TraceUI::cb_modeChoice, (void *)TRACE_ANTIALIAS_NORMAL},
	{"Antialias Jittering",			FL_ALT+'j', (Fl_Callback *)TraceUI::cb_modeChoice, (void *)TRACE_JITTER},
	{"Antialias Adaptive",			FL_ALT+'a', (Fl_Callback *)TraceUI::cb_modeChoice, (void *)TRACE_ADAPTIVE_ANTIALIAS},
    {0}
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_nSampleSize = 1;
	m_fDisScale = 1.87;
	m_bRayVisual = false;
	m_bBSPAccel = true;
	m_nSpotP = 128;
	m_fCutoff = 0.2;
	m_fThresh = 0.00001;
	m_mainWindow = new Fl_Window(100, 40, 380, 290, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 380, 25);
		m_menubar->menu(menuitems);

		// install mode chooser
		m_modeChooser = new Fl_Choice(10, 30, 180, 20, "Trace Mode");
		m_modeChooser->user_data((void*)(this));
        m_modeChooser->labelfont(FL_COURIER);
		m_modeChooser->menu(traceModeMenu);
		m_modeChooser->align(FL_ALIGN_RIGHT);
		m_modeChooser->callback(cb_modeChoice);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 55, 180, 20, "Depth");
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
		m_sizeSlider = new Fl_Value_Slider(10, 80, 180, 20, "Size");
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

		// install slider distance scale
		m_disscaleSlider = new Fl_Value_Slider(10, 105, 180, 20, "Distance Scale (Log 10)");
		m_disscaleSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_disscaleSlider->type(FL_HOR_NICE_SLIDER);
        m_disscaleSlider->labelfont(FL_COURIER);
        m_disscaleSlider->labelsize(12);
		m_disscaleSlider->minimum(-0.99);
		m_disscaleSlider->maximum(3.00);
		m_disscaleSlider->step(0.01);
		m_disscaleSlider->value(m_fDisScale);
		m_disscaleSlider->align(FL_ALIGN_RIGHT);
		m_disscaleSlider->callback(cb_disscaleSlides);

		// install slider potlight p
		m_spotpSlider = new Fl_Value_Slider(10, 130, 180, 20, "spot light p");
		m_spotpSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_spotpSlider->type(FL_HOR_NICE_SLIDER);
        m_spotpSlider->labelfont(FL_COURIER);
        m_spotpSlider->labelsize(12);
		m_spotpSlider->minimum(16);
		m_spotpSlider->maximum(512);
		m_spotpSlider->step(10);
		m_spotpSlider->value(m_nSpotP);
		m_spotpSlider->align(FL_ALIGN_RIGHT);
		m_spotpSlider->callback(cb_spotpSlides);

		m_cutoffSlider = new Fl_Value_Slider(10, 155, 180, 20, "spot light cutoff");
		m_cutoffSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_cutoffSlider->type(FL_HOR_NICE_SLIDER);
        m_cutoffSlider->labelfont(FL_COURIER);
        m_cutoffSlider->labelsize(12);
		m_cutoffSlider->minimum(0.1);
		m_cutoffSlider->maximum(1.0);
		m_cutoffSlider->step(0.1);
		m_cutoffSlider->value(m_fCutoff);
		m_cutoffSlider->align(FL_ALIGN_RIGHT);
		m_cutoffSlider->callback(cb_cutoffSlides);
		
		// install slider cutoff size
		m_sampleSlider = new Fl_Value_Slider(10, 180, 180, 20, "Sample Size");
		m_sampleSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sampleSlider->type(FL_HOR_NICE_SLIDER);
        m_sampleSlider->labelfont(FL_COURIER);
        m_sampleSlider->labelsize(12);
		m_sampleSlider->minimum(1);
		m_sampleSlider->maximum(5);
		m_sampleSlider->step(1);
		m_sampleSlider->value(m_nSampleSize);
		m_sampleSlider->align(FL_ALIGN_RIGHT);
		m_sampleSlider->callback(cb_sampleSizeSlides);
		m_sampleSlider->deactivate();

		m_threshSlider = new Fl_Value_Slider(10, 205, 180, 20, "Threshold");
		m_threshSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_threshSlider->type(FL_HOR_NICE_SLIDER);
        m_threshSlider->labelfont(FL_COURIER);
        m_threshSlider->labelsize(12);
		m_threshSlider->minimum(0.00001);
		m_threshSlider->maximum(0.001);
		m_threshSlider->step(0.00001);
		m_threshSlider->value(m_fThresh);
		m_threshSlider->align(FL_ALIGN_RIGHT);
		m_threshSlider->callback(cb_threshSlides);

		// install ray visualize button
		m_rayVisualButton = new Fl_Check_Button(10, 230, 180, 20, "Ray Number Visualize");
		m_rayVisualButton->user_data((void*)(this));
		m_rayVisualButton->labelfont(FL_COURIER);
		m_rayVisualButton->value(0);
		m_rayVisualButton->callback(cb_rayVisualCheck);

		// install bsp accel button
		m_rayVisualButton = new Fl_Check_Button(10, 255, 180, 20, "BSP Acceleration");
		m_rayVisualButton->user_data((void*)(this));
		m_rayVisualButton->labelfont(FL_COURIER);
		m_rayVisualButton->value(1);
		m_rayVisualButton->callback(cb_BSPAccelCheck);

		m_renderButton = new Fl_Button(280, 52, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(280, 80, 70, 25, "&Stop");
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