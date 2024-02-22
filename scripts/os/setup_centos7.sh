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
# This script is mainly supposed to be used only by CentOS 7 docker image to
# provide a pre-build image with all pre-installed and pre-configured system
# packages and project dependencies.

set -e

#------------------------------------------------------------------------------#
# Official Red Hat Enterprise Linux RPM software packages                      #
#------------------------------------------------------------------------------#

declare -a INSTALL_PKGS=(
    # compilation
    "devtoolset-12-gcc"
    "devtoolset-12-gcc-c++"
    "devtoolset-12-gcc-gfortran"
    "devtoolset-12-gdb"
    "devtoolset-12-make"
    # essential
    "git"
    "rh-python38-python"
    "rh-python38-python-devel"
    "rh-python38-python-pip"
)

# Add Springdale repo for devtoolset-12 packages
# https://stackoverflow.com/questions/59888333

cat << \EOF >> "/etc/yum.repos.d/springdale-scl.repo"
[Springdale-SCL]
name=Springdale SCL Base $releasever - $basearch
mirrorlist=http://springdale.princeton.edu/data/springdale/SCL/$releasever/$basearch/mirrorlist
#baseurl=http://springdale.princeton.edu/data/springdale/SCL/$releasever/$basearch
gpgcheck=1
gpgkey=http://springdale.princeton.edu/data/springdale/$releasever/$basearch/os/RPM-GPG-KEY-springdale
EOF

# Install packages

yum install -y centos-release-scl-rh epel-release
yum install -y --setopt=tsflags=nodocs "${INSTALL_PKGS[@]}"
rpm -V "${INSTALL_PKGS[@]}"
yum -y clean all --enablerepo='*'

#------------------------------------------------------------------------------#
# Python                                                                       #
#------------------------------------------------------------------------------#

# One of our packages install Python 3.6 which is ok, but sphinxcontrib-openapi
# can't be used with it, because it fails with this error:
#
#         from __future__ import annotations
#         ^
#     SyntaxError: future feature annotations is not defined
#
# That way we also install rh-python38 which replaces python3.

# Btw, canonical way to enable SCL doesn't work here...
# source scl_source enable rh-python38

# shellcheck source=/dev/null
source /opt/rh/rh-python38/enable

# Install packages

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
# ln -s /opt/rh/devtoolset-10/root/usr/include/c++/10/ /usr/include/c++

# ln -s /opt/rh/gcc-toolset-10/root/lib/gcc/x86_64-redhat-linux/10/ /usr/lib/gcc/x86_64-redhat-linux/10

#
# Conan
#

# As we need to build some of Boost libraries we implicitly depends on b2 utility.
# The main problem here is that b2 is build wuith too modern glibc and libraries.
#
# This results in build crash:
#   b2: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.21' not found (required by b2)
#
# I tried a lot of ways to solve this issue:
#   - see https://github.com/FoldingAtHome/fah-issues/issues/1147
#   - see https://stackoverflow.com/questions/19386651
#   - see https://stackoverflow.com/questions/29376130
#   - see https://stackoverflow.com/questions/42597701
#
# I even came up to this dirty hack that almost worked:
#
# function __hack {
#   libstdcpp_deb_link='http://ftp.de.debian.org/debian/pool/main/g/gcc-4.9/libstdc++6_4.9.2-10+deb8u1_amd64.deb'
#   libstdcpp_deb_dir='/tmp/libstdcpp_deb'
#   libstdcpp_deb_file="libstdc++6.deb"
#
#   mkdir "${libstdcpp_deb_dir}"
#   cd "${libstdcpp_deb_dir}"
#   curl -o "${libstdcpp_deb_file}" "${libstdcpp_deb_link}"
#   ar -x "${libstdcpp_deb_file}"
#   tar xvf 'data.tar.xz'
#
#   mkdir '/opt/libstdcpp'
#   cp './usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.20' '/opt/libstdcpp/libstdc++.so.6'
#   rm -rf "${libstdcpp_deb_dir}"
#
#   export LD_LIBRARY_PATH="/opt/libstdcpp:${LD_LIBRARY_PATH}"
# }
#
# But it has been dropped because it's error prone a lot.
# By some miracle, I found these two issues:
#   - https://github.com/conan-io/conan-center-index/issues/1116
#   - https://stackoverflow.com/questions/66487350
# that helped me to come up with a quite good workaround for this problem.

# Create and update Conan settings

conan config init

sed -i -e 's/os_build: \[/os_build: \[CentOS, /g' "${HOME}/.conan/settings.yml"
sed -i -e $'s/Linux:/Linux:\\\n    CentOS:/g' "${HOME}/.conan/settings.yml"

#
# SCL (Software Collections)
#

# Make SCL available globally in user environment.
# https://stackoverflow.com/questions/55901985
cat << EOF >> "/etc/profile.d/scl_enable.sh"
source scl_source enable devtoolset-12
source scl_source enable rh-python38
EOF
