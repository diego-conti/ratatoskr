using namespace Wedge;
using namespace ratatoskr;

namespace ExtDerivative {
struct Parameters {
	unique_ptr<LieGroup> G;
	ex form;
};

auto parameters_description=make_parameter_description<Parameters>
(
	"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
	"form","a differential form on the Lie algebra",differential_form(&Parameters::form,&Parameters::G)
);

auto program = make_program_description(
	"ext-derivative", "Compute the exterior derivative of a form a on a Lie algebra",
	parameters_description, [] (Parameters& parameters) {
		cout<<latex<<parameters.G->d(parameters.form)<<endl;
	}
);
}

