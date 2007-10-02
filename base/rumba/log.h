#ifndef RUMBA_LOG_H
#define RUMBA_LOG_H
#include <string>
#include <iostream>


namespace RUMBA {


/**
  * A class used for logging.  Each log object is assigned 
  * a serial number. This is especially useful for tracking the lifetime of
  * different objects. Typically, one creates the log by passing
  * the name of either the class or function being logged, and uses the
  logName() method to expose the stream, for example:
  \code 
  RUMBA::Log log ("classname");
  log.logName() << "Constructing log object" << "\n";
  \endcode
  * To log to a different file or stream, pass the address of the stream
  * you wish to log to as a constructor argument.
  * 
  */
class Log
{
public:
	~Log();

	Log(std::string Name="", std::ostream* s = 0);
	Log& logName();


	template<class T>
	Log& operator<<	(  T right ) 
	{ 
		if ( Out )
		{
			*Out << right ;
		}
		return *this;

	}

private:
	std::string Name;
	std::ostream* Out;
	int MySerial; // unique identifier
	static int Serial; // global object count

};


} // namespace RUMBA

#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
