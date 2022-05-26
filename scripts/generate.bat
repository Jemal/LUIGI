@echo off

git submodule update --init --recursive

pushd %~dp0\..\
premake5 vs2022
popd

pause