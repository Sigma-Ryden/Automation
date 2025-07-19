#ifndef EIGHTEST_CORE_RUN_HPP
#define EIGHTEST_CORE_RUN_HPP

#include <Eightest/Core.hpp>

int main()
{
    TRY_CATCH(EXECUTE_ALL());
    return !TESTING_STAT();
}

#endif // EIGHTEST_CORE_RUN_HPP
