//Example program to illustrate ex parsing
using namespace ratatoskr;
namespace Convert {
struct Parameters {
	double amount;
	ex conversion;
};
auto parameters_description=make_parameter_description<Parameters>(
	"amount","The amount to convert",&Parameters::amount,
	"conversion-ratio","The conversion ratio",expression(&Parameters::conversion)
);
auto program = make_program_description(
	"convert", "Convert to a different unit measure by applying a coefficient",
	parameters_description, [] (Parameters& parameters) {
		cout<<latex<<parameters.conversion*parameters.amount<<endl;
	}
);
}
