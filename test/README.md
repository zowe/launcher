<!--
This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.
-->

# Tests

## Config-syntax

Simple shell script to test basic functionality, such as treating items in environment variable `CONFIG`, `haInstance` and typical start-up messages.
Return code is the number detected errors. To see an output, use any parameter, for example `./config-syntax.sh print`.

## Environments

Script for testing `zowe.environments`:
* Escaping strings with `\` or `"`. It checks only if the variable is in output.
* Ignoring environment variables, which are not valid unix names.
* Return code is the number detected errors.

See [config](./files/zowe.environments.yaml) used for testing.
