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

# we use njsc++ shipped with node.js to build
# we hardcoded where njsc is located, so this pipeline may not work well on other platform
export PATH=$PATH:/ZOWE/node/node-v14.15.1-os390-s390x/bin

# build
echo "$SCRIPT_NAME build zowe launcher ..."
cd "$SCRIPT_DIR/content"
make -f zosMakefile

# clean up content folder
echo "$SCRIPT_NAME cleaning up pax folder ..."
cd "$SCRIPT_DIR/content"
rm -fr Jenkinsfile Makefile zosMakefile src .git*
