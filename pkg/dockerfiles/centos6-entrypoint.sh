#!/bin/sh

./pkg/mk-rel-rpm.sh
cp $BUILD_DIR/_builddir/*.rpm $OUTPUT_DIR
cp $BUILD_DIR/_builddir/x86_64/*.rpm $OUTPUT_DIR
