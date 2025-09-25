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

LAUNCHER_OUTPUT=$("${LAUNCHER}" "ha1" 2>&1 | grep 'DEBUG shared env pos1')

printf "%s\n\n" "${LAUNCHER_OUTPUT}"

printf "${ZOWE_YAML} TEST_VAR*:\n"
cat "${ZOWE_YAML}" | grep TEST_VAR

exit 0
