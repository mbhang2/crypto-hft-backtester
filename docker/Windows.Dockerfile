# escape=`

# Copyright 2024 Zinchenko Serhii <zinchenko.serhii@pm.me>.
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

# NOTE(b110011): Ignore a bunch of docker linter errors:
#   - DL3013 - it's ok, we have pinned versions in 'pyproject.toml'.
#   - DL3021 - it's ok, we have '\\' at the end.
#   - DL3059 - it's ok to have multiple consecutive 'RUN'.
#   - SC1072 - just ignore this.
#   - SC2035 - glob in copy doesn't work.
#   - SC2102 - it's ok to have [<string>].
#   - SC3057 - we don't use string indexing here.
# hadolint global ignore=DL3013,DL3021,DL3059,SC1072,SC2035,SC2102,SC3057

FROM mcr.microsoft.com/windows/servercore:1809 as base

SHELL ["cmd", "/S", "/C"]

ADD "https://aka.ms/vs/17/release/vs_buildtools.exe" "C:\\tmp\\vs_buildtools.exe"
RUN "C:\\tmp\\vs_buildtools.exe" --nocache --norestart --noUpdateInstaller --quiet --wait `
    --installPath C:\\BuildTools `
    --add Microsoft.VisualStudio.Component.VC.CMake.Project `
    --add Microsoft.VisualStudio.Component.Windows10SDK.19041 `
    --add Microsoft.VisualStudio.Workload.VCTools.x86.x64

RUN powershell -ExecutionPolicy Bypass -Command `
    $Env:chocolateyVersion = '1.4.0' ; `
    $Env:chocolateyUseWindowsCompression = 'false' ; `
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; `
    iex ((New-Object System.Net.WebClient).DownloadString('http://chocolatey.org/install.ps1')) && `
    setx PATH "%PATH%;%ALLUSERSPROFILE%\\chocolatey\\bin"

RUN choco feature disable --name showDownloadProgress && `
    choco install -y cmake --installargs 'ADD_CMAKE_TO_PATH=System' --version 3.27.0 && `
    choco install -y git && `
    choco install -y python --version 3.8

COPY pyproject.toml C:\\tmp\\
RUN pip3 install --no-cache-dir C:\\tmp\\[conan]

RUN C:\\BuildTools\\Common7\\Tools\\VsDevCmd.bat && `
    conan config init && `
    conan profile update settings.compiler.cppstd=23 default && `
    conan remote add bproto https://gitlab.com/api/v4/projects/40501309/packages/conan

FROM base as conan-pkgs

COPY conan\\conanfile.txt C:\\tmp\\
RUN conan install C:\\tmp\\ -if C:\\tmp\\build -of C:\\tmp\\build --build=missing `
        -pr:b default `
        -pr:h default `
    	-s build_type=Release && `
    conan remove * -s -f

# Copy helper scripts
COPY scripts\\cicd\\helpers\\build_cpp.ps1 `
     scripts\\cicd\\helpers\\test_cpp.ps1 `
     C:\\cicd\\

# Remove all temporary files
RUN rmdir /s /q C:\\tmp

WORKDIR C:\\Workspace
