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

FROM python:3.12-alpine

WORKDIR /tmp

# Alpine docker images comes up with Bourne Shell and not Bash.
# How deep is rabbit hole?
#
# See:
# + Docker containers:
#   - https://docs.gitlab.com/ee/ci/docker/using_docker_images.html
#   - https://gitlab.com/gitlab-org/gitlab-runner/-/issues/1758
#   - https://forum.gitlab.com/t/9429
# + Bourne Shell:
#   - https://stackoverflow.com/questions/58141868
#   - https://unix.stackexchange.com/questions/140502

# hadolint ignore=DL3018
RUN apk add --no-cache bash>=5.1.16

# Setup system and install packages

COPY scripts/os/setup_alpine3.sh \
     pyproject.toml \
     ./
RUN bash setup_alpine3.sh && rm -rf ./*

# Install CI/CD scripts and linters config files

COPY scripts/os/install_files.sh ./
COPY .* scripts/cicd/*.* scripts/cicd/*/* ./
RUN bash install_files.sh && rm -rf ./*

# Change shell in image

ENV BASH_ENV "/etc/profile"
SHELL ["/bin/bash", "-c"]

WORKDIR /workspace
