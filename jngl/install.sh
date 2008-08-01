prefix=$1
mkdir -p $prefix/include/
mkdir -p $prefix/lib/
mkdir -p $prefix/lib/pkgconfig/
cp jngl.hpp $prefix/include/
cp libjngl.a $prefix/lib/
cp jngl.pc $prefix/lib/pkgconfig/
