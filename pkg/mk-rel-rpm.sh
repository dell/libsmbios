#!/bin/bash
# vim:et:ai:ts=4:sw=4:filetype=sh:tw=0:

set -x

cur_dir=$(cd $(dirname $0); pwd)
cd $cur_dir/../

umask 002

set -e

chmod -R +w _builddir ||:
rm -rf _builddir

mkdir _builddir
pushd _builddir
../autogen.sh
if [ -n "$CI" ]; then
	CI="CI=--define '_with_unit_tests 1'"
fi
make rpm "$CI"
