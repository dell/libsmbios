#!/bin/sh

astyle --style=ansi -c --indent-namespaces "$@"

#indent -kr -i8 -ts8 -sob -l80 -ss -bs -psl "$@"

