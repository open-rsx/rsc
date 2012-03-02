
SET(CMAKE_MODULE_PATH "${RSC_CMAKE_MODULE_PATH}")
MESSAGE(STATUS "Installing CMake modules to: ${CMAKE_MODULE_PATH}")

INSTALL(FILES ExtractTBBVersion.cpp.in
              TestAttributeConstructor.cpp
              TestCRTInit.cpp
              CheckInitMethod.cmake
              CheckLSBTypes.cmake
              CMakeDoxyfile.in
              CMakeVariableDump.cmake
              EnableCoverageReport.cmake
              EnableSlocCount.cmake
              FindBoostThreadpool.cmake
              FindBoostUUID.cmake
              FindCBF.cmake
              FindCppcheck.cmake
              FindEigen2.cmake
              FindFRI.cmake
              Findgcovr.cmake
              FindJsonCpp.cmake
              FindLcov.cmake
              FindLog4cxx.cmake
              FindMongo.cmake
              FindOpenKC.cmake
              FindProtocolBuffers.cmake
              Findsloccount.cmake
              FindSpread.cmake
              FindTBB.cmake
              FindXenomai.cmake
              GenerateCppcheck.cmake
              GenerateDoxygen.cmake
              IgnoreReturnCode.cmake
              InstallFilesRecursive.cmake
              PadString.cmake
              ParseArguments.cmake
              PedanticCompilerWarnings.cmake
              SubversionIsWc.cmake
              SuperPedanticCompilerWarnings.cmake
        DESTINATION ${CMAKE_MODULE_PATH})