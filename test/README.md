<!--
This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.
-->

# Tests

## Config-syntax

Simple shell script to test basic functionality:
* Treating items in the environment variable `CONFIG` and `haInstance`.
* Typical start-up messages.
* To see the output, please run the script with any parameter, for example: `./config-syntax.sh print`.

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
