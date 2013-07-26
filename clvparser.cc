#include "clvparser.h"
void ClvParser::setup_code(){

}

ClvParser::complete_table* ClvParser::complete_at(){
	return nullptr;
}
#include <iostream>
void ClvParser::complete_at_async(){
	parse_thread=std::thread{
		[=]{
			std::shared_ptr<complete_table> complete_info{complete_at()};
			//need notify editor ui, show
			for(auto& line:*complete_info){
				std::cerr<<std::get<0>(line)<<std::endl;
			}
		}
	};
	parse_thread.detach();
}

void ClvParser::analyze_syntax_async(){

}
