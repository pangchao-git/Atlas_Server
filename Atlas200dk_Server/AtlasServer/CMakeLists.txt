cmake_minimum_required(VERSION 3.0)

project(AtlasServer)

set(BOOST_ROOT  "/home/pc/work/code_view/whayer_work/Atlas200dk_Server/atlas-aarch64/boost-aarch64/")


find_package(Boost REQUIRED COMPONENTS system thread log program_options chrono filesystem serialization date_time)
find_package(Threads REQUIRED)

set(OPENSSL_ROOT_DIR  "/home/pc/work/code_view/whayer_work/Atlas200dk_Server/atlas-aarch64/openssl-aarch64/")
find_package(OpenSSL 1.0.1 REQUIRED)
set(OPENSSL_VERSION "1.0.1")

set(LOG4CPP_INCLUDE_DIR "/home/pc/work/code_view/whayer_work/Atlas200dk_Server/atlas-aarch64/log4cpp/include")
set(OPENCV_INCLUDE_DIR "$ENV{DDK_PATH}/include/third_party/opencv/include/")

#添加Atlas200的依赖的头文件
set(ATLAS200DK_INCLUDE_DIR
        "$ENV{DDK_PATH}/include/inc/"
        "$ENV{DDK_PATH}/include/third_party/protobuf/include/"
        "$ENV{DDK_PATH}/include/third_party/cereal/include"
        "$ENV{DDK_PATH}/include/libc_sec/include"
        "$ENV{DDK_PATH}/include/third_party/opencv/include")

#添加Atlas200的依赖的头文件依赖的库
set(ATLAS200DK_LINK_DIR
        /home/pc/work/code_view/whayer_work/Atlas200dk_Server/AtlasServer/3rdPart/atlas200dk_sdk/
        $ENV{NPU_HOST_LIB} $ENV{HOME}/ascend_ddk/host/lib
        $ENV{HOME}/ascend_ddk/device/lib
        )
set(ATLAS200DK_LIB_DIR
        $ENV{NPU_HOST_LIB}/libmatrixdaemon.so
        /home/pc/work/code_view/whayer_work/Atlas200dk_Server/AtlasServer/3rdPart/Atlas200dk_sdk/libAtlasApi.so)



# Configure compiler options ...
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -DBOOST_LOG_DYN_LINK")

AUX_SOURCE_DIRECTORY(source DIR_SRC1)
AUX_SOURCE_DIRECTORY(source/AnalyseSingle/Algorithm DIR_AM)
AUX_SOURCE_DIRECTORY(source/AnalyseSingle/AnalyseBase DIR_CM)
AUX_SOURCE_DIRECTORY(source/AnalyseSingle/ThreadPool DIR_TP)
AUX_SOURCE_DIRECTORY(source/AnalyseSingle DIR_SRC4)
AUX_SOURCE_DIRECTORY(source/Manager/sqlite DIR_DB)
AUX_SOURCE_DIRECTORY(source/Manager DIR_SRC3)
AUX_SOURCE_DIRECTORY(source/Device DIR_DEV)
AUX_SOURCE_DIRECTORY(source/foundation DIR_SRC2)
add_executable(${PROJECT_NAME} ${DIR_SRC1} ${DIR_DB} ${DIR_AM} ${DIR_CM} ${DIR_TP} ${DIR_SRC4} ${DIR_SRC3} ${DIR_SRC2} ${DIR_DEV})

set(CPPRESTSDK_INCLUDE_DIR "/home/pc/work/code_view/whayer_work/Atlas200dk_Server/AtlasServer/3rdPart/cpprestsdk/include")

set(MICROSERVICE_INCLUDE_DIR "./source/foundation/include" "./source/Manager" "./source/AnalyseSingle" "./source/AnalyseSingle/include")
message(MICROSERVICE " ${MICROSERVICE_INCLUDE_DIR}")
set(HEADER_SEARCH_PATHS ${OPENCV_INCLUDE_DIR} ${CPPRESTSDK_INCLUDE_DIR}
        ${LOG4CPP_INCLUDE_DIR} ${MICROSERVICE_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${ATLAS200DK_INCLUDE_DIR})

# library search paths ...
set(OPENSSL_LIBS "${OPENSSL_LIBRARIES}")
set(LIBZ_LIBRARY "/usr/lib/aarch64-linux-gnu/libz.so.1")
set(CPPRESTSDK_LIBRARY "/home/pc/work/code_view/whayer_work/Atlas200dk_Server/AtlasServer/3rdPart/cpprestsdk/lib/libcpprest.so"
                        "/home/pc/work/code_view/whayer_work/Atlas200dk_Server/AtlasServer/build/libs/liblog4cpp.so"
                        "/home/pc/work/code_view/whayer_work/Atlas200dk_Server/AtlasServer/build/libs/libHYAlgorithm.so")
set(DDK_LIBRARY "/home/pc/.mindstudio/huawei/ddk/1.32.0.B080/RC/host-aarch64_Ubuntu16.04.3/lib/libopencv_world.so")
set(LIBRARIES_SEARCH_PATHS "-ldl" ${CPPRESTSDK_LIBRARY} ${OPENSSL_LIBS}
        ${Boost_LIBRARIES} ${DDK_LIBRARY}
        ${LIBZ_LIBRARY} ${ATLAS200DK_LIB_DIR})
set(LIBAARCH_LIBPATH "/usr/lib/aarch64-linux-gnu/")
message(BOOST_LIBS " ${Boost_LIBRARIES}")
message(OPENSSL_LIBS " ${OPENSSL_LIBRARIES}")
message(CPPRESTSDK_LIBRARY " ${CPPRESTSDK_LIBRARY}")
message(LIBRARIES_SEARCH_PATHS " ${LIBAARCH_LIBPATH}")


include_directories(${HEADER_SEARCH_PATHS} ${LIBAARCH_LIBPATH}
        ${ATLAS200DK_LINK_DIR}
        )
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_SEARCH_PATHS} )
