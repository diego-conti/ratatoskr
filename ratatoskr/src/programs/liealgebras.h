using namespace Wedge;
using namespace ratatoskr;

namespace ExtDerivative {
	struct Parameters {
		unique_ptr<LieGroup> G;
		ex form;
	};

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"form","a differential form on the Lie algebra",differential_form(&Parameters::form,&Parameters::G)
	);

	auto program = make_program_description(
		"ext-derivative", "Compute the exterior derivative of a form a on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.G->d(parameters.form)<<endl;
		}
	);
}

namespace ClosedForms {
	struct Parameters {
		unique_ptr<AbstractLieGroup<false>> G;
		int p;
	};

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"p","a positive integer",&Parameters::p
	);

	auto program = make_program_description(
		"closed-forms", "Compute the space of closed p-forms",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.G->ClosedForms(parameters.p)<<endl;
		}
	);
}

namespace Subalgebra {
	struct Parameters {
		unique_ptr<LieGroupHasParameters<false>> G;
		unique_ptr<LieGroupHasParameters<false>> H;
	};

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"subalgebra","comma-separated list of generators of the subalgebra",lie_subalgebra(&Parameters::H,&Parameters::G)
	);

	auto program = make_program_description(
		"subalgebra-without-parameters", "Compute the structure constant of a Lie subalgebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.H->canonical_print(os)<<endl;
		}
	);
}

namespace SubalgebraWithParameters {
	struct Parameters {
		unique_ptr<LieGroupHasParameters<true>> G;
		unique_ptr<LieGroupHasParameters<true>> H;
		GlobalSymbols symbols;
	};

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra with parameters",lie_algebra(&Parameters::G,&Parameters::symbols),
		"subalgebra","comma-separated list of generators of the subalgebra",lie_subalgebra(&Parameters::H,&Parameters::G,&Parameters::symbols)
	);

	auto program = make_program_description(
		"subalgebra-with-parameters", "Compute the structure constant of a Lie subalgebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.H->canonical_print(os)<<endl;
		}
	);
}
