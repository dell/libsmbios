#!/bin/sh

./pkg/mk-rel-rpm.sh
cp $BUILD_DIR/_builddir/*.rpm $OUTPUT_DIR
