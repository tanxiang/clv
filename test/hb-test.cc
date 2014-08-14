//g++  `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config harfbuzz --libs` --std=c++11
#include <cairo-ft.h>
#include <hb-ft.h>
#include <iostream>
#include <gtkmm/drawingarea.h>

class MyArea : public Gtk::DrawingArea
{
public:
  MyArea();
  virtual ~MyArea();

protected:
  //Override default signal handler:
  virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

private:
  void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
  void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int rectangle_width, int rectangle_height);

};

class HBCairoFace:public Cairo::UserFontFace{
};

MyArea::MyArea()
{
}

MyArea::~MyArea()
{
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();

	const int rectangle_width = width;
	const int rectangle_height = height / 2;
	
	GError *error = NULL;
	GMappedFile *mf = g_mapped_file_new ("SourceCodePro-Regular.otf", false, &error);
	if (!mf) {
		std::cerr<<error->message<<"\n";
		g_error_free (error);
		return true;
	}

	hb_blob_t* blob = hb_blob_create (g_mapped_file_get_contents (mf),g_mapped_file_get_length (mf),
		HB_MEMORY_MODE_READONLY_MAY_MAKE_WRITABLE,(void *) mf, (hb_destroy_func_t) g_mapped_file_unref);
	hb_face_t *face = hb_face_create (blob, 0);
	auto font = hb_font_create (face);
		std::cerr<<"font:"<<font<<'\n';
		std::cerr<<"hb_ft_font_get_face(font):"<<hb_ft_font_get_face(font)<<'\n';
	hb_blob_destroy(blob);

	unsigned int upem = hb_face_get_upem (face);
	hb_font_set_scale (font, upem, upem);
	hb_ft_font_set_funcs (font);
	auto cr_font = Cairo::FtFontFace::create(hb_ft_font_get_face(font),0);
	auto sc_font = Cairo::ScaledFont::create(cr_font,Cairo::scaling_matrix(12,12),Cairo::identity_matrix());
	if(sc_font)
		std::cerr<<"cr-font\n";
	else
		std::cerr<<"cr-font null\n";
	hb_face_destroy (face);
	cr->set_scaled_font(sc_font);
	auto buffer = hb_buffer_create ();
	hb_buffer_add_utf8 (buffer, "tacde", 6, 0, 6);
	    hb_buffer_set_direction (buffer, HB_DIRECTION_LTR);
	    
	hb_shape (font, buffer, NULL, 0);
	auto hb_glyph = hb_buffer_get_glyph_infos (buffer, NULL);
	auto hb_position = hb_buffer_get_glyph_positions (buffer, NULL);
	std::vector< Cairo::Glyph > glyphs{hb_buffer_get_length (buffer)};
	int i=0,x=20,y=30;
	for(auto& gl:glyphs){
		gl.index=hb_glyph++->codepoint;
		gl.x =( hb_position->x_offset + x);
		gl.y =(-hb_position->y_offset + y);
		x +=  hb_position->x_advance;
		y += -hb_position->y_advance;
		hb_position++;
		std::cerr<<"position++\n";
	}
	glyphs.push_back(Cairo::Glyph{0xFFFFFFFF,x,y});
	
	cr->show_glyphs(glyphs);
	return true;
}

void MyArea::draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr,
	int width, int height)
{
	cr->rectangle(0, 0, width, height);
	cr->fill();
}

void MyArea::draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
	int rectangle_width, int rectangle_height)
{
	// http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
	Pango::FontDescription font;

	font.set_family("Monospace");
	font.set_weight(Pango::WEIGHT_BOLD);

	// http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
	Glib::RefPtr<Pango::Layout> layout = create_pango_layout("Hi there!");

	layout->set_font_description(font);

	int text_width;
	int text_height;

	//get the text dimensions (it updates the variables -- by reference)
	layout->get_pixel_size(text_width, text_height);

	// Position the text in the middle
	cr->move_to((rectangle_width-text_width)/2, (rectangle_height-text_height)/2);

	layout->show_in_cairo_context(cr);
}

#include <gtkmm/application.h>
#include <gtkmm/window.h>

int main(int argc, char* argv[])
{
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  Gtk::Window window;
  window.set_title("Drawing text example");

  MyArea area;
  window.add(area);
  area.show();

  return app->run(window);
}
