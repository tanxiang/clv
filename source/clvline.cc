#include "clvline.h"
#include "dbg.h"
#include "clvhb.hh"

namespace clv{

#if 0
//FIXME need helper 
void line::sync_glyphs(const Cairo::RefPtr<Cairo::Context>& cr,int y,unsigned int g_index){
	cr->set_source_rgb(1,1,1);
	cr->select_font_face("Source Code Pro",Cairo::FONT_SLANT_NORMAL,Cairo::FONT_WEIGHT_NORMAL );
	cr->set_font_size(16);
	if(synced)
		return;
	std::vector<glyphs_group>::size_type groups_index=0;
	glyphs_group::size_type glyphs_index=0;
	for(auto& group:line_glyphs){
		if(group.first().size()>=g_index){
			glyphs_index=g_index;
			break;
		}
		g_index-=group.first().size();
		groups_index++;
	}
	if(groups_index == line_glyphs.size()){//groups_index cannot > line_glyphs.size() 
		line_glyphs.push_back(std::make_pair(glyphs_group{cr->get_scaled_font()},std::vector<int>{});
	}
	else{//delete to end
		line_glyphs.erase(line_glyphs.begin()+groups_index+1,line_glyphs.end());
		line_glyphs[groups_index].erase(line_glyphs[groups_index].begin()+glyphs_index,line_glyphs[groups_index].end());
	}
	//line_glyphs.clear();
	//glyphs_index.clear();
	size_t c_index=0;
	if(glyphs_index)
		c_index=line_index[groups_index][glyphs_index-1];
	else if(groups_index)
		c_index=*line_index[groups_index-1].rbegin();

	auto scaled_font = cr->get_scaled_font();
	std::vector< Cairo::TextCluster > clusters;
	Cairo::TextClusterFlags flags = Cairo::TEXT_CLUSTER_FLAG_BACKWARD;
	scaled_font->text_to_glyphs(0,y,*this,line_glyphs[0].first(),clusters,flags);//only utf8
	
	//return;

}
#endif

#include <algorithm>
/*
int line::x_to_index(int x){
	auto lower_group = std::lower_bound(line_glyphs.begin(),line_glyphs.end(),x,
			[](glyphs_group &group,int x){return group.empty() ? true : group.rbegin()->x < x;});
	if(lower_group==line_glyphs.end())
		return 0;
	auto lower_gl = std::lower_bound(lower_group->begin(),lower_group->end(),x,
			[](Cairo::Glyph &gl ,int x){return gl.x < x;});
	std::cerr<<"x:"<<lower_gl->x<<"\n";
	return 0;
}
*/

static hbfont def_font{"fonts/SourceCodePro-Regular.otf"};

void line::shape(){
	line_glyphs.push_back(glyphs_group{def_font.ScaledFont()});
	def_font.shape(line_glyphs[0].first,def_font.ScaledFont(),0,0,c_str(),length());
}

bool line::draw_to_context(const Cairo::RefPtr<Cairo::Context> &cr,int y, const Cairo::Rectangle &rect){
	for(auto& glyphs:line_glyphs){
		glyphs.move_y_to(y);
		glyphs.draw_to_context(cr);
	}
	return true;
}

std::vector<glyphs_group>::iterator line::x_to_glyph_itr(int x){
	auto lower_group = std::lower_bound(line_glyphs.begin(),line_glyphs.end(),x,
			[](glyphs_group &group,int x){return group.first.empty() ? true : group.first.rbegin()->x < x;});
	//if(lower_group==line_glyphs.end()){
	//	return --(--lower_group)->end();//FIXME bug
	//}
	auto lower_gl = std::lower_bound(lower_group->first.begin(),lower_group->first.end(),x,
			[](Cairo::Glyph &gl ,int x){return gl.x < x;});
	std::cerr<<"x:"<<lower_gl->x<<"\n";
	return lower_group;
}

bool glyphs_group::draw_to_context(const Cairo::RefPtr<Cairo::Context> &cr){
	if(font_ptr)
		cr->set_scaled_font(font_ptr);
	cr->show_glyphs(first);
	return true;
}

}//namespace