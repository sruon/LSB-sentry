include(ExternalProject)

set(SENTRY_BUILD_DIR ${CMAKE_BINARY_DIR}/sb)
set(SENTRY_INSTALL_DIR ${CMAKE_BINARY_DIR}/si)

ExternalProject_Add(sentry-native-external
        URL https://github.com/getsentry/sentry-native/releases/download/0.9.1/sentry-native.zip
        URL_HASH SHA256=e5349b1a233ac52291e54cba3a6d028781d8173e8b3cd759f17cd27769f02eab
        SOURCE_DIR ${CMAKE_BINARY_DIR}/sn
        BINARY_DIR ${SENTRY_BUILD_DIR}
        CMAKE_ARGS
        -DSENTRY_BACKEND=crashpad
        -DSENTRY_BUILD_EXAMPLES=OFF
        -DSENTRY_BUILD_TESTS=OFF
        -DBUILD_SHARED_LIBS=ON
        -DSENTRY_BUILD_SHARED_LIBS=ON
        -DCRASHPAD_ENABLE_STACKTRACE=On
        -DCMAKE_INSTALL_PREFIX=${SENTRY_INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        BUILD_COMMAND cmake --build ${SENTRY_BUILD_DIR} --config ${CMAKE_BUILD_TYPE}
        INSTALL_COMMAND cmake --install ${SENTRY_BUILD_DIR}
        BUILD_BYPRODUCTS
        ${SENTRY_INSTALL_DIR}/lib/sentry.lib
        ${SENTRY_INSTALL_DIR}/bin/sentry.dll
)

file(MAKE_DIRECTORY ${SENTRY_INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${SENTRY_INSTALL_DIR}/lib)
file(MAKE_DIRECTORY ${SENTRY_INSTALL_DIR}/bin)

add_library(sentry SHARED IMPORTED GLOBAL)
set_target_properties(sentry PROPERTIES
        IMPORTED_LOCATION ${SENTRY_INSTALL_DIR}/bin/sentry.dll
        IMPORTED_IMPLIB ${SENTRY_INSTALL_DIR}/lib/sentry.lib
        INTERFACE_INCLUDE_DIRECTORIES ${SENTRY_INSTALL_DIR}/include
)

add_dependencies(sentry sentry-native-external)

target_link_libraries(xi_map PRIVATE sentry)
target_sources(xi_map PRIVATE ${CMAKE_CURRENT_LIST_DIR}/cpp/sentry.cpp)

add_custom_command(TARGET xi_map POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SENTRY_INSTALL_DIR}/bin/sentry.dll
        $<TARGET_FILE_DIR:xi_map>/sentry.dll
        COMMENT "Copying sentry.dll to output directory"
)

# Also copy crashpad_handler.exe
add_custom_command(TARGET xi_map POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SENTRY_INSTALL_DIR}/bin/crashpad_handler.exe
        $<TARGET_FILE_DIR:xi_map>/crashpad_handler.exe
        COMMENT "Copying crashpad_handler.exe to output directory"
)

# Upload debug symbols after successful build
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH ON)
find_program(SENTRY_CLI sentry-cli)
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH OFF)
if (SENTRY_CLI)
    add_custom_command(TARGET xi_map POST_BUILD
            COMMAND ${SENTRY_CLI} debug-files upload
            --include-sources
            --log-level=info
            $<TARGET_FILE_DIR:xi_map>/xi_map.pdb
            $<TARGET_FILE_DIR:xi_map>/xi_map.exe
            $<TARGET_FILE_DIR:xi_map>/sentry.dll
            $<TARGET_FILE_DIR:xi_map>/libluajit_64.dll
            $<TARGET_FILE_DIR:xi_map>/libmariadb64.dll
            $<TARGET_FILE_DIR:xi_map>/libzmq_64.dll
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Uploading debug symbols to Sentry"
            VERBATIM
    )
else ()
    message(FATAL_ERROR "sentry-cli not found. Please install sentry-cli to build with Sentry support.")
endif ()