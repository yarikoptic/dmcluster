#ifndef RUMBA_PARSE_H
#define RUMBA_PARSE_H
#include <list>
#include <string>
#include <sstream>
#include <map>
#include <rumba/exception.h>

namespace RUMBA 
{

/**
  *	Similar to strtok. Returns a list of tokens in the line. The delim argument is forwarded 
  * to getline()
  */
std::list<std::string> tokenizeCsvLine( std::string line, char delim = ',' );

/*
 *	"Convert" a string to an integer. Throw RUMBA::BadConvert if unsuccesful.
 *
 */
template<class T> T stream_cast(std::string s);

/*
   *
   *	Strip leading and trailing whitespace. The characters in ignore are stripped.
   *	(so it behaves a little like perl's chomp() if you use ignore="\n" )
   */
void strip( std::string& s, const char* ignore = " \t\n" );

/**
  *	The following code pertains to "rcFile"s. An rcFile is a file with the format
  \code
	key1 = value1
	key2 = value2
	\endcode
  * etc. (much like the output of the 'env' unix command). 
  *
  *	rcFind: Search for key, and set the parameter value if key is defined 
  * in rcFile. Return true if key found. eg in the above rcFile, 
  * rcFind(string("key1"), val, file);
  * would set val to "value1"
  *
  */
bool rcFind ( std::istream& in, std::string key, std::string& val );

/**
  *	Return a map containing keys and values in the rcFile
  */
std::map<std::string,std::string> parseRc ( std::istream& );

/**
  *	Decompose a line of text of the form "key = value" into a key/value pair.
  */
void parseRcLine (std::string line, std::string& key, std::string& value);

/**
  *	Return the "extension" of a file name (that is, everything after the last . 
  * in the file name, including that ".") for example, fileExtension(".tar.gz")
  * returns ".gz" If there's no "extension", an empty string is returned.
  */
std::string fileExtension(std::string s);

/**
  *	Removes all files from a list of files that do not have a certain extension.
  * the extension argument should include the "."
  */
void pruneFileList(std::list<std::string>& v, std::string extension);

/**
  *	Remove leading and trailing whitespace, as well as quotes, from a string.
  * If there are no quotes, only whitespace is removed. Returns the modified 
  * string.
  */
std::string extractQuotedString(std::string s);

/**
  *	Extract dl name from a libtool archive (.la) file.
  */
std::string get_dlname(std::string filename);


/** 
  * Attempts to convert a string into a given data type by using stringstream 
  * extraction. Throws RUMBA::BadConvert if unsuccesful
  */
template<class T>
T stream_cast(std::string s)
{
	T result;
	char ch;
	std::istringstream in(s);
	in >> result;
	if ( in >> ch )
		throw RUMBA::BadConvert( std::string("Stream cast failed ") + s );
	return result;

}

/*
 * format any data type as a string using ostream insertion
 */
template<class T>
std::string toString(const T& x)
{
    std::ostringstream out;
    std::string result;
    out << x;
    result=out.str();
    return result;
}

}
#endif

// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
