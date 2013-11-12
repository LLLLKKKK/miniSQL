
#include "Page.hpp"
#include "DbFile.hpp"

namespace miniSQL {

Page::~Page() {
    file->writebackPage(this);
}

}
