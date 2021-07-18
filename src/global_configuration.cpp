#include "global_configuration.h"
#include <string.h>
#include <fstream>

#define saveIfExists(filer, x) \
{ \
	if (get(x)) \
		file << #x << " = " << get(x) << std::endl; \
}

using namespace Config;

Global_configuration::Global_configuration()
{
	store = new char*[MAX];
	int i;
	for (i = 0; i < MAX; i++)
	{
		store[i] = NULL;
	}
	i = 0;
	dataRef = new mapTextEnum[MAX];
	dataRef[i++] = (mapTextEnum){"editor1",     EDITOR1};
	dataRef[i++] = (mapTextEnum){"editor2",     EDITOR2};
	dataRef[i++] = (mapTextEnum){"editor3",     EDITOR3};
	dataRef[i++] = (mapTextEnum){"editor4",     EDITOR4};
	dataRef[i++] = (mapTextEnum){"editor5",     EDITOR5};
	dataRef[i++] = (mapTextEnum){"editorConf1", EDITORCONFIG1};
	dataRef[i++] = (mapTextEnum){"editorConf2", EDITORCONFIG2};
	dataRef[i++] = (mapTextEnum){"editorConf3", EDITORCONFIG3};
	dataRef[i++] = (mapTextEnum){"editorConf4", EDITORCONFIG4};
	dataRef[i++] = (mapTextEnum){"editorConf5", EDITORCONFIG5};
	dataRef[i++] = (mapTextEnum){"dirTool1",    DIRTOOL1};
	dataRef[i++] = (mapTextEnum){"dirTool2",    DIRTOOL2};
	dataRef[i++] = (mapTextEnum){"dirTool3",    DIRTOOL3};
	dataRef[i++] = (mapTextEnum){"fileFilter",  FILEFILTER};
	dataRef[i++] = (mapTextEnum){"dirFilter",   DIRFILTER};
	dataRef[i++] = (mapTextEnum){"directory",   DIRECTORY};
	dataRef[i++] = (mapTextEnum){"confFile",    CONFIGURATION_FILE};
	dataRef[i++] = (mapTextEnum){"btnReload",   BTNRELOAD};
	dataRef[i++] = (mapTextEnum){"btnDir1",     BTNDIR1};
	dataRef[i++] = (mapTextEnum){"btnDir2",     BTNDIR2};
	dataRef[i++] = (mapTextEnum){"btnDir3",     BTNDIR3};
	dataRef[i++] = (mapTextEnum){"btnConfig",   BTNCONFIG};
	dataRef[i++] = (mapTextEnum){"btnAbout",    BTNABOUT};
}

Global_configuration::~Global_configuration()
{
	for (int i = 0; i < MAX; i++)
	{
		if (store[i]) delete[] store[i];
	}
	delete[] store;
	delete[] dataRef;
}

const char * Global_configuration::get(TypeSto number)
{
	if (number >=0 && number < MAX)
	{
		return store[number];
	}
	else
	{
		return NULL;
	}
}

void Global_configuration::set(TypeSto number, const char * value)
{
	if (number >=0 && number < MAX)
	{
		if (store[number]) delete[] store[number];
		if (value == NULL) store[number] = NULL;
		else
		{
			int size = strlen(value) + 1;
			store[number] = new char[size];
			strncpy(store[number], value, size);
		}
	}
}

void Global_configuration::save()
{
	std::ofstream fileConf(get_appropiate_conf_file(), std::ios::out | std::ios::trunc);
	if (fileConf.is_open())
	{
		for (int i = 0; i < MAX; i++)
		{
			if (get(dataRef[i].x))
				fileConf << dataRef[i].str << " = " << get((Config::TypeSto)i) << std::endl;
		}
		fileConf.close();
	}
}

void Global_configuration::load()
{
	std::ifstream fileConf(get_appropiate_conf_file(), std::ios::in);
	char buf[1024];
	int begin_data;
	int length;

	if (fileConf.is_open())
	{
		while (fileConf.getline(buf, 1024))
		{
			//remove garbage
			begin_data = 0;
			while (strchr(" \t", buf[begin_data]) && buf[begin_data] != '\0') begin_data++;

			for (unsigned int i = 0; i < MAX; i++)
			{
				length = strlen(dataRef[i].str);
				//compare
				if (strncasecmp(dataRef[i].str, &buf[begin_data], 1024 < begin_data + length ? 1024 : begin_data + length) == 0)
				{
					//extract
					begin_data += length;
					while (strchr(" \t=:", buf[begin_data]) && buf[begin_data] != '\0') begin_data++;

					length = strlen(&buf[begin_data]);
					if (length > 0)
					{
						//remove queue garbage
						while (length >= 1 && strchr ("\r\n", buf[begin_data + length - 1]) != NULL) length--;
						buf[begin_data + length] = '\0';
						//store
						set(dataRef[i].x, &buf[begin_data]);
					}
					else set(dataRef[i].x, NULL);
					break;
				}
			}
		}
	}
}

const char * Global_configuration::get_appropiate_conf_file()
{
	const char * fileName;
	fileName = get(CONFIGURATION_FILE);
	if (fileName == NULL || strlen(fileName) == 0)
	{
		fileName = DEFAULT_CONFFILE_NAME;
	}
	return fileName;
}

Global_configuration Config::global_configuration;
