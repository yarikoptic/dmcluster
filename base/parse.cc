#include <string>
#include <list>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>

#include <rumba/exception.h>
#include <rumba/parse.h>




void RUMBA::strip ( std::string& s, const char* ignore)
{
	std::string::size_type x = s.find_first_not_of(ignore);
// if our string is only contains "ignored" chars, set it to the empty string.string
	if ( x == std::string::npos ) 
	{
		s = "";
	}
	else
// otherwise, find the last "non-ignored character" amd reassign the right substring.
// test the special case where y == npos!
	{
		std::string::size_type y = s.find_last_not_of(ignore);
		if ( y == std::string::npos )
			s.assign(s,x,y);
		else 
			s.assign( s, x, y-x+1 ); 
	}
}


std::list<std::string> RUMBA::tokenizeCsvLine( std::string line, char delim  )
{
	std::string tmp;
	std::list<std::string> v;
	std::istringstream strin (line);
	while ( getline(strin,tmp,delim))
		v.push_back(tmp);
	return v;	
}

// If the key/value pair is valid, the key is non-empty.
void RUMBA::parseRcLine (std::string line, std::string& key, std::string& value)
{
	std::list<std::string> tokens;
	strip(line);
	if ( line.empty() || line[0] == '#' )
	{
		key = "";
		value = "";
		return;
	}

	tokens = tokenizeCsvLine ( line, '=' );
	if ( tokens.empty() )
	{
		std::cerr << "Something wierd is going on\n";  // this shouldn't happen!
		key = "";
		value = "";
		return;
	}

	key = tokens.front();
	strip(key);
	tokens.pop_front();
	if ( tokens.empty() )
		value = "";
	else 
	{
		value = tokens.front();
		strip(value);
	}
}

std::map<std::string, std::string> RUMBA::parseRc(std::istream & fin)
{
	std::map<std::string, std::string> m;
	std::string line;
	std::string key,val;
	std::list<std::string> tokens;

	while ( getline ( fin, line ))
	{
		strip(line);
		if ( ! line.empty() && line[0] == '#' )
			continue;
		tokens = tokenizeCsvLine ( line, '=' );
		if ( tokens.empty() )
			continue;
		key = tokens.front();
		strip(key);
		tokens.pop_front();
		if ( tokens.empty() )
			val = "";
		else 
		{
			val = tokens.front();
			strip(val);
		}
		m.insert ( make_pair(key,val) );
	}
	return m;
}

bool RUMBA::rcFind ( std::istream& in, std::string key, std::string& val )
{
	if (!in)
		return false;
	in.seekg(0);
	std::string line,k,v;
	
	while ( getline ( in, line ) )
	{
		parseRcLine	(line,k,v);
		if ( k == key )
		{
			val = v;
			return true;
		}
	}
	return false;
}

std::string RUMBA::fileExtension(std::string s)
{
	std::string ext;
	if ( s.find_last_of ("." ) != std::string::npos )
		ext.assign( s, s.find_last_of("."), s.length()); 

	return ext;
}

// remove everything without a given extension Note that one 
// should include the "."
void RUMBA::pruneFileList(std::list<std::string>& v, std::string extension)
{
	std::list<std::string>::iterator it = v.begin();
	while(  it != v.end() && (! v.empty() ) )
	{
		if ( fileExtension(*it) != extension )
			it = v.erase(it);
		else
			++it;
	}
}

std::string RUMBA::extractQuotedString(std::string s)
{
	RUMBA::strip(s);
	std::string sep;
	if ( (s.find_first_of( "\"'" ) != 0) || s.length() == 1 )
		return s;
	else
	{
		if ( s[0] == '"' ) 
			sep = "\"";
		else 
			sep = "'";
		
		if ( s.find_last_of ( sep ) != std::string::npos )
			return s.substr ( 1, s.find_last_of (sep) - 1 );
		else
			return s;
	}
}

std::string RUMBA::get_dlname(std::string filename)
{
	std::ifstream fin(filename.c_str());
	if ( !fin )
		return "";
	std::string value;
	if ( ! RUMBA::rcFind ( fin,std::string("dlname"), value ) )
		return "";
	return RUMBA::extractQuotedString (value);
}
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
