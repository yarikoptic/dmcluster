#ifndef RUMBA_SPLODGE_H
#define RUMBA_SPLODGE_H
#include <string>



namespace RUMBA
{

	class SplodgeAtom;

/**
  * Splodge is a variant class. It's a useful tool for dealing with situations
  * where weak typing is appropriate. It's main function is to blur the 
  * distinction between numeric and string data (in this sense, it is 
  * much like perl's "scalars"). In the RUMBA software, it is used in the
  * implementation of the ArgHandler class, and classes designed to extract
  * data in several forms from image file headers.
  * 
  * The class is quite simple to use. To construct a Splodge, one typically
  * uses a string or a number. Numbers are all represented as doubles, while
  * strings are represented as strings.
  *
  * There are several methods to extract data. Note first that any splodge
  * can be converted to type std::string. Conversions to numeric types are
  * succesful only if the Splodge in question was constructed from a number,
  * or from another numeric Splodge. There are two types of conversions to
  * numeric data types -- one returns a bool if the conversion was succesful
  * and writes the result to a reference parameter (the parameter is 
  * unchanged if the conversion is unsuccesful). Another returns the result
  * of the conversion, and throws RUMBA::BadConvert if the conversion is
  * unsuccesful.
  * 
  *
  */
class 
Splodge 
{
public:
	//! General constructor. Can use this to extend Splodge class
	Splodge(const SplodgeAtom*);
	//! Default constructor. Creates a NULL Splodge.
	Splodge();
	//! Creates a numeric Splodge
	Splodge(short x);
	//! Creates a numeric Splodge
	Splodge(int x);
	//! Creates a numeric Splodge
	Splodge(float x);
	//! Creates a numeric Splodge
	Splodge(double x);
	//! Creates a string Splodge
	Splodge(std::string s);
	//! Creates a string Splodge
	Splodge(const char* s);
	~Splodge();

	//! Copy constructor.
	Splodge(const Splodge& x);

	//! Assignment operator.
	Splodge& operator=(const Splodge& x);

	//! Causes the left operand to take on the numeric value of the argument
	Splodge& operator=(short x);
	//! Causes the left operand to take on the numeric value of the argument
	Splodge& operator=(int x);
	//! Causes the left operand to take on the numeric value of the argument
	Splodge& operator=(float x);
	//! Causes the left operand to take on the numeric value of the argument
	Splodge& operator=(double x);
	//! Causes the left operand to take on the string value of the argument
	Splodge& operator=(std::string x);
	//! Causes the left operand to take on the string value of the argument
	Splodge& operator=(const char* x);


	/**
	 *  Returns true and writes the numeric value of the Splodge to the
	 * parameter if the Splodge does have a numeric value, otherwise 
	 * returns false and leaves the parameter unchanged.
	 */
	bool asShort(short& x) const;

//! Similar to above
	bool asInt(int& x) const ;
//! Similar to above
	bool asFloat(float& x) const;
//! Similar to above
	bool asDouble(double& x) const;
	/** 
	  * Converts the SPlodge to a string. Note that this is the one as*** 
	  * function in the calss that is gauranteed to succeed.
	  */
	std::string asString() const;
	//! Similar to other numeric conversions
	int asInt() const;
	//! Similar to other numeric conversions
	double asDouble() const;

	/**
	  * Returns true if the Splodge is NULL.
	  */
	bool isNull() const;



private:
	SplodgeAtom* rep;
};

std::ostream& operator<< (std::ostream& s, const RUMBA::Splodge& x);

} // namespace RUMBA



#endif
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
