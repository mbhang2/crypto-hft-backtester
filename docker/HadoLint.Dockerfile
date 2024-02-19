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

FROM hadolint/hadolint:v2.12.0-alpine


# Setup system and install packages

# hadolint ignore=DL3018
RUN apk add --no-cache bash>=5.1.16 git \
    && apk add --no-cache --upgrade grep \
    && rm -rf /var/cache/apk/*

ENV BASH_ENV "/etc/profile"
SHELL ["/bin/bash", "-c"]

# Install CI/CD scripts and linters config files

WORKDIR /tmp

COPY scripts/os/install_files.sh ./
COPY .* scripts/cicd/*.* scripts/cicd/*/* ./
RUN bash install_files.sh only_hadolint && rm -rf ./*

WORKDIR /workspace
