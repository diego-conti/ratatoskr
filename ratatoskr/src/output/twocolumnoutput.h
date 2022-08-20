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
