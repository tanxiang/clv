#pragma once
#include <vector>
#include <tuple>
#include <string>
#include <thread>
#include <memory>
#include <condition_variable>

namespace clp{
class Invocation;
}

namespace clv{
class Parser{
	std::thread parse_thread;
	std::unique_ptr<clp::Invocation> invocation;
	std::condition_variable CondReady;
	std::condition_variable CondSearch;
protected:
	std::shared_ptr<std::vector<std::string> > cmd;
public:
	typedef std::vector<std::tuple<std::string> > complete_table;
	void setup_code();
	void setup_cmd();
	std::unique_ptr<complete_table> complete_at();
	void complete_at_async();
	void analyze_syntax_async();
};
}//namespace

