#pragma once
#include <vector>
#include <tuple>
#include <string>
#include <thread>
#include <condition_variable>
#include <memory>

namespace clp{
class Invocation;
}

namespace clv{
class Parser{
	std::thread parse_thread;
	std::condition_variable cond_ready;
	std::condition_variable cond_search;
	std::unique_ptr<clp::Invocation> invocation;
protected:
	std::shared_ptr<std::vector<std::string> > cmd;
public:
	typedef std::vector<std::tuple<std::string> > complete_table;
	Parser();
	void setup_code();
	void setup_cmd();
	std::unique_ptr<complete_table> complete_at();
	void complete_at_async();
	void analyze_syntax_async();
};
}//namespace

