#!/bin/bash

rm -rf result
mkdir result

read -p "Please enter the directory path:"
dir_path=${REPLY}
#dir_path="/opt/analysis"

if [ ${dir_path: -1} != "/" ]; then
    dir_path=${dir_path}"/"
fi

cd ./build

for cpp_files in ${dir_path}*; do
    if [ ${cpp_files: -4} == ".cpp" ]; then
        echo ${cpp_files}
        ./hello ${cpp_files}
    fi
done

echo "The result is stored in the result directory"
