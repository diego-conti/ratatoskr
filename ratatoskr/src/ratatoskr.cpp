#include "ratatoskr.h"
#include "programs/extderivatives.h"

using namespace ratatoskr;

auto alternative_programs = alternative_program_descriptions(
		ext_derivative
);



int main(int argc, char** argv) {
	alternative_programs.run(argc,argv);
}
