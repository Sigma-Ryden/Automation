#ifndef AUTOMATION_CORE_HPP
#define AUTOMATION_CORE_HPP

#include <cassert> // assert

#include <functional> // function
#include <map> // map
#include <string> // string
#include <sstream> // stringstream

// Test definition

// Allow to hide owner classes within different translation units
#define TEST_SPACE(...) namespace __VA_ARGS__

#define TEST(test_module, test_name)                                                                    \
    TEST_SPACE(test_module) {                                                                           \
        static struct test_name : automation::test_t {                                                  \
            test_name() : automation::test_t(#test_module, #test_name) {}                               \
            void run() override;                                                                        \
        } s##test_name;                                                                                 \
    }                                                                                                   \
    void test_module::test_name::run()

// Calculate completed and failed test, provide debug info
#define EXPECT(msg, ...) automation::global().check(__VA_ARGS__, this, msg)
#define ASSERT(msg, ...) if(!EXPECT(msg, __VA_ARGS__)) return

// Tests Execution
#define EXECUTE_TEST(name, ...) automation::global().execute_test(name)
#define EXECUTE_MODULE(name, ...) automation::global().execute_module(name)
#define EXECUTE_ALL(...) automation::global().execute_all()

// Show stat info
#define TESTING_STAT(...) automation::global().stat()

// Will catch exception and show by text printer
#define TRY_CATCH(...) automation::global().try_catch([]{ (void)(__VA_ARGS__); })

// Will allow to show expression by text printer
#define EXPR(...) automation::expression(__VA_ARGS__)

namespace automation
{

template <typename T>
struct expression_t
{
    T const& value;
    std::string string_value;

    expression_t<bool> operator!();
};

template <typename T>
expression_t<T> expression(T const& expression_value, std::string const& expression_string_value)
{
    return expression_t<T>{expression_value, expression_string_value};
}

template <typename T>
expression_t<T> expression(T const& expression_value)
{
    std::string expression_string_value;
    std::stringstream stream; stream << expression_value; stream >> expression_string_value;
    return expression(expression_value, expression_string_value);
}

expression_t<std::nullptr_t> expression(std::nullptr_t const& expression_value);
expression_t<bool> expression(bool const& expression_value);

template <typename T>
expression_t<bool> expression_t<T>::operator!()
{
    return expression(!static_cast<bool>(value), "!(" + string_value + ")");
}

class test_t
{
public:
    const std::string module;
    const std::string name;

public:
    test_t(const std::string& module, const std::string& name);

public:
    virtual void run() = 0;
};

class registry_t
{
public:
    std::map<std::string, std::map<std::string, test_t*>> all;

public:
    unsigned passed = 0;
    unsigned failed = 0;

public:
    void add(test_t* test);

public:
    bool check(bool condition, test_t* test, const std::string& msg);
    bool check(expression_t<bool> const& expression, test_t* test, const std::string& msg);

public:
    void execute_module(const std::string& name); // execute test module with specify name
    void execute_test(const std::string& name); // execute tests with specify name
    void execute_all();

public:
    void stat();

public:
    void try_catch(std::function<void()>&& call) const noexcept;
};

registry_t& global();

} // namespace automation

#define AUTOMATION_EXPRESSION_OPERATOR_GENERIC(op, lhs_brace, rhs_brace)                                \
    template <typename L, typename R>                                                                   \
    automation::expression_t<bool> operator op(automation::expression_t<L> const& lhs_expression,       \
                                               automation::expression_t<R> const& rhs_expression) {     \
        return automation::expression(                                                                  \
            lhs_expression.value op rhs_expression.value,                                               \
            lhs_brace+lhs_expression.string_value+" "#op" "+rhs_expression.string_value+rhs_brace       \
        );                                                                                              \
    }                                                                                                   \
    template <typename L, typename R>                                                                   \
    automation::expression_t<bool> operator op(automation::expression_t<L> const& lhs_expression,       \
                                               R const& rhs_value) {                                    \
        return ::operator op(lhs_expression, automation::expression(rhs_value));                        \
    }                                                                                                   \
    template <typename L, typename R>                                                                   \
    automation::expression_t<bool> operator op(L const& lhs_value,                                      \
                                               automation::expression_t<R> const& rhs_expression) {     \
       return ::operator op(automation::expression(lhs_value), rhs_expression);                         \
    }

AUTOMATION_EXPRESSION_OPERATOR_GENERIC(==,"","")
AUTOMATION_EXPRESSION_OPERATOR_GENERIC(!=,"","")
AUTOMATION_EXPRESSION_OPERATOR_GENERIC(<,"","")
AUTOMATION_EXPRESSION_OPERATOR_GENERIC(<=,"","")
AUTOMATION_EXPRESSION_OPERATOR_GENERIC(>,"","")
AUTOMATION_EXPRESSION_OPERATOR_GENERIC(>=,"","")
AUTOMATION_EXPRESSION_OPERATOR_GENERIC(&&,"(",")")
AUTOMATION_EXPRESSION_OPERATOR_GENERIC(||,"(",")")

#undef AUTOMATION_EXPRESSION_OPERATOR_GENERIC

#endif // AUTOMATION_CORE_HPP
