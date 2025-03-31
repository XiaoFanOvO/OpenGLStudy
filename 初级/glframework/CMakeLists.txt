#递归将本文件夹下所有cpp放到FW中
file(GLOB_RECURSE FW ./ *.cpp)

add_library(fw ${FW} "core.h")
