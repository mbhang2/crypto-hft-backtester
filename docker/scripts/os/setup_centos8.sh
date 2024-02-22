#!/usr/bin/env bash

# Copyright 2020-2023 Zinchenko Serhii <zinchenko.serhii@pm.me>.
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
# This script is mainly supposed to be used only by CentOS 8 docker image to
# provide a pre-build image with all pre-installed and pre-configured system
# packages and project dependencies.

set -e

#------------------------------------------------------------------------------#
# Official Red Hat Enterprise Linux RPM software packages                      #
#------------------------------------------------------------------------------#

# How to install gcc/g++ 9 on CentOS 8 docker (centos:latest)
# See https://stackoverflow.com/questions/61590926

declare -a INSTALL_PKGS=(
    # compilation
    "cmake"
    "gcc-toolset-10-gcc"
    "gcc-toolset-10-gcc-c++"
    "gcc-toolset-10-gcc-gfortran"
    "gcc-toolset-10-gdb"
    "make"
    # essential
    "git"
    "python38-pip"
    "python38-devel"
)

# Fix below issue:
#   Failed to download metadata for repo 'appstream': Cannot prepare internal
#   mirrorlist: No URLs in mirrorlist
# See https://stackoverflow.com/questions/70963985.

sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*
sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*

# Fix doxygen

# See https://pagure.io/epel/issue/89
# See https://serverfault.com/questions/997896

yum install -y dnf-plugins-core
yum config-manager --set-enabled powertools

## Install packages

yum install -y --setopt=tsflags=nodocs "${INSTALL_PKGS[@]}"
rpm -V "${INSTALL_PKGS[@]}"
yum -y clean all --enablerepo='*'

#------------------------------------------------------------------------------#
# Python                                                                       #
#------------------------------------------------------------------------------#

pip3 install --no-cache-dir . '.[conan]'

#------------------------------------------------------------------------------#
# Setup environment                                                            #
#------------------------------------------------------------------------------#

#
# Clang
#

# See https://gatowololo.github.io/blog/clangmissingheaders/
# See https://stackoverflow.com/questions/26333823

# rm -rf /usr/include/c++
# ln -s /opt/rh/gcc-toolset-10/root/usr/include/c++/10/ /usr/include/c++

# ln -s /opt/rh/gcc-toolset-10/root/lib/gcc/x86_64-redhat-linux/10/ /usr/lib/gcc/x86_64-redhat-linux/10

#
# SCL (Software Collections)
#

# Make SCL available globally in user environment.
# https://stackoverflow.com/questions/61590926
declare -r SCL_ENABLE="/etc/profile.d/scl_enable.sh"
echo "source scl_source enable gcc-toolset-10" >> "${SCL_ENABLE}"
