#!/usr/bin/env python

import pandas
import pyproj
import sys


mis = pandas.DataFrame.from_csv(sys.argv[1], sep='\t')

olat = mis['lat'][0]
olon = mis['lon'][0]

projStr = '+proj=tmerc +lon_0={} +lat_0={} +units=m'.format(olon, olat)
p = pyproj.Proj(projStr)

filename = '{}.xml'.format(sys.argv[1])
f = open(filename, 'w')

f.write('<?xml version="1.0" standalone="no" ?>\n<mission>\n')
f.write('\t<desc>\n\t\t{}\n\t</desc>\n'.format(filename))
f.write('\t<global>\n')
f.write('\t\t<location lat="{}" lon="{}" />\n'.format(olat, olon))
f.write('\t\t<turn_radius m="5" />\n')
f.write('\t\t<drop_distance m="4" />\n')
f.write('\t\t<drop_angle deg="40" />\n')
f.write('\t\t<mission_timeout t = "1"/>\n')
f.write('\t\t<!-- Mission timeout not implemented. To have the same drop distance in the corners use drop_angle [rad] = drop_distance / turn_radius -->\n')
f.write('\t</global>\n')


for k in range(0, len(mis['lat'])):
	y, x = p(mis['lon'][k], mis['lat'][k])
	
	if mis['depth_option'][k] == 'D':
		z = mis['dfs'][k]
		depth_mode = 'depth'
	else:
		z = mis['hfb'][k]
		depth_mode = 'altitude'


	f.write('\t<primitive>\n')

	f.write('\t\t<goto>\n')
	
	
	f.write('\t\t\t<position x="{0:.2f}" y="{1:.2f}" z="{2:.2f}"/>\n'.format(x, y, z))
	f.write('\t\t\t<heading deg="{0:.2f}" />\n'.format(mis['dir_degs'][k]))
	f.write('\t\t\t<timeout t="{0:.2f}" />\n'.format(mis['time_mins'][k]*480))
	f.write('\t\t\t<velocity x="{}" />\n'.format(mis['speed_kn'][k]*0.5))
	f.write('\t\t\t<depth mode = "{}" />\n'.format(depth_mode))
	if mis['is_cam1'][k]:
		f.write('\t\t\t<command device="camera" onoff="start" freq="2.0"/>\n')
	else:
		f.write('\t\t\t<command device="camera" onoff="stop"/>\n')

	f.write('\t\t</goto>\n')
		
	f.write('\t</primitive>\n')
    
f.write('</mission>\n')
f.close()




