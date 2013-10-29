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
	add_events(Gdk::BUTTON_PRESS_MASK);
	add_events(Gdk::BUTTON_RELEASE_MASK);
	Glib::signal_timeout().connect(sigc::mem_fun(*this,&ClvFileArea::on_blink_time),1000);
	im_context = gtk_im_multicontext_new();
	g_signal_connect (im_context, "commit", G_CALLBACK (&ClvFileArea::im_commit_proxy), this);
	g_signal_connect (im_context, "preedit-changed",G_CALLBACK (&ClvFileArea::preedit_changed_proxy), this);
	g_signal_connect (im_context, "retrieve-surrounding",G_CALLBACK (&ClvFileArea::retrieve_surrounding_proxy), this);
	g_signal_connect (im_context, "delete-surrounding",G_CALLBACK (&ClvFileArea::delete_surrounding_proxy), this);
	property_can_focus() = true;
	auto style = get_style_context();
	style->context_save();
	style->add_class(GTK_STYLE_CLASS_VIEW);
	style->context_restore();
	set_size_request( 0, file_context.end().get_fill_offset());
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
	this->signal_draw().connect(sigc::mem_fun(*this,&ClvFileArea::on_draw));
#endif
	input_status=STATUS_NONE;
}

#include <iostream>

bool ClvFileArea::on_blink_time(){
	//get_window()->invalidate_rect(Gdk::Rectangle{10,10,10,10}, false);
	//get_window()->invalidate(false);d
	//get_window()->invalidate_region(Cairo::RefPtr<Cairo::Region>region,false)
	Gdk::Rectangle rect{10,10,10,10};
	std::cerr<<"time\n";
	return true;
}

void ClvFileArea::on_realize(){
	GdkWindowAttr attributes;
	auto allocation = get_allocation();
	attributes.x = allocation.get_x();
	attributes.y = allocation.get_y();
	attributes.width =1;// allocation.get_width();
	attributes.height =1;// allocation.get_height();
	std::cerr<<attributes.x<<":"<<attributes.y<<":"<<attributes.width<<":"<<attributes.height<<"\n";
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
	auto client_window = Gdk::Window::create(get_parent_window(), &attributes, GDK_WA_X | GDK_WA_Y );
	//client_window->show();
	register_window(client_window);
	client_window->lower();
	gtk_im_context_set_client_window(im_context,client_window->gobj());
	Gtk::DrawingArea::on_realize();
}

void ClvFileArea::on_unrealize(){
	Gtk::DrawingArea::on_unrealize();
	//need unregister client_window for im??
}

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


bool ClvFileArea::on_focus_in_event(GdkEventFocus* event){
	gtk_im_context_focus_in(im_context);
	input_status=STATUS_NORMAL;
	return false;
}


bool ClvFileArea::on_key_press_event(GdkEventKey *event){
	std::cout<<"press"<<(unsigned int)*event->string<<std::endl;
	switch(input_status){
		case STATUS_INPUT:
		{
			if(*event->string==0x8||*event->string==0x7f){
				std::cout<<"input done\n";//commit change
				input_status=STATUS_DELETE;
			}
			break;
		}
		case STATUS_DELETE:
		{
			if(*event->string!=0x8&&*event->string!=0x7f){
				std::cout<<"delete done\n";//commit change
				input_status=STATUS_INPUT;
			}
			break;
		}
		default:
			if(*event->string==0x8||*event->string==0x7f)
				input_status=STATUS_DELETE;
			else
				input_status=STATUS_INPUT;
	}
	return true;
}
#ifdef CLV_RKET_EVENT
bool ClvFileArea::on_key_release_event(GdkEventKey *event){
	std::cout<<"release"<<event->string<<std::endl;
	switch(input_status){
		case STATUS_KEY_PRESS:
		{
			if(*event->string==0x8||*event->string==0x7f)
				input_status=STATUS_DELETE;
			else
				input_status=STATUS_INPUT;
			break;
		}
		default:
			std::cerr<<"key error no press\n";
	}
	return true;
}
#endif

void ClvFileArea::im_commit_proxy(GtkIMContext *context,const gchar  *str,ClvFileArea* pobj){
	pobj->im_commit(context,str);
}

void ClvFileArea::im_commit(GtkIMContext *context,const gchar *str){
	std::cerr<<"commit"<<str<<'\n';
	if(input_status!=STATUS_INPUT)
		input_status=STATUS_INPUT;//commit change
}

void ClvFileArea::preedit_changed_proxy(GtkIMContext *context,ClvFileArea *pobj){
	pobj->preedit_changed(context);
}

void ClvFileArea::preedit_changed(GtkIMContext *context){
	std::cerr<<"preedit_changed"<<'\n';
}

void ClvFileArea::retrieve_surrounding_proxy(GtkIMContext *context,ClvFileArea *pobj){
	std::cerr<<"retrieve_surrounding_proxy"<<'\n'; 
}

void ClvFileArea::delete_surrounding_proxy(GtkIMContext *context,gint offset,gint n_chars,ClvFileArea *pobj){
	std::cerr<<"delete_surrounding_proxy"<<'\n'; 
}

bool ClvFileArea::on_button_press_event(GdkEventButton* event){
	if(!gtk_widget_has_focus(Widget::gobj())){
		grab_focus();
	}
	//std::cout<<"bt press"<<std::endl;
	if(event->button == GDK_BUTTON_PRIMARY){
		switch(input_status){
		case STATUS_S_CLICKED://to double clicked
		{
			input_status=STATUS_D_CLICKED;
			break;
		}
		default:
			input_status=STATUS_S_CLICKED;
		}
	}
	else if(event->button == GDK_BUTTON_SECONDARY){
	
	}
	else{
		std::cout<<"button_else\n";
	}
	return true;
}

bool ClvFileArea::on_button_release_event(GdkEventButton* event){
	//std::cout<<"bt release"<<std::endl;
	if(event->button == GDK_BUTTON_PRIMARY){
		switch(input_status){
		case STATUS_S_CLICKED://put cursor
		{
			auto line_itr = file_context.get_form_fill(event->y);
			if(line_itr!=file_context.end()){
				line_itr->x_to_index(event->x);
			}
			input_status=STATUS_NORMAL;
			std::cout<<"s click"<<std::endl;
			break;
		}
		case STATUS_D_CLICKED://selected
		{
			auto line_itr = file_context.get_form_fill(event->y);
			if(line_itr!=file_context.end()){
				line_itr->x_to_index(event->x);
			}
			input_status=STATUS_SELECTED;
			std::cout<<"d click"<<std::endl;
			break;
		}
		default:
			input_status=STATUS_NORMAL;
		}
	}
	else if(event->button == GDK_BUTTON_SECONDARY){
	
	}
	else{
		std::cout<<"button_else\n";
	}
	return true;
}

ClvThumArea::ClvThumArea(){

}

ClvThumArea::~ClvThumArea(){

}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	return true;
}

