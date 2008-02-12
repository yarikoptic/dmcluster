#!/usr/bin/python
#emacs: -*- mode: python-mode; py-indent-offset: 4; tab-width: 4; indent-tabs-mode: t -*- 
#ex: set sts=4 ts=4 sw=4 noet:
#------------------------- =+- Python script -+= -------------------------
"""
 @file      plot_output.py
 @date      Thu Feb  7 12:13:01 2008
 @brief


  RUMBA project                    Psychology Department. Rutgers, Newark
  http://psychology.rutgers.edu/RUMBA       http://psychology.rutgers.edu
  e-mail: yoh@psychology.rutgers.edu

 DESCRIPTION (NOTES):

 COPYRIGHT: Yaroslav Halchenko 2008

 LICENSE:

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the 
  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
  MA 02110-1301, USA.

 On Debian system see /usr/share/common-licenses/GPL for the full license.
"""
#-----------------\____________________________________/------------------

__author__ = 'Yaroslav Halhenko'
__revision__ = '$Revision: $'
__date__ = '$Date:  $'
__copyright__ = 'Copyright (c) 2008 Yaroslav Halchenko'
__license__ = 'GPL'

import sys, os, re
import numpy as N
import pylab as pl

iFileName = sys.argv[1]

params = ([], [])						# crit/cluster_size
radiuses, thresholds = [], []
bestline = ''
try:
	f = open(iFileName)
	while True:
		line = f.next()
		if re.match("^\s*Sweeping over different values.*", line):
			line = f.next()
			entries = re.split('[ \t]+', line.strip())
			radiuses = [int(x) for x in entries]
		elif len(radiuses)>0:
			if line.startswith("thr="):
				res = re.match('thr=(?P<thr>\d+)\s*:', line)
				resd = res.groupdict()
				thresholds.append(int(resd['thr']))

				# now we need to extract values for that threshold
				line_ = re.sub("thr=[\d\s:]*", "", line)
				line_ = re.sub("nan", "0", line_)
				line_crits = [float(x) for x in re.sub("/[\d.]+\s*", " ", line_).split()]
				line_css = [int(x) for x in re.sub("\s*[\d.]+/", " ", line_).split()]
				params[0].append(line_crits)
				params[1].append(line_css)
			elif line.startswith("crit="):
				bestline = "Best: " + line
				resbest = re.match("crit=(?P<crit>[\d.na]+)\s+#clusters=(?P<nc>[\d]+)\s+threshold=(?P<thr>\d+)\s+radius=(?P<radius>\d+)\s*", line)
				bestd = resbest.groupdict()

except StopIteration:
	f.close()
except Exception, e:
	print "Troublema", e
	raise

params = N.array(params)

fi = pl.figure(figsize=(8,7))
#pl.subplots_adjust(hspace=0.3)

# plot criterion
#pl.subplot(1,2,1)
im = pl.imshow(params[0,...], interpolation='nearest',
			   aspect=len(radiuses)*1.0/len(thresholds),
			   cmap=pl.image.cm.Greys_r,
			   vmin=0)
maxnc = min(15, N.max(params[1,...]))
for i in xrange(len(thresholds)):
	pl.text(-1.2, len(thresholds)-1-i+0.5, str(int(thresholds[i])),
			fontsize=8,
			color=(0,0,0),
			horizontalalignment='right',verticalalignment='center')

	for j in xrange(len(radiuses)):
		nc = params[1,i,j]

		if nc > 0:
			if thresholds[i] == int(bestd['thr']) and \
				   radiuses[j] == int(bestd['radius']):
				color = (1, 0, 0)
			elif nc>15:
				color = (0,0,1)
			else:
				color = (0,0, 0.5+0.5*(1.0-(maxnc-nc)*1.0)/maxnc)
			pl.text(j+0.5, len(thresholds)-1-i+0.5, str(int(nc)), fontsize=8,
					color=color,
					horizontalalignment='center',verticalalignment='center')
#im.axes.xaxis.set_ticks(radiuses)
#im.axes.yaxis.set_ticklabels([str(x) for x in thresholds])
#im.axes.yaxis.set_ticks(thresholds)
pl.yticks([])
pl.xlabel('Radius')
pl.ylabel('Thresholds')
pl.colorbar()

pl.title(bestline, fontsize='small')

## plot numclusters
#pl.subplot(1,2,2)
#
#pl.imshow(params[1,...], interpolation='nearest',
#		  vmin=0)
#pl.colorbar()
#
# pl.show()

pl.savefig(iFileName + '.pdf')
