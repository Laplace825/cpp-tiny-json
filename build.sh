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

cmake_folder="$cur/cmake_files"

if [ ! -d $cmake_folder/cmake-$build_type-build ];then
    mkdir "${cmake_folder}/cmake-$build_type-build"
fi

build_dir="${cmake_folder}/cmake-${build_type}-build"

cmake -S $cur -B "${build_dir}" -DCMAKE_BUILD_TYPE="$build_type"
mv "${build_dir}/compile_commands.json" $cur/compile_commands.json
cmake --build "${build_dir}" -j16
