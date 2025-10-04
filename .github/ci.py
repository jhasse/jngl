#!/usr/bin/env python3

import os
import platform

ignores = [
	'.git/',
	'build/',
	'glad/',
	'include/',
	'.gitlab-ci-local/',
	'.cache/',
]

error_count = 0

def error(path: str, msg: str) -> None:
	global error_count
	error_count += 1
	print('\x1b[1;31m{}\x1b[0;31m{}\x1b[0m'.format(path, msg))

for root, directory, filenames in os.walk('.'):
	for filename in filenames:
		path = os.path.join(root, filename)[2:]
		if platform.system() == "Windows":
			path = path.replace('\\', '/')
		if any([path.startswith(x) for x in ignores]):
			continue
		with open(path, 'rb') as file:
			line_nr = 1
			try:
				for line in [x.decode() for x in file.readlines()]:
					if len(line) == 0 or line[-1] != '\n':
						error(path, ' missing newline at end of file.')
					if len(line) > 1:
						if line[-2] == '\r':
							error(path, ' has Windows line endings.')
							break
						if line[-2] == ' ' or line[-2] == '\t':
							error(path, ':{} has trailing whitespace.'.format(line_nr))
					line_nr += 1
			except UnicodeError:
				pass # binary file

exit(error_count)
