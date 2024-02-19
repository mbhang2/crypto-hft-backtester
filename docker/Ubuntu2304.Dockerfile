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

FROM ubuntu:23.04

WORKDIR /tmp

# Define a gloval virtial environment for python packages.
# Actual dancing will be done in a setup script.

ENV DOCKER_PYTHON_VENV=/opt/venv
ENV PATH="$DOCKER_PYTHON_VENV/bin:$PATH"

# Setup system and install packages

COPY scripts/os/setup_ubuntu2304.sh \
     pyproject.toml \
     ./
RUN bash setup_ubuntu2304.sh

# Setup Conan

COPY conan/profiles/linux_clang \
     conan/profiles/linux_gcc \
     scripts/conan/setup.sh \
     ./
RUN bash setup.sh

# Fetch Conan dependencies

COPY conan/conanfile.txt \
     scripts/conan/fetch.sh \
     ./
RUN bash fetch.sh

# Remove all temporary files
# hadolint ignore=DL3059
RUN rm -rf ./*

# Install CI/CD scripts and linters config files

COPY scripts/os/install_files.sh ./
COPY .* scripts/cicd/*.* scripts/cicd/*/* ./
RUN bash install_files.sh

ENV BASH_ENV "/etc/profile"

WORKDIR /workspace
