#include <Eightest/Core.hpp>

int main()
{
    TRY_CATCH(EXECUTE_ALL());
    return !TESTING_STAT();
}