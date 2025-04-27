#递归将本文件夹下所有cpp
file(GLOB_RECURSE APP ./  *.cpp)

add_library(app ${APP} )
