#include "ratatoskr.h"
#include "programs/examples.h"
#include "programs/extderivatives.h"

using namespace ratatoskr;

auto alternative_programs = alternative_program_descriptions(
		ExtDerivative::program, Convert::program, Derivative::program, PartialDerivative::program
);



int main(int argc, char** argv) {
	alternative_programs.run(argc,argv);
}
