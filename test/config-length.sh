#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.

LAUNCHER=${LAUNCHER:-../bin/zowe_launcher}
if [ ! -x "$LAUNCHER" ]; then
    printf 'Executable for Zowe launcher not found at: %s\n' "$LAUNCHER"
    exit 1
fi

errors=0

# With PATH_MAX=256, FILE() plus NUL leaves room for 4345 path chars.
# Only slashes used for config -> ZWEL0072E Launcher Could not load configurations
for i in 2 256 1024 4345; do
    CONFIG=$(awk -v count="$i" 'BEGIN { for (i = 0; i < count; i++) printf "/" }')
    export CONFIG
    result=$("$LAUNCHER" "test_ha" 2>&1 | grep ZWEL0072E)
    printf '%s chars - %s\n' "$i" "$result"
    if [ -z "${result}" ]; then
        errors=1
    fi
done

# Only slashes used for config, but will not fit into the buffer -> ZWEL0027E CONFIG env variable too large
for i in 4346 4500 8192 10000; do
    CONFIG=$(awk -v count="$i" 'BEGIN { for (i = 0; i < count; i++) printf "/" }')
    export CONFIG
    result=$("$LAUNCHER" "test_ha" 2>&1 | grep ZWEL0027E)
    printf '%s chars - %s\n' "$i" "$result"
    if [ -z "${result}" ]; then
        errors=1
    fi
done

exit "$errors"
