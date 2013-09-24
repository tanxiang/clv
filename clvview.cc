#include "clvview.h"

bool ClvLineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	if(should_draw_window(cr,get_window())){
		cr->set_source_rgb(0.0, 0.0, 0.0);
		cr->paint();
	}
	return true;
}

ClvFileArea::ClvFileArea(unorder_tree<line> &file):
	file_context(file){
	gtk_widget_add_events (Widget::gobj() , GDK_BUTTON_PRESS_MASK );
	gtk_widget_add_events (Widget::gobj() , GDK_BUTTON_RELEASE_MASK );
	im_context = gtk_im_multicontext_new();
	
	g_signal_connect (im_context, "commit", G_CALLBACK (&ClvFileArea::commit_proxy), this);
	g_signal_connect (im_context, "preedit-changed",G_CALLBACK (&ClvFileArea::commit_proxy), this);
	g_signal_connect (im_context, "retrieve-surrounding",G_CALLBACK (&ClvFileArea::commit_proxy), this);
	g_signal_connect (im_context, "delete-surrounding",G_CALLBACK (&ClvFileArea::commit_proxy), this);
	GdkWindowAttr attributes;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.wclass = GDK_INPUT_ONLY;
	attributes.event_mask = get_events();
	attributes.event_mask |= (GDK_BUTTON_PRESS_MASK |
		GDK_BUTTON_RELEASE_MASK |
		GDK_BUTTON1_MOTION_MASK |
		GDK_BUTTON3_MOTION_MASK |
		GDK_POINTER_MOTION_HINT_MASK |
		GDK_POINTER_MOTION_MASK |
		GDK_ENTER_NOTIFY_MASK |
		GDK_LEAVE_NOTIFY_MASK);
	//gtk_im_context_set_client_window(im_context,get_window()->gobj());
	//gtk_im_context_set_client_window(im_context,nullptr);
	auto client_window = Gdk::Window::create(get_parent_window(), &attributes, GDK_WA_X | GDK_WA_Y);
	register_window(client_window);
	gtk_im_context_set_client_window(im_context,client_window->gobj());
	property_can_focus() = true;
	auto style = get_style_context();
	style->context_save();
	style->add_class(GTK_STYLE_CLASS_VIEW);
	style->context_restore();
	set_size_request( 0, file_context.end().get_fill_offset());
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
	this->signal_draw().connect(sigc::mem_fun(*this,&ClvFileArea::on_draw));
#endif
}

#include <iostream>

void ClvFileArea::draw(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect){
	cr->save();//need RAII mode restore
	auto itr = file_context.get_form_fill(rect.y);
	int height = 0;

	//if(itr != file_context.end())
	height = itr.get_fill_offset() - rect.y;//FIXME need a better way
	while(height<rect.height && itr != file_context.end())
	{
		itr->draw_to_context(cr, itr.get_fill_offset()+itr->get_fill(),rect);
		height += itr->get_fill();
		++itr;
	}
	cr->clip();
	cr->restore();
}



bool ClvFileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	cr->save();
	cr->set_source_rgb(0.3, 0.4, 0.5);
	cr->paint();
	std::vector<Cairo::Rectangle> clip_rects;
	cr->copy_clip_rectangle_list(clip_rects);
	//std::cerr<<"num rect = "<<clip_rects.size()<<std::endl;
	for(auto& clip_rect:clip_rects){
		std::cerr<<"x"<<clip_rect.x<<"y"<<clip_rect.y<<"\tw="<<clip_rect.width<<" h="<<clip_rect.height<<std::endl;
		draw(cr,clip_rect);
	}
	cr->restore();
	return true;//Gtk::TextView::on_draw(cr);
}

bool ClvFileArea::on_key_press_event(GdkEventKey *event){
	std::cout<<"press"<<event->string<<std::endl;
	return true;
}

bool ClvFileArea::on_key_release_event(GdkEventKey *event){
	std::cout<<"release"<<event->string<<std::endl;
	return true;
}

bool ClvFileArea::on_button_press_event(GdkEventButton* event){
	std::cerr<<"pat"<<event->x<<","<<event->y<<"\n";
	auto line_itr=file_context.get_form_fill(event->y);
	if(line_itr!=file_context.end()){
		line_itr->x_to_index(event->x);
	}
	if(gtk_widget_has_focus(Widget::gobj())){
		//std::cout<<"focus"<<std::endl;
	}
	else{
		grab_focus();
		gtk_im_context_focus_in(im_context);
	}
	return true;
}

bool ClvFileArea::on_button_release_event(GdkEventButton* event){
	std::cout<<"release m"<<std::endl;
	return true;
}

ClvThumArea::ClvThumArea(){

}

ClvThumArea::~ClvThumArea(){

}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	return true;
}

