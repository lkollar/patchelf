#include <string>

#include <cstring>
#include <cerrno>
#include <cstdio>
#include <cstdarg>

#include "patchelf.h"


void showHelp(const std::string & progName)
{
        fprintf(stderr, "syntax: %s\n\
  [--set-interpreter FILENAME]\n\
  [--page-size SIZE]\n\
  [--print-interpreter]\n\
  [--print-soname]\t\tPrints 'DT_SONAME' entry of .dynamic section. Raises an error if DT_SONAME doesn't exist\n\
  [--set-soname SONAME]\t\tSets 'DT_SONAME' entry to SONAME.\n\
  [--set-rpath RPATH]\n\
  [--remove-rpath]\n\
  [--shrink-rpath]\n\
  [--allowed-rpath-prefixes PREFIXES]\t\tWith '--shrink-rpath', reject rpath entries not starting with the allowed prefix\n\
  [--print-rpath]\n\
  [--force-rpath]\n\
  [--add-needed LIBRARY]\n\
  [--remove-needed LIBRARY]\n\
  [--replace-needed LIBRARY NEW_LIBRARY]\n\
  [--print-needed]\n\
  [--no-default-lib]\n\
  [--debug]\n\
  [--version]\n\
  FILENAME\n", progName.c_str());
}

int mainWrapped(int argc, char * * argv)
{
    if (argc <= 1) {
        showHelp(argv[0]);
        return 1;
    }

    patchelf_config_t patchelf_config;
    patchelf_config.debugMode = false;
    patchelf_config.forceRPath = false;
    patchelf_config.pageSize = PAGESIZE;

    if (getenv("PATCHELF_DEBUG") != 0) patchelf_config.debugMode = true;

    int i;
    for (i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--set-interpreter" || arg == "--interpreter") {
            if (++i == argc) error("missing argument");
            patchelf_config.newInterpreter = argv[i];
        }
        else if (arg == "--page-size") {
            if (++i == argc) error("missing argument");
            patchelf_config.pageSize = atoi(argv[i]);
            if (patchelf_config.pageSize <= 0) error("invalid argument to --page-size");
        }
        else if (arg == "--print-interpreter") {
            patchelf_config.printInterpreter = true;
        }
        else if (arg == "--print-soname") {
            patchelf_config.printSoname = true;
        }
        else if (arg == "--set-soname") {
            if (++i == argc) error("missing argument");
            patchelf_config.setSoname = true;
            patchelf_config.newSoname = argv[i];
        }
        else if (arg == "--remove-rpath") {
            patchelf_config.removeRPath = true;
        }
        else if (arg == "--shrink-rpath") {
            patchelf_config.shrinkRPath = true;
        }
        else if (arg == "--allowed-rpath-prefixes") {
            if (++i == argc) error("missing argument");
            patchelf_config.allowedRpathPrefixes = splitColonDelimitedString(argv[i]);
        }
        else if (arg == "--set-rpath") {
            if (++i == argc) error("missing argument");
            patchelf_config.setRPath = true;
            patchelf_config.newRPath = argv[i];
        }
        else if (arg == "--print-rpath") {
            patchelf_config.printRPath = true;
        }
        else if (arg == "--force-rpath") {
            /* Generally we prefer to emit DT_RUNPATH instead of
               DT_RPATH, as the latter is obsolete.  However, there is
               a slight semantic difference: DT_RUNPATH is "scoped",
               it only affects the executable or library in question,
               not its recursive imports.  So maybe you really want to
               force the use of DT_RPATH.  That's what this option
               does.  Without it, DT_RPATH (if encountered) is
               converted to DT_RUNPATH, and if neither is present, a
               DT_RUNPATH is added.  With it, DT_RPATH isn't converted
               to DT_RUNPATH, and if neither is present, a DT_RPATH is
               added. */
            patchelf_config.forceRPath = true;
        }
        else if (arg == "--print-needed") {
            patchelf_config.printNeeded = true;
        }
        else if (arg == "--add-needed") {
            if (++i == argc) error("missing argument");
            patchelf_config.neededLibsToAdd.insert(argv[i]);
        }
        else if (arg == "--remove-needed") {
            if (++i == argc) error("missing argument");
            patchelf_config.neededLibsToRemove.insert(argv[i]);
        }
        else if (arg == "--replace-needed") {
            if (i+2 >= argc) error("missing argument(s)");
            patchelf_config.neededLibsToReplace[ argv[i+1] ] = argv[i+2];
            i += 2;
        }
        else if (arg == "--debug") {
            patchelf_config.debugMode = true;
        }
        else if (arg == "--no-default-lib") {
            patchelf_config.noDefaultLib = true;
        }
        else if (arg == "--help" || arg == "-h" ) {
            showHelp(argv[0]);
            return 0;
        }
        else if (arg == "--version") {
            printf(PACKAGE_STRING "\n");
            return 0;
        }
        else {
            patchelf_config.fileNames.push_back(arg);
        }
    }

    if (patchelf_config.fileNames.empty()) error("missing filename");

    patchElf();

    return 0;
}

int main(int argc, char * * argv)
{
    try {
        return mainWrapped(argc, argv);
    } catch (std::exception & e) {
        fprintf(stderr, "patchelf: %s\n", e.what());
        return 1;
    }
}