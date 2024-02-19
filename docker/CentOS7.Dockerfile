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

# Never try to install CentOS 7 SCL packages and build with them something in a
# same Docker file. This is just a one big pain. But if you still want...
#
# First of all I tried a below solution, that is based on:
#   - https://stackoverflow.com/questions/20635472
#   - https://stackoverflow.com/questions/55901985
#
# CentOS7.Dockerfile
#   ...
#   RUN echo "source scl_source enable devtoolset-8" >> /etc/bashrc && \
#       echo "source scl_source enable llvm-toolset-7.0" >> /etc/bashrc
#
#   Make SCL available globally in user environment.
#   SHELL ["/bin/bash", "-c", "source /etc/bashrc"]
#   ...
#   # Make SCL available to anyone how use this docker image.
#   ENTRYPOINT ["/bin/bash", "-c", "source /etc/bashrc"]
#
# I don't know why but the above Docker file doesn't work at all. It literally
# breaks GitLab CI. CI doesn't run 'script:' section with a such docker and
# already report successful result.
#
# Working solution is based on https://github.com/sclorg/devtoolset-container/
# docker container for 7-toolchain.

FROM centos:7

WORKDIR /tmp

COPY scripts/os/setup_centos7.sh \
     pyproject.toml \
     ./
RUN bash setup_centos7.sh

# Enable the SCL for all bash scripts.
ENV BASH_ENV=/etc/profile

COPY conan/profiles/centos7_gcc \
     scripts/conan/setup.sh \
     ./
RUN bash setup.sh "centos7"

COPY conan/conanfile.txt \
     scripts/conan/fetch.sh \
     ./
RUN bash fetch.sh

# Remove all temporary files
# hadolint ignore=DL3059
RUN rm -rf ./*

WORKDIR /workspace
