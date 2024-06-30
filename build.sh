#!/bin/bash

build_type=$1
    
if [ -z $1 ];then
    build_type="Debug"
elif [ $build_type == "-r" ];then
    build_type="Release"
else
    build_type="Debug"
fi

cur=`pwd`

if [ ! -d $cur/cmake_files ];then
    mkdir "$cur/cmake_files"
fi

cd "$cur/cmake_files"

cmake_folder=`pwd`

if [ ! -d $cmake_folder/cmake-$build_type-build ];then
    mkdir "${cmake_folder}/cmake-$build_type-build"
fi

cd "$cmake_folder/cmake-$build_type-build"
cmake $cur -DCMAKE_BUILD_TYPE="$build_type"
mv compile_commands.json $cur/compile_commands.json
cmake --build . -j16
cd $cur 
