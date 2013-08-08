#ifndef _CLVPARSER_H
#define _CLVPARSER_H
#include <vector>
#include <tuple>
#include <string>
#include <thread>
#include <memory>

class ClvParser{
	std::thread parse_thread;
protected:
	std::shared_ptr<std::vector<std::string> > cmd;
public:
	typedef std::vector<std::tuple<std::string> > complete_table;
	void setup_code();
	void setup_cmd();
	complete_table* complete_at();
	void complete_at_async();
	void analyze_syntax_async();
};
#endif
