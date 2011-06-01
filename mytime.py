#! /usr/bin/env python

# convert a string presentation of timedelta to a good format

import datetime
t1 = datetime.datetime.now()
t2 = datetime.datetime.now()
s = str(t2 - t1)
#s = '17707561 days, 17:28:59'
#s = '17:28:59'

has_days = s.find('days')
if has_days == -1:
	s1 = s.split(':')
	s1 = '%sh %sm %ss' % (s1[0], s1[1], s1[2])
else:
	y = s[0:has_days].strip()
	s1 =s[has_days+6:].split(':')
	s1 = '%sd %sh %sm %ss' %(y, s1[0], s1[1], s1[2])
print s1
