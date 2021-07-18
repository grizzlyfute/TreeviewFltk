#include "utils.h"
#include "treeview.h"
#include "options.h"
#include "icons.h"
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#define MAX_CHAR_IN_TREE 1024

// fork or spawn
#if IS_WINDOWS
#include <process.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

//replace find by replace into buf
void strremplace(char **buf, const char *find, const char *replace);
//filters
bool isAdmissibleByFilter(const char * filter, const char * path);

void setWindowIcon(Fl_Window *window, Fl_Pixmap *icon)
{
	#if IS_WINDOWS
	// need resources.rc
	window->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
	#else
	Fl_Window *dummyWindow = new Fl_Window(50, 50);
	dummyWindow->show();				// needed to define a graphics context
	dummyWindow->make_current();
	int maxDim = icon->w();
	if (icon->h() > maxDim) maxDim = icon->h();
	Fl_Offscreen pixmap = fl_create_offscreen(maxDim, maxDim);
	fl_begin_offscreen(pixmap);
	fl_color(FL_BACKGROUND_COLOR);
	fl_rectf(0, 0, maxDim, maxDim);
	icon->draw((maxDim - icon->w())/2, (maxDim - icon->h())/2);
	fl_end_offscreen();
	dummyWindow->hide();
	delete dummyWindow;
	window->icon((char *)pixmap);
	#endif
}

MainForm::MainForm(int w, int h, const char *t) : Fl_Double_Window (w, h, t)
{
	Config::global_configuration.load();
	const int margin_w = w / 60;
	const int margin_h = h / 120;

	begin();

	layout_buttons = new Fl_Pack(margin_w + 0*w/10, margin_h + 0*h/15, 10*w/10 - 2*margin_w, 1*h/15 - 2*margin_h);
	layout_buttons->type(Fl_Pack::HORIZONTAL);
	layout_buttons->spacing(3);
	layout_buttons->begin();

	btn_update = new Fl_Button(0, 0, 2*w/10, h/15);
	btn_update->tooltip("Update current tree (F5/Ctrl-r)");
	btn_update->shortcut(FL_COMMAND + 'r'); //Ctrl-r
	btn_update->box(FL_FLAT_BOX);
	btn_update->down_box(FL_DOWN_BOX);
	btn_update->image(&img_update);
	btn_update->callback(cb_update, this);
	if (stringToBool(Config::global_configuration.get(Config::BTNRELOAD))) btn_update->show();
	else btn_update->hide();

	btn_dirAction1 = new Fl_Button(0, 0, w/6, h/15);
	btn_dirAction1->tooltip("Launch first directory tool on current directory");
	btn_dirAction1->shortcut(FL_F + 6); //F6
	btn_dirAction1->box(FL_FLAT_BOX);
	btn_dirAction1->down_box(FL_DOWN_BOX);
	btn_dirAction1->image(&img_dirAction1);
	btn_dirAction1->callback(cb_dirAction1, this);
	if (stringToBool(Config::global_configuration.get(Config::BTNDIR1))) btn_dirAction1->show();
	else btn_dirAction1->hide();

	btn_dirAction2 = new Fl_Button(0, 0, w/6, h/15);
	btn_dirAction2->tooltip("Launch second directory tool on current directory");
	btn_dirAction2->shortcut(FL_F + 7); //F7
	btn_dirAction2->box(FL_FLAT_BOX);
	btn_dirAction2->down_box(FL_DOWN_BOX);
	btn_dirAction2->image(&img_dirAction2);
	btn_dirAction2->callback(cb_dirAction2, this);
	if (stringToBool(Config::global_configuration.get(Config::BTNDIR2))) btn_dirAction2->show();
	else btn_dirAction2->hide();

	btn_dirAction3 = new Fl_Button(0, 0, w/6, h/15);
	btn_dirAction3->tooltip("Launch thrird directory tool on current directory");
	btn_dirAction3->shortcut(FL_F + 8); //F8
	btn_dirAction3->box(FL_FLAT_BOX);
	btn_dirAction3->down_box(FL_DOWN_BOX);
	btn_dirAction3->image(&img_dirAction3);
	btn_dirAction3->callback(cb_dirAction3, this);
	if (stringToBool(Config::global_configuration.get(Config::BTNDIR3))) btn_dirAction3->show();
	else btn_dirAction3->hide();

	btn_options = new Fl_Button(0, 0, w/6, h/15);
	btn_options->tooltip("Configure default behavior (F9)");
	btn_options->shortcut(FL_COMMAND + 'p'); //Ctrl-p
	btn_options->box(FL_FLAT_BOX);
	btn_options->down_box(FL_DOWN_BOX);
	btn_options->image(&img_options);
	btn_options->callback(cb_openOptions, this);
	if (stringToBool(Config::global_configuration.get(Config::BTNCONFIG))) btn_options->show();
	else btn_options->hide();

	btn_about = new Fl_Button(0, 0, w/6, h/15);
	btn_about->tooltip("About (F1)");
	btn_about->shortcut(FL_F + 1); //F1
	btn_about->box(FL_FLAT_BOX);
	btn_about->down_box(FL_DOWN_BOX);
	btn_about->image(&img_about);
	btn_about->callback(cb_about, this);
	if (stringToBool(Config::global_configuration.get(Config::BTNABOUT))) btn_about->show();
	else btn_about->hide();

	layout_buttons->end();

	inTxt_dir = new Fl_Input(margin_w + 0*w/10, margin_h + 1*h/15, w - 2*w/10 - 2*margin_w, 1*h/15 - 2*margin_h);
	inTxt_dir->value(Config::global_configuration.get(Config::DIRECTORY));
	inTxt_dir->tooltip("Directory to use");
	inTxt_dir->callback(cb_inputDir, this);
	inTxt_dir->when(FL_WHEN_RELEASE | FL_WHEN_ENTER_KEY);

	btn_open_dir = new Fl_Button(margin_w + 8*w/10, margin_h + 1*h/15, 2*w/10 - 2*margin_w, 1*h/15 - 2*margin_h);
	btn_open_dir->tooltip("Open a directory (Ctrl-o)");
	btn_open_dir->shortcut(FL_COMMAND + 'o'); //Crtl-o
	btn_open_dir->box(FL_FLAT_BOX);
	btn_open_dir->down_box(FL_DOWN_BOX);
	btn_open_dir->image(&img_open);
	btn_open_dir->callback(cb_openDir, this);

	// pass over native scroll bar because no horizontal scroll bar
	sb_treeViewScrool = new Fl_Scroll(margin_w + 0*w/10, margin_h + 2*h/15, 10*w/10 - 2*margin_w - Fl::scrollbar_size(), 12*h/15 - 2*margin_h - Fl::scrollbar_size());
	sb_treeViewScrool->type(Fl_Scroll::BOTH);
//	sb_treeViewScrool->type(Fl_Scroll::BOTH_ALWAYS);
	sb_treeViewScrool->align(FL_ALIGN_BOTTOM_LEFT); // do not seem work ....
	sb_treeViewScrool->begin();

	//tr_treeView = new Fl_Tree(margin_w + 0*w/10, margin_h + 2*h/15, 10*w/10 - 2*margin_w, 12*h/15 - 2*margin_h);
	tr_treeView = new Fl_Tree(margin_w + 0*w/10, margin_h + 2*h/15, 0, 0);
	tr_treeView->selectmode(FL_TREE_SELECT_SINGLE);
	tr_treeView->showroot(false);
	tr_treeView->when(FL_WHEN_RELEASE);
	tr_treeView->sortorder(FL_TREE_SORT_ASCENDING);
	tr_treeView->callback(cb_browser, this);

	sb_treeViewScrool->end();

	inTxt_search = new Fl_Input(margin_w + 0*w/10, margin_h + 14*h/15, w - 2*w/10 - 2*margin_w, 1*h/15 - 2*margin_h, "Search:");
	inTxt_search->align(FL_ALIGN_TOP_LEFT);
	inTxt_search->tooltip("Search an item. Same syntax as filter.");
	inTxt_search->callback(cb_search, this);
	inTxt_search->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

	btn_search = new Fl_Button(margin_w + 8*w/10, margin_h + 14*h/15, 2*w/10 - margin_w, 1*h/15 - 2*margin_h);
	btn_search->tooltip("Launch search (F3/Ctrl-f)");
	btn_search->shortcut(FL_COMMAND + 'f'); //Ctrl-f
	btn_search->box(FL_FLAT_BOX);
	btn_search->down_box(FL_DOWN_BOX);
	btn_search->image(&img_search);
	btn_search->callback(cb_search, this);

	// see http://www.fltk.org/doc-2.0/html/symbols.html for symbole @@
	menu_contextualDir = new Fl_Menu_Button(0, 0, 400, 400, NULL);
	menu_contextualDir->type (Fl_Menu_Button::POPUP2);
	menu_contextualDir->add ("@>[] New dir", FL_COMMAND + 'd', cb_newDir, this, 0);
	menu_contextualDir->add ("@+ New file", FL_COMMAND + 'n', cb_newFile, this, FL_MENU_DIVIDER);
	menu_contextualDir->add ("@-> Set as root", '\0', cb_setAsRoot, this, FL_MENU_DIVIDER);
	menu_contextualDir->add ("@<-> Rename", FL_F + 2, cb_renameItem, this, 0);
	menu_contextualDir->add ("@9+ Delete Rec", FL_Delete, cb_deleteDir, this, 0);

	menu_contextualFile = new Fl_Menu_Button(0, 0, 400, 400, NULL);
	menu_contextualFile->type (Fl_Menu_Button::POPUP2);
	menu_contextualFile->add ("@<-> Rename", FL_F + 2, cb_renameItem, this, 0);
	menu_contextualFile->add ("@9+ Delete", FL_Delete, cb_deleteItem, this, 0);

	// $$$$$ TODO : layout

	setWindowIcon(this, &img_icon);

	resizable(this);
	end();

	cb_inputDir(NULL, this);

	treeHintW(0);
	treeHintH(0);
}

MainForm::~MainForm()
{
	delete menu_contextualDir;
	delete menu_contextualFile;
	delete inTxt_search;
	delete tr_treeView;
	delete sb_treeViewScrool;
	delete btn_open_dir;
	delete inTxt_dir;
	delete btn_about;
	delete btn_options;
	delete btn_dirAction3;
	delete btn_dirAction2;
	delete btn_dirAction1;
	delete btn_search;
	delete btn_update;
	delete layout_buttons;
}

int MainForm::handle (int event)
{
	switch (event)
	{
	case FL_KEYDOWN:
	{
		// implementing event like FL_COMMAND + something is bad. because
		// FL_COMMAND (Ctrl) is an event and 'something' is an other event. So,
		// FL_COMMAND + something is two event
		int key = Fl::event_key();
		//$$$$$ int event_state = Fl::event_state();
		switch (key)
		{
			/* $$$$$ case 'f':
				if (event_state & FL_CTRL)
					cb_search(NULL, this);
				break; */

			case FL_F + 3: //F3
				cb_search(NULL, this);
				break;

			case FL_F + 5: //F5
				cb_update(NULL, this);
				break;

			case FL_F + 9: //F9
				cb_openOptions(NULL, this);

			default:
				break;
		}
		break;
	}

	// A mouse button has gone down with the mouse pointing at this widget.
	case FL_PUSH:
	{
		int event_button = Fl::event_button();
		switch (event_button)
		{
			// Manage here mouse action because and let cb_browser manage the
			// selection. We should manage here because FL_TREE_REASON_RESELECT (double click)
			// didn't work with our fltk version (ABI required)
			case FL_LEFT_MOUSE:
			{
				Fl_Tree_Item *item = this->tr_treeView->first_selected_item();
				if (item == NULL ||
					// Out of widget
					Fl::event_x() < item->x() || Fl::event_x() > item->x() + item->w() ||
					Fl::event_y() < item->y() || Fl::event_y() > item->y() + item->h())
				{
					// Do nothing
				}
				// Oepn close on double clik on folder. Catch event only if not on the cross
				else if (item->has_children() && Fl::event_x() > item->x() + 25)
				{
					if (item->is_open())
					{
						item->close();
					}
					else if (item->is_close())
					{
						item->open();
					}
					cb_browser(NULL, this);
				}
				else if (!item->has_children() && item->is_selected())
				{
					for (int i = 0; i < 5; i++)
					{
						if (isAdmissibleByFilter(
							Config::global_configuration.get((Config::TypeSto)(Config::EDITORCONFIG1 + i)),
							                                 item->label()))
						{
							extractAndExecute(item, (Config::TypeSto)(Config::EDITOR1 + i));
							break;
						}
					}
				}
				break;
			}

			case FL_RIGHT_MOUSE:
			{
				Fl_Tree_Item *item = this->tr_treeView->get_item_focus();
				if (item != NULL && !item->is_selected())
				{
					Fl_Tree_Item *itemSelected = this->tr_treeView->first_selected_item();
					if (itemSelected != NULL)
						tr_treeView->deselect(itemSelected);
					tr_treeView->select(item);
				}

				if (item == NULL ||
					// Out of widget
					Fl::event_x() < item->x() || Fl::event_x() > item->x() + item->w() ||
					Fl::event_y() < item->y() || Fl::event_y() > item->y() + item->h())
				{
					// Do nothing
				}
				// item->has_children doesn't manage case of empty dir
				else if (item->usericon() == &img_folder)
				{
					menu_contextualDir->popup();
				}
				else
				{
					menu_contextualFile->popup();
				}
				break;
			}
			default:
				break;
		}
		break;
	}

	default:
		break;
	}
	return Fl_Window::handle(event);
}

void MainForm::cb_update(Fl_Widget *w, void *arg)
{
	cb_inputDir(w, arg);
}

void MainForm::cb_openDir(Fl_Widget *w, void *arg)
{
	Fl_Native_File_Chooser fc(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	fc.directory(((MainForm*)arg)->inTxt_dir->value());
	MainForm * mf = (MainForm*)arg;
	if (mf == NULL) return;

	if (fc.show() == 0)
	{
		mf->inTxt_dir->value(fc.filename());
		cb_inputDir(w, arg);
	}
}

// w == NULL : called from keyboard event
void MainForm::cb_search(Fl_Widget *, void *arg)
{
	MainForm * mf = (MainForm*)arg;
	if (mf == NULL) return;
	const char * search_string = mf->inTxt_search->value();

	if (search_string != NULL && *search_string != '\0')
	{
		Fl_Tree_Item *item = mf->findItemByLabel(search_string);
		if (item != NULL)
		{
			//if previously selected, un-select
			for (Fl_Tree_Item *itemSelected = mf->tr_treeView->first_selected_item();
				 itemSelected != NULL;
				 itemSelected = mf->tr_treeView->next_selected_item(itemSelected))
			{
				itemSelected->deselect();
			}

			mf->tr_treeView->select(item, false);
			// open the item and its parent
			do
			{
				mf->tr_treeView->open(item, false);
			} while ((item = item->parent()) != NULL);
		}
		else
		{
			fl_alert("Item with \"%s\" : not found", search_string);
		}
	}
}

void MainForm::cb_openOptions(Fl_Widget *, void *arg)
{
	MainForm * mf = (MainForm*)arg;
	OptionsForm o (680, 400, "options");
	o.set_modal();
	o.show();
	while (o.shown())
	{
		Fl::wait(); //no Timer (Fl::wait(float)), wait just something append
	}

	// update buttons
	if (stringToBool(Config::global_configuration.get(Config::BTNRELOAD))) mf->btn_update->show();
	else mf->btn_update->hide();
	if (stringToBool(Config::global_configuration.get(Config::BTNDIR1))) mf->btn_dirAction1->show();
	else mf->btn_dirAction1->hide();
	if (stringToBool(Config::global_configuration.get(Config::BTNDIR2))) mf->btn_dirAction2->show();
	else mf->btn_dirAction2->hide();
	if (stringToBool(Config::global_configuration.get(Config::BTNDIR3))) mf->btn_dirAction3->show();
	else mf->btn_dirAction3->hide();
	if (stringToBool(Config::global_configuration.get(Config::BTNCONFIG))) mf->btn_options->show();
	else mf->btn_options->hide();
	if (stringToBool(Config::global_configuration.get(Config::BTNABOUT))) mf->btn_about->show();
	else mf->btn_about->hide();
	mf->layout_buttons->redraw();
}

void MainForm::cb_dirAction1(Fl_Widget *, void *arg)
{
	MainForm * mf = (MainForm*)arg;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	if (item == NULL)
	{
		 item = mf->tr_treeView->first(); //root
		 if (item) item = item->next();
	}

	if (item != NULL) mf->extractAndExecute(item, Config::DIRTOOL1);
}

void MainForm::cb_dirAction2(Fl_Widget *, void *arg)
{
	MainForm * mf = (MainForm*)arg;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	if (item == NULL)
	{
		 item = mf->tr_treeView->first(); //root
		 if (item) item = item->next();
	}

	if (item != NULL) mf->extractAndExecute(item, Config::DIRTOOL2);
}

void MainForm::cb_dirAction3(Fl_Widget *, void *arg)
{
	MainForm * mf = (MainForm*)arg;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	if (item == NULL)
	{
		 item = mf->tr_treeView->first(); //root
		 if (item) item = item->next();
	}

	if (item != NULL) mf->extractAndExecute(item, Config::DIRTOOL3);
}

void MainForm::cb_about(Fl_Widget *, void *)
{
	// TODO : general help
	fl_message("TreeView v0.1");
}

void MainForm::cb_browser(Fl_Widget *, void * arg)
{
	MainForm * mf = (MainForm*)arg;
	//avoid infinite event if focus is losed and regain after and mouse released was missing

	// Find item that was clicked
	Fl_Tree_Item *item = (Fl_Tree_Item*)mf->tr_treeView->callback_item();
	if (item == NULL) item = mf->tr_treeView->root();

	if (mf->tr_treeView->callback_reason() == FL_TREE_REASON_OPENED ||
		mf->tr_treeView->callback_reason() == FL_TREE_REASON_CLOSED)
	{
		// resize tree in case of open / close, because no native horizontal scrollbar
		unsigned int maxCharHz = strlen(item->label());
		const int scz = mf->sb_treeViewScrool->scrollbar_size() > 0 ?
						mf->sb_treeViewScrool->scrollbar_size() :
						Fl::scrollbar_size();
		for (int i = 0; i < item->children(); i++)
		{
			//item lenght + n characters by parents
			maxCharHz = max(maxCharHz, strlen(item->child(i)->label()) + item->depth() * 3);
		}
		if (mf->tr_treeView->callback_reason() == FL_TREE_REASON_OPENED)
		{
			mf->treeHintW(mf->tr_treeView->w() + maxCharHz * mf->tr_treeView->item_labelsize()/2 - scz);
			mf->treeHintH(mf->tr_treeView->h() + item->children() * mf->tr_treeView->item_labelsize () - scz);
		}
		else if (mf->tr_treeView->callback_reason() == FL_TREE_REASON_CLOSED)
		{
			mf->treeHintW(mf->tr_treeView->w() - maxCharHz * mf->tr_treeView->item_labelsize()/2 + scz);
			mf->treeHintH(mf->tr_treeView->h() - item->children() * mf->tr_treeView->item_labelsize () + scz);
		}
		mf->tr_treeView->size(max(mf->treeHintW(), mf->sb_treeViewScrool->w() -scz), max(mf->treeHintH(), mf->sb_treeViewScrool->h() - scz));
	}
}


void MainForm::cb_inputDir(Fl_Widget *, void *arg)
{
	MainForm * mf = (MainForm*)arg;
	if (mf == NULL) return;
	if (mf->inTxt_dir->value() != NULL && strcmp(mf->inTxt_dir->value(), "") != 0)
		mf->populateTree(mf->inTxt_dir->value());
}

void MainForm::cb_newDir(Fl_Widget *, void *arg)
{
	MainForm *mf = (MainForm*)(arg);
	if (mf == NULL) return;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	if (item == NULL) return;
	const char *newName = fl_input ("New name", "");
	if (newName != NULL)
	{
		char * newPath = new char[MAX_CHAR_IN_TREE];
		mf->extractPath(item, newPath, MAX_CHAR_IN_TREE);
		int ind = strlen(newPath);
		if (ind > 0 && newPath[ind-1] != PATH_SEP)
		{
			newPath[ind++] = PATH_SEP;
		}
		strncpy (&newPath[ind], newName, MAX_CHAR_IN_TREE - ind);
		#if IS_WINDOWS
		if (CreateDirectory (newPath, NULL) == 0)
		{
			fl_alert("Fail to create directory '%s' (error %ld)", newPath, GetLastError());
		}
		#else
		if (mkdir (newPath, 0755) != 0)
		{
			fl_alert("Fail to create director '%s' (%s)", newPath, strerror(errno));
		}
		#endif
		else
		{
			// $$$$$ bad -> direct add ° check type
			cb_update(NULL, mf);
		}
	}
}

void MainForm::cb_newFile(Fl_Widget *, void *arg)
{
	MainForm *mf = (MainForm*)(arg);
	if (mf == NULL) return;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	if (item == NULL) return;
	const char *newName = fl_input ("New name", "");
	if (newName != NULL)
	{
		char * newPath = new char[MAX_CHAR_IN_TREE];
		mf->extractPath(item, newPath, MAX_CHAR_IN_TREE);
		int ind = strlen(newPath);
		if (ind > 0 && newPath[ind-1] != PATH_SEP)
		{
			newPath[ind++] = PATH_SEP;
		}
		strncpy (&newPath[ind], newName, MAX_CHAR_IN_TREE - ind);
		FILE *file = fopen(newPath, "r");
		if (file != NULL)
		{
			fclose(file);
			fl_alert ("File '%s' already exist (%s)", newPath, strerror(errno));
		}
		else
		{
			file = fopen(newPath, "w");
			if (!file)
			{
				fl_alert ("Can not create file '%s' (%s)", newPath, strerror(errno));
			}
			else
			{
				fclose (file);
			// $$$$$ bad -> direct add ° check type
				cb_update(NULL, mf);
			}
		}
		delete[] (newPath);
	}
}

void MainForm::cb_setAsRoot(Fl_Widget *w, void *arg)
{
	MainForm *mf = (MainForm*)(arg);
	if (mf == NULL) return;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	char * path = new char[MAX_CHAR_IN_TREE];
	mf->extractPath(item, path, MAX_CHAR_IN_TREE);
	mf->inTxt_dir->value(path);
	delete[] path;

	cb_inputDir(w, arg);
}

void MainForm::cb_renameItem(Fl_Widget *, void *arg)
{
	MainForm *mf = (MainForm*)(arg);
	if (mf == NULL) return;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	if (item == NULL) return;
	const char *newName = fl_input ("New name", item->label());
	if (newName != NULL)
	{
		char * oldPath = new char[MAX_CHAR_IN_TREE];
		char * newPath = new char[MAX_CHAR_IN_TREE];
		memset (oldPath, 0, MAX_CHAR_IN_TREE);
		mf->extractPath(item, oldPath, MAX_CHAR_IN_TREE);
		memcpy (newPath, oldPath, MAX_CHAR_IN_TREE);
		char * tmp = strrchr (newPath, PATH_SEP);
		if (tmp != NULL)
		{
			strncpy(tmp + 1, newName, MAX_CHAR_IN_TREE - (tmp + 1 - newPath));
			fprintf(stdout, "'%s' -> '%s'\n", oldPath, newPath);
			if (rename(oldPath, newPath) == 0)
			{
			// $$$$$ bad -> retire && direct add + check type
				item->label(newName);
			}
			else
			{
				fl_alert ("Fail to to rename '%s' to '%s' (%s)", oldPath, newPath, strerror(errno));
			}
		}
		else
		{
			fprintf(stderr, "Invalid path '%s'\n", newPath);
		}
		delete[] oldPath;
		delete[] newPath;
	}
}

void MainForm::cb_deleteItem(Fl_Widget *, void *arg)
{
	MainForm *mf = (MainForm*)(arg);
	if (mf == NULL) return;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	if (item == NULL) return;

	char * path = new char[MAX_CHAR_IN_TREE];
	mf->extractPath(item, path, MAX_CHAR_IN_TREE);

	if (*path != '\0' && fl_choice ("Are you sure to delete file '%s'", "Yes", "No", NULL, path) == 0)
	{
		if (std::remove(path) == 0)
		{
			mf->tr_treeView->remove (item);
		}
		else
		{
			fl_alert ("Fail to remove file '%s' (%s)", path, strerror(errno));
		}
	}

	delete[] path;
}

void MainForm::cb_deleteDir(Fl_Widget *, void *arg)
{
	// TODO $$$$$ write a recursive portable function. Use command for now
	MainForm *mf = (MainForm*)(arg);
	if (mf == NULL) return;

	Fl_Tree_Item *item = mf->tr_treeView->first_selected_item();
	if (item == NULL) return;

	char * path = new char[MAX_CHAR_IN_TREE];
	mf->extractPath(item, path, MAX_CHAR_IN_TREE);

	if (*path != '\0' && fl_choice ("Are you sure to delete (recursively) directory '%s'", "Yes", "No", NULL, path) == 0)
	{
		char * command = new char[MAX_CHAR_IN_TREE];
		int ret = 0;
		command[0] = 0;
		#if IS_WINDOWS
		snprintf (command, MAX_CHAR_IN_TREE, "rd /S /Q \"%s\"", path);
		#else
		snprintf (command, MAX_CHAR_IN_TREE, "rm -Rf \"%s\"", path);
		#endif
		ret = system(command);
		if (ret != 0)
		{
			fl_alert("Command '%s' return with code %d", command, ret);
		}
		else
		{
			mf->tr_treeView->remove (item);
		}
		delete[] command;
	}

	delete[] path;
}

void strremplace(char **src, const char *find, const char *replace)
{
	if (src == NULL || find == NULL || replace == NULL || *src == NULL) return;
	const int s_rpl = strlen(replace);
	const int s_fnd = strlen(find);
	const char *next = *src;
	int t,s;

	while ((next = strstr(next, find)) != NULL)
	{
		// compute size before change
		t = abs(next - *src) / sizeof(char);
		s = strlen (*src + t + s_fnd) + 1;
		*src = (char*)realloc(*src, (strlen(*src) + 1 + s_rpl - s_fnd)*sizeof(char));
		// move after(before() + find) into after(before() + replace)
		memmove(*src + t + s_rpl, *src + t + s_fnd, s);
		*(*src + t + s_rpl + s - 1) = '\0';
		strncpy(*src + t, replace, s_rpl);
		next = *src + t + s_rpl;
	}
}

//filter : '*.c;*.h;-*.o'
void MainForm::populateTree(const char * pathname)
{
	static char * internalPathName = NULL;
	static char * internalTree = NULL;
	static short int s_tree = 0, s_path = 0;
	static short int level = 0;

	if (level > 50 ||
		pathname == NULL ||
		tr_treeView == NULL ||
		strncasecmp(pathname, "..", 3) == 0 ||
		strncasecmp(pathname, ".", 2) == 0) return;

	const short int size_pathname = strlen(pathname);

	// Init
	if (level++ == 0)
	{
		short int size_pathname = strlen(pathname);
		tr_treeView->clear();
		tr_treeView->begin();

		internalTree = (char*)malloc(MAX_CHAR_IN_TREE*sizeof(char));
		internalPathName = (char*)malloc(MAX_CHAR_IN_TREE*sizeof(char));
		s_tree = 1;
		s_path = size_pathname;
		strncpy(internalTree, "", s_tree);

		// extract the last repository and set it as root of the tree
		while (s_path > 0 && pathname[s_path - 1] == PATH_SEP) s_path--;
		while (s_path > 0 && pathname[s_path - 1] != PATH_SEP) s_path--;
		strncpy(internalPathName, pathname, s_path + 1 < MAX_CHAR_IN_TREE ? s_path + 1 : MAX_CHAR_IN_TREE);
		internalPathName[s_path < MAX_CHAR_IN_TREE ? s_path : MAX_CHAR_IN_TREE - 1] = '\0';

		// remove trail PATH_SEP, witch cause problem
		int s = (strlen(pathname + s_path) + 1);
		char * tmp = (char*)malloc(s*sizeof(char));
		strncpy(tmp, pathname + s_path, s);
		while (s > 1 && tmp[s - 2] == PATH_SEP)
		{
			tmp[s - 2] = '\0';
			s--;
		}

		//populateTree(pathname + s_path);
		populateTree(tmp);
		free(tmp);
	}
	else
	{
		s_tree += size_pathname + 1;
		s_path += size_pathname + 1;
		if (s_tree < MAX_CHAR_IN_TREE && s_path < MAX_CHAR_IN_TREE)
		{
			if (strncmp(internalTree, "", 1) == 0)
			{
				strncpy(internalTree, pathname, MAX_CHAR_IN_TREE);
				s_tree--;
			}
			else
			{
				strcat(internalTree, "/");
				strcat(internalTree, pathname);
			}

			char tmp[2] = {PATH_SEP, '\0'};
			strcat(internalPathName, tmp);
			strcat(internalPathName, pathname);

			DIR *dir = opendir (internalPathName);
			struct dirent * entry_dir;

			if (dir != NULL)
			{
				if (isAdmissibleByFilter(Config::global_configuration.get(Config::DIRFILTER), pathname) ||
					level == 2) //admissible or root
				{
					int nb_entries = 0;
					entry_dir = readdir (dir);
					while (entry_dir != NULL)
					{
						nb_entries++;
						populateTree(entry_dir->d_name);
						entry_dir = readdir (dir);
					}

					Fl_Tree_Item * item = NULL;
					if (nb_entries <= 2)
					{
						// Dir is empty. entries are '.' and '..'
						item = tr_treeView->add(internalTree);
						item->usericon(&img_folder);
						item->close();
					}
					else
					{
						item = tr_treeView->find_item(internalTree);
						if (item != NULL)
						{
							item->usericon(&img_folder);
							if (level > 2) item->close();
						}
					}
				}
				closedir (dir);
			}
			else if (errno == ENOTDIR) // it's a file (we hope !)
			{
				if (isAdmissibleByFilter(Config::global_configuration.get(Config::FILEFILTER), pathname))
				{
					Fl_Tree_Item * item = tr_treeView->add(internalTree);
					if (isAdmissibleByFilter("*.asm;*.bat;*.c;*.cpp;*.cs;*.cxx;*.java;*.js;*.jsp;*.php;*.py;*.sh;*.sql;*.swift:*.vb;*.verilog;*.vhdl", pathname))
					{
						item->usericon(&img_documentSource);
					}
					else if (isAdmissibleByFilter("*.csv;*.doc;*.docx;*.hlp;*.htm;*.html;*.log;*.lst;makefile*;*.odp;*.ods;*.odt;*.pdf;*.ps;*.ppt;*.pptx;*.rss;*.txt;*.xhtml;*.xls;*.xlsx", pathname))
					{
						item->usericon(&img_documentText);
					}
					else if (isAdmissibleByFilter("*.conf;*.config.*;*.css;*.h;*.ini;*.sample;*.tpl;*.qss;*.xml;*.yml", pathname))
					{
						item->usericon(&img_documentListText);
					}
					else if (isAdmissibleByFilter("*.a;*.apk;*.bin;*.class;*.com;*.dll;*.dmg;*.elf;*.exe;*.iso;*.javac;*.o;*.map;*.msi;*.vcd;", pathname))
					{
						item->usericon(&img_documentBin);
					}
					else if (isAdmissibleByFilter("*.7z;*.ar;*.arj;*.bz;*.deb;*.gz;*.pkg;*.rar;*.rpm;*.tar.bz;*.tar.bz;*.tbz;*.tgz;*.z;*.zip", pathname))
					{
						item->usericon(&img_archive);
					}
					else if (isAdmissibleByFilter("*.bmp;*.ico;*.jpeg;*.jpg;*.gif;*.png;*.psd;*.svg;*.tif;*.tiff;*.xcf;*.xhtml;*.xpm", pathname))
					{
						item->usericon(&img_picture);
					}
					else if (isAdmissibleByFilter("*.3gp;*.aac;*.aif;*.aup;*.avi;*.cda;*.flac;*.flv;*.mid;*.mp3;*.mp4;*.mpa;*.mpg;*.mov;*.ogg;*.wav;*.wma;*.wpl", pathname))
					{
						item->usericon(&img_multimedia);
					}
					else
					{
						item->usericon(&img_documentDefault);
					}
				}
			}

			// erase path
			while (s_tree > 0 && internalTree[s_tree -1] != '/') s_tree--;
			while (s_path > 0 && internalPathName[s_path -1] != PATH_SEP) s_path--;
			if (s_tree > 0) internalTree[s_tree - 1] = '\0';
			else internalTree[0] = '\0';
			if (s_path > 0) internalPathName[s_path - 1] = '\0';
			else internalPathName[0] = '\0';
		}
		else
		{
			//Restore previous value
			s_tree -= size_pathname;
			s_path -= size_pathname;
		}
	}

	if (--level == 0)
	{
		//Free
		free(internalTree);
		internalTree = NULL;
		s_tree = 0;
		free(internalPathName);
		internalPathName = NULL;
		s_path = 0;

		tr_treeView->end();
		tr_treeView->showroot(0);

		tr_treeView->redraw();
	}
}

class Fl_Tree_Item * MainForm::findItemByLabel(const char * labelContains)
{
	Fl_Tree_Item * item = NULL;
	Fl_Tree_Item * startItem = tr_treeView->first_selected_item();

	//Transform in something admissible by filter, like strstr without case
	int s = strlen(labelContains), k = 0;
	char * labelContainsFilter = new char[s + 3];
	if (labelContains[k] != '*') labelContainsFilter[k++] = '*';
	strcpy(labelContainsFilter+k, labelContains);
	k += s;
	if (labelContains[k] != '*') labelContainsFilter[k++] = '*';
	labelContainsFilter[k++] = '\0';

	// start at the current item
	if (startItem != NULL) startItem = tr_treeView->next(startItem);
	else startItem = tr_treeView->root();

	for (item = startItem; item != NULL; item = tr_treeView->next(item) )
	{
		if (item == tr_treeView->root()) continue;
		else if (isAdmissibleByFilter(labelContainsFilter, item->label())) return item;
	}

	for (item = tr_treeView->first(); item != NULL && item != startItem; item = tr_treeView->next(item))
	{
		if (item == tr_treeView->root()) continue;
		else if (isAdmissibleByFilter(labelContainsFilter, item->label())) return item;
	}

	return NULL; // not found
}

void executeCommand(const char * cmd)
{
	char **argv = NULL;
	int argc = 0;
	int count = 0;
	const char * begin = cmd;
	bool jump = false;
	while (begin[count] != '\0')
	{
		//escape char
		if ((begin[count] == '\\' && begin[count+1] == '"') ||
			(begin[count] == '\\' && begin[count+1] == ' ')) count+=2;
		//quote => ignore space
		else if (begin[count] == '"') {jump = !jump; count++;}
		//space found => copy word
		else if (begin[count] == ' ' && !jump)
		{
			argv = (char**)realloc (argv, (argc+1)*sizeof(char*));
			argv[argc] = (char*)malloc((count+1) * sizeof(char));
			strncpy (argv[argc], begin, count);
			argv[argc][count] = '\0';
			argc++;
			begin = &begin[count+1];
			count = 0;
		}
		else count++;
	}
	//copy the last word
	argv = (char**)realloc (argv, (argc+1)*sizeof(char*));
	argv[argc] = (char*)malloc((count+1) * sizeof(char));
	strncpy (argv[argc], begin, count);
	argv[argc][count] = '\0';
	argc++;

	//Remove '"' and '\ '
	for (count = 0; count < argc; count++)
	{
		strremplace(&argv[count], "\"", "");
		strremplace(&argv[count], "\\ ", " ");
	}

	//put NULL at end of arg list
	argv = (char**)realloc(argv, (argc+1)*sizeof(char*));
	argv[argc] = NULL;

	#if IS_WINDOWS
	//if (spawnvp (P_DETACH, argv[0], argv) < 0) //space problem.
	if (spawnlp (P_DETACH, argv[0], cmd, NULL) < 0)
	{
		fprintf(stderr, "Error while executing '%s'\n", argv[0]);
	}
	#elif defined(__unix__)
	if (fork() == 0)
	{
		// Never return unless error
		if (execvp (argv[0], argv) < 0)
		{
			fprintf(stderr, "Error while executing '%s' (%s)\n", argv[0], strerror(errno));
			abort();
		}
	}
	#else
	system(cmd);
	#endif

	for (;argc > 0; argc--) free(argv[argc-1]);
	free (argv);
}

void MainForm::extractPath(Fl_Tree_Item *item, char *pathname, int len)
{
	if (item == NULL || pathname == NULL || len <= 0) return;
	strncpy(pathname, inTxt_dir->value(), len);
	pathname[len - 1] = '\0';
	int ind = strlen(pathname);
	if (ind > 0 && ind < len - 1 && pathname[ind-1] != PATH_SEP)
	{
		pathname[ind] = PATH_SEP;
		pathname[ind + 1] = '\0';
		ind++;
	}
	if (item->is_selected() &&
		//do not add 1st child, we will delete it after
		// (and this cause bug, because no end of sep '/' if adding first child
		item != tr_treeView->root()->child(0))
	{
		tr_treeView->item_pathname(&pathname[ind], len - ind, item);
	}
	pathname[len -1] = '\0';

	//Remove root label of the tree
	char *tmp = strchr(&pathname[ind], '/');
	if (tmp != NULL) memmove (&pathname[ind], tmp + 1, strlen(tmp) + 1);

	// '/' ->PATH_SEP
	for (ind = 0; pathname[ind] != '\0'; ind++)
	{
		if (pathname[ind] == '/') pathname[ind] = PATH_SEP;
	}
}

void MainForm::extractAndExecute(Fl_Tree_Item * item, Config::TypeSto command)
{
	if (Config::global_configuration.get (command) && strlen(Config::global_configuration.get (command)) > 0)
	{
		//extract full path
		int ind  = 0;
		char * pathname = new char[MAX_CHAR_IN_TREE];
		extractPath (item, pathname, MAX_CHAR_IN_TREE);

		// If this is an action on director, and item is a file, remove the tail of path (file)
		if ((command == Config::DIRTOOL1 ||
			 command == Config::DIRTOOL2 ||
			 command == Config::DIRTOOL3) &&
			!item->has_children())
		{
			for (ind = strlen(pathname) -1; ind >= 0 && pathname[ind] != PATH_SEP; ind--);
			if (ind >= 0) pathname[ind] = '\0';
		}

		//extract command
		//implement synchronization of data. Re-use cmd pointer
		char * cmd = (char*)malloc((strlen(Config::global_configuration.get (command))+1)*sizeof(char));
		strcpy(cmd, Config::global_configuration.get (command));
		strremplace (&cmd, "\t", " ");
		strremplace (&cmd, "%path%", "\"%path%\"");
		strremplace (&cmd, "%path%", pathname);

		executeCommand(cmd);
		free(cmd);
		delete [] pathname;
		cmd = NULL;
	}
}
/* Test cases:
	 filter = *.cpp, p = icons.cpp -> OK
	 filter = icons.h, p = icons.h -> OK
	 filter = *ico*, p = icons.cpp -> OK
	 filter = *ico*, p = jicons.cpp -> OK
	 filter = *ico*, p = jicoicons.cpp -> OK
	 filter = *icons*, p = jicoicons.cpp -> OK
	 filter = ic*e, p = icone -> OK
	 filter = **ico**, p = icon -> OK
	 filter = **ico**, p = ico -> OK
	 filter = **ico**, p = xico -> OK
	 filter = ic**ones, p = icones -> OK
	 filter = ic**es, p = icones -> OK
	 filter = , p = icones -> OK
	 filter = *, p =	-> OK
	 filter = **, p =	-> OK
	 filter = , p =	-> OK
	 filter = ic**es, p = iones -> NO
	 filter = *.cpp, p = icons.h -> NO
	 filter = *ico*, p = jtcoxcons.cpp -> NO
	 filter = ic*e, p = icones -> NO
	 filter = **ico**, p = xixo -> NO
	 filter = icone, p =	-> NO */
bool isAdmissibleByFilter(const char * filter, const char * path)
{
	const char *p;
	const char *tmp;
	bool exclude;
	bool ok = false;
	const char * laststarpos;
	static const char sep[] = ";:,|";

	if (filter == NULL || *filter == '\0' || path == NULL)
	{
		ok = true;
	}
	else
	{
		while (filter != NULL && *filter != '\0')
		{
			exclude = (*filter == '-');
			if (exclude)
			{
				filter++;
				ok = true;
			}
			tmp = strpbrk(filter, sep);

			p = path;
			laststarpos = NULL;
			// if this filter contains path
			while (*filter != '\0' && filter != tmp && *p != '\0')
			{
				while (*filter == '*')
				{
					laststarpos = filter;
					filter++;
				}
				if (toupper(*filter) == toupper(*p))
				{
					filter++;
				}
				else if (laststarpos != NULL)
				{
					if (*(laststarpos + 1) != '\0')
					{
						filter = laststarpos + 1;
						if (toupper(*(filter)) == toupper(*p)) filter++;
					}
					else filter = laststarpos;
				}
				else break;
				p++;
			}

			while (*filter == '*') filter++; // remove '*' at the end
			// found
			if ((*filter == '\0' || filter == tmp) && *p == '\0')
			{
				ok = !exclude;
				break;
			}

			// compute next
			while (tmp && strchr(sep, *tmp) != NULL && *tmp != '\0') tmp++; //jump over sep
			filter = tmp;
		}
	}

	return ok;
}

void MainForm::treeHintH(int h) { tree_hint_h = h; }
int MainForm::treeHintH() const { return tree_hint_h; }
void MainForm::treeHintW(int w) { tree_hint_w = w; }
int MainForm::treeHintW() const { return tree_hint_w; }

// use full to resize tree when resize
void MainForm::resize(int x, int y, int w, int h)
{
	Fl_Double_Window::resize(x, y, w, h);
	// size(W, H) is a shortcut for resize(x(), y(), W, H)
	tr_treeView->size(max(treeHintW(), w - 2*w/30), max(treeHintH(), 11*h/15 - 2*h/60));
}
