
#include "Page.h"
#include "DbFile.h"

namespace miniSQL {

Page::~Page() {
    file->writebackPage(this);
}

}
