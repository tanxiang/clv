#include <iostream>
#include "clvparser.h"

namespace clv{
void Parser::setup_code(){

}

std::unique_ptr<Parser::complete_table> Parser::complete_at(){
	return std::unique_ptr<complete_table>{};
}

void Parser::complete_at_async(){
	parse_thread=std::thread{
		[=]{
			std::shared_ptr<complete_table> complete_info{complete_at()};
			//auto complete_info_sp = std::make_shared<complete_table>(complete_at());
			//need notify editor ui, show
			for(auto& line:*complete_info){
				std::cerr<<std::get<0>(line)<<std::endl;
			}
		}
	};
	parse_thread.detach();
}

void Parser::analyze_syntax_async(){

}

}//namespace
