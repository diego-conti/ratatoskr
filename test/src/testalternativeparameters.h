/*******************************************************************************
 *  Copyright (C) 2022 by Diego Conti diego.conti@unipi.it 
 *  This file is part of Ratatoskr.                          
 *  Ratatoskr is free software; you can redistribute it and/or modify         
 *  it under the terms of the GNU General Public License as published by  
 *  the Free Software Foundation; either version 3 of the License, or     
 *  (at your option) any later version.                                   
 *                                                                          
 *  Ratatoskr is distributed in the hope that it will be useful,              
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *  GNU General Public License for more details.                          
 *                                                                           
 *  You should have received a copy of the GNU General Public License     
 *  along with Wedge; if not, write to the                                
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 *  
 *******************************************************************************/
#include <cxxtest/TestSuite.h>
#include "test.h"

#include "parameters/parameters.h"
#include "conversions/conversions.h"

using namespace GiNaC;
using namespace Wedge;
using namespace ratatoskr;

struct CommandLineParameters {
	unique_ptr<LieGroup> G;
	unique_ptr<PseudoRiemannianStructure> g;
};


auto description_metric=make_parameter_description
(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&CommandLineParameters::G),
		alternative("pseudo-riemannian metric on the Lie algebra")
				("metric-by-musical", "metric defined by the musical 'flat' isomorphism", metric_by_flat(&CommandLineParameters::g,&CommandLineParameters::G))
				("diagonal-metric", "diagonal metric defined by a space-separated list of coefficients", diagonal_metric(&CommandLineParameters::g,&CommandLineParameters::G))
);


class AlternativeParametersTestSuite : public CxxTest::TestSuite
{
public:

	void testParseMetricByMusicalIsomorphism() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,12", "--metric-by-musical=3,-2*2,1"};
		int argc=std::size(argv);
		auto parameters=description_metric.parametersFromCommandLine(argc,argv);
		auto& G=*(parameters.G);
		auto& g=parameters.g->ScalarProduct();
		TS_ASSERT_EQUALS(g.Flat(G.e(1)),G.e(3));
		TS_ASSERT_EQUALS(g.Flat(G.e(2)),-2*G.e(2));
		TS_ASSERT_EQUALS(g.Flat(G.e(3)),G.e(1));
	}

	void testParseDiagonalMetric() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,12", "--diagonal-metric=-1,-2,3"};
		int argc=std::size(argv);
		auto parameters=description_metric.parametersFromCommandLine(argc,argv);
		auto& G=*(parameters.G);
		auto& g=parameters.g->ScalarProduct();
		TS_ASSERT_EQUALS(g.Flat(G.e(1)),-G.e(1));
		TS_ASSERT_EQUALS(g.Flat(G.e(2)),-2*G.e(2));
		TS_ASSERT_EQUALS(g.Flat(G.e(3)),3*G.e(3));
	}

	void testNoMetric() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,12"};
		int argc=std::size(argv);
		TS_ASSERT_THROWS(description_metric.parametersFromCommandLine(argc,argv),MissingParameter);
	}
	void testTwoMetrics() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,12", "--metric-by-musical=3,-2*2,1", "--diagonal-metric=-1,-2,3"};
		int argc=std::size(argv);
		TS_ASSERT_THROWS(description_metric.parametersFromCommandLine(argc,argv),TooManyAlternatives);
	}
};
