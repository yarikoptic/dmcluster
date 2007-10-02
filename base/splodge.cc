#include <rumba/splodge.h>
#include <rumba/parse.h>

#include <rumba/splodge_impl.h>

#ifdef TEST_SPLODGE
using std::cout; 
using std::endl;
#endif


using namespace RUMBA;

//----------------------------------------
//	SplodgeAtom
//----------------------------------------

SplodgeAtom::~SplodgeAtom() {}
bool SplodgeAtom::isNull() { return false; }

//----------------------------------------
// 		SplodgeNull
//----------------------------------------

SplodgeNull*  SplodgeNull::clone() const { return new SplodgeNull; }
SplodgeNull::SplodgeNull(){}
bool SplodgeNull::asShort(short&) const{ return false; }
bool SplodgeNull::asInt(int&) const{return false; }
bool SplodgeNull::asFloat(float&) const{return false; }
bool SplodgeNull::asDouble(double&) const{return false; }
bool SplodgeNull::isNull(){ return true; }
std::string SplodgeNull::asString() const{return "[NULL]"; }
int SplodgeNull::asInt() const
{ 
	throw RUMBA::Exception("Null splodge exception");  
}
double SplodgeNull::asDouble() const
{ 
	throw RUMBA::Exception("Null splodge exception");  
}





//----------------------------------------
// 	SplodgeNumber
//----------------------------------------

SplodgeNumber* SplodgeNumber::clone() const { return new SplodgeNumber(n); }
bool SplodgeNumber::asShort(short& x)const
{ 	
	x = (short)n; 
	return true; 
}
bool SplodgeNumber::asInt(int& x)const
{ 
	x = (int)n; 
	return true; 
}
bool SplodgeNumber::asFloat(float& x)const
{ 
	x = n; 
	return true; 
}
bool SplodgeNumber::asDouble(double & x)const
{ 
	x = n; 
	return true; 
}
std::string SplodgeNumber::asString() const
{
	std::ostringstream s;
	s << n;
	return s.str();
}

double SplodgeNumber::asDouble() const
{
	return n;
}

int SplodgeNumber::asInt() const
{
	return static_cast<int>(n);
}

SplodgeNumber::~SplodgeNumber()
{
#ifdef TEST_SPLODGE
		std::cout<<"death"<<std::endl;
#endif
}

//-------------------------------------------
//		SplodgeString
//-------------------------------------------

SplodgeString* SplodgeString::clone() const { return new SplodgeString(s); }

SplodgeString::SplodgeString(std::string s) : s(s)
{
#ifdef TEST_SPLODGE
	std::cout<<"birth"<<std::endl;
#endif
}
bool SplodgeString::asShort(short& ) const 
{ 
	return false; 
}
bool SplodgeString::asInt(int& ) const 
{ 
	return false; 
}
bool SplodgeString::asFloat(float& ) const 
{ 
	return false; 
}
bool SplodgeString::asDouble(double& ) const 
{ 
	return false; 
}

int SplodgeString::asInt() const
{
	int x;
	if ( asInt(x))
		return x;
	else
		throw RUMBA::BadConvert ( std::string("Couldn't convert to string: ") + s );
}

double SplodgeString::asDouble() const
{
	double x;
	if (asDouble(x))
		return x;
	else
		throw RUMBA::BadConvert ( std::string("Couldn't convert to string: ") + s );
}

std::string SplodgeString::asString() const
{
	return s;
}
SplodgeString::~SplodgeString()
{
#ifdef TEST_SPLODGE
	std::cout<<"death"<<std::endl;
#endif
}


Splodge::Splodge() 
	: rep(0)
{ 
	rep = new SplodgeNull; 
//	std::cerr << "[NULL]" << rep  << std::endl;
}

Splodge::Splodge(const SplodgeAtom* x)
	: rep(x->clone())
{}


Splodge::Splodge(short x) 
{ 
	rep = new SplodgeNumber( (double)x ); 
	// std::cerr << rep  << std::endl;

}
Splodge::Splodge(int x) 
{ 
	rep = new SplodgeNumber( (double)x ); 
	// std::cerr << rep  << std::endl;

}
Splodge::Splodge(float x) 
{ 
	rep = new SplodgeNumber( (double)x ); 
	// std::cerr << x << " " << rep  << std::endl;

}
Splodge::Splodge(double x) 
{ 
	rep = new SplodgeNumber( x ); 
	// std::cerr << x << " " << rep  << std::endl;

}
Splodge::Splodge(std::string s) 
{ 
	rep = new SplodgeString ( s ); 
	// std::cerr << s << " " << rep  << std::endl;

}
Splodge::Splodge(const char* s) 
{ 
	rep = new SplodgeString (std::string(s)); 
	// std::cerr << s << " " << rep  << std::endl;

}
Splodge::~Splodge() 
{ 
// std::cerr << "Deleting: " << this << " " << rep  << std::endl;	
	delete rep;


}

Splodge::Splodge(const Splodge& x):rep(x.rep->clone())
{ 
}

Splodge& Splodge::operator=(const Splodge& x)
{
	SplodgeAtom* tmp = x.rep->clone();
	delete rep;
	rep = tmp;
	return *this;
}
			


Splodge& Splodge::operator=(short x)
{ 
	delete rep; 
	rep = new SplodgeNumber((double)x);  
	return *this;
}
Splodge& Splodge::operator=(int x)
{ 
	delete rep; 
	rep = new SplodgeNumber((double)x);  
	return *this;
}
Splodge& Splodge::operator=(float x)
{ 
	delete rep; 
	rep = new SplodgeNumber((double)x);  
	return *this;
}
Splodge& Splodge::operator=(double x)
{ 
	delete rep; 
	rep = new SplodgeNumber(x); 
	return *this;
}
Splodge& Splodge::operator=(std::string x)
{
	delete rep; 
	rep = new SplodgeString(x); 
	return *this;
}
Splodge& Splodge::operator=(const char* x)
{
	delete rep; 
	rep = new SplodgeString(std::string(x)); 
	return *this;
}


bool Splodge::asShort(short& x) const 
{ 
	return rep->asShort(x); 
}
bool Splodge::asInt(int& x) const 
{ 
	return rep->asInt(x); 
}
bool Splodge::asFloat(float& x) const 
{ 
	return rep->asFloat(x); 
}
bool Splodge::asDouble(double& x) const 
{ 
	return rep->asDouble(x); 
}
std::string Splodge::asString() const 
{ 
	return rep->asString(); 
}

int Splodge::asInt() const
{
	return rep->asInt();
}

double Splodge::asDouble() const
{
	return rep->asDouble();
}

bool Splodge::isNull() const
{
	return rep->isNull();
}

std::ostream& RUMBA::operator<< (std::ostream& s, const Splodge& x)
{ 
	s << x.asString();
	return s;
}



#ifdef TEST_SPLODGE
// one can perform a memory audit like this:
// ./a.out | grep ^birth | wc
// ./a.out | grep ^death | wc
// This will give a count of constructor and destructor calls.
// The results should be the same. If there's more births, there's a memory
// leak. If there's more deaths, something really wierd is going on. 


void doItAll (RUMBA::Splodge& x)
{
	double d(0);
	float f(0);
	short s(0);
	int i(0);
	if ( x.asShort(s) )
		std::cout << "asShort: " << s << std::endl;
	if ( x.asInt(i) )
		std::cout << "asInt: " << i << std::endl;
	if ( x.asFloat(f) )
		std::cout << "asFloat: " << f << std::endl;
	if ( x.asDouble(d) )
		std::cout << "asDouble: " << f << std::endl;

	std::cout << "asString: " << x.asString() << std::endl;


}
int main()
{
	RUMBA::Splodge s = 5.4;
	doItAll(s);
	RUMBA::Splodge t;
	doItAll(t);

	// test copy constructors
	RUMBA::Splodge u = s;
	doItAll(u);
	RUMBA::Splodge w("Hello world");
	u = w;
	RUMBA::Splodge x(u);
	doItAll(u);
	doItAll(x);


}

#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
