#!/usr/bin/env python3

from distutils.core import setup
from setuptools.dist import Distribution
import shutil, os

if not os.path.exists('build/lib.win32-3.6'):
	os.makedirs('build/lib.win32-3.6')
shutil.copy('../visualcpp/Release/Python.dll', 'build/lib.win32-3.6/jngl.pyd')

class BinaryDistribution(Distribution):
	"""Distribution which always forces a binary package with platform name"""
	def has_ext_modules(foo):
		return True

setup(
	name="jngl",
	version="1.4.0",
	description="JNGL for Python",
	author="Jan Niklas Hasse",
	author_email="jhasse@bixense.com",
	url="https://bixense.com/jngl",
	data_files=[(".", ["../visualcpp/Release/jngl.dll", "../epoxy-0.dll"])],
	py_modules=['jngl'],
	distclass=BinaryDistribution,
)
