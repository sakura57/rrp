#include "ArgParse.hxx"

/*
 * the argMap provides mappings of argument strings
 * to their archetypes
 */
const ArgObject::ArgArchMap
    ArgObject::argMap = ArgObject::ArgArchMap
    ({
        {"--v", ArgArchetype(ArgClass::VERBOSE, 0, "Enables verbose output.")},
        {"--i", ArgArchetype(ArgClass::INPUT_FILE, 1, "Specify the input filename.")},
        {"--o", ArgArchetype(ArgClass::OUTPUT_FILE, 1, "Specify the output filename.")},
        {"--h", ArgArchetype(ArgClass::HELP, 0, "Display the help page.")}
    });

/*
 * A list of arguments which are required.
 * This vector is cloned by an ArgParse instance
 * and then popped from as arguments 
 */
const std::vector<std::string> ArgParse::requiredArgs = std::vector<std::string>
    ({
        "--i",
        "--o"
    });
