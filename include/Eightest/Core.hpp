#ifndef EIGHTEST_CORE_HPP
#define EIGHTEST_CORE_HPP

#include <utility> // forward
#include <functional> // function
#include <map> // map
#include <string> // string
#include <sstream> // stringstream


// Test definition

// will redirect eightest registry
#ifndef EIGHTEST_REGISTRY
    #define EIGHTEST_REGISTRY (&eightest::global)
#endif

// Allow to hide owner classes within different translation units
#define TEST_SPACE(...) namespace __VA_ARGS__

#define TEST(test_module, test_name) \
    TEST_SPACE(test_module) { \
        static struct test_name : eightest::test_t { \
            test_name() : eightest::test_t(#test_module, #test_name, EIGHTEST_REGISTRY) {} \
            void run() override; \
        } s##test_name; \
    } \
    void test_module::test_name::run()

// Calculate completed and failed test, provide debug info
#define EXPECT(msg, ...) EIGHTEST_REGISTRY->check(__VA_ARGS__, this, msg)
#define ASSERT(msg, ...) if(!EXPECT(msg, __VA_ARGS__)) return

// Tests Execution
#define EXECUTE_TEST(name, ...) EIGHTEST_REGISTRY->execute_test(name)
#define EXECUTE_MODULE(name, ...) EIGHTEST_REGISTRY->execute_module(name)
#define EXECUTE_ALL(...) EIGHTEST_REGISTRY->execute_all()

// Show stat info
#define TESTING_STAT(...) EIGHTEST_REGISTRY->stat()

// Will catch exception and show by text printer
#define TRY_CATCH(...) EIGHTEST_REGISTRY->try_catch([]{ (void)(__VA_ARGS__); })

#define STAT_HANDLER(...) EIGHTEST_REGISTRY->stat_handler = __VA_ARGS__

// Will allow to show expression by text printer
#define EXPR(...) eightest::expression(__VA_ARGS__)


namespace eightest
{

template <typename T>
struct expression_t
{
    T value;
    std::string string_value;

    expression_t<bool> operator!();
};

template <typename T>
struct expression_t<T&>
{
    T& value;
    std::string string_value;

    expression_t<bool> operator!();
};

template <typename T>
expression_t<T> expression(T&& expression_value, std::string const& expression_string_value)
{
    return expression_t<T>{std::forward<T>(expression_value), expression_string_value};
}

template <typename T>
expression_t<T> expression(T&& expression_value)
{
    std::string expression_string_value;
    std::stringstream stream; stream << expression_value; stream >> expression_string_value;
    return expression(std::forward<T>(expression_value), expression_string_value);
}

expression_t<std::nullptr_t> expression(std::nullptr_t expression_value);
expression_t<bool> expression(bool expression_value);

template <typename T>
expression_t<bool> expression_t<T>::operator!()
{
    return expression(!static_cast<bool>(value), "!(" + string_value + ")");
}

class registry_t;

class test_t
{
public:
    std::string const module;
    std::string const name;

public:
    test_t(const std::string& module, const std::string& name, registry_t* registry);

public:
    virtual void run() = 0;
};

class registry_t
{
public:
    std::map<std::string, std::map<std::string, test_t*>> all;
    std::function<void(std::string const&)> stat_handler = &default_stat_handler;

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
    bool stat();
    static void default_stat_handler(std::string const& text);

public:
    void try_catch(std::function<void()>&& call) const noexcept;
};

extern registry_t global;

} // namespace eightest

#define EIGHTEST_EXPRESSION_OPERATOR_GENERIC(op, lhs_brace, rhs_brace) \
    template <typename L, typename R> \
    eightest::expression_t<bool> operator op(eightest::expression_t<L> const& lhs_expression, \
                                               eightest::expression_t<R> const& rhs_expression) { \
        return eightest::expression( \
            lhs_expression.value op rhs_expression.value, \
            lhs_brace+lhs_expression.string_value+" "#op" "+rhs_expression.string_value+rhs_brace \
        ); \
    } \
    template <typename L, typename R> \
    eightest::expression_t<bool> operator op(eightest::expression_t<L> const& lhs_expression, \
                                               R const& rhs_value) { \
        return ::operator op(lhs_expression, eightest::expression(rhs_value)); \
    } \
    template <typename L, typename R> \
    eightest::expression_t<bool> operator op(L const& lhs_value, \
                                               eightest::expression_t<R> const& rhs_expression) { \
       return ::operator op(eightest::expression(lhs_value), rhs_expression); \
    }

EIGHTEST_EXPRESSION_OPERATOR_GENERIC(==,"","")
EIGHTEST_EXPRESSION_OPERATOR_GENERIC(!=,"","")
EIGHTEST_EXPRESSION_OPERATOR_GENERIC(<,"","")
EIGHTEST_EXPRESSION_OPERATOR_GENERIC(<=,"","")
EIGHTEST_EXPRESSION_OPERATOR_GENERIC(>,"","")
EIGHTEST_EXPRESSION_OPERATOR_GENERIC(>=,"","")
EIGHTEST_EXPRESSION_OPERATOR_GENERIC(&&,"(",")")
EIGHTEST_EXPRESSION_OPERATOR_GENERIC(||,"(",")")

#undef EIGHTEST_EXPRESSION_OPERATOR_GENERIC

#endif // EIGHTEST_CORE_HPP
