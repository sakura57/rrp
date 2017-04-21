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
        {"--o", ArgArchetype(ArgClass::OUTPUT_FILE, 1, "Specify the output filename.")}
    });
