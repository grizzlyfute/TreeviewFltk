#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tree.H>
#include "global_configuration.h"
#include "utils.h"

#if IS_WINDOWS
#define PATH_SEP '\\'
#elif defined(__unix__)
#define PATH_SEP '/'
#else
#define PATH_SEP '/'
#endif

class MainForm : public Fl_Double_Window
{
	public:
		MainForm(int w, int h, const char *t);
		virtual ~MainForm();
		int handle (int event);

// $$$$ TODO check *const

		void populateTree(const char * initialPathName);
		Fl_Tree_Item * findItemByLabel(const char * labelContains);
		void extractPath(Fl_Tree_Item *, char *path, int len);
		void extractAndExecute(Fl_Tree_Item * item, Config::TypeSto command);

		void treeHintH(int h);
		int treeHintH() const;
		void treeHintW(int w);
		int treeHintW() const;

		virtual void resize(int x, int y, int w, int h);


	private:
		int tree_hint_h;
		int tree_hint_w;

		// $$$$$ check public / private nor accessos
		Fl_Pack * layout_buttons;
		Fl_Button * btn_update;
		Fl_Button * btn_open_dir;
		Fl_Button * btn_search;
		Fl_Button * btn_about;
		Fl_Button * btn_options;
		Fl_Button * btn_dirAction1;
		Fl_Button * btn_dirAction2;
		Fl_Button * btn_dirAction3;
		Fl_Tree * tr_treeView;
		Fl_Scroll * sb_treeViewScrool;
		Fl_Input * inTxt_dir;
		Fl_Input * inTxt_search;
		Fl_Menu_Button * menu_contextualDir;
		Fl_Menu_Button * menu_contextualFile;

		/* callbacks */
		static void cb_update(Fl_Widget *, void *);
		static void cb_openDir(Fl_Widget *, void *);
		static void cb_search(Fl_Widget *, void *);
		static void cb_openOptions(Fl_Widget *, void *);
		static void cb_dirAction1(Fl_Widget *, void *);
		static void cb_dirAction2(Fl_Widget *, void *);
		static void cb_dirAction3(Fl_Widget *, void *);
		static void cb_about(Fl_Widget *, void *);
		static void cb_browser(Fl_Widget *, void *);
		static void cb_inputDir(Fl_Widget *, void *);
		static void cb_newDir(Fl_Widget *, void *);
		static void cb_newFile(Fl_Widget *, void *);
		static void cb_setAsRoot(Fl_Widget *w, void *arg);
		static void cb_renameItem(Fl_Widget *, void *);
		static void cb_deleteItem(Fl_Widget *, void *);
		static void cb_deleteDir(Fl_Widget *, void *);
};

// $$$$$ TODO : put callback in *this and remove public



#endif /* TREEVIEW_H */
