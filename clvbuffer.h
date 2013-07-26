#ifndef _CLVBUFFER_H
#define _CLVBUFFER_H
#include <gtkmm.h>
class ClvBuffer : public Gtk::TextBuffer{
	ClvBuffer(const ClvBuffer&) = delete;
	ClvBuffer& operator=(const ClvBuffer&) = delete;
protected:
	virtual void on_insert(const TextBuffer::iterator& pos, const Glib::ustring& text, int bytes) override;
public:
	virtual ~ClvBuffer(){};
	ClvBuffer();
};
#endif
