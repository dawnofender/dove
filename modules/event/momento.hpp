#ifndef MOMENTOEVENT_HPP
#define MOMENTOEVENT_HPP

#include "event.hpp"
#include "../core/archive/archive.hpp"
#include <spanstream>

    
    // TODO: 
    // consider implementing this inside class

    // // world may be big, so we want to avoid copying the data twice.
    // // (we already copy it once into data, but current ostringstream method internally coppies it again.)
    // // to do this, stream from Archive into a buffer:
    // struct VectorStreamBuf : public std::streambuf {
    //     VectorStreamBuf(std::vector<char>& buffer) {
    //         this->setp(buffer.data(), buffer.data() + buffer.size());
    //     }
    // };


class Momento : public Event {
CLASS_DECLARATION(Momento)
public: 
    
    Momento(std::shared_ptr<Thingy> world = nullptr);

    virtual void restore() override;
    virtual void invoke() override;
    
    void capture();

private: 
    std::string data = "";

};

#endif
