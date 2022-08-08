#include "wedge/wedge.h"
#include "boost/program_options.hpp"
using namespace Wedge;

namespace po = boost::program_options;

class Co


int main(int argc, char* argv[]) {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("lie-algebra", po::value<string>(),
                  "Lie algebra (mandatory)")
            ("diagonal-metric", po::value<string>(),
                  "diagonal metric with coefficients")
            ("generic-diagonal-metric", "generic diagonal metric")
            ("flat", po::value<string>(), "metric assigned by giving a comma-separated list of the forms e_1^\\flat, ... e_n^\flat");
				try {
	        po::variables_map vm;
        	po::store(po::parse_command_line(argc, argv, desc), vm);
	        po::notify(vm);
  	      if (vm.count("help") || !vm.count("lie-algebra")) print_help(argv[0],desc);
					auto errvalue=process(vm["lie-algebra"].as<string>(),vm);
	        return errvalue;
        }
        catch (const boost::program_options::unknown_option& e) {
        		print_help(argv[0],desc);
        		return 1;
        }
}
