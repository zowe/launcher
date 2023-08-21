This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.

<h1 align="center">Zowe Launcher</h1>

The Zowe launcher is a part of the Zowe server architecture that was added as an optional program for HA/FT usage in v1.
In v2, the Zowe laucher became the sole way to start the Zowe servers.
The launcher's purpose is to start, restart, and stop each Zowe server component which has a `start` command,
And in doing so it watches over such components for health (restarting them if they crash) and log management.

## Current features
* Stopping Zowe using the conventional `P` operator command
* Ability to handle modify commands
* Stopping and starting specific Zowe components without restarting the entire Zowe

## Future features
* Issuing WTOs indicating the start and termination of specific components (this should simplify the integration with z/OS automation)
* Passing modify commands to Zowe components
* Clean termination of the components in case if the launcher gets cancelled

## Building

```
cd zowe-launcher/build 
./build.sh
```

The launcher binary will be saved into the bin directory.

## Prerequisites

* Zowe 2.4.0

## Deployment

* Find the JCL used to start your Zowe instance (ZWESLSTC)
* Stop that Zowe instance
* Find the dataset used by that STC
* Copy the binary from the bin directory into that dataset with whatever name you want
* If necessary, update the PGM name to match the name of the resulting copy you just did
* Restart that Zowe instance

## Operating the launcher

* To start the launcher use the `S` operator command:
```
S ZWELNCH
```
* To stop use the `P` operator command:
```
P ZWELNCH
```
* To stop a specific component use the following modify command:
```
F ZWELNCH,APPL=STOP(component_name)
```
* To start a specific component use the following modify command:
```
F ZWELNCH,APPL=START(component_name)
```
* To list the components use the following modify command:
```
F ZWELNCH,APPL=DISP
```

## Community

This part of Zowe is currently developed by the zOS squad, which you can find on Slack at #zowe-zos-interface or 
