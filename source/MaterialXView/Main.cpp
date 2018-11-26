#include <MaterialXView/Viewer.h>

#include <iostream>

int main(int argc, char* const argv[])
{  
    std::vector<std::string> tokens;
    for (int i = 1; i < argc; i++)
    {
        tokens.push_back(std::string(argv[i]));
    }

    mx::StringVec libraryFolders = { "stdlib", "sxpbrlib" };
    mx::FileSearchPath searchPath;
    for (int i = 0; i < tokens.size(); i++)
    {
        const std::string& token = tokens[i];
        const std::string& nextToken = i + 1 < tokens.size() ? tokens[i + 1] : mx::EMPTY_STRING;
        if (token == "-l" && !nextToken.empty())
        {
            libraryFolders.push_back(nextToken);
        }
        if (token == "-s" && !nextToken.empty())
        {
            searchPath = mx::FileSearchPath(nextToken);
        }
    }
    searchPath.append(mx::FilePath::getCurrentPath() / mx::FilePath("documents/Libraries"));

    try
    {
        ng::init();
        {
            ng::ref<Viewer> viewer = new Viewer(libraryFolders, searchPath);
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
