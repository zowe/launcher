#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.

LAUNCHER_TEST_DIR=$(CDPATH= cd "$(dirname "$0")/.." && pwd) || exit 1
. "${LAUNCHER_TEST_DIR}/lib/common.sh" || exit 1

ZOWE_YAML="${LAUNCHER_TEST_FILES}/zowe.environments.yaml"

LAUNCHER_OUTPUT=
LAUNCHER_OUTPUT_TEST_VAR=
LAUNCHER_OUTPUT_KEYS=
export ZLDEBUG='ON'
export CONFIG="FILE(${ZOWE_YAML})"

LAUNCHER_OUTPUT=$("${LAUNCHER_BINARY}" "ha1" 2>&1)

printf_debug '%s' "${LAUNCHER_OUTPUT}"

printf_debug "\n- zowe.environments.TEST_VAR_* expected to be used or ignored\n"
# awk prints between TEST_VAR_START= and TEST_VAR_END=
LAUNCHER_OUTPUT_TEST_VAR=$(printf "%s" "${LAUNCHER_OUTPUT}" | awk '/TEST_VAR_START=/{flag=1}/TEST_VAR_END=/{print; flag=0}flag')
while read -r line; do
    if [ -n "$(echo "${line}" | grep 'TEST_VAR_')" ]; then
        key=$(echo "${line}" | cut -d: -f1)
        if [ -n "$(echo "${line}" | grep '#')" ]; then
            output=$(echo "${LAUNCHER_OUTPUT_TEST_VAR}" | grep "${key}")
            if [ -n "${output}" ]; then
                printf_debug "- pass: %s\n" "${output}"
            else
                printf "- failed: '%s' not found!\n" "${key}"
                LAUNCHER_ERRORS=2
            fi

        fi
    fi
done < "${ZOWE_YAML}"

printf_debug "\n- zowe.environments.* expected to be ignored - invalid unix variable name\n"
LAUNCHER_OUTPUT_KEYS=$(printf "%s" "${LAUNCHER_OUTPUT}" | grep -e 'Key in configuration')
while read -r line; do
    if [ -n "$(echo "${line}" | grep -e '<ignored>')" ]; then
        key="$(echo "${line}" | awk -F: '{ print $1}')"
        key="\`zowe.environments.${key}\`"
        matchKey="$(printf "%s" "${LAUNCHER_OUTPUT_KEYS}" | grep -e "${key}")"
        if [ -n "${matchKey}" ]; then
            printf_debug "- pass: '%s' found: %s\n" "${key}" "${matchKey}"
        else
            printf "- failed '%s' not found!\n" "${key}"
            LAUNCHER_ERRORS=2
        fi
    fi
done < "${ZOWE_YAML}"

exit $LAUNCHER_ERRORS
