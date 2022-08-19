#include "ratatoskr.h"
#include "programs/examples.h"
#include "programs/liealgebras.h"
#include "programs/curvature.h"

using namespace ratatoskr;

auto alternative_programs = alternative_program_descriptions(
		Convert::program, Derivative::program, PartialDerivative::program,
		ExtDerivative::program, ClosedForms::program, Subalgebra::program, SubalgebraWithParameters::program,
		Curvature::program
);



int main(int argc, char** argv) {
	alternative_programs.run(argc,argv);
}
