#include "clvview.h"

bool ClvLineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	if(should_draw_window(cr,get_window())){
		cr->set_source_rgb(0.0, 0.0, 0.0);
		cr->paint();
	}
	return true;
}
#include <iostream>
ClvFileArea::ClvFileArea(context<line> &file):
	Glib::ObjectBase(typeid(ClvFileArea)),
	Gtk::Scrollable(),
	Gtk::DrawingArea(),
	file_context(file)
	//clvhAdjustment(*this, "hadjustment" , Gtk::Adjustment::create(0,0,100)),
	//clvvAdjustment(*this, "vadjustment" , Gtk::Adjustment::create(0,0,100)),
	//clvhAdjustment(*this, "hadjustment"),
	//clvvAdjustment(*this, "vadjustment"),
	//clvhScrollPolicy(*this, "hscroll-policy" , Gtk::SCROLL_NATURAL),
	//clvvScrollPolicy(*this, "vscroll-policy" , Gtk::SCROLL_NATURAL)
	{

#if 1
	//This shows the GType name, which must be used in the CSS file.
	std::cout << "GType name: " << G_OBJECT_TYPE_NAME(Glib::Object::gobj()) << std::endl;
	//This shows that the GType still derives from GtkWidget:
	std::cout << "Gtype is a Scrollable?:" <<( GTK_IS_SCROLLABLE(Glib::Object::gobj()) ? "Yes" : "No" )<< std::endl;
	std::cout << "Gtype is a DrawingArea?:" <<( GTK_IS_DRAWING_AREA(Glib::Object::gobj()) ? "Yes" : "No" )<< std::endl;
#endif
	add_events(Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK|Gdk::SCROLL_MASK|Gdk::TOUCH_MASK|Gdk::SMOOTH_SCROLL_MASK);

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
	/*
	set_hadjustment(clvhAdjustment.get_value());
	set_vadjustment(clvvAdjustment.get_value());
	std::cout << "hadjustment:" << get_hadjustment().operator->()<<std::endl;
	std::cout << "clvhAdjustment:"<< clvhAdjustment.get_value().operator->() << std::endl;
	clvhAdjustment.get_value()->signal_changed().connect(sigc::mem_fun(*this,&ClvFileArea::on_adjustment));
	clvhAdjustment.get_value()->configure(0,0,50000,1,10,100);
	clvvAdjustment.get_value()->signal_changed().connect(sigc::mem_fun(*this,&ClvFileArea::on_adjustment));
	clvvAdjustment.get_value()->configure(0,0,50000,1,10,100);
	*/
	set_size_request( 0, file_context.end().get_fill_offset());

#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
	this->signal_draw().connect(sigc::mem_fun(*this,&ClvFileArea::on_draw));
#endif
	input_status=STATUS_NONE;
}

#include <iostream>

void ClvFileArea::on_realize(){
	GdkWindowAttr attributes;
	auto allocation = get_allocation();
	attributes.x = allocation.get_x();
	attributes.y = allocation.get_y();
	attributes.width =1;// allocation.get_width();
	attributes.height =1;// allocation.get_height();
	std::cerr<<__FUNCTION__<<attributes.x<<":"<<attributes.y<<":"<<attributes.width<<":"<<attributes.height<<"\n";
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
	auto client_window = Gdk::Window::create(get_parent_window(), &attributes, Gdk::WA_X | Gdk::WA_Y );
	//client_window->show();
	register_window(client_window);
	client_window->lower();
	gtk_im_context_set_client_window(im_context,client_window->gobj());
	Gtk::DrawingArea::on_realize();
	//Scrollable::add_interface(Scrollable::get_type());
}

void ClvFileArea::on_unrealize(){
	Gtk::DrawingArea::on_unrealize();
	//FIXME:need unregister client_window for im??
}

bool ClvFileArea::on_configure_event(GdkEventConfigure* event){
	cover_surface_ptr = get_window()->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA, get_allocation().get_width(), get_allocation().get_width());

	return false;
}

void ClvFileArea::on_size_allocate(Gtk::Allocation& allocation){

	std::cout << "hadjustment:" << get_hadjustment().operator->()<<std::endl;
	//std::cout << "clvhAdjustment:"<< clvhAdjustment.get_value().operator->() << std::endl;
	get_hadjustment()->signal_changed().connect(sigc::mem_fun(*this,&ClvFileArea::on_adjustment));
	get_hadjustment()->configure(0,0,50000,1,10,100);
	get_vadjustment()->signal_changed().connect(sigc::mem_fun(*this,&ClvFileArea::on_adjustment));
	get_vadjustment()->configure(0,0,50000,1,10,100);
	Gtk::DrawingArea::on_size_allocate(allocation);
}

bool ClvFileArea::on_scroll_event(GdkEventScroll* event){
	switch(event->direction){
		case GDK_SCROLL_SMOOTH:
		break;
		default:
		;
	}
	return true;
}


void ClvFileArea::set_activates(bool setting){
	if(setting){
		blink_time_out = Glib::signal_timeout().connect(sigc::mem_fun(*this,&ClvFileArea::on_blink_time),1000);
	}
	else if(blink_time_out)
		blink_time_out.disconnect();
	//std::cerr<<"activates"<<this<<':'<<setting<<'\n';
}

bool ClvFileArea::on_blink_time(){
	static bool bc;
	if(bc){
		auto cr = get_window()->create_cairo_context();
		cr->set_source(cover_surface_ptr,0,0);
		cr->paint();
	}
	else{
		auto cr = get_window()->create_cairo_context();
#ifdef CLV_SURFACE_BLINK
		cr->set_source(surface_ptr,0,0);
		cr->rectangle(110,310,100,100);
		cr->fill();
		double x,y;
		surface_ptr->get_device_offset(x,y);
		std::cerr<<"surface:"<<surface_ptr->cobj()<<std::endl;
#endif
		Gdk::Rectangle rect{110,310,100,100};
		Gdk::Cairo::add_rectangle_to_path(cr,rect);
		get_window()->invalidate_rect(rect , false);
	}
	bc=!bc;

	for(auto& cursor:file_context.cursors){
		;
	}
	return true;
}

//void ClvFileArea::on_hide(){
//}

void ClvFileArea::draw(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect){
	cr->save();//need RAII mode restore
	auto itr = file_context.get_form_fill(rect.y);
	int height = 0;
	//if(itr != file_context.end())
	height = itr.get_fill_offset() - rect.y;//FIXME need a better way
	while(height<rect.height && itr != file_context.end()){
		itr->draw_to_context(cr, itr.get_fill_offset()+itr->get_fill(),rect);
		height += itr->get_fill();
		++itr;
	}	
	cr->clip();
	cr->restore();
}

bool ClvFileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	cr->set_source_rgb(0.3, 0.4, 0.5);
	cr->paint();
	//cr->set_source(cover_surface_ptr,0,0);
	std::vector<Cairo::Rectangle> clip_rects;
	cr->copy_clip_rectangle_list(clip_rects);
	for(auto& clip_rect:clip_rects){
		//std::cerr<<"x"<<clip_rect.x<<"y"<<clip_rect.y<<"\tw="<<clip_rect.width<<" h="<<clip_rect.height<<std::endl;
		draw(cr,clip_rect);
	}
	std::cerr<<"context:"<<cr->cobj()<<std::endl;
#ifdef CLV_SURFACE_BLINK
	surface_ptr = cr->get_target();
#endif
	return true;//Gtk::TextView::on_draw(cr);
}


bool ClvFileArea::on_focus_in_event(GdkEventFocus* event){
	gtk_im_context_focus_in(im_context);
	input_status=STATUS_NORMAL;
	return false;
}

bool ClvFileArea::on_focus_out_event(GdkEventFocus* event){
	std::cerr<<__FUNCTION__<<"\n";
	return false;
}

bool ClvFileArea::on_key_press_event(GdkEventKey *event){
	std::cout<<__FUNCTION__<<(unsigned int)*event->string<<std::endl;
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
	std::cout<<__FUNCTION__<<event->string<<std::endl;
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
				if(line_itr->empty()){
					std::cerr<<"BUG:empty line:"<<__func__<<std::endl;
					return true;
				}
				auto glyph_itr = line_itr->x_to_glyph_itr(event->x);
				std::cout<<"s click"<<std::endl;
				auto cr = Cairo::Context::create(cover_surface_ptr);
				cr->set_source_rgba(1,1,1,0.7);
				cr->rectangle(glyph_itr->x,glyph_itr->y,2,-18);
				cr->fill();
				cr->set_operator(Cairo::Operator::OPERATOR_XOR);
			}
			input_status=STATUS_NORMAL;
			break;
		}
		case STATUS_D_CLICKED://selected
		{
			auto line_itr = file_context.get_form_fill(event->y);
			if(line_itr!=file_context.end()){
				line_itr->x_to_glyph_itr(event->x);
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

