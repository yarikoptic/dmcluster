#ifndef RUMBA_EXCEPTION_H
#define RUMBA_EXCEPTION_H

#include <string>

namespace RUMBA
{
/**
  * This simple class is the base class for all Exception classes in the
  * RUMBA software. Hence one can use 
  * \code catch (RUMBA::Exception& e) \endcode to catch nonstandard
  * exceptions thrown by RUMBA software.
  */
class Exception {
public:
	Exception ();
	Exception ( std::string err );
	virtual ~Exception();
	//! Returns a diagnostic message
	std::string error();
protected:
	std::string error_message;
};

/**
  * Thrown if a checked operations requires division by zero.
  */
class DivByZero : public Exception 
{
public:
	DivByZero ();
	DivByZero(std::string err);
};

/**
  * Throw when a cast attempt fails.
  */
class BadConvert : public Exception
{
public:
	BadConvert () ;
	BadConvert (std::string err ) ;
};

/** 
  * This gets thrown if the data is bad.
  */
class BadFile : public Exception
{
public:
	BadFile () ;
	BadFile (std::string err );
};

/**
  * Parent class for exceptions thrown by ArgHandler.
  */
class BadArgument : public Exception
{
public:
	BadArgument ();
	BadArgument (std::string err );
};


}//namepsace RUMBA

#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
