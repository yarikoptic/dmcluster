#include <rumba/log.h>
#include <rumba/rumba_system.h>
#include <fstream>

#ifndef DEBUG
#define LOG_TO_FILE
#endif

int RUMBA::Log::Serial = 0;




namespace  RUMBA
{

class LogStream
{
public:
	static std::ostream* use();
		static void release();
private:
	static std::ostream* s;
	static int Count;
};
}

int RUMBA::LogStream::Count = 0;
std::ostream* RUMBA::LogStream::s = 0;


std::ostream* RUMBA::LogStream::use()
{
	if (!Count++)
	{
#ifdef LOG_TO_FILE
		s = new std::ofstream("rumba.log");
#else
		s = &std::cerr;
#endif
	}
	return s;
}



void RUMBA::LogStream::release()
{
	if (!--Count)
	{
#ifdef LOG_TO_FILE
		delete s;
#endif
	}
}


RUMBA::Log::~Log()
{
	if ( Out )
		*Out << std::flush;
	RUMBA::LogStream::release();
}

RUMBA::Log::Log(std::string Name, std::ostream* s )
	: Name(Name),  Out(s), MySerial(Serial)
{
        if (RUMBA::rumba_getenv("RUMBA_NO_DEBUG")) 
            Out = 0;
        else if (RUMBA::rumba_getenv("RUMBA_DEBUG") && !s) 
            Out = RUMBA::LogStream::use();
        else /* default */
            Out = 0;
	++Serial;
}

RUMBA::Log& RUMBA::Log::logName()
{
	if (Out)
		*Out << Name << " " << MySerial << ":";
	return *this;
}



#ifdef TEST_LOG

class Foo {
public:
Foo() : L(std::cerr, "Foo") { L.logName(); L << "Creating object" << endl; }
Foo(int x ) : L(std::cerr, "Foo") { L.logName(); L << "Creating object " << x << endl; }

~Foo()  { L.logName(); L << "Destroying object" << endl; }

private:
	RUMBA::Log L;
};

int main()
{
	Foo x; Foo y(3);
}

#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
