#include "clvtkmm.h"
#include <pangomm.h>
#include <iostream>

bool ClvArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();

  const int rectangle_width = width;
  const int rectangle_height = height / 2;

  // Draw a black rectangle
  cr->set_source_rgb(0.0, 0.0, 0.0);
  draw_rectangle(cr, rectangle_width, rectangle_height);

  // and some white text
  cr->set_source_rgb(1.0, 1.0, 1.0);
  draw_text(cr, rectangle_width, rectangle_height);

  // flip the image vertically
  // see http://www.cairographics.org/documentation/cairomm/reference/classCairo_1_1Matrix.html
  // the -1 corresponds to the yy part (the flipping part)
  // the height part is a translation (we could have just called cr->translate(0, height) instead)
  // it's height and not height / 2, since we want this to be on the second part of our drawing
  // (otherwise, it would draw over the previous part)
  Cairo::Matrix matrix(1.0, 0.0, 0.0, -1.0, 0.0, height);

  // apply the matrix
  cr->transform(matrix);

  // white rectangle
  cr->set_source_rgb(1.0, 1.0, 1.0);
  draw_rectangle(cr, rectangle_width, rectangle_height);

  // black text
  cr->set_source_rgb(0.0, 0.0, 0.0);
  draw_text(cr, rectangle_width, rectangle_height);

  return true;
}

void ClvArea::draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr,
                            int width, int height)
{
  cr->rectangle(0, 0, width, height);
  cr->fill();
}

void ClvArea::draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
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

/*
Glib::RefPtr<PrintFormOperation> PrintFormOperation::create(){
	return Glib::RefPtr<PrintFormOperation>{new PrintFormOperation{}};
}

void PrintFormOperation::on_begin_print(const Glib::RefPtr<Gtk::PrintContext>& print_context){
	//Create and set up a Pango layout for PrintData based on the passed
	//PrintContext: We then use this to calculate the number of pages needed, and
	//the lines that are on each page.
	m_refLayout = print_context->create_pango_layout();
	Pango::FontDescription font_desc("sans 12");
	m_refLayout->set_font_description(font_desc);
	const double width = print_context->get_width();
	const double height = print_context->get_height();
	m_refLayout->set_width(static_cast<int>(width * Pango::SCALE));
	//Set and mark up the text to print:
	Glib::ustring marked_up_form_text;
	marked_up_form_text += "<b>Name</b>: " + m_Name + "\n\n";
	marked_up_form_text += "<b>Comments</b>: " + m_Comments;
	m_refLayout->set_markup(marked_up_form_text);
	//Set the number of pages to print by determining the line numbers
	//where page breaks occur:
	const int line_count = m_refLayout->get_line_count();
	Glib::RefPtr<Pango::LayoutLine> layout_line;
	double page_height = 0;
	for (int line = 0; line < line_count; ++line){
		Pango::Rectangle ink_rect, logical_rect;
		layout_line = m_refLayout->get_line(line);
		layout_line->get_extents(ink_rect, logical_rect);
		const double line_height = logical_rect.get_height() / 1024.0;
		if (page_height + line_height > height){
			m_PageBreaks.push_back(line);
			page_height = 0;
		}
		page_height += line_height;
	}
	set_n_pages(m_PageBreaks.size() + 1);
}

void PrintFormOperation::on_draw_page(const Glib::RefPtr<Gtk::PrintContext>& print_context, int page_nr)
{
	//Decide which lines we need to print in order to print the specified page:
	int start_page_line = 0;
	int end_page_line = 0;
	if(page_nr == 0)
		start_page_line = 0;
	else
		start_page_line = m_PageBreaks[page_nr - 1];
	if(page_nr < static_cast<int>(m_PageBreaks.size()))
		end_page_line = m_PageBreaks[page_nr];
	else
		end_page_line = m_refLayout->get_line_count();
	//Get a Cairo Context, which is used as a drawing board:
	Cairo::RefPtr<Cairo::Context> cairo_ctx = print_context->get_cairo_context();
	//We'll use black letters:
	cairo_ctx->set_source_rgb(0, 0, 0);
	//Render Pango LayoutLines over the Cairo context:
	Pango::LayoutIter iter = m_refLayout->get_iter();
	double start_pos = 0;
	int line_index = 0;
	do{
		if(line_index >= start_page_line){
			Glib::RefPtr<Pango::LayoutLine> layout_line = iter.get_line();
			Pango::Rectangle logical_rect = iter.get_line_logical_extents();
			int baseline = iter.get_baseline();
			if (line_index == start_page_line)
				start_pos = logical_rect.get_y() / 1024.0;
			cairo_ctx->move_to(logical_rect.get_x() / 1024.0,baseline / 1024.0 - start_pos);
			layout_line->show_in_cairo_context(cairo_ctx);
		}
		line_index++;
	}
	while(line_index < end_page_line && iter.next_line());
}


clvtkwindow::clvtkwindow():m_button("Hello World"){// creates a new button with label "Hello World".

	// Sets the border width of the window.
	set_border_width(10);
	// When the button receives the "clicked" signal, it will call the
	// on_button_clicked() method defined below.
	m_button.signal_clicked().connect(sigc::mem_fun(*this,&clvtkwindow::on_button_clicked));
	// This packs the button into the Window (a container).
	add(m_button);
	// The final step is to display this newly created widget...
	m_button.show();
}

void clvtkwindow::on_button_clicked(){
	std::cout << "Hello World" << std::endl;
}
*/
