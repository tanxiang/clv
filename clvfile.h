#include <string>
#include <vector>
#include <list>

class Line : public std::string {};
static inline std::istream &operator>>(std::istream &is, Line &l)
{
    std::getline(is,l);
    return is;
}

typedef std::vector<Line> FileText;
typedef std::list<FileText> FilesText;

