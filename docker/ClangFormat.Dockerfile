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

# Take a look at docker files from below repositories if needed:
#   - https://github.com/angular/clang-format
#   - https://github.com/Unibeautify/docker-beautifiers

#------------------------------------------------------------------------------#
# Build clang-format                                                           #
#------------------------------------------------------------------------------#

FROM alpine:3.18 as clang-format-build

# Pass `--build-arg LLVM_TAG=master` for latest llvm commit
ARG LLVM_TAG
ENV LLVM_TAG ${LLVM_TAG:-llvmorg-18-init}

# HadoLint: It's ok to ignore this issue, as we don't care much about pkg versions.
# hadolint ignore=DL3018
RUN apk update \
    && apk add --no-cache build-base git cmake ninja python3 \
    && rm -rf /var/cache/apk/*

# Download and setup
WORKDIR /build
RUN git clone --branch ${LLVM_TAG} --depth 1 https://github.com/llvm/llvm-project.git

# Apply patches
WORKDIR /build/llvm-project
COPY patches/clang-format/* ./
RUN git apply ./*.patch

# Build
WORKDIR /build/llvm-project/build
RUN cmake -GNinja \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DLLVM_BUILD_STATIC=ON \
    -DLLVM_ENABLE_LIBCXX=ON \
    -DLLVM_ENABLE_PROJECTS='clang' \
    ../llvm \
    && ninja -j"$(nproc)" clang-format

#------------------------------------------------------------------------------#
# Install clang-format                                                         #
#------------------------------------------------------------------------------#

FROM python:3-alpine3.18

COPY --from=clang-format-build /build/llvm-project/build/bin/clang-format /usr/bin
COPY --from=clang-format-build /build/llvm-project/clang/tools/clang-format/clang-format-diff.py /usr/local/bin/

# Setup system and install packages

# hadolint ignore=DL3018,SC2261
RUN apk add --no-cache bat bash>=5.1.16 git \
    && apk add --no-cache --upgrade grep \
    && rm -rf /var/cache/apk/*

ENV BASH_ENV "/etc/profile"
SHELL ["/bin/bash", "-c"]

# Install CI/CD scripts and linters config files

WORKDIR /tmp

COPY scripts/os/install_files.sh ./
COPY .* scripts/cicd/*.* scripts/cicd/*/* ./
RUN bash install_files.sh only_clang_format && rm -rf ./*

WORKDIR /workspace
