#ifndef RUMBA_SYSTEM_H
#define RUMBA_SYSTEM_H
//	The code in here is used to isolate nonportable code.
//	Some of this stuff is system specific.


#include <string>
#include <list>


using std::string;

namespace RUMBA
{
	/**
	  * Returns a list of files in a directory
	  */
	std::list<std::string> ls (std::string dirname);

	/**
	  * return true if the host is little endian, false otherwise.
	  */
	bool littleEndianHost();
	std::string get_rumba_config();

	bool rumba_file_exists(const char* filename);

	/**
	  * Portable version of getenv(). returns NULL if the variable in question
	  * is not found, so use carefully !
	  */
	const char* rumba_getenv( const char * varname );

	// finds the module directory
	std::string find_file_modules();


} // namespace RUMBA

#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
