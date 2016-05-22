cmake_minimum_required(VERSION 3.2)
project(vlcrc)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(project_base_dir "${PROJECT_SOURCE_DIR}")
set(cxx_headers_dir  "${project_base_dir}/include")
set(cxx_sources_dir  "${project_base_dir}/src")

set(cxx_headers )
set(cxx_sources )

set(cxx_public_compile_features )
set(cxx_public_compile_options )
set(cxx_public_compile_definitions )
set(cxx_public_libraries )

set(cxx_private_compile_features )
set(cxx_private_compile_options )
set(cxx_private_compile_definitions )
set(cxx_private_libraries )

list(APPEND cxx_private_compile_features
    cxx_auto_type
    cxx_nullptr
    cxx_lambdas
    cxx_lambda_init_captures
    cxx_local_type_template_args
    cxx_range_for
    cxx_rvalue_references)

find_package(LIBVLC REQUIRED)

list(APPEND cxx_private_libraries
    ${LIBVLC_LIBRARY})

list(APPEND cxx_sources
    "${cxx_sources_dir}/main.cpp")

add_executable(${PROJECT_NAME}
    ${cxx_headers}
    ${cxx_sources})
target_compile_features(${PROJECT_NAME}
    PUBLIC
    ${cxx_public_compile_features}
    PRIVATE
    ${cxx_private_compile_features})
target_compile_options(${PROJECT_NAME}
    PUBLIC
    ${cxx_public_compile_options}
    PRIVATE
    ${cxx_private_compile_options})
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    ${cxx_public_compile_definitions}
    PRIVATE
    ${cxx_private_compile_definitions})
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${LIBVLC_INCLUDE_DIR}
    ${cxx_headers_dir})
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${cxx_public_libraries}
    PRIVATE
    ${cxx_private_libraries})
