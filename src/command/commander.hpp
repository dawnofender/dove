
// say the user enters this: 
//    /create Thingy "cube"
// use ThingFactory to create a command object with the type of the first word: create
// now, since classes are capitalized, we'll just capitalize the input before interfacing with ThingFactory
// the rest is in the command object's hands. we'll call its execute function with arguments given: Thingy "cube"
// 
// use an istream for arguments. make sure they are streamed in binary format
    

    template<typename CommandType>
    executeCommand(std::string && input) {
        std::unique_ptr<Thing> uniqueBase = ThingFactory::instance().create(thingType); 
        
    }
