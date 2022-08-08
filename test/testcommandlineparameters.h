#include <cxxtest/TestSuite.h>
#include "test.h"

#include "commandlineparameters.h"

using namespace GiNaC;
using namespace Wedge;

class CommandLineTestSuite : public CxxTest::TestSuite
{
	void testnothing() {
		TS_ASSERT(false);
	}
};
