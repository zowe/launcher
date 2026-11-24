#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.

LAUNCHER_TEST_DIR=$(CDPATH= cd "$(dirname "$0")" && pwd) || exit 1
. "${LAUNCHER_TEST_DIR}/lib/common.sh" || exit 1

passed=0
failed=0
for test_script in "${LAUNCHER_TEST_DIR}/testCases/"*.sh; do
    [ -f "$test_script" ] || continue
    test_name=${test_script##*/}
    printf '\n[%s]\n' "$test_name"
    if sh "$test_script" "$@"; then
        printf '\n[%s - PASS]\n' "$test_name"
        passed=$((passed + 1))
    else
        status=$?
        printf '\n[%s - FAIL: %s]' "$test_name" "$status"
        failed=$((failed + 1))
    fi
done

if [ "$((passed + failed))" -eq 0 ]; then
    printf 'No test scripts found in %s/testCases\n' "$LAUNCHER_TEST_DIR" >&2
    exit 1
fi

printf '\nResults: %s passed, %s failed\n' "$passed" "$failed"
[ "$failed" -eq 0 ] || exit 2
