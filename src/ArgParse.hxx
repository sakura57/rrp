/*
    ArgParse.hxx
    
    Part of the R2A03 Recompiler Project
*/

#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

//list of possible arg types
enum class ArgClass
{
    INVALID,
    VERBOSE,
    INPUT_FILE,
    OUTPUT_FILE
};

class ArgParseException
{
public:
    explicit ArgParseException(int const i, std::string const &s)
        : _pos(i), _msg(s)
    { };
    
    int const getPos() const
    {
        return _pos;
    };
    
    std::string const getMsg() const
    {
        return _msg;
    };
private:
    int _pos;
    std::string _msg;
};

class ArgArchetype
{
public:
    ArgArchetype(ArgClass const argClass,
                 int const additionalParams,
                 std::string const &szDesc)
        : _class(argClass), _params(additionalParams), _desc(szDesc)
    { };
    
    ArgArchetype(ArgArchetype const &o)
        : _class(o._class), _params(o._params), _desc(o._desc)
    { };
    
    ArgArchetype(void)
        : _class(ArgClass::INVALID), _params(0)
    { };
    
    int const getAdditionalParams() const
    {
        return _params;
    };
    
    ArgClass const getClass() const
    {
        return _class;
    };
    
    std::string const &getDesc() const
    {
        return _desc;
    };
    
private:
    ArgClass _class;
    int _params;
    std::string _desc;
};

class ArgObject
{
public:
    typedef std::unordered_map<std::string, ArgArchetype>::iterator ArgArchIterator;
    typedef std::pair<std::string, ArgArchetype> ArgArchEntry;
    typedef std::unordered_map<std::string, ArgArchetype> ArgArchMap;
    
private:
    static const ArgArchMap argMap;
    ArgArchetype arch;
    std::vector<std::string> params;
    
public:
    ArgObject() = delete;
    explicit ArgObject(std::string const &s)
    {
        auto argClassIter = argMap.find(s);
        
        if(argClassIter != argMap.end())
        {
            arch = ArgArchetype(argClassIter->second);
        }
        else
        {
            throw ArgParseException(-1, "Invalid argument");
            arch = ArgArchetype();
        }
    };
    
    ArgArchetype const &getArch(void) const
    {
        return arch;
    };
    
    void push_param(std::string const &param)
    {
        params.push_back(param);
    };
    
    static auto archBegin(void)
        -> decltype(argMap.begin())
    {
        return argMap.begin();
    };
    
    static auto archEnd(void)
        -> decltype(argMap.end())
    {
        return argMap.end();
    };
};

class ArgParse
{
public:
    typedef std::vector<ArgObject>::iterator ArgIterator;
    
    ArgParse() = delete;
    explicit ArgParse(int argc, char **argv)
    {
        int i = 1;
        for(; i < argc; ++i)
        {
            try {
                ArgObject &&arg = ArgObject(argv[i]);
                
                ArgArchetype const &argArch = arg.getArch();
                
                //if the argument is expecting additional parameters
                //make sure we handle it
                int topArgs = i + argArch.getAdditionalParams();
                if(topArgs >= argc)
                {
                    throw ArgParseException(-1, "Argument expected more parameters");
                }
                
                while(i < topArgs)
                {
                    arg.push_param(argv[i++]);
                }
                
                _args.emplace_back(arg);
            } catch(ArgParseException e) {
                throw ArgParseException(i, e.getMsg());
            }
        }
    };
    
    ArgIterator begin(void)
    {
        return _args.begin();
    };
    
    ArgIterator end(void)
    {
        return _args.end();
    };
    
    
private:
    std::vector<ArgObject> _args;
};
