
#include <string>
#include <vector>
#include <set>
#include <map>

typedef struct {
    bool debugMode;
    bool forceRPath;
    std::vector<std::string> fileNames;
    int pageSize;
    std::string newInterpreter;
    bool printInterpreter;
    bool printSoname;
    bool setSoname;
    bool newSoname;
    bool removeRPath;
    bool shrinkRPath;
    bool setRPath;
    std::string newRPath;
    bool printRPath;
    std::vector<std::string> allowedRpathPrefixes;
    bool printNeeded;
    std::set<std::string> neededLibsToAdd;
    std::set<std::string> neededLibsToRemove;
    std::map<std::string, std::string> neededLibsToReplace;
    bool noDefaultLib;
} patchelf_config_t;

void patchElf();
__attribute__((noreturn)) void error(std::string msg);
std::vector<std::string> splitColonDelimitedString(const char * s);