# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/src/dep_spdlog")
  file(MAKE_DIRECTORY "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/src/dep_spdlog")
endif()
file(MAKE_DIRECTORY
  "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/src/dep_spdlog-build"
  "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix"
  "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/tmp"
  "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/src/dep_spdlog-stamp"
  "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/src"
  "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/src/dep_spdlog-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/src/dep_spdlog-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Public/opengl_eaxmple/build/dep_spdlog-prefix/src/dep_spdlog-stamp${cfgdir}") # cfgdir has leading slash
endif()
