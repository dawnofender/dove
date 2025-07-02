#include "archive.hpp"


ArchiveWriter::ArchiveWriter(std::ostream *os) 
    : out(os) {}

ArchiveReader::ArchiveReader(std::istream *is) 
    : in(is) {}


Archive::Archive(std::istream* is)
    : in(is) {
    setMode(Mode::LOAD);
}

Archive::Archive(std::ostream* os)
    : out(os) {
    setMode(Mode::SAVE);
}

