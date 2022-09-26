#!/bin/sh -e
set -xe

################################################################################
# This program and the accompanying materials are made available under the terms of the
# Eclipse Public License v2.0 which accompanies this distribution, and is available at
# https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.
################################################################################


# contants
SCRIPT_NAME=$(basename "$0")
SCRIPT_DIR=$(pwd)

# To solve this error following KC: https://www.ibm.com/support/pages/compiler-invocation-results-error-fsum3067-archive-library-not-found-or-error-fsum3008-incorrect-suffix-used
# FSUM3008 Specify a file with the correct suffix (.c, .i, .s, .o, .x, .p, .I, or .a), or a corresponding data set name, instead of -L/usr/lpp/cbclib/lib.FSUM3008 Specify a file with the correct suffix (.c, .i, .s, .o, .x, .p, .I, or .a), or a corresponding data set name, instead of -L/usr/lpp/cbclib/lib.
export _CC_CCMODE=1
export _CXX_CCMODE=1
export _C89_CCMODE=1

# build
echo "$SCRIPT_NAME build zowe launcher ..."
cd "$SCRIPT_DIR/content/build"
./build.sh

# clean up content folder
echo "$SCRIPT_NAME cleaning up pax folder ..."
cd "$SCRIPT_DIR/content"
rm -fr Jenkinsfile Makefile build src deps *.o *.lst .git*
