#!/bin/bash

cur_dir=$(cd $(dirname $0); pwd)
docker build -f $cur_dir/dockerfiles/Dockerfile.${1} -t libsmbios-${1} -- .
