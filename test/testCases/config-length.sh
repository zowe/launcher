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

# With PATH_MAX=256, FILE() plus NUL leaves room for 4345 path chars.
# Slash-only paths that fit fail to load; longer paths exceed the buffer.
for i in 2 256 1024 4345 4346 4500 8192 10000; do
    if [ "$i" -le 4345 ]; then
        expected=ZWEL0072E
    else
        expected=ZWEL0027E
    fi
    CONFIG=$(awk -v count="$i" 'BEGIN { for (i = 0; i < count; i++) printf "/" }')
    export CONFIG
    result=$("$LAUNCHER_BINARY" "test_ha" 2>&1 | grep -F "$expected")
    printf_debug '%s %5d chars - %s\n' '-' "$i" "$result"
    if [ -z "${result}" ]; then
        printf '%s%s\n' '- not found: ' "$expected"
        LAUNCHER_ERRORS=2
    fi
done

exit "$LAUNCHER_ERRORS"
