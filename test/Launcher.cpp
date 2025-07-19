#ifdef EIGHTEST_CORE_RUN
#include <Eightest/Core.hpp>

#if defined(_WIN32)
#   include <windows.h>
#   define EIGHTEST_LOAD(name) LoadLibraryA(#name.dll)
#elif defined(__linux__)
#   include <dlfcn.h>
#   define EIGHTEST_LOAD(name) dlopen(#name.so, RTLD_NOW)
#elif defined(__APPLE__)
#   include <dlfcn.h>
#   define EIGHTEST_LOAD(name) dlopen(#name.dylib, RTLD_NOW)
#endif // if

int main()
{
    EIGHTEST_LOAD(EIGHTEST_CORE_RUN);
    TRY_CATCH(EXECUTE_ALL());
    return !TESTING_STAT();
}
#endif // EIGHTEST_CORE_RUN