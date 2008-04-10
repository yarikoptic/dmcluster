#include <rumba/arghandler.h>
#include <rumba/parse.h>
#include <algorithm>


void destroy_stdargs();

namespace RUMBA
{




std::vector<Argument>& 
StdArgs()
{
	static std::vector<Argument>*  v = new std::vector<Argument>;
	if (v->empty())
	{
	v->push_back(Argument ( "infile", RUMBA::ALPHA, 'i' ));
	v->push_back(Argument ( "outfile", RUMBA::ALPHA , 'o'));
	v->push_back(Argument ( "verbose", RUMBA::FLAG, 'v'));
	v->push_back(Argument ( "help", RUMBA::FLAG, 'h'));
	atexit(destroy_stdargs);
	}
	return *v;
}

//	typedef std::list<RUMBA::Splodge> SplodgeList ;
}
void destroy_stdargs()
{
	std::vector<RUMBA::Argument>& x = RUMBA::StdArgs();
	delete &x;
}
using namespace RUMBA;

Argument::Argument ( std::string longArg, enum Argtype type, char shortArg )
    : longArg(longArg),  shortArg(shortArg),type(type),
    required(false), defaultValue(RUMBA::Splodge()), multi(false)
    { }

Argument::Argument ( 
        std::string longArg, 
        enum Argtype type, 
        char shortArg, 
        const RUMBA::Splodge& defaultValue, 
        bool required  , 
        bool multi  
)
: longArg(longArg),  shortArg(shortArg),type(type),
required(required), defaultValue(defaultValue), multi(multi)
{ }

Argument::Argument()
: longArg(""), shortArg('\0'), type(RUMBA::FLAG),required(false),
		defaultValue(RUMBA::Splodge()),multi(false)
{}


Argument::~Argument()
{

}


ArgHandler::ArgHandler(int argc, char** argv, Argument* args )
	: NoMoreFlags(false), ArgsCounted(0), argv( argv, argv+argc),argc(argc)
{
//	std::copy ( StdArgs().begin(), StdArgs().end(), std::back_inserter (ArgsAvailable) );

	// prefer user supplied arguments to defaults
	if ( args)
		for ( Argument* aPtr = args; aPtr->longArg != ""; aPtr++ )
			ArgsAvailable.push_back ( *aPtr );

	for ( std::vector<RUMBA::Argument>::const_iterator it =StdArgs().begin();
			it != StdArgs().end(); ++it )
	{
		ArgsAvailable.push_back(*it);
	}

	init();
	checkRequired();
}

void ArgHandler::setMultiDefaultArg(const char* argname)
{

	for ( std::vector<Argument>::iterator aPtr = StdArgs().begin(); 
			aPtr != StdArgs().end(); 
			aPtr++ )
		if (aPtr->longArg == argname )
			aPtr->multi = true;
}

void ArgHandler::setRequiredDefaultArg(const char* argname)
{
	for ( std::vector<Argument>::iterator aPtr = StdArgs().begin(); 
			aPtr != StdArgs().end(); 
			aPtr++ )
		if (aPtr->longArg == argname )
			aPtr->required = true;
}

void ArgHandler::init()
{
	initMulti(); // load MultiArgv with blanks
	std::string argument;
	std::string err;

	while ( ArgsCounted < argc)
	{
		argument = argv[ArgsCounted];

		++ArgsCounted;

		if ( NoMoreFlags || argument[0] != '-' )
		{
			if ( ArgsCounted > 1 )
            {
                // special case: backslash-escape the dash
                if (argument[0] == '\\' && argument[1] == '-' )
                    Loose.push_back(argument.substr(1,std::string::npos));
                else
				    Loose.push_back(argument);
            }
		}
		else if ( argument.substr ( 0,2 ) == "--" )
		{
			processArg ( argument.substr(2,std::string::npos) );
		}
		else // argument[0] == '-'
		{
			if ( argument.length() >2 ) 
			{
				err = std::string("ill formed argument ") + argument;
				throw ArgHandlerException(err);
			}
			processShortArg ( argument.substr(1,std::string::npos) );
		}
	}
	loadDefaultArgs();
}

void ArgHandler::loadDefaultArgs()
{
	for (std::vector<Argument>::const_iterator it = ArgsAvailable.begin();
			it != ArgsAvailable.end();
			++it
			)
	{
		if ( ! Argv.count(it->longArg) &&  ! it->defaultValue.isNull() )
			Argv[it->longArg] = it->defaultValue;

	}
}



void ArgHandler::initMulti()
{
	std::vector<RUMBA::Splodge> blank;

	for ( std::vector<Argument>::const_iterator it = ArgsAvailable.begin(); 
			it != ArgsAvailable.end();
			++it)
	{
		if ( it->multi )
			MultiArgv.insert ( std::make_pair ( it->longArg, blank ));
	}

}

bool ArgHandler::arg(const char* argname)
{
	std::map<std::string, RUMBA::Splodge>::iterator it;
	it = Argv.find( argname );
	return ( it != Argv.end() );
}

void ArgHandler::arg_or_loose(const char* argname, std::string& val)
{
	if (arg(argname))
	{
		arg(argname,val);
	}
	else if (!Loose.empty())
	{
		val = Loose.front();
		Loose.erase(Loose.begin());
	}
	else
	{
		throw MissingArgumentException(val); 
	}

}

void ArgHandler::arg(const char* argname, std::string& val)
{
	val = lookup(argname)->second.asString();
}

void ArgHandler::arg(const char* argname, double& val)
{
	double x;
	x = lookup(argname)->second.asDouble();
	val = x;
}

void ArgHandler::arg(const char* argname, float& val)
{
	double x;
	x = lookup(argname)->second.asDouble();
	val = x;
}

void ArgHandler::arg(const char* argname, int& val)
{
	int x;
	x = lookup(argname)->second.asInt();
	val = x;
}

void ArgHandler::arg(const char* argname, uint& val)
{
	int x;
	x = (uint) lookup(argname)->second.asInt();
	val = x;
}

void ArgHandler::arg(const char* argname, short& val)
{
	int x;
	x = lookup(argname)->second.asInt();
	val = x;
}

void ArgHandler::arg(const char* argname, char& val)
{
	int x;
	x = lookup(argname)->second.asInt();
	val = x;
}

std::map<std::string, RUMBA::Splodge>::const_iterator 
ArgHandler::lookup(const char* argname)
{
	std::map<std::string, RUMBA::Splodge>::iterator it;

	it = Argv.find( argname );
	if ( it == Argv.end() )
		throw ArgHandlerException( std::string("Lookup failed: ") + argname );
	return it;
}




const std::vector<RUMBA::Splodge>& 
ArgHandler::multiarg(const char* argname)
{
	std::map<std::string, std::vector<RUMBA::Splodge> >::iterator it;
	
	it = MultiArgv.find( argname );
	if ( it == MultiArgv.end() )
		throw InvalidArgumentException(argname);
	return it->second;
}


void ArgHandler::print(std::ostream& s)
{
	s << "Arguments Available" << std::endl;
	s << "-------------------" << std::endl;
	for ( std::vector<Argument>::const_iterator it = ArgsAvailable.begin();
			it != ArgsAvailable.end(); ++it )
	{
		s << it->shortArg << " " <<  it->longArg << std::endl;
	}

	s << "Arguments:" << std::endl;
	s << "----------" << std::endl;

	for ( std::map<std::string, RUMBA::Splodge>::const_iterator it = Argv.begin();
			it != Argv.end(); 
			++it
			)
		s << it -> first << " " << it->second << std::endl;

	s << std::endl;


	s << "Multi Arguments:" << std::endl;
	s << "----------------" << std::endl;
	for ( std::map<std::string, std::vector<RUMBA::Splodge> >::const_iterator it = MultiArgv.begin();
			it != MultiArgv.end(); 
			++it
			)
		for ( std::vector<RUMBA::Splodge>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
			s << it->first<< " " << *it2 << std::endl;

	s << std::endl;

	s << "Loose:" << std::endl;
	s << "------" << std::endl;

	for ( std::vector<std::string>::const_iterator it = Loose.begin();
			it != Loose.end(); 
			++it 
			)
		s << *it << std::endl;

	s << "------" << std::endl;
	s << " end  " << std::endl;
	s << "------" << std::endl;
}

void ArgHandler::processArg(std::string arg)
{
	// find arg in table:
	std::vector<Argument>::const_iterator it;
	for ( 
			it = ArgsAvailable.begin(); 
			it != ArgsAvailable.end() && it->longArg != arg; 
			++it 
		)
		;
	if ( it == ArgsAvailable.end() )
		throw InvalidArgumentException(arg);

	if ( ! it->multi )
	{
		if ( Argv.find(arg) != Argv.end() ) // error
			throw DuplicateArgumentException(arg);	// should give some message about duplicate arg.
		getNextArg (it->longArg, it->type );
	}
	else 
	{
		getNextMultiArg (it->longArg, it->type );		
	}
	
}

void ArgHandler::processShortArg(std::string arg)
{
	log.logName() << "arghandler: Entering shortArg() " << "\n";
	std::string longArg;
	for ( std::vector<Argument>::const_iterator it = ArgsAvailable.begin();
			it != ArgsAvailable.end();
			++it 
			)
	{
		log.logName() << it->shortArg << " " << arg[0] << "\n";
		if ( it->shortArg == arg[0] )
			longArg = it->longArg;
	}
	log.logName() << "arghandler:" << longArg << "\n";

	if ( longArg.empty() )
		throw InvalidArgumentException( arg); 
	else 
	{
		processArg ( longArg );
	}
	log.logName() << "arghandler: Leaving shortArg() " << "\n";

}

void ArgHandler::getNextArg(std::string arg, enum Argtype t, bool multi)
{
	RUMBA::Splodge result;

	if ( t == RUMBA::FLAG )
	{
		result = 1;
	}
	else if ( t == RUMBA::NUMERIC )
	{
		if ( ArgsCounted >= static_cast<int>(argv.size()) )
			throw ArgHandlerException ( arg + " requires an argument" );
		try {
			result = RUMBA::stream_cast<double> ( argv[ ArgsCounted ] );
			++ArgsCounted;
		}
		catch (...)
		{
			throw InvalidArgumentException(argv[ArgsCounted]); 
		}
	}
	else if ( t == RUMBA::ALPHA )
	{
		if ( ArgsCounted >= static_cast<int>(argv.size()) )
			throw ArgHandlerException ( arg + " requires an argument" );
		result = argv[ArgsCounted];
		++ArgsCounted;
	}

	
	if (!multi)	
		Argv.insert ( std::make_pair ( arg, result ) );
	else
		MultiArgv[arg].push_back ( result );



}


void ArgHandler::getNextMultiArg(std::string arg, enum Argtype t)
{
	while ( ArgsCounted < argc && argv[ArgsCounted][0] != '-'  )
		getNextArg(arg, t, 1);
}

bool ArgHandler::checkRequired()
{
	if ( arg("help"))
		return true;

	for ( 
			std::vector<Argument>::const_iterator it = ArgsAvailable.begin(); 
			it != ArgsAvailable.end(); 
			++it 
		)	
	{
		if ( it->required )
		{
			if (!it->multi)
			{
				try {
					lookup(it->longArg.c_str()); 
				}
				catch (...) 
				{
					throw MissingArgumentException(it->longArg); 
				}
			}
			else
			{
				if ( MultiArgv[it->longArg].empty() )
				{
					throw MissingArgumentException(it->longArg);
				}
				else
					return true;
			}
		}
	}
	return false;
}

/*
int main(int argc, char** argv)
{
	ArgHandler::setMultiDefaultArg("infile");
	ArgHandler::setRequiredDefaultArg("infile");

	try {
		ArgHandler a (argc, argv);
		a.print();
		SplodgeList infile;
	
		infile = a.multiarg("infile");
	}
	catch (MissingArgumentException& s)
	{
		cout << "Error, missing required argument: " << s.error() << endl;
	}
	catch (DuplicateArgumentException& s)
	{
		cout << "Duplicate argument: " << s.error() << endl;
	}
	catch (ArgHandlerException& s)
	{
		cout << "Error: " << s.error() << endl;
	}


}
*/
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
