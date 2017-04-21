/*
	Part of the R2A03 Recompiler Project

	Defines the entry-point of the application, argument parsing, and initialization.
*/

#include <iostream>
#include <fstream>
#include "Translator.hxx"
#include "ArgParse.hxx"

void printValidSyntax(void)
{
    auto iter = ArgObject::archBegin();
    auto archEnd = ArgObject::archEnd();
    std::cout << "Possible arguments:" << std::endl;
    do
    {
        int additionalParams = iter->second.getAdditionalParams();
        std::cout << "\t" << iter->first;
        std::cout << " |\t" << iter->second.getDesc() << std::endl;
        if(additionalParams)
        {
            std::cout << "\t\tExpects " << additionalParams;
            std::cout << " additional parameter";
            std::cout << ((additionalParams == 1)?".":"s.") << std::endl;
        }
    } while(++iter != archEnd);
}

int main(int argc, char **argv)
{
	std::cout << "RRP-POC v0.2a" << std::endl;
	std::cout << "Copyright (C) 2016 Jacob Farnsworth" << std::endl;
    
    std::shared_ptr<ArgParse> argParse(nullptr);
    
    try {
        argParse = std::make_shared<ArgParse>(argc, argv);
        
        if(argParse->hasArg(ArgClass::HELP))
        {
            printValidSyntax();
            return 0; 
        }
        
        argParse->verifyRequired();
    } catch(ArgParseException e) {
        int pos = e.getPos();
        if(pos == -1)
        {
            std::cout << "ArgParseException: ";
            std::cout << e.getMsg() << std::endl;
        }
        else
        {
            std::cout << "ArgParseException on argument ";
            std::cout << e.getPos() << " \"" << argv[e.getPos()] <<  "\"" << std::endl;
            std::cout << "\t" << e.getMsg() << std::endl;
        }
        return 0;
    }

	RRP::Translator t("test");
	std::ifstream f;
	//char * filename = argv[1];
    std::string filename = argParse->getFirstParam(ArgClass::INPUT_FILE);
	f.open(filename, std::ios::binary | std::ios::in);
	if(!f.is_open())
	{
		std::cout << "Error: Could not open " << filename << std::endl;
		return 0;
	}
	try {
		t.process_file(f);
		t.output_bitcode("rrpout.bc");
	} catch (RRP::Translator::Exception e) {
		std::cout << "Exception while processing file." << std::endl;
		std::cout << "Encountered " << RRP::Translator::exceptText[std::get<0>(e)];
		std::cout << " near " << std::hex << (int)std::get<1>(e) << std::endl;
	}
	return 0;
}
