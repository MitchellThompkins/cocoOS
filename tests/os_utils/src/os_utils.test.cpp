#include "CppUTest/TestHarness.h"
#include "test_macros.hpp"

#include "os_assert.h"

static const char* g_cb_file = NULL;
static uint16_t    g_cb_line = 0;
static const char* g_cb_expr = NULL;
static int         g_cb_count = 0;
static int         g_cb2_count = 0;

static void test_callback(const char* f, uint16_t l, const char* e)
{
    g_cb_file = f;
    g_cb_line = l;
    g_cb_expr = e;
    ++g_cb_count;
}

static void test_callback2(const char* f, uint16_t l, const char* e)
{
    (void)f; (void)l; (void)e;
    ++g_cb2_count;
}

static void helper_assert_false(void)
{
    os_assert(false);
}

static void helper_assert_true(void)
{
    os_assert(true);
}

static int helper_assert_with_return_false(void)
{
    os_assert_with_return(false, 7);
    return 0;
}

TEST_GROUP(TestOsUtils)
{
    void setup()
    {
        os_on_assert_attach_callback(NULL);
        g_cb_file  = NULL;
        g_cb_line  = 0;
        g_cb_expr  = NULL;
        g_cb_count  = 0;
        g_cb2_count = 0;
    }

    void teardown()
    {
        os_on_assert_attach_callback(NULL);
    }
};

TEST(TestOsUtils, assert_with_callback_invokes_and_returns)
{
    UT_CATALOG_ID("UTILS-1");
    UT_CATALOG_ID("UTILS-2");

    os_on_assert_attach_callback(test_callback);
    os_on_assert("f.c", 42, "x>0");

    CHECK_EQUAL(1, g_cb_count);
    STRCMP_EQUAL("f.c", g_cb_file);
    CHECK_EQUAL(42, g_cb_line);
    STRCMP_EQUAL("x>0", g_cb_expr);
}

TEST(TestOsUtils, callback_replacement_fires_only_new_callback)
{
    UT_CATALOG_ID("UTILS-2");

    os_on_assert_attach_callback(test_callback);
    os_on_assert("a.c", 1, "a");
    CHECK_EQUAL(1, g_cb_count);
    CHECK_EQUAL(0, g_cb2_count);

    g_cb_count = 0;
    os_on_assert_attach_callback(test_callback2);
    os_on_assert("b.c", 2, "b");
    CHECK_EQUAL(0, g_cb_count);
    CHECK_EQUAL(1, g_cb2_count);
}

TEST(TestOsUtils, os_assert_macro_triggers_on_false_noop_on_true)
{
    UT_CATALOG_ID("UTILS-1");

    os_on_assert_attach_callback(test_callback);

    helper_assert_false();
    CHECK_EQUAL(1, g_cb_count);

    g_cb_count = 0;
    helper_assert_true();
    CHECK_EQUAL(0, g_cb_count);
}

TEST(TestOsUtils, os_assert_with_return_fires_and_returns_value)
{
    UT_CATALOG_ID("UTILS-3");

    os_on_assert_attach_callback(test_callback);

    const int result = helper_assert_with_return_false();
    CHECK_EQUAL(7, result);
    CHECK_EQUAL(1, g_cb_count);
}
