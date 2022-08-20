#ifndef COMMAND_LINE_PARAMETERS_H
#define COMMAND_LINE_PARAMETERS_H
#include "tuplecontainer.h"
#include "errors.h"

namespace po = boost::program_options;


namespace ratatoskr {
template<typename TupleOfParameterDescriptions,typename Parameters>
class SequenceOfParameterDescriptions {
	static_assert(!is_undefined_v<Parameters>,"SequenceOfParameterDescriptions requires a nonempty tuple of nonempty parameter descriptions");
	TupleOfParameterDescriptions parameter_descriptions;
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
	string human_readable_description(int indent=0) const {
		stringstream s;
		auto add_description = [&s,indent] (auto& desc) {
				s<<desc.human_readable_description(indent+1);
			};
		iterate_over_tuple(add_description,parameter_descriptions);
		return s.str();
	}
	virtual ~SequenceOfParameterDescriptions()=default;
};

template<typename TupleOfParameterDescriptions,typename Parameters>
class DescriptionOfCommandLineParameters : public SequenceOfParameterDescriptions<TupleOfParameterDescriptions,Parameters> {
	static_assert(!is_undefined_v<Parameters>,"DescriptionOfCommandLineParameters requires a nonempty tuple of nonempty parameter descriptions");
public:
	using SequenceOfParameterDescriptions<TupleOfParameterDescriptions,Parameters> ::SequenceOfParameterDescriptions;
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
