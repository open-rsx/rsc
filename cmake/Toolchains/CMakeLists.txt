if(INSTALL_TOOLCHAINS)

    message(STATUS "Installing CMake toolchain files to: ${RSC_CMAKE_TOOLCHAIN_PATH}")

    install(FILES Oncilla_RB110.cmake
            DESTINATION ${RSC_CMAKE_TOOLCHAIN_PATH})

endif()
