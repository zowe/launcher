/*
  This program and the accompanying materials are made available
  under the terms of the Eclipse Public License v2.0 which
  accompanies this distribution, and is available at
  https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

const NO_ACTION = "No action required.";
const CONTACT_SUPPORT = "Contact support."
const ZOS_MIN_SUPPORTED = '2.5.0'

export const MESSAGES = [
    {
        id: "ZWEL0001I",
        text: "component %s started",
        reason: "The component `<component-name>` was started.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0002I",
        text: "component %s stopped",
        reason: "The component `<component-name>` was stopped.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0003I",
        text: "new component initialized %s, restart_cnt=%d, min_uptime=%d seconds, share_as=%s",
        reason: `The component \`<component-name>\` was initialized.
  - \`restart_cnt\` - The number of attempts to restart the component in case of failure
  - \`min_uptime\` - The minimum uptime that the component can be considered as successfully started
  - \`share_as\` - One of the following values: \`<yes|no|must>\`. The value indicates whether child processes of the component start in the same address space. For details, see [_BPX_SHAREAS](https://www.ibm.com/docs/en/zos/${ZOS_MIN_SUPPORTED}?topic=shell-setting-bpx-shareas-bpx-spawn-script) in the IBM documentation.`,
        action: NO_ACTION
    },
    {
        id: "ZWEL0004I",
        text: "component %s(%d) terminated, status = %d",
        reason: "The component `<component-name>`(`<process-id>`) terminated with the status `<code>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0005I",
        text: "next attempt to restart component %s in %d seconds",
        reason: "Next attempt to restart component `<component-name>` in `<n>` seconds.",
        action: `${NO_ACTION}. The component \`<component-name>\` will be restarted in \`<n>\` seconds.`
    },
    {
        id: "ZWEL0006I",
        text: "starting components",
        reason: "Starting the components.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0007I",
        text: "components started",
        reason: "The components are started.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0008I",
        text: "stopping components",
        reason: "Stopping the components.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0009I",
        text: "components stopped",
        reason: "The components are stopped.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0010I",
        text: "launcher has the following components:",
        reason: "The launcher has the following components.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0011I",
        text: "name = %16.16s, PID = %d",
        reason: "Name = `<component-name>`, PID = `<process-id>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0012I",
        text: "starting console listener",
        reason: "Starting the console listener.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0013I",
        text: "command '%s' received",
        reason: "The command `<command>` was received.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0014I",
        text: "termination command received",
        reason: "The termination command was received.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0015I",
        text: "console listener stopped",
        reason: "The console listener was stopped.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0016I",
        text: "start component list: '%s'",
        reason: "Start the component list `<component-list>`",
        action: NO_ACTION
    },
    {
        id: "ZWEL0017I",
        text: "ROOT_DIR is '%s'",
        reason: "The ROOT_DIR (`zowe.runtimeDirectory`) is `<zowe-runtime-directory>`",
        action: NO_ACTION
    },
    {
        id: "ZWEL0018I",
        text: "Zowe instance prepared successfully",
        reason: "Zowe instance prepared successfully.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0019I",
        text: "Zowe Launcher stopping",
        reason: "Zowe Launcher is stopping.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0020I",
        text: "loading '%s'",
        reason: "Zowe Launcher is loading a `<file>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0021I",
        text: "Zowe Launcher starting",
        reason: "Zowe Launcher is starting.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0022I",
        text: "Zowe Launcher stopped",
        reason: "Zowe Launcher was stopped.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0023I",
        text: "Zowe YAML config file is '%s'",
        reason: "Zowe YAML config file is `<path-to-zowe-yaml>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0024I",
        text: "HA_INSTANCE_ID is '%s'",
        reason: "The HA_INSTANCE_ID name is `<ha-instance-name>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0025I",
        text: "restart_intervals for component '%s'= %s",
        reason: `Restart intervals for component \`<component-name>\` = \`<restart-intervals>.\`
  Restart intervals is defined in \`zowe.launcher.restartIntervals\`.`,
        action: NO_ACTION
    },
    {
        id: "ZWEL0058I",
        text: "WORKSPACE_DIR is '%s'",
        reason: "The WORKSPACE_DIR (`zowe.workspaceDirectory`) is `<path-to-workspace-dir>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0069I",
        text: "Configuration is valid",
        reason: "The configuration is valid.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0026E",
        text: "%s env variable not found",
        reason: "The environmental variable `<variable-name>` was not found.",
        action: "In launcher's STC, under `DD` statement `STDENV`, review the `<variable-name>`."
    },
    {
        id: "ZWEL0027E",
        text: "%s env variable too large",
        reason: "`<variable-name>` environmental variable is too large.",
        action: "In launcher's STC, under `DD` statement `STDENV`, review the `<variable-name>`."
    },
    {
        id: "ZWEL0028E",
        text: "failed to get component list",
        reason: "Failed to get the component list.",
        action: "Review the components defined in the configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0029E",
        text: "start component list is empty",
        reason: "Start component list is empty.",
        action: "Review the components defined in the configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0030E",
        text: "failed to prepare Zowe instance",
        reason: "Failed to prepare the zowe instance.",
        action: "Check previous messages in the Zowe Launcher `SYSPRINT` to find the reason and correct it."
    },
    {
        id: "ZWEL0031E",
        text: "failed to setup signal handlers",
        reason: "Failed to setup signal handlers.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0032E",
        text: "failed to find %s='%s', check if the dir exists",
        reason: "Failed to find `<dir-type>`=`<dir-path>`, check if the directory exists.",
        action: "Verify `<dir-type>` is correctly defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0033E",
        text: "failed to get ROOT_DIR dir",
        reason: "Failed to get ROOT_DIR (`zowe.runtimeDirectory`).",
        action: "Review `zowe.runtimeDirectory` defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0034E",
        text: "ROOT_DIR is empty string",
        reason: "ROOT_DIR (`zowe.runtimeDirectory`) is empty string.",
        action: "Review `zowe.runtimeDirectory` defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0035E",
        text: "invalid command line arguments, provide HA_INSTANCE_ID as a first argument",
        reason: "Invalid command line arguments, provide `<HA_INSTANCE_ID>` as a first argument.",
        action: "Review the command and provide `<HA_INSTANCE_ID>` as a first argument."
    },
    {
        id: "ZWEL0036E",
        text: "failed to initialize launcher context",
        reason: "Failed to initialize launcher context.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0037E",
        text: "max component number reached, ignoring the rest",
        reason: "Maximal number of components reached, ignoring the rest.",
        action: "Review the components defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0038E",
        text: "failed to restart component %s, max retries reached",
        reason: "The component `<component-name>` terminates, and the start limit of the launcher has been reached,\n  thereby preventing component restart.",
        action: "Review the logs to determine the cause of component terminations.\n  When the problem has been corrected, restart the main Zowe task or\n  restart the component manually, to continue using the component.\n  For more information on restarting Zowe or individual components,\n  see [Starting and stopping Zowe](https://docs.zowe.org/stable/user-guide/start-zowe-zos)."
    },
    {
        id: "ZWEL0039E",
        text: "cannot start component %s - already running",
        reason: "Cannot start the component `<component-name>` because it is already running.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0040E",
        text: "failed to start component %s",
        reason: "Failed to start the component `<component-name>`.",
        action: "Review the component defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0041E",
        text: "bad value supplied, command ignored",
        reason: "Bad value for the command supplied, such command is ignored.",
        action: "Review the command."
    },
    {
        id: "ZWEL0042E",
        text: "command not recognized",
        reason: "Command not recognized.",
        action: "Review the command."
    },
    {
        id: "ZWEL0043E",
        text: "failed to start console thread",
        reason: "Failed to start the console thread.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0044E",
        text: "failed to stop console thread",
        reason: "Failed to stop the console thread.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0045E",
        text: "error converting zowe.yaml file - %s",
        reason: "Error converting zowe.yaml file - `<path-to-zowe-yaml>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0046E",
        text: "error reading zowe.yaml file - %s",
        reason: "There is an error while reading zowe.yaml file - `<path-to-zowe.yaml>`",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0047E",
        text: "failed to parse zowe.yaml - %s",
        reason: "Failed to parse the zowe.yaml - `<path-to-zowe-yaml>`.",
        action: "Verify that the YAML has no syntax errors."
    },
    {
        id: "ZWEL0048E",
        text: "failed to open zowe.yaml - %s: %s",
        reason: "Failed to open zowe.yaml - `<path-to-zowe-yaml>`",
        action: "Verify if the YAML provided exists and the user running Zowe has permission to read it."
    },
    {
        id: "ZWEL0049E",
        text: "failed to restart component %s:",
        reason: "Failed to restart the component `<component-name>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0050E",
        text: "cannot read output from comp %s(%d) - %s",
        reason: "Cannot read the output from component `<component-name>` (`<process-id>`) - `<error-text>`",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0055E",
        text: "failed to create file for stdin(%s) - %s",
        reason: "Failed to create the file for stdin(`<file>`) - `<error-text>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0056E",
        text: "failed to open file for stdin(%s) - %s",
        reason: "Failed to open the file for stdin(`<file>`) - `<error-text>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0057E",
        text: "failed to create workspace dir '%s'",
        reason: "Failed to create the workspace directory `<path-to-workspace-dir>`.",
        action: "Verify that the directory is valid and the Zowe user has permission to create it."
    },
    {
        id: "ZWEL0059E",
        text: "failed to get WORKSPACE_DIR dir",
        reason: "Failed to get the WORKSPACE_DIR (`zowe.workspaceDirectory`) directory.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0060E",
        text: "WORKSPACE_DIR is empty string",
        reason: "The WORKSPACE_DIR (`zowe.workspaceDirectory`) is empty string.",
        action: "Correct the Zowe YAML to define the `zowe.workspaceDirectory` value."
    },
    {
        id: "ZWEL0061E",
        text: "failed to find %s='%s', check if the file exists",
        reason: "Failed to find ?=?, check if the file exists.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0062E",
        text: "failed to create dir '%s' - %s",
        reason: "Failed to create the directory `<directory>` - `<error-text>`.",
        action: "Review the error text to determine the action to take."
    },
    {
        id: "ZWEL0064E",
        text: "failed to run command %s - %s",
        reason: "Failed to run the command `<command>` - `<error-text>`.",
        action: "Review the error text to determine the action to take."
    },
    {
        id: "ZWEL0065E",
        text: "error reading output from command '%s' - %s",
        reason: "There is an error reading the output from command `<command>` - `<error-text>`",
        action: "Review the error text to determine the action to take."
    },
    {
        id: "ZWEL0067E",
        text: "PARMLIB() entries must all have the same member name",
        reason: "PARMLIB() entries must all have the same member name.",
        action: "Review the member names are identical for all PARMLIB() entries."
    },
    {
        id: "ZWEL0068E",
        text: "PARMLIB() entries must have a member name",
        reason: "PARMLIB() entries must have a member name.",
        action: "Review the dataset name contains the member name in PARMLIB entry."
    },
    {
        id: "ZWEL0070E",
        text: "Configuration has validity exceptions:",
        reason: "Configuration has validity exceptions.",
        action: "Review the exceptions and correct the configuration."
    },
    {
        id: "ZWEL0071E",
        text: "Internal failure during validation, please contact support",
        reason: "Internal failure during validation, please contact support.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0072E",
        text: "Launcher could not load configurations",
        reason: "Launcher could not load the configurations.",
        action: "Review the configuration entries."
    },
    {
        id: "ZWEL0073E",
        text: "Launcher could not load schemas, status=%d",
        reason: "Launcher could not load schemas, status=`<return-code>`",
        action: `For the \`status=5\`, locate the \`zowe.runtimeDirectory\` in the configuration.

  Check the \`zowe.runtimeDirectory/schemas\` contains four \`.json\` files shown below:

  \`\`\`
  manifest-schema.json
  server-common-json
  trivial-component-schema.json
  zowe-yaml-schema.json
  \`\`\`

  On occasion the error occurs because the \`zowe.runtimeDirectory\` is pointing to a valid directory, but one which doesn't contain a valid Zowe runtime environment is one of the first failures during a Zowe launch.`
    },
    {
        id: "ZWEL0074E",
        text: "Log context was not created",
        reason: "The logging context was not created.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0051W",
        text: "failed to read zowe.yaml, launcher will use default settings",
        reason: "Failed to read zowe.yaml, launcher will use default settings.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0052W",
        text: "not all components started",
        reason: "Not all components were started.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0053W",
        text: "not all components stopped gracefully",
        reason: "Not all components were stopped gracefully.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0054W",
        text: "component %s not found",
        reason: "The component `<component-name>` was not found.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0063W",
        text: "Component %s(%d) will be terminated using SIGKILL",
        reason: "Component `<component-name>`(`<process-id>`) will be terminated using `SIGKILL`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0066W",
        text: "command '%s' ended with code %d",
        reason: "The command `<command>` ended with return code `<return-code>`.",
        action: NO_ACTION
    }
]
