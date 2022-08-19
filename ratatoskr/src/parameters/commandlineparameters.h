#ifndef COMMAND_LINE_PARAMETERS_H
#define COMMAND_LINE_PARAMETERS_H
#include "tuplecontainer.h"
#include "errors.h"

namespace po = boost::program_options;

inline po::options_description empty_options_description() {
	return {"Usage"};
}

namespace ratatoskr {
template<typename Parameters, typename TupleOfParameterDescriptions>
class SequenceOfParameterDescriptions {
	TupleOfParameterDescriptions parameter_descriptions;
protected:
	po::options_description description(po::options_description options=empty_options_description()) const {
		add_option_description(options);
		return options;
	}
public:
	SequenceOfParameterDescriptions(TupleOfParameterDescriptions parameter_descriptions) : parameter_descriptions{parameter_descriptions}{}
	int fill(Parameters& parameters,int argc, const char** argv) const {
		int parameters_filled=0;
		auto fill_parameter = [argc,argv,&parameters,&parameters_filled] (auto& desc) {
			parameters_filled+=desc.fill(parameters, argc, argv);
		};
		iterate_over_tuple(fill_parameter,parameter_descriptions);
		return parameters_filled==tuple_size_v<TupleOfParameterDescriptions>? 1 : 0;
	}
	void add_option_description(po::options_description& options) const {
		auto add_to_options_description = [&options] (auto& desc) {
			desc.add_option_description(options);
		};

		iterate_over_tuple(add_to_options_description,parameter_descriptions);
	}
	string human_readable_description() const {
		stringstream s;
		s<<description();
		return s.str();
	}
	virtual ~SequenceOfParameterDescriptions()=default;
};

template<typename Parameters, typename TupleOfParameterDescriptions>
class DescriptionOfCommandLineParameters : public SequenceOfParameterDescriptions<Parameters, TupleOfParameterDescriptions> {
public:
	using SequenceOfParameterDescriptions<Parameters, TupleOfParameterDescriptions> ::SequenceOfParameterDescriptions;
	Parameters parametersFromCommandLine(int argc, const char** argv) const {
		try {
			Parameters params;
			if (!this->fill(params,argc,argv)) throw MissingParameter();
			return params;
		}
		catch (const po::error& e) {
			throw BoostError(e.what());
		}
	}
};

}
#endif
