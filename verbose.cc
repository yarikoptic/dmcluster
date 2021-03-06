
/*emacs: -*- mode: c++-mode; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: t -*- */
/*ex: set sts=4 ts=4 sw=4 noet:*/
//---------------------------- =+- C / C++ -+= ----------------------------
/**
 * @file      verbose.cc
 * @date      Thu Oct  4 11:40:17 2007
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

#include "verbose.h"

VerboseOutput vout(std::cerr, 1);
