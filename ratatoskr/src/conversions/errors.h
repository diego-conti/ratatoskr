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
#ifndef CONVERSION_ERRORS_H
#define CONVERSION_ERRORS_H

#include <stdexcept>
namespace ratatoskr {

class ConversionError : public CommandLineError {
public:
	ConversionError(const std::string& error) : CommandLineError{"error parsing parameter: "s+error} {}
};

class UnevenMatrix : public CommandLineError {
public:
	UnevenMatrix(int r, int c) : CommandLineError{"uneven matrix: expecting a sequence of "+to_string(r)+" rows, each with "+to_string(c)+" elements separated by a comma"} {}
};
}
#endif
