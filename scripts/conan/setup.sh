#!/usr/bin/env bash

# Copyright 2023 Zinchenko Serhii <zinchenko.serhii@pm.me>.
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
# Setup all necessary Conan stuff.
#
# ATTENTION:
#
# This script is mainly supposed to be used only by docker images to provide
# a pre-build images with all pre-installed system packages and project
# dependencies.

set -e

#######################################
# Create default Conan files.
# Arguments:
#   None
#######################################
init_defaults()
{
    conan config init

    # TODO(b110011): Uncomment when migration to Conan 2 is finished.
    # conan profile detect
}

#######################################
# Setup additional Conan profiles.
# Arguments:
#   $1 - a prefix for conan profiles names (default: linux)
#######################################
setup_profiles()
{
    if [[ -n "${1}" ]]; then
        local -r prefix="${1}"
    else
        local -r prefix="linux"
    fi

    for i in "/tmp/${prefix}"_*; do
        mv "$i" "/root/.conan/profiles/${i#*_}"
    done
}

#######################################
# Setup additional remotes for Conan package repositories.
# Arguments:
#   None
#######################################
setup_remotes()
{
    conan remote add bproto "https://gitlab.com/api/v4/projects/40501309/packages/conan"
}

#######################################
# Main entry point for this script.
# Arguments:
#   $1 - a prefix for conan profiles names
#######################################
main()
{
    local -r prefix="${1}"

    echo -e "\033[0;33mInit a default conan setup\033[0m"
    init_defaults

    echo -e "\033[0;33mSetup custom conan profiles\033[0m"
    setup_profiles "${prefix}"

    echo -e "\033[0;33mSetup custom conan remotes\033[0m"
    setup_remotes
}

main "$@"
