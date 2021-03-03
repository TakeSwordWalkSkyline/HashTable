set(NAME "StringHash")
set(LIBNAME _${NAME})
#file(SOURCES ${CMAK_CURRENT_SOURCE_DIR}*.cpp)
set(SOURCES
  StringHash.cpp)

# foreach(sourcefile ${SOURCES})
#   string(REPLACE ".cpp" ".h" testname ${sourcefile})
# endforeach(sourcefile ${SOURCES})

add_library(${LIBNAME} STATIC ${SOURCES})
  
