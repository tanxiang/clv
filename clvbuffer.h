#ifndef _CLVBUFFER_H
#define _CLVBUFFER_H
class ClvBuffer : public Gtk::TextBuffer{
	ClvBuffer(const ClvBuffer&) = delete;
	ClvBuffer& operator=(const ClvBuffer&) = delete;
public:
	virtual ~ClvBuffer();
};
#endif