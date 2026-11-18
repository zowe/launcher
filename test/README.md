<!--
This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.
-->

# Tests

## Other tests

The `other` directory contains the JavaScript changelog tests.
Run `node --test test/other/changelog.test.js test/other/set-changelog.test.js` from the
repository root. These tests use temporary repositories to verify creating fragments
from PR descriptions, merging, validation, previews, and fragment cleanup without
changing the repository's changelog.

## Launcher tests

The `launcher` directory contains the shell tests and their `files` and
`fakeRuntime` fixtures. These tests require `bin/zowe_launcher` built for z/OS.
Run them on z/OS from the repository root:

```shell
sh test/launcher/config-syntax.sh
sh test/launcher/environments.sh
```

The scripts resolve paths from their own directory, so they can also be run
from `test/launcher` or another working directory.

### Config-syntax

Simple shell script to test basic functionality:
* Treating items in the environment variable `CONFIG` and `haInstance`.
* Typical start-up messages.
* To see the output, run the script with any parameter, for example: `sh test/launcher/config-syntax.sh print` from the repository root.

### Environments

Test script for `zowe.environments`. It performs the following checks:
* String Escaping: it verifies how strings with special characters are escaped by checking if the variable is present in the output.
* Environment variable filtering: it ignores environment variables that do not have valid Unix names.
* See [config](./launcher/files/zowe.environments.yaml) used for testing.
