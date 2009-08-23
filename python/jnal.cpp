#include "../jnal.hpp"

#include <boost/python.hpp>

using namespace boost::python;
using namespace jnal;

BOOST_PYTHON_MODULE(jnal)
{
	def("Play", Play);
}
