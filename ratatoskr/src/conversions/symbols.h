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
#ifndef GLOBAL_SYMBOLS_H
#define GLOBAL_SYMBOLS_H
namespace ratatoskr {
using namespace Wedge;

WEDGE_DECLARE_NAMED_ALGEBRAIC(Parameter,realsymbol);


template<typename... Name>
lst make_symbols(Name&&... name) {
	return lst{Parameter{std::forward<Name>(name)}...};
}

struct GlobalSymbols {
	lst symbols() const {
		static lst symbols=make_symbols(
			N.a,N.b,N.c,N.d,N.e,N.f,N.g,N.h,N.i,N.j,N.k,N.l,N.m,N.n,N.o,N.p,N.q,N.r,N.s,N.t,N.u,N.v,N.w,N.x,N.y,N.z,
			N.A,N.B,N.C,N.D,N.E,N.F,N.G,N.H,N.I,N.J,N.K,N.L,N.M,N.N,N.O,N.P,N.Q,N.R,N.S,N.T,N.U,N.V,N.W,N.X,N.Y,N.Z,
			N.alpha,N.beta,N.gamma,N.Gamma,N. delta,N.Delta,N.epsilon,N.zeta,N.eta,N.theta,N.Theta,N.kappa,N.lambda,N.Lambda,
			N.mu,N.nu,N.xi,N.Xi,N.rho,N.pi,N.Pi,N.sigma,N.Sigma,N.tau,N.upsilon,N.Upsilon,N.phi,N.Phi,N.chi,N.psi,N.Psi,N.omega,N.Omega
		);
		return symbols;
	}
	ex by_name(const string& s) {
		for (auto x: symbols())
			if (ex_to<Parameter>(x).get_name()==s) return x;
		throw NoSymbolWithName(s);
	}
};

template<class SymbolClass=symbol>
class Symbol {
	ex symbol;
public:	
	Symbol()=default;
	Symbol(const string& name) : symbol{SymbolClass{name}} 
	{}
	lst symbols() const {
		return lst{symbol};
	}	
	operator SymbolClass () const {
		return ex_to<SymbolClass>(symbol);
	}
};

class Symbols {
	lst symbols_;
public:
	Symbols() = default;
	Symbols(const GlobalSymbols& symbols) : symbols_(symbols.symbols()) {}
	Symbols(const lst& symbols) : symbols_(symbols) {}
	template<class SymbolClass>
	Symbols(const Symbol<SymbolClass>& symbol) : symbols_(symbol.symbols()) {}
	lst symbols() const {
		return symbols_;
	}
	ex ex_from_string(const string& s) const {
		try {
			return ex{s,symbols_};
		}
		catch (...) {
			cerr<<s<<endl;
			cerr<<symbols_<<endl;
			throw ParseError{s};
		}
	}
};

template<typename Parameters, typename SymbolClass>
auto new_symbol(Symbol<SymbolClass> Parameters::*p) {
	auto converter=[] (const string& parameter) {
		return Symbol<SymbolClass>{parameter};
	};
	return generic_converter(p,converter);
}

template<typename Parameters, typename SymbolClass=symbol>
auto new_symbols(Symbols Parameters::*p) {
	auto converter=[] (const vector<string>& parameter) {
		lst result;
		for (auto& x: parameter) result.append(ex{SymbolClass{x}});
		return Symbols{result};
	};
	return generic_converter<vector<string>>(p,converter);
}

}
#endif
