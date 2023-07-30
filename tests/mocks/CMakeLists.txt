add_library(mocks
    src/task_mocks.cpp
)

target_include_directories(mocks
    PUBLIC
        include
)

target_link_libraries(mocks
    PRIVATE
        CppUTest::CppUTest
        CppUTest::CppUTestExt
)
