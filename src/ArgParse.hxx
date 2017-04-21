/*
    ArgParse.hxx
    
    Part of the R2A03 Recompiler Project
*/

#pragma once

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <memory>

//list of possible arg types
enum class ArgClass
{
    INVALID,
    VERBOSE,
    INPUT_FILE,
    OUTPUT_FILE,
    HELP
};

/*
 * ArgParseException class
 * Represents an exception encountered while
 * parsing arguments.
 */
class ArgParseException
{
public:
	/*
	 * Constructor
	 * Construct the ArgParseException object.
	 */
    explicit ArgParseException(int const i, std::string const &s)
        : _pos(i), _msg(s)
    { };
    
	/*
	 * getPos() function
	 * Returns the position the exception was encountered.
	 */
    int const getPos() const
    {
        return _pos;
    };
    
	/*
	 * getMsg() function
	 * Returns the message associated with this exception.
	 */
    std::string const getMsg() const
    {
        return _msg;
    };
private:
    int _pos;
    std::string _msg;
};

/*
 * ArgArchetype class
 * Represents an argument "archetype" - that is,
 * encapsulates class/type of the argument, how many
 * additional parameters it is expecting, and a string
 * description.
 */
class ArgArchetype
{
public:
	/*
	 * Constructor
	 * Construct the ArgArchetype object.
	 */
    ArgArchetype(ArgClass const argClass,
                 int const additionalParams,
                 std::string const &szDesc)
        : _class(argClass), _params(additionalParams), _desc(szDesc)
    { };
    
	/*
	 * Copy constructor
	 * Construct the ArgArchetype object from another.
	 */
    ArgArchetype(ArgArchetype const &o)
        : _class(o._class), _params(o._params), _desc(o._desc)
    { };
    
	/*
	 * Constructor
	 * Construct the ArgArchetype object.
	 * By default, the type is INVALID.
	 */
    ArgArchetype(void)
        : _class(ArgClass::INVALID), _params(0)
    { };
    
	/*
	 * getAdditionalParams() function
	 * Returns the number of additional parameters expected
	 * by this argument.
	 */
    int const getAdditionalParams() const
    {
        return _params;
    };
    
	/*
	 * getClass() function
	 * Returns the class/type of this argument.
	 */
    ArgClass const getClass() const
    {
        return _class;
    };
    
	/*
	 * getDesc() function
	 * Returns the string description of this exception.
	 */
    std::string const &getDesc() const
    {
        return _desc;
    };
    
private:
    ArgClass _class;
    int _params;
    std::string _desc;
};

/*
 * ArgObject class
 * Represents a living argument object.
 * Encapsulates archetype, vector of additional
 * parameters, and static mapping of argument strings
 * to their archetypes.
 */
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
	/*
	 * Default constructor (deleted)
	 */
    ArgObject() = delete;
	
	/*
	 * Constructor
	 * Construct the ArgObject object.
	 */
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
    
	/*
	 * getArch() function
	 * Returns the archetype associated with this argument.
	 */
    ArgArchetype const &getArch(void) const
    {
        return this->arch;
    };
    
    std::vector<std::string> const &getParams(void) const
    {
        return this->params;
    };
    
	/*
	 * push_param function
	 * add the specified parameter to the additional
	 * parameters vector
	 */
    void push_param(std::string const &param)
    {
        params.push_back(param);
    };
    
	/*
	 * begin iterating the arch map
	 */
    static auto archBegin(void)
        -> decltype(argMap.begin())
    {
        return argMap.begin();
    };
    
	/*
	 * end iterating the arch map
	 */
    static auto archEnd(void)
        -> decltype(argMap.end())
    {
        return argMap.end();
    };
};

/*
 * ArgParse object
 * Presents an easy way of parsing arguments
 * via its constructor, accepting the same parameters
 * as main(). One may then choose to iterate the
 * parsed arguments.
 */
class ArgParse
{
public:
    typedef std::vector<ArgObject>::iterator ArgIterator;
    static const std::vector<std::string> requiredArgs;
    
    std::vector<std::string> _remainingRequiredArgs;
    
    ArgParse() = delete;
	
	/*
	 * ArgParse constructor
	 * Given raw argument data, abstract the data
	 * into ArgObjects.
	 */
    explicit ArgParse(int argc, char **argv)
        : _remainingRequiredArgs(requiredArgs)
    {
        int i = 1;
        for(; i < argc; ++i)
        {
            try {
                std::string argStr(argv[i]);
                
                //ArgObject's constructor will throw
                //if the argument is unrecognized
                ArgObject &&arg = ArgObject(argStr);
                
                if(_remainingRequiredArgs.size())
                {
                    auto requiredArgIter = std::find(_remainingRequiredArgs.begin(), _remainingRequiredArgs.end(), argStr);
                    
                    if(requiredArgIter != _remainingRequiredArgs.end())
                    {
                        //arg was required, remove it from the remaining
                        //required arg vector
                        _remainingRequiredArgs.erase(requiredArgIter);
                    }
                }
                
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
                    arg.push_param(argv[++i]);
                }
                
                _args.emplace_back(arg);
            } catch(ArgParseException e) {
                throw ArgParseException(i, e.getMsg());
            }
        }
    };
    
    /*
     * throws if not all required args
     * were supplied. ideally, call immediately after
     * constructor
     */
    void verifyRequired(void)
    {
        if(_remainingRequiredArgs.size())
        {
            std::string missingArgs(_remainingRequiredArgs.back());
            _remainingRequiredArgs.pop_back();
            
            for(std::string &arg : _remainingRequiredArgs)
            {
                missingArgs += ", " + arg;
            }
            
            throw ArgParseException(-1, "Required arguments missing: " + missingArgs);
        }
    };
    
    /*
     * returns the ArgObject
     * for a certain class
     */
    ArgObject get(ArgClass const argClass)
    {
        for(auto &a : _args)
        {
            if(a.getArch().getClass() == argClass)
            {
                return a;
            }
        }
    };
    
    /*
     * simply returns true or false
     * depending on whether the specified argument
     * was supplied
     */
    bool hasArg(ArgClass const argClass)
    {
        for(auto &a : _args)
        {
            if(a.getArch().getClass() == argClass)
            {
                return true;
            }
        }
        
        return false;
    };
    
    /*
     * get the first additional param that
     * was supplied for this arg.
     * useful for args which expect
     * only one additional param
     */
    std::string getFirstParam(ArgClass const argClass)
    {
        ArgObject o = get(argClass);
        
        return *(o.getParams().begin());
    };
    
	/*
	 * begin iterating the args
	 */
    ArgIterator begin(void)
    {
        return _args.begin();
    };
    
	/*
	 * end iterating the args
	 */
    ArgIterator end(void)
    {
        return _args.end();
    };
    
    
private:
    std::vector<ArgObject> _args;
};
