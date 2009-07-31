#include <stdlib.h>
#include <rumba/rumba_system.h>
#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <rumba/exception.h>
#include <rumba_config.h>
#include <rumba/parse.h>


#define OS_UNIX

#ifdef OS_UNIX
#include <unistd.h>
#endif

// Works in borland as well as UNIX.
#include <dirent.h>

// should work on UNIX and Windows
#include <sys/stat.h>


using std::string;
using std::list;







list<string> RUMBA::ls (string dirname)
{
	list<string> result;
	struct dirent* d;
	DIR* myDir;

	myDir = opendir ( dirname.c_str() );
	if ( myDir == 0 )
	{
		#ifdef USE_EXCEPTIONS
		throw  RUMBA::BadFile ( "Can't open directory" );
		#else
		return result;
		#endif
	}

	// compiler gives nasty messages if we use while(d=readdir(myDir) )
	while ( (d = readdir ( myDir )) != NULL )
	{
		result.push_back ( d->d_name );
	}

	closedir ( myDir );

	return result;
}

bool RUMBA::littleEndianHost()
{
	short x=1;
	return (bool)(*((char*)(&x)));
}

const char* RUMBA::rumba_getenv( const char * varname )
{
	const char* value = getenv(varname );
	return value;
}

// check to see whether a given file exists
bool RUMBA::rumba_file_exists(const char* filename)
{
	struct stat buf;
	if ( stat ( filename, &buf ) == -1 )
	{
		return false;
	}
	else
		return true;
}

// First look for $HOME/.rumba/rumbalib.conf, then /etc/rumba/rumbalib.conf,
// then $RUMBA_PREFIX/etc/rumba/rumbalib.conf
std::string RUMBA::get_rumba_config()
{
	const char* dir = RUMBA::rumba_getenv("HOME");
	std::string filename;
	if ( dir )
	{
		filename = std::string(dir) + std::string ( RUMBA_USER_CONFIG_FILE );
		if ( rumba_file_exists ( filename.c_str() ) )
			return filename;
	}

	filename = RUMBA_SYS_CONFIG_FILE;
	if ( rumba_file_exists ( filename.c_str() ) )
		return filename;

	filename = RUMBA_PREFIX;
	filename += RUMBA_SYS_CONFIG_FILE;

	if ( rumba_file_exists (filename.c_str() ) )
		return filename;
	// fail
	return "";
}


std::string RUMBA::find_file_modules()
{
	std::string rumba_dir;
	const char*  rumba_env = rumba_getenv("RUMBA");
	std::string config_file;
	if ( rumba_env )
		rumba_dir = std::string (rumba_env);
	else
	{
		// RUMBA is undefined, so we try to find the config file
		config_file = get_rumba_config();
		if ( config_file.empty() )
			rumba_dir = RUMBA_PREFIX;
		else
		{
			std::ifstream conf(config_file.c_str());
			RUMBA::rcFind ( conf, "RUMBA", rumba_dir );
			if ( rumba_dir.empty() )
				rumba_dir = RUMBA_PREFIX;
		}
	}
	// now try to find libanalyzeFile.so

	std::string file_modules = rumba_dir + std::string ( "/lib/filetypes" );
	std::string test = file_modules + "/libAnalyzeFile.so";
	if ( ! rumba_file_exists(test.c_str())  ) // Check if file exists.
	{
		std::cerr << "Can't find libAnalyzeFile.so. Please set RUMBA \n "
			<< "to point to your rumba installation directory" << std::endl;
		return "";
	}
	else
		return file_modules;
}


// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
