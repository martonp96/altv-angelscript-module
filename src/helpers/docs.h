#pragma once

#include "cpp-sdk/SDK.h"
#include "Log.h"
#include "angelscript/include/angelscript.h"
#include "../resource.h"
#ifdef AS_GENERATE_DOCUMENTATION
#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#endif

#define PAD_SPACE "    "

namespace Helpers
{
    class DocsGenerator
    {
        std::string module;
        std::vector<std::pair<std::string, std::string>> declarations;
        std::vector<std::pair<std::string, std::string>> funcDefs;
        std::vector<std::pair<std::string, std::string>> eventDeclarations;
    public:
        DocsGenerator(std::string module) : module(module) {};

        void PushDeclaration(std::string decl, std::string desc)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            declarations.push_back(std::pair(decl, desc));
            #endif
        }
        void PushFuncDef(std::string funcdef, std::string desc)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            funcDefs.push_back(std::pair(funcdef, desc));
            #endif
        }
        void PushEventDeclaration(std::string funcDef, std::string globalFunc)
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            eventDeclarations.push_back(std::pair(funcDef.insert(0, "funcdef "), globalFunc));
            #endif
        }

        void Generate()
        {
            #ifdef AS_GENERATE_DOCUMENTATION
            std::stringstream stream;

            // Add generation date to top of file
            std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char* timeStr = std::ctime(&currentTime);
            stream << "// Generated on " << timeStr << "\n";

            // Add namespace
            stream << "namespace alt" << "\n";
            stream << "{" << "\n";

            // Add func defs
            stream << PAD_SPACE << "// ********** Funcdefs **********" << "\n";
            for(auto def : funcDefs)
            {
                stream << "\n";
                stream << PAD_SPACE << "// " << def.second << "\n";
                stream << PAD_SPACE << "funcdef " << def.first << ";" << "\n";
            }

            stream << "\n";

            // Add function declarations
            stream << PAD_SPACE << "// ********** Functions **********" << "\n";
            for(auto decl : declarations)
            {
                stream << "\n";
                stream << PAD_SPACE << "// " << decl.second << "\n";
                stream << PAD_SPACE << decl.first << ";" << "\n";
            }

            stream << "\n";

            // Add event declarations
            stream << PAD_SPACE << "// ********** Events **********" << "\n";
            for(auto decl : eventDeclarations)
            {
                stream << "\n";
                stream << PAD_SPACE << decl.first << ";" << "\n";
                stream << PAD_SPACE << decl.second << ";" << "\n";
            }

            // Close namespace
            stream << "}" << "\n";

            // Write the docs to file
            std::ofstream file;
            file.open(module.append("Docs.as"));
            file << stream.str();
            file.close();
            #endif
        }
    };
}
