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
#ifndef TWO_COLUMN_OUTPUT_H
#define TWO_COLUMN_OUTPUT_H
namespace ratatoskr {
inline string two_column_output(const string& column1, const string& column2,int indent=0) {
	constexpr int COLUMN_SIZE=30;
	stringstream s;
	s<<string(indent*2,' ');
	s<<column1;
	auto used=s.str().size();
	if (used>COLUMN_SIZE-1)
		s<<endl<<string(COLUMN_SIZE,' ');
	else
		s<<string(COLUMN_SIZE-used,' ');
	s<<column2<<endl;
	return s.str();
}
}
#endif
