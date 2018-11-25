#include <MaterialXView/Viewer.h>

#include <iostream>

int main(int argc, char* const argv[])
{  
    std::vector<std::string> tokens;
    for (int i = 1; i < argc; i++)
    {
        tokens.push_back(std::string(argv[i]));
    }

    mx::FileSearchPath searchPath;
    for (int i = 0; i < tokens.size(); i++)
    {
        const std::string& token = tokens[i];
        const std::string& nextToken = i + 1 < tokens.size() ? tokens[i + 1] : mx::EMPTY_STRING;
        if (token == "-s")
        {
            searchPath = mx::FileSearchPath(nextToken);
        }
    }

    try
    {
        ng::init();
        {
            ng::ref<Viewer> viewer = new Viewer(searchPath);
            viewer->setVisible(true);
            ng::mainloop(-1);
        }
    
        ng::shutdown();
    }
    catch (const std::runtime_error& e)
    {
        std::string error_msg = std::string("Fatal error: ") + std::string(e.what());
        std::cerr << error_msg << std::endl;
        return -1;
    }

    return 0;
}
