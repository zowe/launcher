#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.

# Callers set LAUNCHER_TEST_DIR to the absolute test/ directory before sourcing.

# Resolve an override relative to the caller's working directory before cd.
LAUNCHER_BINARY=${LAUNCHER_BINARY:-${LAUNCHER_TEST_DIR}/../bin/zowe_launcher}
case "$LAUNCHER_BINARY" in
    /*) ;;
    *) LAUNCHER_BINARY="$(pwd)/${LAUNCHER_BINARY}" ;;
esac
if [ ! -f "$LAUNCHER_BINARY" ] || [ ! -x "$LAUNCHER_BINARY" ]; then
    printf 'Zowe launcher not found or not executable: %s\n' "$LAUNCHER_BINARY" >&2
    exit 1
fi
export LAUNCHER_BINARY

# Fixtures (including fakeRuntime in the YAML) use paths relative to test/.
cd "$LAUNCHER_TEST_DIR" || exit 1
LAUNCHER_TEST_FILES='./files'
LAUNCHER_DEBUG=${1:-}
LAUNCHER_ERRORS=0

# Accept the same format and arguments as printf when debug output is enabled.
printf_debug() {
    [ -n "${LAUNCHER_DEBUG:-}" ] || return 0
    printf "$@"
}

# Exit codes: 0 = success, 1 = setup error, 2 = failed test case(s).
