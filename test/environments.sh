#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.

# Start with any parameter -> prints Launcher's output
#   rc of this = number of errors found for "WARN Key in configuration `zowe.environments.<key>` is invalid"

print=
errors=0

if [ ! -z "${1}" ]; then
    print='1'
fi

LAUNCHER='../bin/zowe_launcher'
if [ ! -f "${LAUNCHER}" ]; then
    echo "Executable \"${LAUNCHER}\" not found."
    exit 1
fi

ZOWE_YAML="./files/zowe.environments.yaml"

LAUNCHER_OUTPUT=
LAUNCHER_OUTPUT_TEST_VAR=
LAUNCHER_OUTPUT_KEYS=
export ZLDEBUG='ON'
export CONFIG="FILE(${ZOWE_YAML})"

LAUNCHER_OUTPUT=$("${LAUNCHER}" "ha1" 2>&1)

if [ ! -z "${print}" ]; then
    printf "%s" "${LAUNCHER_OUTPUT}"
fi

# awk prints between TEST_VAR_START= and TEST_VAR_END=
LAUNCHER_OUTPUT_TEST_VAR=$(printf "%s" "${LAUNCHER_OUTPUT}" | awk '/TEST_VAR_START=/{flag=1}/TEST_VAR_END=/{print; flag=0}flag')
printf "%s\n\n" "${LAUNCHER_OUTPUT_TEST_VAR}"

awk '/TEST_VAR_START/{flag=1}/TEST_VAR_END/{print; flag=0}flag' "${ZOWE_YAML}"

printf "\n---zowe.environments.* expected to be ignored ---\n\n"

LAUNCHER_OUTPUT_KEYS=$(printf "%s" "${LAUNCHER_OUTPUT}" | grep -e 'Key in configuration')

while read -r line; do
    if [ -n "$(echo "${line}" | grep -e '<ignored>')" ]; then
        key="$(echo "${line}" | awk -F: '{ print $1}')"
        key="\`zowe.environments.${key}\`"
        matchKey="$(printf "%s" "${LAUNCHER_OUTPUT_KEYS}" | grep -e "${key}")"
        if [ -n "${matchKey}" ]; then
            printf "Key '%s' found:\n  %s\n" "${key}" "${matchKey}"
        else
            echo "Key '${key}' not found!"
            errors=`expr $errors + 1`
        fi
    fi
done < "${ZOWE_YAML}"

exit $errors
