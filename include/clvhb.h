#include <hb.h>
#include <hb-ot.h>
#include <cairo-ft.h>
#include <hb-ft.h>

namespace clv{
class hbbuffer{
	hb_buffer_t *buffer = hb_buffer_create ();
public:
	~hbbuffer(){
		
	}

};

class hb_shaper{
protected:

public:

};
}