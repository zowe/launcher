<!--
This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.
-->

# Tests

## Running the tests

On z/OS, build the launcher, then run all tests from the repository root:

```sh
sh test/index.sh
```

Pass any argument (for example, `sh test/index.sh print`) to include launcher
output in the tests.
The runner discovers every `*.sh` file directly in `testCases/` in filename order,
executes each in a separate shell, continues after failures, and prints a
passed/failed summary. Other files and subdirectories are skipped.

Individual tests can still be run with, for example, `sh test/testCases/config-syntax.sh print`.
Scripts resolve their fixtures from the `test/` directory, so they can be invoked
from any working directory using the appropriate path to the script.

The default binary is `bin/zowe_launcher`. Override it for the suite or any
individual test with `LAUNCHER_BINARY`; relative paths are resolved from your current directory:

```sh
LAUNCHER_BINARY=/path/to/zowe_launcher sh test/index.sh
```

Exit codes are `0` for success, `1` for setup errors, and `2` for test failures.
The runner exits with `1` if its initial setup fails or no tests are found,
or `2` if any test script fails.

Shared setup lives in `lib/common.sh`: launcher validation, working directory,
fixture paths, verbosity, and error initialization. To add a test, create a
`*.sh` file in `testCases/` using this setup; no change to `index.sh` is needed:

```sh
#!/bin/sh
LAUNCHER_TEST_DIR=$(CDPATH= cd "$(dirname "$0")/.." && pwd) || exit 1
. "${LAUNCHER_TEST_DIR}/lib/common.sh" || exit 1

# Add test checks here; set LAUNCHER_ERRORS=2 on failure.
exit "$LAUNCHER_ERRORS"
```

## Debug the tests

* Any parameter for debug output, for example: `sh test/testCases/config-syntax.sh print`.

## Config-syntax

Simple shell script to test basic functionality:
* Treating items in the environment variable `CONFIG` and `haInstance`.
* Typical start-up messages.

## Config-length

Test for [check config length](https://github.com/zowe/launcher/pull/233).
* When the config path starts with slash, it is wrapped into `FILE()`.
* This tests the buffer overflow check.
* The first group expects the launcher to report `ZWEL0072E Launcher Could not load configurations`
* The second group `ZWEL0027E CONFIG env variable too large`

## Environments

Test script for `zowe.environments`. It performs the following checks:
* String Escaping: it verifies how strings with special characters are escaped by checking if the variable is present in the output.
* Environment variable filtering: it ignores environment variables that do not have valid Unix names.
* See [config](./files/zowe.environments.yaml) used for testing.
