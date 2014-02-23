#include "clvview.h"
#include "dbg.h"

bool ClvLineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	if(should_draw_window(cr,get_window())){
		cr->set_source_rgb(0.0, 0.0, 0.0);
		cr->paint();
	}
	return true;
}

ClvFileArea::ClvFileArea(context<line> &file):
	Glib::ObjectBase(typeid(ClvFileArea)),
	Gtk::Scrollable(),
	Gtk::DrawingArea(),
	file_context(file),
	backing_surface_x(0),backing_surface_y(0),
	backing_surface_w(0),backing_surface_h(0)
	//clvhScrollPolicy(*this, "hscroll-policy" , Gtk::SCROLL_NATURAL),
	//clvvScrollPolicy(*this, "vscroll-policy" , Gtk::SCROLL_NATURAL)
	{
#if 1
	//This shows the GType name, which must be used in the CSS file.
	debug << "GType name: " << G_OBJECT_TYPE_NAME(Glib::Object::gobj()) << std::endl;
	//This shows that the GType still derives from GtkWidget:
	debug << "Gtype is a Scrollable?: " <<( GTK_IS_SCROLLABLE(Glib::Object::gobj()) ? "Yes" : "No" )<< std::endl;
	debug << "Gtype is a DrawingArea?: " <<( GTK_IS_DRAWING_AREA(Glib::Object::gobj()) ? "Yes" : "No" )<< std::endl;
#endif
	add_events(Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK|Gdk::SCROLL_MASK|Gdk::TOUCH_MASK|Gdk::SMOOTH_SCROLL_MASK);

	im_context = gtk_im_multicontext_new();
	g_signal_connect (im_context, "commit",G_CALLBACK (&ClvFileArea::im_commit_proxy), this);
	g_signal_connect (im_context, "preedit-changed",G_CALLBACK (&ClvFileArea::preedit_changed_proxy), this);
	g_signal_connect (im_context, "retrieve-surrounding",G_CALLBACK (&ClvFileArea::retrieve_surrounding_proxy), this);
	g_signal_connect (im_context, "delete-surrounding",G_CALLBACK (&ClvFileArea::delete_surrounding_proxy), this);
	property_can_focus() = true;
	auto style = get_style_context();
	style->context_save();
	style->add_class(GTK_STYLE_CLASS_VIEW);
	style->context_restore();
	//set_size_request( 0, file_context.end().get_fill_offset());
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
	this->signal_draw().connect(sigc::mem_fun(*this,&ClvFileArea::on_draw));
#endif
	input_status=STATUS_NONE;
}

void ClvFileArea::on_realize(){
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
	get_hadjustment()->configure(0,0,1200,1,10,100);
	get_vadjustment()->configure(0,0,2400,1,10,100);
#if 0
	debug<<"get_parent"<<get_parent()<<std::endl;
	debug<<"get_hadjustment:"<<get_hadjustment().operator->()<<":this="<<this<<std::endl;
	debug<<"get_vadjustment:"<<get_vadjustment().operator->()<<":this="<<this<<std::endl;
	GtkAdjustment *adj = NULL;
	g_object_get (Gtk::Scrollable::gobj(), "hadjustment", &adj, NULL);
	if (adj)
		g_object_unref (adj);
  	debug<<Gtk::Scrollable::gobj()<<"gobj_hadjustment:"<<adj<<":"<<get_hadjustment()->gobj()<<std::endl;
#endif
	get_hadjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&ClvFileArea::on_hadjustment));
	get_vadjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&ClvFileArea::on_vadjustment));
	Gtk::DrawingArea::on_realize();
	//get_window()->resize(3000,3000);
}

void ClvFileArea::on_unrealize(){
	Gtk::DrawingArea::on_unrealize();
	//FIXME:need unregister client_window for im??
}

bool ClvFileArea::on_configure_event(GdkEventConfigure* event){
	debug<<__func__<<std::endl;
	cover_surface_ptr = get_window()->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA, get_allocation().get_width(), get_allocation().get_width());
	return false;
}

void ClvFileArea::on_size_allocate(Gtk::Allocation& allocation){
	debug << "size_allocate:" <<allocation.get_height()<<std::endl;
	Gtk::DrawingArea::on_size_allocate(allocation);
	if(get_realized()){
		get_window()->move_resize(allocation.get_x(),allocation.get_y(),allocation.get_width(),allocation.get_height());
	}
	Gtk::Allocation context_allocation{0,0,get_hadjustment()->get_upper(),get_vadjustment()->get_upper()};

	//allocation.
}

void ClvFileArea::set_activates(bool setting){
	if(setting){
		blink_time_out = Glib::signal_timeout().connect(sigc::mem_fun(*this,&ClvFileArea::on_blink_time),1000);
	}
	else if(blink_time_out)
		blink_time_out.disconnect();
	//debug<<"activates"<<this<<':'<<setting<<'\n';
}

bool ClvFileArea::on_blink_time(){
	return true;
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
		debug<<"surface:"<<surface_ptr->cobj()<<std::endl;
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
void ClvFileArea::on_hadjustment(){
	//debug<<"hadjustment:"<<std::endl;
	int x,y;
	get_window()->get_position(x,y);
	int new_x = - get_hadjustment()->get_value();
	get_window()->move(new_x,y);
}

void ClvFileArea::on_vadjustment(){
	//debug<<"vadjustment:window="<<get_window().operator->()<<std::endl;
	int x,y;
	get_window()->get_position(x,y);
	int new_y = - get_vadjustment()->get_value();
	get_window()->move(x,new_y);
}

/* The extra size of the offscreen surface we allocate
   to make scrolling more efficient */
#define DEFAULT_EXTRA_SIZE 64
int ClvFileArea::extra_width = DEFAULT_EXTRA_SIZE;
int ClvFileArea::extra_height = DEFAULT_EXTRA_SIZE;

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
	debug<<get_window()->get_width()<<'x'<<get_window()->get_height()<<std::endl;

	Cairo::RectangleInt view_rect{0,0,get_window()->get_width(),get_window()->get_height()};
	Cairo::RectangleInt canvas_rect{-get_hadjustment()->get_value(),-get_vadjustment()->get_value(),1200,2400};
	Cairo::RectangleInt view_in_canvas_pos{-canvas_rect.x,-canvas_rect.y,view_rect.width,view_rect.height};
	int surface_w = view_rect.width;
	int surface_h = view_rect.height;
	//auto bg = get_window()->get_background_pattern();
	//if(bg && bg->get_type()==Cairo::PATTERN_TYPE_SOLID)
	//	cairo_pattern_get_rgba(); //alpha==1.0??
	//Cairo::Content def_content = Cairo::CONTENT_ALPHA or Cairo::CONTENT_COLOR;//like gtktextview
	if(canvas_rect.width > surface_w)
		surface_w = std::min(surface_w + extra_width,canvas_rect.width); 
	if(canvas_rect.height > surface_h)
		surface_h = std::min(surface_h + extra_height,canvas_rect.height);
	//check alpha surface
	if(!backing_surface_ptr ||
		backing_surface_w < std::max(surface_w - 32, view_rect.width) || backing_surface_w > surface_w + 32||
		backing_surface_h < std::max(surface_h - 32, view_rect.height) || backing_surface_h > surface_h + 32){//or need re-build surface
		backing_surface_ptr = get_window()->create_similar_surface(Cairo::CONTENT_ALPHA,surface_w,surface_h);
		backing_surface_w=surface_w,backing_surface_h=surface_h;
		debug<<"fill alpha_surface_ptr:"<<backing_surface_ptr.operator->()<<std::endl;

	}
	auto backing_cr = Cairo::Context::create(backing_surface_ptr);
	backing_cr->set_source_rgba(0,0,0,0);
	backing_cr->set_operator(Cairo::OPERATOR_SOURCE);
	backing_cr->paint();
	backing_cr->set_operator(Cairo::OPERATOR_OVER);
	backing_cr->save();
	//draw(backing_cr,)
	backing_cr->restore();

	cr->set_source_rgb(0.3, 0.4, 0.5);
	cr->paint();
	//cr->set_source(cover_surface_ptr,0,0);
	std::vector<Cairo::Rectangle> clip_rects;
	cr->copy_clip_rectangle_list(clip_rects);
	for(auto& clip_rect:clip_rects){
		debug<<"x"<<clip_rect.x<<"y"<<clip_rect.y<<"\tw="<<clip_rect.width<<" h="<<clip_rect.height<<std::endl;
		draw(cr,clip_rect);
	}
	//debug<<"context:"<<cr->cobj()<<std::endl;
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
	debug<<__FUNCTION__<<std::endl;
	return false;
}

bool ClvFileArea::on_key_press_event(GdkEventKey *event){
	//debug<<__FUNCTION__<<event->string<<std::endl;
	switch(input_status){
		case STATUS_INPUT:
		{
			if(*event->string==0x8||*event->string==0x7f){
				//debug<<"input done\n";//commit change
				input_status=STATUS_DELETE;
			}
			break;
		}
		case STATUS_DELETE:
		{
			if(*event->string!=0x8&&*event->string!=0x7f){
				//debug<<"delete done\n";//commit change
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

void ClvFileArea::im_commit_proxy(GtkIMContext *context,const gchar  *str,ClvFileArea* pobj){
	pobj->im_commit(context,str);
}

void ClvFileArea::im_commit(GtkIMContext *context,const gchar *str){
	debug<<"commit"<<str<<'\n';
	if(input_status!=STATUS_INPUT)
		input_status=STATUS_INPUT;//commit change
}

void ClvFileArea::preedit_changed_proxy(GtkIMContext *context,ClvFileArea *pobj){
	pobj->preedit_changed(context);
}

void ClvFileArea::preedit_changed(GtkIMContext *context){
	debug<<"preedit_changed"<<'\n';
}

void ClvFileArea::retrieve_surrounding_proxy(GtkIMContext *context,ClvFileArea *pobj){
	debug<<"retrieve_surrounding_proxy"<<'\n'; 
}

void ClvFileArea::delete_surrounding_proxy(GtkIMContext *context,gint offset,gint n_chars,ClvFileArea *pobj){
	debug<<"delete_surrounding_proxy"<<'\n'; 
}

bool ClvFileArea::on_button_press_event(GdkEventButton* event){
	debug<<this<<':'<<get_vadjustment()->get_upper()<<std::endl;
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
		//debug<<"button_else\n";
	}
	return true;
}

bool ClvFileArea::on_button_release_event(GdkEventButton* event){
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
				//debug<<"s click"<<std::endl;
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

ClvThumArea::ClvThumArea(){

}

ClvThumArea::~ClvThumArea(){

}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	return true;
}

