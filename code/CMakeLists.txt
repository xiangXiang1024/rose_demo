#cmake最低版本需求，不加入此行会受到警告信息
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
#项目名称
PROJECT(HELLO)
#rose
include_directories(/usr/rose/include/rose)
link_directories(/usr/rose/lib)

#把当前目录(.)下所有源代码文件和头文件加入变量SRC_LIST
AUX_SOURCE_DIRECTORY(. SRC_LIST)

link_libraries(rose)

#boost
#add_compile_options(-lboost_regex)
#include_directories(/usr/local/include/boost)
#link_directories(/usr/local/lib)

find_package(Boost REQUIRED COMPONENTS
#要使用的boost库
 system
 chrono
)
if(NOT Boost_FOUND)
    message("Not found Boost")
endif()

include_directories(${Boost_INCLUDE_DIRS})
message("${Boost_INCLUDE_DIRS}")
message("${Boost_LIBRARIES}")


#LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu/)
#link_directories(/usr/lib/x86_64-linux-gnu/)


#include_directories(./include)
#生成应用程序 hello (在windows下会自动生成hello.exe)
ADD_EXECUTABLE(hello ${SRC_LIST})


target_link_libraries(hello ${Boost_LIBRARIES} /usr/lib/x86_64-linux-gnu/libquadmath.so.0) #链接到工程
install(TARGETS hello RUNTIME DESTINATION bin)
