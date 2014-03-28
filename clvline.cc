#include "clvline.h"
#include "dbg.h"

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
		if(group.size()>=g_index){
			glyphs_index=g_index;
			break;
		}
		g_index-=group.size();
		groups_index++;
	}
	if(groups_index == line_glyphs.size()){//groups_index cannot > line_glyphs.size() 
		line_glyphs.push_back(glyphs_group{});
		line_index.push_back(std::vector<int>{});
	}
	else{//delete to end
		line_glyphs.erase(line_glyphs.begin()+groups_index+1,line_glyphs.end());
		line_index.erase(line_index.begin()+groups_index+1,line_index.end());
		line_glyphs[groups_index].erase(line_glyphs[groups_index].begin()+glyphs_index,line_glyphs[groups_index].end());
		line_index[groups_index].erase(line_index[groups_index].begin()+glyphs_index,line_index[groups_index].end());
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
	scaled_font->text_to_glyphs(0,y,*this,line_glyphs[0],clusters,flags);//only utf8
	
	//return;

}
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
glyphs_group::iterator line::x_to_glyph_itr(int x){
	auto lower_group = std::lower_bound(line_glyphs.begin(),line_glyphs.end(),x,
			[](glyphs_group &group,int x){return group.empty() ? true : group.rbegin()->x < x;});
	if(lower_group==line_glyphs.end()){
		return --(--lower_group)->end();//FIXME bug
	}
	auto lower_gl = std::lower_bound(lower_group->begin(),lower_group->end(),x,
			[](Cairo::Glyph &gl ,int x){return gl.x < x;});
	std::cerr<<"x:"<<lower_gl->x<<"\n";
	return lower_gl;
}

bool line::draw_to_context(const Cairo::RefPtr<Cairo::Context> &cr,int y, const Cairo::Rectangle &rect){
	//cr->save();
	sync_glyphs(cr,y);
	for(auto& glyphs:line_glyphs)
		glyphs.draw_to_context(cr);
	//cr->restore();
	return true;
}


bool glyphs_group::draw_to_context(const Cairo::RefPtr<Cairo::Context> &cr){
	cr->save();
	//cr->stroke();
	if(font_ptr)
		cr->set_scaled_font(font_ptr);
	cr->show_glyphs(*this);
	cr->restore();
	return true;
}
