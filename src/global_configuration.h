#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#define DEFAULT_CONFFILE_NAME "treeview.conf"
namespace Config
{
  enum TypeSto
  {
    EDITOR1,
    EDITOR2,
    EDITOR3,
    EDITOR4,
    EDITOR5,
    EDITORCONFIG1,
    EDITORCONFIG2,
    EDITORCONFIG3,
    EDITORCONFIG4,
    EDITORCONFIG5,
    DIRTOOL1,
    DIRTOOL2,
    DIRTOOL3,
    FILEFILTER,
    DIRFILTER,
    DIRECTORY,
	CONFIGURATION_FILE,
	BTNRELOAD,
	BTNDIR1,
	BTNDIR2,
	BTNDIR3,
	BTNCONFIG,
	BTNABOUT,
	MAX,
  };
  struct mapTextEnum
  {
	  const char *str;
	  TypeSto x;
  };

  class Global_configuration
  {
    public:
      Global_configuration();
	  // TODO Global_configuration(const Global_configuration&);
       virtual ~Global_configuration();

      const char * get(TypeSto number);
      void set(TypeSto number, const char * value);

	void save();
	void load();

    private:
	  const char * get_appropiate_conf_file();
      char **store;
      mapTextEnum * dataRef;
  };

  extern Global_configuration global_configuration;
};

#endif /* GLOBAL_CONFIG_H */
