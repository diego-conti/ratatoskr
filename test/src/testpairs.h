/*******************************************************************************
 *  Copyright (C) 2022 by Diego Conti 
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
	pair<int,float> int_and_float;
	pair<string,string> string_and_string;
	pair<int,string> int_and_string;
	pair<string,int> string_and_int;
	vector<pair<int,int>> pairs;
};

auto description=make_parameter_description
(
		"int-and-float","int and float", comma_separated_pair(&CommandLineParameters::int_and_float),
		"string-and-string","string and string", comma_separated_pair(&CommandLineParameters::string_and_string),
		"int-and-string","int and string", comma_separated_pair(&CommandLineParameters::int_and_string),
		"string-and-int","string and int", comma_separated_pair(&CommandLineParameters::string_and_int),
		"pairs", "a list of pairs", vector_of_comma_separated_pairs(&CommandLineParameters::pairs)
);

class PairsTestSuite : public CxxTest::TestSuite
{
public:
	void testPairsFrom() {
		TS_ASSERT_EQUALS((pair_from_csv<int,int>("2,3").first),2);
		TS_ASSERT_EQUALS((pair_from_csv<int,int>("2,3").second),3);
		TS_ASSERT_EQUALS((pair_from_csv<string,int>("2,3").first),"2");
		TS_ASSERT_EQUALS((pair_from_csv<int,string>("2,3").second),"3");
	}
	void testPairs() {
		const char* (argv[]) {"program invocation", "--int-and-float=2,2.01", "--string-and-string=abc,def",
			"--int-and-string=-1,def", "--string-and-int=abc,1", "--pairs=1,2", "3,4"
		};
		int argc=std::size(argv);
		auto parameters=description.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.int_and_float,make_pair(2,2.01f));
		TS_ASSERT_EQUALS(parameters.string_and_string,make_pair("abc"s,"def"s));
		TS_ASSERT_EQUALS(parameters.int_and_string,make_pair(-1,"def"s));
		TS_ASSERT_EQUALS(parameters.string_and_int,make_pair("abc"s,1));
		TS_ASSERT_EQUALS(parameters.pairs.size(),2);
		TS_ASSERT_EQUALS(parameters.pairs[0],make_pair(1,2));
		TS_ASSERT_EQUALS(parameters.pairs[1],make_pair(3,4));
	}
	void testSplice() {
		string s=",123,,321,";
		TS_ASSERT_EQUALS(splice(s).size(),4);
		TS_ASSERT_EQUALS(splice(s)[0],"");
		TS_ASSERT_EQUALS(splice(s)[1],"123");
		TS_ASSERT_EQUALS(splice(s)[2],"");
		TS_ASSERT_EQUALS(splice(s)[3],"321");

	}
};
