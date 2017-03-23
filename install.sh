#!/bin/bash

prefix=$1
if [ "$prefix" == "" ]; then
	echo "Usage: ./install.sh <prefix>"
	exit 1
fi

set -x
mkdir -p $prefix/include/
mkdir -p $prefix/include/jngl/
mkdir -p $prefix/lib/
mkdir -p $prefix/lib/pkgconfig/
cp src/jngl.hpp $prefix/include/
cp src/jngl/*.hpp $prefix/include/jngl/
cp libjngl.a $prefix/lib/
cp jngl.pc $prefix/lib/pkgconfig/
