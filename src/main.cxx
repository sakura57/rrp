/*
	Part of the R2A03 Recompiler Project

	Defines the entry-point of the application, argument parsing, and initialization.
*/

#define RRP_DEBUG_BUILD

#include <iostream>
#include <fstream>
#include "Translator.hxx"

int main(int argc, char **argv)
{
	std::cout << "RRP-POC v0.2a" << std::endl;
	std::cout << "Copyright (C) 2016 Jacob Farnsworth" << std::endl;

	if(argc != 2)
	{
		std::cout << "Error: Invalid syntax." << std::endl;
		std::cout << "Syntax: " << argv[0] << " <filename>" << std::endl;
		return 0; 
	}

	RRP::Translator t("test");
	std::ifstream f;
	f.open(argv[1], std::ios::binary | std::ios::in);
	try {
		t.process_file(f);
		t.output_bitcode("test");
	} catch (RRP::Translator::Exception e) {
		std::cout << "Exception while processing file." << std::endl;
		std::cout << "Encountered " << RRP::Translator::exceptText[std::get<0>(e)];
		std::cout << " near " << std::hex << (int)std::get<1>(e) << std::endl;
	}
	return 0;
}
