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
#ifndef RATATOSKR_METRICS_H
#define RATATOSKR_METRICS_H
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

template<typename Parameters, typename ParameterType, typename GroupType>
auto metric_by_on_coframe(unique_ptr<ParameterType> Parameters::*p,unique_ptr<GroupType> Parameters::*G,pair<int,int> Parameters::*signature) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G, pair<int,int> signature) {
		if (signature.first<0 || signature.second<0 || signature.first+signature.second!=G->Dimension()) throw ConversionError("signature should be a pair of nonnegative integers summing to the dimension");
		auto on_coframe=ParseDifferentialForms(G->e(),parameter.c_str());
		return make_unique<StandardPseudoRiemannianStructure>(G.get(),on_coframe, signature.first);
	};
	return generic_converter(p,converter,G,signature);
}

template<typename Parameters, typename ParameterType, typename GroupType>
auto metric_by_on_coframe(unique_ptr<ParameterType> Parameters::*p,unique_ptr<GroupType> Parameters::*G,vector<int> Parameters::*timelike_indices) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G, const vector<int>& timelike_indices) {
		auto on_coframe=ParseDifferentialForms(G->e(),parameter.c_str());
		return as_unique(PseudoRiemannianStructureByOrthonormalFrame::FromTimelikeIndices(G.get(),on_coframe, timelike_indices));
	};
	return generic_converter(p,converter,G,timelike_indices);
}
template<typename Parameters, typename ParameterType, typename GroupType>
auto metric_by_on_frame(unique_ptr<ParameterType> Parameters::*p,unique_ptr<GroupType> Parameters::*G,vector<int> Parameters::*timelike_indices) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G, const vector<int>& timelike_indices) {
		Frame on_coframe=ParseDifferentialForms(G->e(),parameter.c_str());
		return as_unique(PseudoRiemannianStructureByOrthonormalFrame::FromTimelikeIndices(G.get(),on_coframe.dual(), timelike_indices));
	};
	return generic_converter(p,converter,G,timelike_indices,symbols);
}

template<typename Parameters, typename ParameterType, typename GroupType>
auto metric_by_on_frame(unique_ptr<ParameterType> Parameters::*p,unique_ptr<GroupType> Parameters::*G,pair<int,int> Parameters::*signature) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G, pair<int,int> signature) {
		if (signature.first<0 || signature.second<0 || signature.first+signature.second!=G->Dimension()) throw ConversionError("signature should be a pair of nonnegative integers summing to the dimension");
		Frame on_coframe=ParseDifferentialForms(G->e(),parameter.c_str());
		return make_unique<StandardPseudoRiemannianStructure>(G.get(),on_coframe.dual(), signature.first);
	};
	return generic_converter(p,converter,G,signature,symbols);
}

matrix metric_from_eflats(const LieGroup& G, const exvector& deflat) {
	matrix g(G.Dimension(),G.Dimension());
	for (int i=0;i<G.Dimension();++i)
	for (int j=0;j<G.Dimension();++j)
		g(i,j)=Hook(G.e()[i],deflat[j]);
	return g;
}

template<typename Parameters, typename ParameterType, typename GroupType>
auto metric_by_flat(unique_ptr<ParameterType> Parameters::*p,unique_ptr<GroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G) {
		auto deflat=ParseDifferentialForms(G->e(),parameter.c_str());
		return as_unique(PseudoRiemannianStructureByMatrix::FromMatrixOnFrame(G.get(),G->e(),metric_from_eflats(*G,deflat)));
	};
	return generic_converter(p,converter,G);
}

template<typename Parameters, typename ParameterType, typename GroupType>
auto diagonal_metric(unique_ptr<ParameterType> Parameters::*p,unique_ptr<GroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G) {		
		auto matrix=diagonal_matrix_from_string(parameter,Symbols{});
		if (matrix.rows()!=G->Dimension())
			throw InvalidParameter("trying to construct diagonal matrix of order "s+to_string(G->Dimension())+" but "+to_string(matrix.rows())+ " entries were specified");
		return as_unique(PseudoRiemannianStructureByMatrix::FromMatrixOnFrame(G.get(),G->e(),matrix));
	};
	return generic_converter(p,converter,G);
}

}
#endif
