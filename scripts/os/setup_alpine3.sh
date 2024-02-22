#!/usr/bin/env bash

# Copyright 2022-2023 Zinchenko Serhii <zinchenko.serhii@pm.me>.
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
# This script is mainly supposed to be used only by Alpine 3 docker image to
# provide a pre-build image with all pre-installed and pre-configured system
# packages and project dependencies.

set -e

#------------------------------------------------------------------------------#
# Official Alpine 3 packages                                                   #
#------------------------------------------------------------------------------#

declare -a INSTALL_PKGS=(
    "bat"
    "curl"
    "gcc" # for licenseheaders
    "git"
    "less" # with additional flags
    "musl-dev" # for licenseheaders
    "shellcheck"
    "shfmt"
)

# Install packages

apk add --no-cache "${INSTALL_PKGS[@]}"

# grep that comes with current alpine image doesn't support -P.
# https://github.com/jish/pre-commit/issues/265
apk add --no-cache --upgrade grep

rm -rf /var/cache/apk/*

#------------------------------------------------------------------------------#
# Python                                                                       #
#------------------------------------------------------------------------------#

pip3 install --no-cache-dir . '.[docs]' '.[linters]' '.[misc]'
