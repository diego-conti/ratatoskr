namespace ratatoskr {
inline vector<string> splice(const string& s, char sep=',') {
	vector<string> result;
	int begin=0;
	do {
		int end=s.find(sep,begin);
		if (end==string::npos) end=s.size();
		result.push_back(s.substr(begin,end-begin));
		begin=end+1;
	} while (begin<s.size());
	return result;
}
}
