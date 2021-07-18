#include "treeview.h"
#include "global_configuration.h"
#include <FL/Fl.H>
#include <getopt.h>
#include <iostream>
#include <string.h>
using namespace std;

int main(int argc, char * argv[])
{
	// Set config file in same directory of program
	int ret = strlen(argv[0]) + strlen(DEFAULT_CONFFILE_NAME) + 1;
	char * defaultConfigFile = new char[ret];
	strncpy(defaultConfigFile, argv[0], ret);
	for (ret = strlen(argv[0]) - 1; ret >= 0; ret--)
	{
		if (defaultConfigFile[ret] == PATH_SEP || defaultConfigFile[ret] == '/')
		{
			defaultConfigFile[ret+1] = '\0';
			break;
		}
	}
	strcat(defaultConfigFile, DEFAULT_CONFFILE_NAME);
	Config::global_configuration.set(Config::CONFIGURATION_FILE, defaultConfigFile);
	delete[] defaultConfigFile;
	defaultConfigFile = NULL;

	static struct option long_options[] =
	{
		{"help", no_argument, NULL, 'h'},
		{"file", required_argument, NULL, 'f'},
		{NULL, 0, NULL, 0}
	};

	bool execute = true;
	ret = 0;
	while ((ret = getopt_long(argc, argv, "f:h", long_options, NULL)) > 0 && execute)
	{
		switch (ret)
		{
			case 'h':
				cout << "usage: "  << argv[0] << " [-f file]" << endl;
				cout << " Program options" << endl;
				cout << "--help | -h            This help" << endl;
				cout << "--file | -f            Select file as configuration file" << endl;
				cout << endl;
				cout << "FLTK Options" << endl;
				execute = false;

				break;
			case 'f':
				Config::global_configuration.set(Config::CONFIGURATION_FILE, optarg);
				break;

			default:
				break;
		}
	}
	if (execute && optind > 0)
	{
		argv = &argv[optind - 1];
		argc -= optind - 1;
	}

	MainForm mainForm(175, 800, "Project Browser");
	mainForm.show(argc, argv);

	if (execute)
	{
		Fl::run();
	}
	else
	{
		mainForm.hide();
	}
	return 0;
}
