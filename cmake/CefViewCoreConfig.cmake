include(FetchContent)

# fetch CefViewCore project
FetchContent_Declare(
    CefViewCore
    GIT_REPOSITORY https://github.com/CefView/CefViewCore.git
    GIT_TAG fe335787e8ce5d9bf719617a248c5a9bf7e8e35c
)

# set CEF version to be used
# "89.0.12+g2b76680+chromium-89.0.4389.90"    # GOOD
# "91.1.23+g04c8d56+chromium-91.0.4472.164"   # GOOD
# "95.7.12+g99c4ac0+chromium-95.0.4638.54"    # GOOD
# "99.2.15+g71e9523+chromium-99.0.4844.84"    # GOOD
# "102.0.10+gf249b2e+chromium-102.0.5005.115" # GOOD
# "104.4.18+g2587cf2+chromium-104.0.5112.81"  # BAD   # crash on macOS - debug mode
# "104.4.26+g4180781+chromium-104.0.5112.102" # BAD   # crash on macOS - debug mode
# "105.3.25+g0ca6a9e+chromium-105.0.5195.54"  # BAD   # debugbreak - debug mode
# "105.3.28+g002805e+chromium-105.0.5195.54"  # BAD   # debugbreak - debug mode
# "107.1.9+g1f0a21a+chromium-107.0.5304.110"  # BAD   # debugbreak - debug mode
# "110.0.26+g732747f+chromium-110.0.5481.97"  # BAD   # TO-BE-FIXED
# "113.3.1+g525fa10+chromium-113.0.5672.128"  # So far so good - doesn't support Xcode 15
# "119.4.7+g55e15c8+chromium-119.0.6045.199"
# set(CEF_SDK_VERSION "119.4.7+g55e15c8+chromium-119.0.6045.199")
# set(CEF_SDK_VERSION "108.4.13+ga98cd4c+chromium-108.0.5359.125")
# set(CEF_SDK_VERSION "109.1.18+gf1c41e4+chromium-109.0.5414.120")
# set(CEF_SDK_VERSION "114.2.13+g6792e13+chromium-114.0.5735.200")
# set(CEF_SDK_VERSION "123.0.13+gfc703fb+chromium-123.0.6312.124")

# -DCEF_SDK_VERSION=108.4.13+ga98cd4c+chromium-108.0.5359.125
# -DCEF_SDK_VERSION=109.1.18+gf1c41e4+chromium-109.0.5414.120
# -DCEF_SDK_VERSION=114.2.13+g6792e13+chromium-114.0.5735.200
# -DCEF_SDK_VERSION=123.0.13+gfc703fb+chromium-123.0.6312.124


if (NOT DEFINED CEF_SDK_VERSION)
    message(FATAL_ERROR "CEF_SDK_VERSION is not defined")
endif()


# 设置 http proxy
set(ENV{http_proxy} "http://127.0.0.1:10809")
set(ENV{https_proxy} "http://127.0.0.1:10809")

# populate CefViewCore project
FetchContent_MakeAvailable(CefViewCore)

message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# set sub fodler for all CefViewCore targets
set_target_properties(CefViewCore PROPERTIES FOLDER core)

# set sub-folder for all Helper targets
foreach(CefViewWingTarget ${CefViewCore_HELPER_APP_TARGETS})
    set_target_properties(${CefViewWingTarget} PROPERTIES FOLDER core)
endforeach(CefViewWingTarget)

# set sub-folder for all libcef_dll_wrapper target
set_target_properties(libcef_dll_wrapper PROPERTIES FOLDER core)

# export all CefViewCore variables
if(QCefView_HAS_PARENT_DIRECTORY)
    set(CefViewCore_EXPORT_INCLUDE_PATH ${CefViewCore_EXPORT_INCLUDE_PATH} PARENT_SCOPE)
    set(CefViewCore_HELPER_APP_TARGETS ${CefViewCore_HELPER_APP_TARGETS} PARENT_SCOPE)
    set(CefViewCore_CEF_BINARY_DIR ${CefViewCore_CEF_BINARY_DIR} PARENT_SCOPE)
    set(CefViewCore_CEF_RESOURCE_DIR ${CefViewCore_CEF_RESOURCE_DIR} PARENT_SCOPE)
    set(CefViewCore_CEF_INCLUDE_DIR ${CefViewCore_CEF_INCLUDE_DIR} PARENT_SCOPE)
endif()


# 立即执行命令
execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Executing immediate command for CefViewCore"
    COMMAND ${CMAKE_COMMAND} -E echo "Command executed at: ${CMAKE_CURRENT_TIMESTAMP}" >> ${CMAKE_CURRENT_BINARY_DIR}/immediate_command_log.txt

    # src/cefviewcore/CMakeLists.txt 对比 build/${CEF_VERSION_MAJOR}/vs2019_x64-Debug/_deps/cefviewcore-src/CMakeLists.txt 不一样则替换
    COMMAND ${CMAKE_COMMAND}
    -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cefviewcore/CMakeLists.txt"
    "${CMAKE_BINARY_DIR}/_deps/cefviewcore-src/CMakeLists.txt"

    # src/cefviewcore/src/CMakeLists.txt 对比 build/${CEF_VERSION_MAJOR}/vs2019_x64-Debug/_deps/cefviewcore-src/src/CMakeLists.txt" 不一样则替换
    COMMAND ${CMAKE_COMMAND}
    -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cefviewcore/src/CMakeLists.txt"
    "${CMAKE_BINARY_DIR}/_deps/cefviewcore-src/src/CMakeLists.txt"

    # 删除文件夹 如果存在
    COMMAND ${CMAKE_COMMAND}
    -E remove_directory
    "${CMAKE_BINARY_DIR}/_deps/cefviewcore-src/include"

    # 删除文件夹 如果存在
    COMMAND ${CMAKE_COMMAND}
    -E remove_directory
    "${CMAKE_BINARY_DIR}/_deps/cefviewcore-src/src/CefView"

    # 删除文件夹 如果存在
    COMMAND ${CMAKE_COMMAND}
    -E remove_directory
    "${CMAKE_BINARY_DIR}/_deps/cefviewcore-src/src/CefWing"

    # 删除文件夹 如果存在
    COMMAND ${CMAKE_COMMAND}
    -E remove_directory
    "${CMAKE_BINARY_DIR}/_deps/cefviewcore-src/src/Shared"

    RESULT_VARIABLE cmd_result
    OUTPUT_VARIABLE cmd_output
    ERROR_VARIABLE cmd_error
)

# 检查命令执行结果
if(NOT cmd_result EQUAL 0)
    message(WARNING "Command execution failed with error: ${cmd_error}")
else()
    message(STATUS "Command executed successfully. Output: ${cmd_output}")
endif()
