# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-src"
  "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-build"
  "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-subbuild/cpputest-populate-prefix"
  "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-subbuild/cpputest-populate-prefix/tmp"
  "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-subbuild/cpputest-populate-prefix/src/cpputest-populate-stamp"
  "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-subbuild/cpputest-populate-prefix/src"
  "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-subbuild/cpputest-populate-prefix/src/cpputest-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-subbuild/cpputest-populate-prefix/src/cpputest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/mthompkins/workspace/cocoOS/test/build/test_a9/_deps/cpputest-subbuild/cpputest-populate-prefix/src/cpputest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
