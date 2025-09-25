#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.

# ---------------------------------------------------
# Start with any parameter -> prints Launcher's output
# ---------------------------------------------------

print=

if [ ! -z "${1}" ]; then
    print='1'
fi

LAUNCHER='../bin/zowe_launcher'
ZOWE_YAML="./files/zowe.environments.yaml"

LAUNCHER_OUTPUT=
export ZLDEBUG='ON'
export CONFIG="FILE(${ZOWE_YAML})"

if [ ! -z "${print}" ]; then
    "${LAUNCHER}" "ha1"
fi

# awk prints between TEST_VAR_START= and TEST_VAR_END=
LAUNCHER_OUTPUT=$("${LAUNCHER}" "ha1" 2>&1 | awk '/TEST_VAR_START=/{flag=1}/TEST_VAR_END=/{print; flag=0}flag')
printf "%s\n\n" "${LAUNCHER_OUTPUT}"

awk '/TEST_VAR_START/{flag=1}/TEST_VAR_END/{print; flag=0}flag' "${ZOWE_YAML}"

exit 0
