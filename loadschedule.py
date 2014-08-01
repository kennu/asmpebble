#!/usr/bin/env python
from __future__ import print_function
import urllib2
import re
from datetime import datetime
from time import mktime

IGNORE_CATEGORIES = re.compile(r'.*Jury.*|.*Game.*')
IGNORE_LOCATIONS = re.compile(r'.*PMS.*|.*Infodesk.*')

print('#include "pebble.h"')
print('#include "schedule.h"')
print('struct program programs[] = {');

for line in urllib2.urlopen('http://www.assembly.org/summer14/schedule/assembly-summer-2014-all.ics').read().split('\n'):
    line = line.strip()
    if line == 'BEGIN:VEVENT':
        event = {}
    elif line == 'END:VEVENT':
        if not (IGNORE_CATEGORIES.match(event['CATEGORIES']) or IGNORE_LOCATIONS.match(event['LOCATION'])):
            print('  {', str(event['DTSTART']) + ',', str(event['DTEND']) + ',', '"' + event['SUMMARY'] + '" },')
    elif line.startswith('DTSTART;VALUE=DATE-TIME:'):
        event['DTSTART'] = int(mktime(datetime.strptime(line[24:], '%Y%m%dT%H%M%S').timetuple())) + 3*3600
    elif line.startswith('DTEND;VALUE=DATE-TIME:'):
        event['DTEND'] = int(mktime(datetime.strptime(line[22:], '%Y%m%dT%H%M%S').timetuple())) + 3*3600
    elif line.startswith('SUMMARY:'):
        event['SUMMARY'] = line[8:].replace('\\', '')
    elif line.startswith('CATEGORIES:'):
        event['CATEGORIES'] = line[11:]
    elif line.startswith('LOCATION:'):
        event['LOCATION'] = line[9:].replace('\\', '')
print('  { 0, 0, 0 }')
print('};')
