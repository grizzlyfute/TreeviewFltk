#ifndef OPTIONS_H
#define OPTIONS_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Check_Button.H>

class OptionsForm : public Fl_Double_Window
{
  public:
    OptionsForm(int w, int h, const char *t);
    virtual ~OptionsForm();

  private:
	Fl_Tabs *tabs;
	Fl_Return_Button *btn_ok;
	Fl_Button *btn_cancel;

	Fl_Group *tab1;
	Fl_Scroll *scrollArea1;
	Fl_Pack *layout_vertical1;
	Fl_Input *inTxt_filterFile;
	Fl_Input *inTxt_filterDir;
	Fl_Input *inTxt_directory;

	Fl_Group *tab2;
	Fl_Scroll *scrollArea2;
	Fl_Pack *layout_vertical2;
	Fl_Choice *inChoice_fileEditors;
	Fl_Input *inTxt_editor[5];
	Fl_Input *inTxt_editorConfig[5];
	Fl_Choice *inChoice_directoryTool;
	Fl_Input *inTxt_dirTool[3];

	Fl_Group *tab3;
	Fl_Scroll *scrollArea3;
	Fl_Pack *layout_vertical3;
	Fl_Check_Button *btnReload;
	Fl_Check_Button *btnRunDir[3];
	Fl_Check_Button *btnConfig;
	Fl_Check_Button *btnAbout;

	/* callbacks */
	static void cb_ok(Fl_Widget *, void *);
	static void cb_cancel(Fl_Widget *, void *);
	static void cb_dirTool(Fl_Widget *, void *);
	static void cb_fileEditor(Fl_Widget *, void *);
};

#endif /* OPTIONS_H */
