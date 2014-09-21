#pragma once
#include <vector>
#include <tuple>
#include <string>
#include <thread>
#include <memory>
namespace clv{
class Parser{
	std::thread parse_thread;
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

