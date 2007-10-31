/*emacs: -*- mode: c++-mode; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: t -*- */
/*ex: set sts=4 ts=4 sw=4 noet:*/
//---------------------------- =+- C / C++ -+= ----------------------------
/**
 * @file      verbose.h
 * @date      Thu Oct  4 11:34:53 2007
 * @brief
 *
 *
 *  RUMBA project                    Psychology Department. Rutgers, Newark
 *  http://psychology.rutgers.edu/RUMBA       http://psychology.rutgers.edu
 *  e-mail: yoh@psychology.rutgers.edu
 *
 * DESCRIPTION (NOTES):
 *
 * COPYRIGHT: Yaroslav Halchenko 2007
 *
 * LICENSE:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the 
 *  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 *
 * On Debian system see /usr/share/common-licenses/GPL for the full license.
 */
//-----------------\____________________________________/------------------
#ifndef VERBOSE_H
#define VERBOSE_H

// Either to compile verbose detailed output (verbose printouts specific
// for some part of the code
#define VERBOSE_DETAILED

#ifdef VERBOSE_DETAILED
#define VDOUT(lev, args...) vout << lev << args;
#else
#define VDOUT(lev, args...)
#endif

#define POINT_OUT(p) p.x() << ", "

#include <iostream>

class VerboseOutputWorker
{
private:
    std::ostream & out;
    const bool doout;
public:
    VerboseOutputWorker(std::ostream & lout, const bool ldoout=true)
        :out(lout), doout(ldoout){}

    template <class T>
    VerboseOutputWorker& operator << (const T smth)
    {
        if (doout) out << smth;
        return *this;
    }

};

/*
  Reserved detailed levels of verbose output
   91  -- input
   106 -- bucket creation
 */
class VerboseOutput
{
private:
    VerboseOutputWorker doout, noout;
    int level;
public:
    VerboseOutput(std::ostream & lout,
                  const int llevel=1)
        : doout(lout, true)
        , noout(lout, false)
        {
            setLevel(llevel);
        }

    void setLevel(const int llevel) { level = llevel; }

    VerboseOutputWorker& operator << (const int clevel)
    {
		bool indent = (clevel>=0);
		int clevel_ = abs(clevel);
        // I am stupid... could not figure out cleaner way... pardon me
		// detailed levels start > 10. Then we output only debugs
		//  on levels up to 4 and then that specific detailed level.
        if ((level <= 10 && clevel_ <= level)					\
			|| (level > 10 && (clevel_ == level || clevel_<=4)))
        {
			if (indent)
			{
				int indent = (clevel_<=10?clevel_-1:10);
				for (int i=0; i<indent; i++) doout << " ";
			}
            return doout;
        }
        else                 return noout;
    }
};


// should be a singleton (can be properly implemented later on XXX) everywhere
extern VerboseOutput vout;

#endif
