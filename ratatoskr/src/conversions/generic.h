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
#ifndef GENERIC_H
#define GENERIC_H
#include "../input/pairfrom.h"
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

template<typename Container>
matrix generic_diagonal_matrix(unsigned int dimension, Container& store_parameters) {
	matrix g{dimension, dimension};
	for (unsigned int i=0;i<dimension;++i)
		store_parameters.append(g(i,i)=Parameter(N.g(i+1)));
	return g;
}

template<typename Parameters, typename GroupType>
auto generic_diagonal_metric(unique_ptr<PseudoRiemannianStructure> Parameters::*p, unique_ptr<GroupType> Parameters::*G,lst Parameters::*metric_parameters) {
	auto converter=[] (unique_ptr<LieGroup>& G,lst& metric_parameters) {
		unsigned int dimension=G->Dimension();
		auto g=generic_diagonal_matrix(dimension,metric_parameters);
		return as_unique(PseudoRiemannianStructureByMatrix::FromMatrixOnFrame(G.get(),G->e(),g));
	};
	return generic_option(p,converter,G,metric_parameters);
}

template<typename Container>
matrix generic_symmetric_matrix(int n, Container& store_parameters) {
	matrix m(n,n);
	for (int i=1;i<=n;++i)
	for (int j=i;j<=n;++j) {
		ex newparameter=n<10? Parameter{N.a(i*10+j)} : Parameter{N.a(i,j)};		
		m(i-1,j-1)= m(j-1,i-1)=newparameter;
		Insert(store_parameters,newparameter);
	}
	return m;
}

template<typename Container>
matrix generic_symmetric_matrix_from_list_of_nonzero_entries(int n, const vector<pair<int,int>>& indices, Container& store_parameters) {
	matrix m(n,n);
	for (auto p: indices) {
		ex newparameter=Parameter{N.a(p.first*10+p.second)};
		if (p.first<1 || p.first>n) throw ConversionError("index not in range in generic_symmetric_matrix_from_list_of_nonzero_entries");
		if (p.second<1 || p.second>n) throw ConversionError("index not in range in generic_symmetric_matrix_from_list_of_nonzero_entries");
		m(p.second-1,p.first-1)= m(p.first-1,p.second-1)=newparameter;
		Insert(store_parameters,newparameter);
	}
	return m;
}

template<typename Container>
matrix generic_symmetric_matrix_from_list_of_zero_entries(int n, const vector<pair<int,int>>& indices, Container& store_parameters) {
	set<pair<int,int>> all_pairs;
	for (int i=1;i<=n;++i)
	for (int j=i;j<=n;++j)
		all_pairs.emplace(i,j);
	for (auto p: indices) {
		all_pairs.erase(p);
		all_pairs.erase(make_pair(p.second,p.first));
	}	
	vector<pair<int,int>> difference(all_pairs.begin(),all_pairs.end());
	return generic_symmetric_matrix_from_list_of_nonzero_entries(n,difference,store_parameters);
}


template<typename Parameters, typename GroupType>
auto generic_metric_from_nonzero_entries(unique_ptr<PseudoRiemannianStructure> Parameters::*p, unique_ptr<GroupType> Parameters::*G,lst Parameters::*metric_parameters) {
	auto converter=[] (const vector<string>& nonzero_entries, unique_ptr<LieGroup>& G,lst& metric_parameters) {
		unsigned int dimension=G->Dimension();
		vector<pair<int,int>> nonzero_indices;
		transform(nonzero_entries.begin(),nonzero_entries.end(),back_inserter(nonzero_indices),pair_from_csv<int,int>);
		auto g=generic_symmetric_matrix_from_list_of_nonzero_entries(G->Dimension(),nonzero_indices,metric_parameters);
		return as_unique(PseudoRiemannianStructureByMatrix::FromMatrixOnFrame(G.get(),G->e(),g));
	};
	return generic_converter<vector<string>>(p,converter,G,metric_parameters);
}

template<typename Parameters, typename GroupType>
auto generic_metric_from_zero_entries(unique_ptr<PseudoRiemannianStructure> Parameters::*p, unique_ptr<GroupType> Parameters::*G,lst Parameters::*metric_parameters) {
	auto converter=[] (const vector<string>& zero_entries, unique_ptr<LieGroup>& G,lst& metric_parameters) {
		unsigned int dimension=G->Dimension();
		vector<pair<int,int>> zero_indices;
		transform(zero_entries.begin(),zero_entries.end(),back_inserter(zero_indices),pair_from_csv<int,int>);
		auto g=generic_symmetric_matrix_from_list_of_zero_entries(G->Dimension(),zero_indices,metric_parameters);
		return as_unique(PseudoRiemannianStructureByMatrix::FromMatrixOnFrame(G.get(),G->e(),g));
	};
	return generic_converter<vector<string>>(p,converter,G,metric_parameters);
}

template<typename Parameters, typename ParameterType>
auto generic_lie_algebra(unique_ptr<ParameterType> Parameters::*p ,lst Parameters::*structure_constants) {
	auto converter=[] (int parameter, lst& structure_constants) {
		auto result=make_unique<GenericLieGroup>(parameter);
		exvector symbols;
		for (auto e: result->e()) GetSymbols<StructureConstant>(symbols,result->d(e));
		for (auto symbol : symbols)
			structure_constants.append(symbol);
		return move(result);
	};
	return generic_converter<int>(p,converter,structure_constants);
}

}
#endif
