#!/usr/bin/env python3

import fileinput
import sys
import subprocess
import mimetypes

replacements = [
	(' &amp;&#160;', '&amp;&#160;'),
	(' &amp;&amp;)', '&amp;&amp;)'),
	(' &amp;)', '&amp;)'),
	(' &amp;', '&amp; '),
	(' *)', '*)'),
	(' *=', '* = '),
	(' *&#160;', '*&#160;'),
	(' (', '('),
	('=delete', ' = <b>delete</b>'),
	('&lt; ', '&lt;'),
	(' &gt;', '&gt;'),
]

mimetype = mimetypes.guess_type(sys.argv[1])

if mimetype[0] != None and (mimetype[0] == 'image/png' or mimetype[0].startswith('font/')):
	print('Skipping ' + sys.argv[1])
	exit(0)

# print('Processing {} ({}) ...'.format(sys.argv[1], mimetype))

try:
	for line in fileinput.input(inplace=True, backup='.bak'):
		tmp = line
		for replacement in replacements:
			tmp = tmp.replace(replacement[0], replacement[1])
		print(tmp, end="")
except Exception as e:
	print('Error processing {}: {}'.format(sys.argv[1], e))
