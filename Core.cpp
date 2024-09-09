#include <iostream> // cout

#include <Automation/Core.hpp>

namespace automation
{

expression_t<std::nullptr_t> expression(std::nullptr_t const& expression_value)
{
    return expression(expression_value, "nullptr");
}

expression_t<bool> expression(bool const& expression_value)
{
    return expression(expression_value, expression_value == true ? "true" : "false");
}

test_t::test_t(const std::string& module, const std::string& name)
    : module(module), name(name)
{
    global().add(this);
}

static std::string info_format(test_t* test, std::string const& msg, std::string const& string_value, bool condition)
{
    return (condition ? "[   OK   ] " : "[ FAILED ] ")
         + std::string(test->module)
         + "::"
         + test->name
         + '.'
         + msg
         + (condition && string_value.size() > 0 ? "\n" : " [  WITH  ] " + string_value + "\n");
}

static void update_stat(unsigned& passed, unsigned& failed, bool ok)
{
    passed += ok;
    failed += not ok;
}

void registry_t::add(test_t* test)
{
    all[test->module][test->name] = test;
}

bool registry_t::check(bool condition, test_t* test, const std::string& msg)
{
    return check(expression(condition), test, msg);
}

bool registry_t::check(expression_t<bool> const& expression, test_t* test, const std::string& msg)
{
    auto const condition = static_cast<bool>(expression.value);
    update_stat(passed, failed, condition);
    std::cout << info_format(test, msg, expression.string_value, condition);
    return condition;
}

void registry_t::execute_module(const std::string& name)
{
    auto it = all.find(name);
    if (it == all.end()) return;

    auto& module = it->second;
    for (auto& name_test : module) name_test.second->run();
}

void registry_t::execute_test(const std::string& name)
{
    for (auto& name_module : all)
    {
        auto& module = name_module.second;

        auto it = module.find(name);
        if (it == module.end()) continue;

        it->second->run();
    }
}

void registry_t::execute_all()
{
    for (auto& name_module : all)
    {
        auto& module = name_module.second;
        for (auto& name_test : module) name_test.second->run();
    }
}

static std::string stat_format(unsigned passed, unsigned failed)
{
    return "\nOK: "
         + std::to_string(passed)
         + " FAILED: "
         + std::to_string(failed)
         + " TOTAL: "
         + std::to_string(passed+failed)
         + '\n';
}

void registry_t::stat()
{
    std::cout << stat_format(passed, failed);
}

void registry_t::try_catch(std::function<void()>&& call) const noexcept
{
    try { call(); }
    catch(const char* e) { std::cout << e; }
    catch(std::exception& e) { std::cout << e.what(); }
    catch(...) { std::cout << "Unexpected error."; }
}

registry_t& global()
{
    static registry_t registry; return registry;
}

} // namespace automation
