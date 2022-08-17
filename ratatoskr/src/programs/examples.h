//Example programs to illustrate ex parsing
using namespace ratatoskr;
namespace Convert {
struct Parameters {
	float amount;
	ex conversion;
};
auto parameters_description=make_parameter_description<Parameters>(
	"amount","The amount to convert",&Parameters::amount,
	"conversion-ratio","The conversion ratio",expression(&Parameters::conversion)
);
auto program = make_program_description(
	"convert", "Convert to a different unit of measurement by applying a coefficient",
	parameters_description, [] (Parameters& parameters, ostream& os) {
		os<<parameters.conversion*parameters.amount<<endl;
	}
);
}

namespace Derivative {
struct Parameters {
	ex function;
	ex variable;
};
auto parameters_description=make_parameter_description<Parameters>(
	"variable","a variable",new_symbol(&Parameters::variable),
	"function","a function of one variable",expression(&Parameters::function,&Parameters::variable)
);
auto program = make_program_description(
	"derivative", "Take the derivative of a function of one variable",
	parameters_description, [] (Parameters& parameters, ostream& os) {
		os<<parameters.function.diff(ex_to<symbol>(parameters.variable))<<endl;
	}
);
}

namespace PartialDerivative {
struct Parameters {
	GlobalSymbols symbols;
	ex function;
	ex variable;
};
auto parameters_description=make_parameter_description<Parameters>(
	"variable","a variable",expression(&Parameters::variable,&Parameters::symbols),
	"function","a function of one or more variables",expression(&Parameters::function,&Parameters::symbols)
);
auto program = make_program_description(
	"partial-derivative", "Take a partial derivative of a function of more variables",
	parameters_description, [] (Parameters& parameters, ostream& os) {
		os<<parameters.function.diff(ex_to<symbol>(parameters.variable))<<endl;
	}
);
}
