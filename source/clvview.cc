#include "clvview.h"
#include "dbg.h"

namespace clv{

bool LineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	if(should_draw_window(cr,get_window())){
		cr->set_source_rgb(0.0, 0.0, 0.0);
		cr->paint();
	}
	return true;
}

FileArea::FileArea(context<line> &file):
	file_context(file) {
	//debug<<__PRETTY_FUNCTION__<<std::endl;
	add_events(Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK|Gdk::SCROLL_MASK|Gdk::TOUCH_MASK|Gdk::SMOOTH_SCROLL_MASK|Gdk::EXPOSURE_MASK);
	g_signal_connect (im_context, "commit",G_CALLBACK (&FileArea::im_commit_proxy), this);
	g_signal_connect (im_context, "preedit-changed",G_CALLBACK (&FileArea::preedit_changed_proxy), this);
	g_signal_connect (im_context, "retrieve-surrounding",G_CALLBACK (&FileArea::retrieve_surrounding_proxy), this);
	g_signal_connect (im_context, "delete-surrounding",G_CALLBACK (&FileArea::delete_surrounding_proxy), this);
	property_can_focus() = true;
	auto style = get_style_context();
	style->context_save();
	style->add_class(GTK_STYLE_CLASS_VIEW);
	style->context_restore();
	//set_size_request( 0, file_context.end().get_fill_offset());
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
	this->signal_draw().connect(sigc::mem_fun(*this,&FileArea::on_draw));
#endif
	input_status=STATUS_NONE;
}

FileArea::~FileArea(){
	//debug<<__PRETTY_FUNCTION__<<std::endl;
	//FIXME im_context?
	g_object_unref (im_context);
}

void FileArea::on_realize(){
	//debug<<__PRETTY_FUNCTION__<<std::endl;
	GdkWindowAttr attributes;
	auto allocation = get_allocation();
	attributes.x = allocation.get_x();
	attributes.y = allocation.get_y();
	attributes.width =1;// allocation.get_width();
	attributes.height =1;// allocation.get_height();
	//debug<<__FUNCTION__<<attributes.x<<":"<<attributes.y<<":"<<attributes.width<<":"<<attributes.height<<"\n";
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.wclass = GDK_INPUT_ONLY;
	attributes.event_mask = get_events();
	attributes.event_mask |= (Gdk::BUTTON_PRESS_MASK |
		Gdk::BUTTON_RELEASE_MASK |
		Gdk::BUTTON1_MOTION_MASK |
		Gdk::BUTTON3_MOTION_MASK |
		Gdk::POINTER_MOTION_HINT_MASK |
		Gdk::POINTER_MOTION_MASK |
		Gdk::ENTER_NOTIFY_MASK |
		Gdk::LEAVE_NOTIFY_MASK );
#if 1
	auto client_window = Gdk::Window::create(get_parent_window(), &attributes, Gdk::WA_X | Gdk::WA_Y );
	//client_window->show();
	register_window(client_window);
	client_window->lower();
	gtk_im_context_set_client_window(im_context,client_window->gobj());
#endif
	Gtk::DrawingArea::on_realize();
	//get_window()->set_user_data(gobj());
}

void FileArea::on_unrealize(){
	//debug<<__PRETTY_FUNCTION__<<std::endl;
	Gtk::DrawingArea::on_unrealize();
	//FIXME:need unregister client_window for im??
}

bool FileArea::on_configure_event(GdkEventConfigure* event){
	//debug<<__PRETTY_FUNCTION__<<std::endl;
	//glyphs_surface_ptr = get_window()->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA, get_allocation().get_width(), get_allocation().get_width());
	return false;
}

void FileArea::on_size_allocate(Gtk::Allocation& allocation){
	Gtk::DrawingArea::on_size_allocate(allocation);
	if(get_realized()){
		debug <<__PRETTY_FUNCTION__<<allocation.get_height()<<std::endl;
		get_window()->move_resize(allocation.get_x(),allocation.get_y(),allocation.get_width(),allocation.get_height());
	}
}

void FileArea::set_activates(bool setting){
	if(setting){
		blink_time_out = Glib::signal_timeout().connect(sigc::mem_fun(*this,&FileArea::on_timer),100);
	}
	else if(blink_time_out)
		blink_time_out.disconnect();
	//debug<<"activates"<<this<<':'<<setting<<'\n';
}

bool FileArea::on_timer(){
	auto draw_object_cr = get_window()->create_cairo_context();
	auto dms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-refresh_time).count(); 
	dms%=1000;
	for(auto& cursor:file_context.cursors){
		;
	}
	Gdk::Rectangle rect{110,10,3,18};
	Gdk::Cairo::add_rectangle_to_path(draw_object_cr,rect);
	draw_object_cr->set_source_rgb(0.3,0.4,0.5);
	draw_object_cr->fill_preserve();
	draw_object_cr->set_source(glyphs_surface_ptr,0,0);
	draw_object_cr->fill_preserve();
	draw_object_cr->set_source_rgba(1,1,1,static_cast<double>(dms)/1000);
	draw_object_cr->fill();
	return true;
}

//void FileArea::on_hide(){
//}

struct draw_level_store{
	draw_level_store(const Cairo::RefPtr<Cairo::Context>& context):cr(context){
		cr->save();
	}
	~draw_level_store(){
		cr->restore();
	}
	auto operator->(){
		return cr.operator->();
	}
private:
	const Cairo::RefPtr<Cairo::Context>& cr;
};

void FileArea::draw_rect(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect){
	draw_level_store store{cr};
	store->set_source_rgb(1,1,1);

	auto itr = file_context.get_form_fill(rect.y);
	int height = 0;
	//if(itr != file_context.end())

	height = itr.get_fill_offset() - rect.y;//FIXME need a better way
	while(height<rect.height && itr != file_context.end()){
		itr->draw_to_context(cr, itr.get_fill_offset()+itr->get_fill(),rect);
		height += itr->get_fill();
		++itr;
	}	
	store->clip();
}


bool FileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	debug<<__PRETTY_FUNCTION__<<std::endl;
	glyphs_surface_ptr = cr->get_target();
	debug<<get_window()->get_width()<<'x'<<get_window()->get_height()<<std::endl;

	cr->set_source_rgba(0,0,0,0.0);

	std::vector<Cairo::Rectangle> clip_rects;
	cr->copy_clip_rectangle_list(clip_rects);
	for(auto& clip_rect:clip_rects){
		debug<<"x"<<clip_rect.x<<"y"<<clip_rect.y<<"\tw="<<clip_rect.width<<" h="<<clip_rect.height<<std::endl;
		draw_rect(cr,clip_rect);
	}
	cr->paint();
	return true;
}


bool FileArea::on_focus_in_event(GdkEventFocus* event){
	gtk_im_context_focus_in(im_context);
	input_status=STATUS_NORMAL;
	return false;
}

bool FileArea::on_focus_out_event(GdkEventFocus* event){
	debug<<__FUNCTION__<<std::endl;
	return false;
}

bool FileArea::on_key_press_event(GdkEventKey *event){
	//debug<<__FUNCTION__<<event->string<<std::endl;
	switch(input_status){
		case STATUS_INPUT:
		{
			if(*event->string==0x8||*event->string==0x7f){
				input_status=STATUS_DELETE;
			}
			break;
		}
		case STATUS_DELETE:
		{
			if(*event->string!=0x8&&*event->string!=0x7f){
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
bool FileArea::on_key_release_event(GdkEventKey *event){
	//debug<<__FUNCTION__<<event->string<<std::endl;
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
			debug<<"key error no press\n";
	}
	return true;
}
#endif

void FileArea::im_commit_proxy(GtkIMContext *context,const gchar  *str,FileArea* pobj){
	pobj->im_commit(context,str);
}

void FileArea::im_commit(GtkIMContext *context,const gchar *str){
	debug<<"commit"<<str<<'\n';
	if(input_status!=STATUS_INPUT)
		input_status=STATUS_INPUT;//commit change
}

void FileArea::preedit_changed_proxy(GtkIMContext *context,FileArea *pobj){
	pobj->preedit_changed(context);
}

void FileArea::preedit_changed(GtkIMContext *context){
	debug<<"preedit_changed"<<'\n';
}

void FileArea::retrieve_surrounding_proxy(GtkIMContext *context,FileArea *pobj){
	debug<<"retrieve_surrounding_proxy"<<'\n'; 
}

void FileArea::delete_surrounding_proxy(GtkIMContext *context,gint offset,gint n_chars,FileArea *pobj){
	debug<<"delete_surrounding_proxy"<<'\n'; 
}

bool FileArea::on_button_press_event(GdkEventButton* event){
//	debug<<this<<':'<<get_vadjustment()->get_upper()<<std::endl;
	if(!gtk_widget_has_focus(Widget::gobj())){
		grab_focus();
	}
	////debug<<"bt press"<<std::endl;
	if(event->button == GDK_BUTTON_PRIMARY){
		switch(input_status){
		case STATUS_S_CLICKED://to double clicked
			//static int i;
			//get_vadjustment()->set_value(i+=10);
			input_status=STATUS_D_CLICKED;
			break;
		default:
			input_status=STATUS_S_CLICKED;
		}
	}
	else if(event->button == GDK_BUTTON_SECONDARY){
	
	}
	else{
		debug<<"button_else\n";
	}
	return true;
}

bool FileArea::on_button_release_event(GdkEventButton* event){
	////debug<<"bt release"<<std::endl;
	if(event->button == GDK_BUTTON_PRIMARY){
		switch(input_status){
		case STATUS_S_CLICKED://put cursor
		{
			auto line_itr = file_context.get_form_fill(event->y);
			if(line_itr!=file_context.end()){
				if(line_itr->empty()){
					debug<<"BUG:empty line:"<<__func__<<std::endl;
					return true;
				}
				auto glyph_itr = line_itr->x_to_glyph_itr(event->x);
				
				auto cr = Cairo::Context::create(glyphs_surface_ptr);
				cr->set_source_rgba(1,1,1,0.7);
				//cr->rectangle(glyph_itr->x,glyph_itr->y,2,-18);
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
			//debug<<"d click"<<std::endl;
			break;
		}
		default:
			input_status=STATUS_NORMAL;
		}
	}
	else if(event->button == GDK_BUTTON_SECONDARY){
	
	}
	else{
		//debug<<"button_else\n";
	}
	return true;
}

ThumArea::ThumArea(){

}

ThumArea::~ThumArea(){

}

bool ThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	return true;
}

};//namespace
