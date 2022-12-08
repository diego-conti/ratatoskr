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
#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>
namespace ratatoskr {

class CommandLineError  : public std::runtime_error {
public:
	using runtime_error::runtime_error;
};

class BoostError : public CommandLineError {
public:
	BoostError(const char* what) : CommandLineError(what) {}
};

class MissingParameter : public CommandLineError {
public:
	MissingParameter(const std::string& parametername) : CommandLineError{"required parameter not specified: "s+parametername} {}
	MissingParameter() : CommandLineError{"Not all required parameters have been specified"} {}
};

class TooManyAlternatives : public CommandLineError {
public:
	TooManyAlternatives(const std::string& parametername) : CommandLineError{"parameter specified more than once: "s+parametername} {}
};

class InvalidParameter : public CommandLineError {
public:
	InvalidParameter(const std::string& error) : CommandLineError{"invalid parameter value: "s+error} {}
};

class ParseError : public CommandLineError {
public:
	ParseError(const std::string& error) : CommandLineError{"error parsing parameter: "s+error} {}
};

class DefinitionError :  public std::logic_error {
public:
	using logic_error::logic_error;
};
}
#endif
