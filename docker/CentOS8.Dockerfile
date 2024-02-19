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

FROM centos:8

WORKDIR /tmp

COPY scripts/os/setup_centos8.sh  \
     pyproject.toml \
     ./
RUN bash setup_centos8.sh

# Enable the SCL for all bash scripts.
ENV BASH_ENV=/etc/profile

COPY conan/profiles/linux_clang \
     conan/profiles/linux_gcc \
     scripts/conan/setup.sh \
     ./
RUN bash setup.sh

COPY conan/conanfile.txt \
     scripts/conan/fetch.sh \
     ./
RUN bash fetch.sh

# Remove all temporary files
# hadolint ignore=DL3059
RUN rm -rf ./*

WORKDIR /workspace
