#include "../../core/component/component.hpp"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Connector : public Component {
CLASS_DECLARATION(Connector)
public:
    Connector(std::string && name = "Connector")

}

// one class will just work with moving the data
// connect, disconnect, listen, accept, send, recieve

// a component on the root object will use this class to communicate with a server / other players
// this component knows about every networked object, and when changes happen, it will communicate

// but i think for now we should just work with events. don't differentiate between networked and non-networked objects until later.
// players share the world's timeline, and each event is sent to everyone else. 
