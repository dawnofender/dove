#include "archiveThingy.hpp"

CLASS_DEFINITION(Thing, Archive)


Archive::Archive(std::istream* is)
    : in(is) {}

Archive::Archive(std::ostream* os)
    : out(os) {}

