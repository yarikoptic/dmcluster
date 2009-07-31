#ifndef RUMBA_ARGHANDLER_H
#define RUMBA_ARGHANDLER_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <rumba/splodge.h>
#include <rumba/exception.h>
#include <rumba/log.h>


namespace RUMBA
{


/**
* A type to represent what data type an argument is.
*/
enum Argtype {
	FLAG,
	ALPHA,
	NUMERIC
};


/**
   Class to represent arguments.
*/
struct Argument
{
	std::string longArg;
	char shortArg;
	enum RUMBA::Argtype type;
	bool required;
	RUMBA::Splodge defaultValue;
	bool multi;


	Argument (
			std::string longArg,
			enum Argtype type,
			char shortArg = 0
		);

	Argument (
			std::string longArg,
			enum Argtype type,
			char shortArg,
			const RUMBA::Splodge& defaultValue,
			bool required = false,
			bool multi = false
		);
	Argument();
	~Argument();
};



/**
  * 	The ArgHandler class is designed to simplify the routine task
  *		of command-line parseing. Typically, one uses it as follows:
  First, we need an array of arguments. It must be "terminated", a
  terminator argument is created by using the default Argument constructor:
\code
       Argument myArgs[] = {
  			Argument ( "arga", ALPHA, 'a', "default" ),
			Argument ( "argb", NUMERIC 'b', Splodge(), true ),
			Argument ( "foo", FLAG ),
			Argument ()
			};
\endcode

	Then in the main program, one creates the ArgHandler object. Note
	that this should be done in a try/catch block (more on exception
	handling later):
\code
	try
	{
		ArgHandler argh(argc,argv,myArgs);
\endcode

	Having created the ArgHandler, we recover the arguments:

\code
	std::string a;
	double b;
	bool foo;
	bool have_b = true;
	argh.arg("arga", arga);
	if ( argh.arg("argb"))
		argh.arg("argb", argb);
	else
		have_b = false;
	foo = argh.arg("foo");

\endcode

 */



class ArgHandler
{
public:
	/**
	  * constructor. Use this if you have no custom arguments.
  	*/
	ArgHandler(int argc, char** argv, Argument* = 0);

	/**
	  * print debug info
	  */
	void print(std::ostream& s = std::cout);

	/** initialise argHandler object (prepare for argument extraction).
	  * You shouldn't need to use this.
	  */
	void init();

	/**
	  * make one of the default arguments (infile,outfile) a multi-argument
	  */
	static void setMultiDefaultArg(const char* argname);

	/**
	  * make one of the default arguments (infile,outfile) a required-argument
	  */
	static void setRequiredDefaultArg(const char* argname);

	/**
	  * returns true if argument was given on command line, false otherwise.
	  * This can be used to check for flags, *and* to see if long arguments
	  * were given
	  */
	bool arg(const char* argname);

	/**
	  * Retrieve a string argument. Throws an exception if there's no default
	  * value for that argument, and no argument was given.
	  */
	void arg(const char* argname, std::string&);

	/**
	  * Retrieve a character (ALPHA) argument. Throws an exception if
	  * there's no default
	  * value for that argument, and no argument was given.
	  */
	void arg(const char* argname, char&);

	/**
	  * Retrieve a NUMERIC argument as a short int. Throws an exception if
	  * there's no default
	  * value for that argument, and no argument was given.
	  */
	void arg(const char* argname, short&);

	/**
	  * Retrieve a NUMERIC argument as type int. Throws an exception if
	  * there's no default
	  * value for that argument, and no argument was given.
	  */
	void arg(const char* argname, int&);

	/**
	  * Retrieve a NUMERIC argument as type int. Throws an exception if
	  * there's no default
	  * value for that argument, and no argument was given.
	  */
	void arg(const char* argname, unsigned int&);


	/**
	  * Retrieve a NUMERIC argument as type float . Throws an exception if
	  * there's no default
	  * value for that argument, and no argument was given.
	  */
	void arg(const char* argname, float&);

	/**
	  * Retrieve a NUMERIC argument as type float . Throws an exception if
	  * there's no default
	  * value for that argument, and no argument was given.
	  */
	void arg(const char* argname, double&);

	/**
	  * Retrieve the given argument name, or the first loose argument if the
	  * argument is unavailable
	  */

	void arg_or_loose(const char* argname, std::string&);


	/** recover a multiarg. If no argument was given, and empty list is
	  * returned.
	  */
	const std::vector<RUMBA::Splodge> & multiarg(const char* argname);

	/**
	  * return loose arguments. If there aqe none, return an empty list.
	  */
	const std::vector<std::string> & loose() { return Loose; }

	/**
	  * Return an iterator whose key is the argument name, and value is
	  * the argument value. A numeric argument or a flag will be stored as a
	  * numeric splodge. ALPHA arguments will be stored as a Splodge string.
	  */
	std::map<std::string, RUMBA::Splodge>::const_iterator
		lookup(const char* argname);

private:
	void getNextArg(std::string arg, enum Argtype t, bool multi = false);
	void getNextMultiArg(std::string arg, enum Argtype t);
	void processArg(std::string arg );
	void processShortArg(std::string arg);
	void initMulti();
	void loadDefaultArgs();

	// verify required arguments are included.
	bool checkRequired();

	// arguments given by user OR arguments with default values.
	std::map<std::string, RUMBA::Splodge> Argv;
	std::map<std::string, std::vector<RUMBA::Splodge> > MultiArgv;

	std::vector<std::string> Loose; // loose arguments.

	std::vector<Argument> ArgsAvailable; // command line arguments available.

	bool NoMoreFlags;	// set this if -- is encountered.
	int ArgsCounted;
	std::vector<std::string> argv;
	int argc;
	RUMBA::Log log;
};

class ArgHandlerException
: public RUMBA::Exception
{
public:
	ArgHandlerException(const std::string& s)
		: RUMBA::Exception(std::string("Program invocation error: ") + s){}
};

class MissingArgumentException
: public ArgHandlerException
{
public:
	MissingArgumentException(const std::string& s)
		: ArgHandlerException(std::string("Missing Argument: ")+s){}
};

class InvalidArgumentException
: public ArgHandlerException
{
public:
	InvalidArgumentException(const std::string& s)
		: ArgHandlerException(std::string("Invalid Argument: ")+s){}

};

class DuplicateArgumentException
: public ArgHandlerException
{
public:
	DuplicateArgumentException(const std::string& s)
		: ArgHandlerException(std::string("Duplicate Argument: ")+s){}
};

} // namespace RUMBA

#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
