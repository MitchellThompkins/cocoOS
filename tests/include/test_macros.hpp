#ifndef TEST_MACROS_HPP
#define TEST_MACROS_HPP

#define UT_CATALOG_ID(text) \
  do { UTestFriend f(UtestShell::getCurrent()); f.print(text); } while(0)

class UTestFriend : protected UtestShell
{
    public:
        UTestFriend( UtestShell* t) : _test(t)
        {}

        void print(const char* text)
        {
            dynamic_cast<decltype(this)>(_test)->getTestResult()->print( " " );
            dynamic_cast<decltype(this)>(_test)->getTestResult()->print( text );
            dynamic_cast<decltype(this)>(_test)->getTestResult()->print( " " );
        }

    private:
        UtestShell* _test;
};

#endif
