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
#include "ratatoskr.h"
#include "programs/examples.h"
#include "programs/liealgebras.h"
#include "programs/curvature.h"
#include "programs/killing.h"
#include "programs/spinors.h"
#include "programs/covariantderivative.h"

using namespace ratatoskr;

auto alternative_programs = alternative_program_descriptions(
		Convert::program, Derivative::program, PartialDerivative::program, Invert::program,
		ExtDerivative::program, ClosedForms::program, Subalgebra::program, SubalgebraWithParameters::program, Derivations::program,
		Curvature::program, Killing::program, 
		Nabla::program, Clifford::program,
		CovariantDerivative::program
);



int main(int argc, char** argv) {
	alternative_programs.run(argc,argv);
}
