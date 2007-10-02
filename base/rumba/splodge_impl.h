#ifndef RUMBA_SPLODGE_IMPL_H
#define RUMBA_SPLODGE_IMPLH

#include <string>
#include <sstream>
#include <iostream>


namespace RUMBA
{
class SplodgeAtom {
public:
	virtual SplodgeAtom* clone() const = 0;
	virtual bool asShort(short& x) const=0;
	virtual bool asInt(int& x) const=0;
	virtual bool asFloat(float& x) const=0;
	virtual bool asDouble(double& x) const=0;
	virtual std::string asString() const=0;
	virtual double asDouble() const=0;
	virtual int asInt() const=0;
	virtual bool isNull();

	virtual ~SplodgeAtom();

};

class SplodgeNull : public SplodgeAtom {
	public: 
		SplodgeNull();
		virtual SplodgeNull* clone() const;
		virtual bool isNull();
		virtual bool asShort(short& x ) const;
		virtual bool asInt(int& x ) const;
		virtual bool asFloat(float& x ) const;
		virtual bool asDouble(double& x ) const;
		virtual std::string asString() const;
		virtual int asInt() const;
		virtual double asDouble() const;
};


class SplodgeNumber : public SplodgeAtom {
public:
	SplodgeNumber(double x):n(x){
#ifdef TEST_SPLODGE
		std::cout<<"birth"<<std::endl;
#endif
	}
	virtual SplodgeNumber* clone() const;

	virtual bool asShort(short& x) const;
	virtual bool asInt(int& x) const;
	virtual bool asFloat(float& x) const;
	virtual bool asDouble(double & x) const;
	virtual std::string asString() const;
	virtual int asInt() const;
	virtual double asDouble() const;

	virtual ~SplodgeNumber();

private:
	double n;	

};

class SplodgeString : public SplodgeAtom {
public:
	SplodgeString(std::string s);
	virtual SplodgeString* clone() const;
	virtual bool asShort(short& ) const;
	virtual bool asInt(int& ) const; 
	virtual bool asFloat(float& ) const;
	virtual bool asDouble(double& ) const;
	virtual std::string asString() const;
	virtual int asInt() const; 
	virtual double asDouble() const;
	virtual ~SplodgeString();

private: 
	std::string s;
	

};

}; /* namespace RUMBA */

#endif /* RUMBA_SPLODGE_IMPL_H */
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
