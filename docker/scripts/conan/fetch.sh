#!/usr/bin/env bash

# Copyright 2021-2023 Zinchenko Serhii <zinchenko.serhii@pm.me>.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# DESCRIPTION:
#
# Fetch Conan dependencies for a all compilers and build types supported by
# project.
#
# ATTENTION:
#
# This script is mainly supposed to be used only by docker images to provide
# a pre-build images with all pre-installed system packages and project
# dependencies.

set -e

#######################################
# Fetch Conan dependencies for a specific set of compilers and build type.
# Arguments:
#   $1 - C compiler
#   $2 - C++ compiler
#   $3 - CMake build type
#######################################
fetch()
{
    local -r cc="${1}"
    local -r cxx="${2}"
    local -r type="${3}"

    echo -e "\033[0;33mFetching Conan dependencies for CC=${cc}, CXX=${cxx}, CMAKE_BUILD_TYPE=${type} configuration...\033[0m"

    # FMI: https://stackoverflow.com/questions/75103211
    # shellcheck disable=SC2046
    if [ $(command -v "${cxx}" &> /dev/null ; echo $?) -ne 0 ]; then
        echo -e "\033[0;31mCould not find compiler set in environment variable CXX (${cxx})! Skipping current setup! \033[0m"
        return
    fi

    # NOTE(b110011): We need both '-if' and '-of' due to
    #   https://github.com/conan-io/conan/issues/11483.
    conan install . -if build -of build --build=missing \
        -pr:b "${cc}" \
        -pr:h "${cc}" \
        -s:b build_type="${type}" \
        -s:h build_type="${type}"

    rm -rf build
}

#######################################
# Main entry point for this script.
# Arguments:
#   None
#######################################
main()
{
    fetch 'clang' 'clang++' 'Debug'
    fetch 'clang' 'clang++' 'Release'
    fetch 'gcc' 'g++' 'Debug'
    fetch 'gcc' 'g++' 'Release'

    # Clean-up conan cache
    # https://stackoverflow.com/questions/50247671
    conan remove "*" -s -f

    # TODO(b110011): Replace when migration to Conan 2 is finished.
    # https://docs.conan.io/2/reference/commands/cache.html#conan-cache-clean
    # https://github.com/conan-io/conan/issues/11562
    # conan cache clean "*" -b -d -s
}

main
