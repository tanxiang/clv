#include <string>
#include <vector>
#include <list>

class line : public std::string {};
static inline std::istream &operator>>(std::istream &is, line &l)
{
    std::getline(is, l);
    return is;
}

typedef std::vector<line> file_text;
typedef std::list<file_text> files_text;
