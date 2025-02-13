#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH/..

if [ $# -ne 1 ]; then
    >&2 echo "usage: $0 project_name"
fi

project_name=$(echo "$1" | sed -e "s/\\s/_/g" -e "s/-/_/g")

# "$(git rev-parse HEAD)$(test $(git status --porcelain | wc -l) -gt 0 && printf -- -dirty)"
GIT_HASH=$(git rev-parse HEAD)
GIT_DIRTY=$(git status --porcelain | wc -l)

if [ $GIT_DIRTY -ne 0 ]; then
    GIT_HASH="${GIT_HASH}-dirty"
fi

target_hpp="include/${project_name}_version_info.hpp"
target_cpp="src/generated/version_info.cpp"

tmp_cpp=$(mktemp gen_version_info_cpp.src.XXXXXXXX) 
tmp_hpp=$(mktemp gen_version_info_cpp.hdr.XXXXXXXX) 

sed \
    -e "s/XXXPROJECT_NAMEXXX/$project_name/g" \
    -e "s/XXXGIT_HASHXXX/$GIT_HASH/g" \
    code_generation_templates/version_info.hpp > $tmp_hpp

sed \
    -e "s/XXXPROJECT_NAMEXXX/$project_name/g" \
    -e "s/XXXGIT_HASHXXX/$GIT_HASH/g" \
    code_generation_templates/version_info.cpp > $tmp_cpp

cp_hpp=0
cp_cpp=0

if [ -f $target_hpp ]; then
    diff $target_hpp $tmp_hpp > /dev/null
    if [ $? -ne 0 ]; then
        cp_hpp=1
    fi
else
    cp_hpp=1
fi

if [ $cp_hpp -ne 0 ]; then
    cp $tmp_hpp $target_hpp
fi

if [ -f $target_cpp ]; then
    diff $target_cpp $tmp_cpp > /dev/null
    if [ $? -ne 0 ]; then
        cp_cpp=1
    fi
else
    cp_cpp=1
fi

if [ $cp_cpp -ne 0 ]; then
    cp $tmp_cpp $target_cpp
fi

rm -f $tmp_cpp $tmp_hpp
