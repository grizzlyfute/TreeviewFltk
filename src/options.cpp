#include "utils.h"
#include "options.h"
#include "global_configuration.h"
#include <FL/Enumerations.H>
#include <FL/Fl.H>

#define X_MARGIN 5
#define Y_MARGIN 3

OptionsForm::OptionsForm(int w, int h, const char *t) : Fl_Double_Window (w, h, t)
{
	Config::global_configuration.load();
	const int scz = Fl::scrollbar_size();

	begin();

	tabs = new Fl_Tabs(X_MARGIN, Y_MARGIN, w - 2*X_MARGIN, h - (30 + 2*Y_MARGIN) - 2*Y_MARGIN);
	tabs->begin();

	tab1 = new Fl_Group(tabs->x() + X_MARGIN, tabs->y() + 30 + Y_MARGIN, tabs->w() - 2*X_MARGIN, tabs->h() - 2*Y_MARGIN, "General");
	tab1->begin();

	scrollArea1 = new Fl_Scroll(tab1->x(), tab1->y(), tab1->w(), tab1->h() - 30);
	scrollArea1->type(Fl_Scroll::BOTH);
	scrollArea1->begin();

	layout_vertical1 = new Fl_Pack(scrollArea1->x(), scrollArea1->y(), scrollArea1->w() - scz, scrollArea1->h() - scz);
	layout_vertical1->type(Fl_Pack::VERTICAL);
	layout_vertical1->spacing(15 + Y_MARGIN);
	layout_vertical1->begin();

	inTxt_filterFile = new Fl_Input(0, 0, layout_vertical1->w(), layout_vertical1->h()/8, "Default filter files");
	inTxt_filterFile->value(neverNull(Config::global_configuration.get(Config::FILEFILTER)));
	inTxt_filterFile->tooltip ("Filter for files ['*'=joker, '-'=exclude, ';'=separator] (example : '*.c;*.h' or '-*.o')");
	inTxt_filterFile->align(FL_ALIGN_TOP_LEFT);

	inTxt_filterDir = new Fl_Input(0, 0, layout_vertical1->w(), layout_vertical1->h()/8, "Default filter directories");
	inTxt_filterDir->value(neverNull(Config::global_configuration.get(Config::DIRFILTER)));
	inTxt_filterDir->tooltip ("Filter ['*'=joker, '-'=exclude, ';'=separator] (example : 'src;doc' or '-obj')");
	inTxt_filterDir->align(FL_ALIGN_TOP_LEFT);

	inTxt_directory = new Fl_Input(0, 0, layout_vertical1->w(), layout_vertical1->h()/8, "Default root directory:");
	inTxt_directory->value(neverNull(Config::global_configuration.get(Config::DIRECTORY)));
	inTxt_directory->tooltip ("Autosearch in this directory when opening program");
	inTxt_directory->align(FL_ALIGN_TOP_LEFT);

	layout_vertical1->end();
	scrollArea1->end();
	tab1->end();

	tab2 = new Fl_Group(tabs->x() + X_MARGIN, tabs->y() + 30 + Y_MARGIN, tabs->w() - 2*X_MARGIN, tabs->h() - 2*Y_MARGIN, "Tools");
	tab2->begin();

	scrollArea2 = new Fl_Scroll(tab2->x(), tab2->y(), tab2->w(), tab2->h() - 30);
	scrollArea2->type(Fl_Scroll::BOTH);
	scrollArea2->begin();

	layout_vertical2 = new Fl_Pack(scrollArea2->x(), scrollArea2->y(), scrollArea2->w() - scz, scrollArea2->h() - scz);
	layout_vertical2->type(Fl_Pack::VERTICAL);
	layout_vertical2->spacing(15 + Y_MARGIN);
	layout_vertical2->begin();

	// TODO use FL_box to group
	inChoice_fileEditors = new Fl_Choice(0, 0, layout_vertical2->w(), layout_vertical2->h()/(5+3), "File editor");
	inChoice_fileEditors->align(FL_ALIGN_TOP_LEFT);
	inChoice_fileEditors->tooltip ("Select the file editor to configure");
	inChoice_fileEditors->callback (cb_fileEditor, this);
	inChoice_fileEditors->when(FL_WHEN_CHANGED);
	inChoice_fileEditors->add("Editor 1");
	inChoice_fileEditors->add("Editor 2");
	inChoice_fileEditors->add("Editor 3");
	inChoice_fileEditors->add("Editor 4");
	inChoice_fileEditors->add("Editor 5");

	for (int i = 0; i < 5; i++)
	{
		inTxt_editor[i] = new Fl_Input(0, 0, layout_vertical2->w(), layout_vertical2->h()/(5+3), "Editor (%path% = file):");
		inTxt_editor[i]->value(neverNull(Config::global_configuration.get((Config::TypeSto)(Config::EDITOR1 + i))));
		inTxt_editor[i]->tooltip ("The editor to use");
		inTxt_editor[i]->align(FL_ALIGN_TOP_LEFT);
		inTxt_editor[i]->hide();

		inTxt_editorConfig[i] = new Fl_Input(0, 0, layout_vertical2->w(), layout_vertical2->h()/(5+3), "File appliable to the editor (*.c;*.h)");
		inTxt_editorConfig[i]->value(neverNull(Config::global_configuration.get((Config::TypeSto)(Config::EDITORCONFIG1 + i))));
		inTxt_editorConfig[i]->tooltip ("Choose files which use the editor (like filter syntax)");
		inTxt_editorConfig[i]->align(FL_ALIGN_TOP_LEFT);
		inTxt_editorConfig[i]->hide();
	}

	inChoice_directoryTool = new Fl_Choice(0, 0, layout_vertical2->w(), layout_vertical2->h()/(5+3), "Directory editor");
	inChoice_directoryTool->add("Directory tool 1");
	inChoice_directoryTool->add("Directory tool 2");
	inChoice_directoryTool->add("Directory tool 3");
	inChoice_directoryTool->callback (cb_dirTool, this);
	inChoice_directoryTool->when(FL_WHEN_CHANGED);
	inChoice_directoryTool->tooltip ("Choose program to applies to directory");
	inChoice_directoryTool->align(FL_ALIGN_TOP_LEFT);

	for (int i = 0; i < 3; i++)
	{
		inTxt_dirTool[i] = new Fl_Input(0, 0, layout_vertical2->w(), layout_vertical2->h()/(5+3), "Directory tools (%path% = directory):");
		inTxt_dirTool[i]->value(neverNull(Config::global_configuration.get((Config::TypeSto)(Config::DIRTOOL1 + i))));
		inTxt_dirTool[i]->tooltip ("The directory tools to use");
		inTxt_dirTool[i]->align(FL_ALIGN_TOP_LEFT);
		inTxt_dirTool[i]->hide();
	}

	layout_vertical2->end();
	scrollArea2->end();
	tab2->end();

	tab3 = new Fl_Group(tabs->x() + X_MARGIN, tabs->y() + 30 + Y_MARGIN, tabs->w() - 2*X_MARGIN, tabs->h() - 2*Y_MARGIN, "Buttons");
	tab3->begin();

	scrollArea3 = new Fl_Scroll(tab3->x(), tab3->y(), tab3->w(), tab3->h() - 30);
	scrollArea3->type(Fl_Scroll::BOTH);
	scrollArea3->begin();

	layout_vertical3 = new Fl_Pack(scrollArea3->x(), scrollArea3->y() + 30, scrollArea3->w(), scrollArea3->h() -30);
	layout_vertical3->type(Fl_Pack::VERTICAL);
	layout_vertical3->spacing(Y_MARGIN);
	layout_vertical3->begin();

	btnReload = new Fl_Check_Button(0, 0, layout_vertical3->w(), 30, "button reload");
	btnReload->tooltip("Activate reload button");
	btnReload->value(stringToBool(Config::global_configuration.get(Config::BTNRELOAD)));

	btnRunDir[0] = new Fl_Check_Button(0, 0, layout_vertical3->w(), 30, "button directory 1");
	btnRunDir[0]->tooltip("Activate first directory button");
	btnRunDir[0]->value(stringToBool(Config::global_configuration.get(Config::BTNDIR1)));

	btnRunDir[1] = new Fl_Check_Button(0, 0, layout_vertical3->w(), 30, "button directory 2");
	btnRunDir[1]->tooltip("Activate second directory button");
	btnRunDir[1]->value(stringToBool(Config::global_configuration.get(Config::BTNDIR2)));

	btnRunDir[2] = new Fl_Check_Button(0, 0, layout_vertical3->w(), 30, "button directory 3");
	btnRunDir[2]->tooltip("Activate thrid directory button");
	btnRunDir[2]->value(stringToBool(Config::global_configuration.get(Config::BTNDIR3)));

	btnConfig = new Fl_Check_Button(0, 0, layout_vertical3->w(), 30, "boutton config");
	btnConfig->tooltip("Activate config button");
	btnConfig->value(stringToBool(Config::global_configuration.get(Config::BTNCONFIG)));

	btnAbout = new Fl_Check_Button(0, 0, layout_vertical3->w(), 30, "boutton about");
	btnAbout->tooltip("Activate about button");
	btnAbout->value(stringToBool(Config::global_configuration.get(Config::BTNABOUT)));

	layout_vertical3->end();
	scrollArea3->end();
	tab3->end();

	tabs->end();

	btn_cancel = new Fl_Button(w - 2*(75 + 2*X_MARGIN), h - (30 + 2*Y_MARGIN), 75, 30, "Cancel"); //$$$$$ autocalul 75
	btn_cancel->tooltip("Abort");
	btn_cancel->shortcut(FL_Escape);
	btn_cancel->callback(cb_cancel, this);

	btn_ok = new Fl_Return_Button(w - (75 + 2*X_MARGIN), h - (30 + 2*Y_MARGIN), 75, 30, "OK");
	btn_ok->tooltip("Save and exit");
	btn_ok->callback(cb_ok, this);

	resizable(this);

	end();
}

OptionsForm::~OptionsForm ()
{
	delete btnAbout;
	delete btnConfig;
	for (int i = 0; i < 3; i++)
		delete btnRunDir[i];
	delete btnReload;
	delete layout_vertical3;
	delete scrollArea3;
	delete tab3;
	for (int i = 0; i < 3; i++)
		delete inTxt_dirTool[i];
	delete inChoice_directoryTool;
	for (int i = 0; i < 5; i++)
		delete inTxt_editorConfig[i];
	for (int i = 0; i < 5; i++)
		delete inTxt_editor[i];
	delete inChoice_fileEditors;
	delete layout_vertical2;
	delete scrollArea2;
	delete tab2;
	delete inTxt_directory;
	delete inTxt_filterDir;
	delete inTxt_filterFile;
	delete layout_vertical1;
	delete scrollArea1;
	delete tab1;
	delete btn_cancel;
	delete btn_ok;
	delete tabs;
}

void OptionsForm::cb_ok(Fl_Widget *, void *arg)
{
	OptionsForm * o = (OptionsForm*)arg;

	// general config
	for (int i = 0; i < 5; i++)
	{
		Config::global_configuration.set((Config::TypeSto)(Config::EDITOR1 + i) , o->inTxt_editor[i]->value());
		Config::global_configuration.set((Config::TypeSto)(Config::EDITORCONFIG1 + i), o->inTxt_editorConfig[i]->value());
	}
	for (int i = 0; i < 3; i++)
	{
		Config::global_configuration.set((Config::TypeSto)(Config::DIRTOOL1 + i), o->inTxt_dirTool[i]->value());
	}
	Config::global_configuration.set(Config::FILEFILTER, o->inTxt_filterFile->value());
	Config::global_configuration.set(Config::DIRFILTER, o->inTxt_filterDir->value());
	Config::global_configuration.set(Config::DIRECTORY, o->inTxt_directory->value());

	// buttons
	Config::global_configuration.set(Config::BTNRELOAD, boolToString(o->btnReload->value()));
	Config::global_configuration.set(Config::BTNDIR1, boolToString(o->btnRunDir[0]->value()));
	Config::global_configuration.set(Config::BTNDIR2, boolToString(o->btnRunDir[1]->value()));
	Config::global_configuration.set(Config::BTNDIR3, boolToString(o->btnRunDir[2]->value()));
	Config::global_configuration.set(Config::BTNCONFIG, boolToString(o->btnConfig->value()));
	Config::global_configuration.set(Config::BTNABOUT, boolToString(o->btnAbout->value()));

	// generate file
	Config::global_configuration.save();

	o->hide();
}

void OptionsForm::cb_cancel(Fl_Widget *, void *arg)
{
	OptionsForm *o = (OptionsForm*)arg;
	o->hide();
}

void OptionsForm::cb_dirTool(Fl_Widget *, void *arg)
{
	OptionsForm *o = (OptionsForm*)arg;

	for (int i = 0; i < 3; i++)
	{
		o->inTxt_dirTool[i]->hide();
	}
	if (o->inChoice_directoryTool->value() >= 0 && o->inChoice_directoryTool->value() < 3)
	{
		o->inTxt_dirTool[o->inChoice_directoryTool->value()]->show();
	}
	o->scrollArea2->redraw();
}

void OptionsForm::cb_fileEditor(Fl_Widget *, void *arg)
{
	OptionsForm *o = (OptionsForm*)arg;

	for (int i = 0; i < 5; i++)
	{
		o->inTxt_editor[i]->hide();
		o->inTxt_editorConfig[i]->hide();
	}
	if (o->inChoice_fileEditors->value() >= 0 && o->inChoice_fileEditors->value() < 5)
	{
		o->inTxt_editor[o->inChoice_fileEditors->value()]->show();
		o->inTxt_editorConfig[o->inChoice_fileEditors->value()]->show();
	}
	o->scrollArea2->redraw();
}

