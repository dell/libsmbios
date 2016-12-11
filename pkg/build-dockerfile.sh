#!/bin/bash

cur_dir=$(cd $(dirname $0); pwd)

cp $cur_dir/dockerfiles/Dockerfile.${1} $cur_dir/../Dockerfile
cp $cur_dir/dockerfiles/${1}-entrypoint.sh $cur_dir/../
docker build -t ${1}-libsmbios -- .
