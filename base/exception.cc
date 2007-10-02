#include <rumba/exception.h>
#include <iostream>

RUMBA::Exception
::~Exception(){}
RUMBA::Exception
::Exception () : error_message("") {
	#ifdef DEBUG
	std::cerr << "Calling RUMBA::Exception ( ) " << std::endl;
	#endif

}
RUMBA::Exception
::Exception ( std::string err ) : error_message ( err ) {
	#ifdef DEBUG
	std::cerr << "Calling RUMBA::Exception (string ) " << std::endl;
	#endif
}

std::string
RUMBA::Exception::
error() { return error_message; }

RUMBA::DivByZero
::DivByZero (){};

RUMBA::DivByZero
::DivByZero(std::string err)
: RUMBA::Exception(err){};

RUMBA::BadConvert
::BadConvert () {}

RUMBA::BadConvert
::BadConvert (std::string err )
: RUMBA::Exception(err) {}

RUMBA::BadFile
::BadFile () {};

RUMBA::BadFile
::BadFile (std::string err )
: RUMBA::Exception ( err )
{
	#ifdef DEBUG
	std::cerr << "Calling RUMBA::BadFile ( string ) " << std::endl;
	#endif
}

RUMBA::BadArgument
::BadArgument () {};

RUMBA::BadArgument
::BadArgument (std::string err )
: RUMBA::Exception ( err ) {}

// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
