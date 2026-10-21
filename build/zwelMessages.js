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
        define: "MSG_COMP_STARTED",
        text: "component %s started",
        reason: "The component `<component-name>` was started.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0002I",
        define: "MSG_COMP_STOPPED",
        text: "component %s stopped",
        reason: "The component `<component-name>` was stopped.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0003I",
        define: "MSG_COMP_INITED",
        text: "new component initialized %s, restart_cnt=%d, min_uptime=%d seconds, share_as=%s",
        reason: `The component \`<component-name>\` was initialized.
  - \`restart_cnt\` - The number of attempts to restart the component in case of failure
  - \`min_uptime\` - The minimum uptime that the component can be considered as successfully started
  - \`share_as\` - One of the following values: \`<yes|no|must>\`. The value indicates whether child processes of the component start in the same address space. For details, see [_BPX_SHAREAS](https://www.ibm.com/docs/en/zos/${ZOS_MIN_SUPPORTED}?topic=shell-setting-bpx-shareas-bpx-spawn-script) in the IBM documentation.`,
        action: NO_ACTION
    },
    {
        id: "ZWEL0004I",
        define: "MSG_COMP_TERMINATED",
        text: "component %s(%d) terminated, status = %d",
        reason: "The component `<component-name>`(`<process-id>`) terminated with the status `<code>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0005I",
        define: "MSG_NEXT_RESTART",
        text: "next attempt to restart component %s in %d seconds",
        reason: "Next attempt to restart component `<component-name>` in `<n>` seconds.",
        action: `${NO_ACTION}. The component \`<component-name>\` will be restarted in \`<n>\` seconds.`
    },
    {
        id: "ZWEL0006I",
        define: "MSG_STARTING_COMPS",
        text: "starting components",
        reason: "Starting the components.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0007I",
        define: "MSG_COMPS_STARTED",
        text: "components started",
        reason: "The components are started.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0008I",
        define: "MSG_STOPPING_COMPS",
        text: "stopping components",
        reason: "Stopping the components.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0009I",
        define: "MSG_COMPS_STOPPED",
        text: "components stopped",
        reason: "The components are stopped.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0010I",
        define: "MSG_LAUNCHER_COMPS",
        text: "launcher has the following components:",
        reason: "The launcher has the following components.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0011I",
        define: "MSG_LAUNCHER_COMP",
        text: "name = %16.16s, PID = %d",
        reason: "Name = `<component-name>`, PID = `<process-id>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0012I",
        define: "MSG_START_CONSOLE",
        text: "starting console listener",
        reason: "Starting the console listener.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0013I",
        define: "MSG_CMD_RECV",
        text: "command '%s' received",
        reason: "The command `<command>` was received.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0014I",
        define: "MSG_TERM_CMD_RECV",
        text: "termination command received",
        reason: "The termination command was received.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0015I",
        define: "MSG_CONSOLE_STOPPED",
        text: "console listener stopped",
        reason: "The console listener was stopped.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0016I",
        define: "MSG_START_COMP_LIST",
        text: "start component list: '%s'",
        reason: "Start the component list `<component-list>`",
        action: NO_ACTION
    },
    {
        id: "ZWEL0017I",
        define: "MSG_ROOT_DIR",
        text: "ROOT_DIR is '%s'",
        reason: "The ROOT_DIR (`zowe.runtimeDirectory`) is `<zowe-runtime-directory>`",
        action: NO_ACTION
    },
    {
        id: "ZWEL0018I",
        define: "MSG_INST_PREPARED",
        text: "Zowe instance prepared successfully",
        reason: "Zowe instance prepared successfully.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0019I",
        define: "MSG_LAUNCHER_STOPPING",
        text: "Zowe Launcher stopping",
        reason: "Zowe Launcher is stopping.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0020I",
        define: "MSG_LOADING_YAML",
        text: "loading '%s'",
        reason: "Zowe Launcher is loading a `<file>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0021I",
        define: "MSG_LAUNCHER_START",
        text: "Zowe Launcher starting",
        reason: "Zowe Launcher is starting.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0022I",
        define: "MSG_LAUNCHER_STOPPED",
        text: "Zowe Launcher stopped",
        reason: "Zowe Launcher was stopped.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0023I",
        define: "MSG_YAML_FILE",
        text: "Zowe YAML config file is '%s'",
        reason: "Zowe YAML config file is `<path-to-zowe-yaml>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0024I",
        define: "MSG_HA_INST_ID",
        text: "HA_INSTANCE_ID is '%s'",
        reason: "The HA_INSTANCE_ID name is `<ha-instance-name>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0025I",
        define: "MSG_RESTART_INTRVL",
        text: "restart_intervals for component '%s'= %s",
        reason: `Restart intervals for component \`<component-name>\` = \`<restart-intervals>.\`
  Restart intervals is defined in \`zowe.launcher.restartIntervals\`.`,
        action: NO_ACTION
    },
    {
        id: "ZWEL0058I",
        define: "MSG_WKSP_DIR",
        text: "WORKSPACE_DIR is '%s'",
        reason: "The WORKSPACE_DIR (`zowe.workspaceDirectory`) is `<path-to-workspace-dir>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0069I",
        define: "MSG_CFG_VALID",
        text: "Configuration is valid",
        reason: "The configuration is valid.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0026E",
        define: "MSG_ENV_NOT_FOUND",
        text: "%s env variable not found",
        reason: "The environmental variable `<variable-name>` was not found.",
        action: "In launcher's STC, under `DD` statement `STDENV`, review the `<variable-name>`."
    },
    {
        id: "ZWEL0027E",
        define: "MSG_ENV_TOO_LARGE",
        text: "%s env variable too large",
        reason: "`<variable-name>` environmental variable is too large.",
        action: "In launcher's STC, under `DD` statement `STDENV`, review the `<variable-name>`."
    },
    {
        id: "ZWEL0028E",
        define: "MSG_COMP_LIST_ERR",
        text: "failed to get component list",
        reason: "Failed to get the component list.",
        action: "Review the components defined in the configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0029E",
        define: "MSG_COMP_LIST_EMPTY",
        text: "start component list is empty",
        reason: "Start component list is empty.",
        action: "Review the components defined in the configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0030E",
        define: "MSG_INST_PREP_ERR",
        text: "failed to prepare Zowe instance",
        reason: "Failed to prepare the Zowe instance.",
        action: "Check previous messages in the Zowe Launcher `SYSPRINT` to find the reason and correct it."
    },
    {
        id: "ZWEL0031E",
        define: "MSG_SIGNAL_ERR",
        text: "failed to setup signal handlers",
        reason: "Failed to setup signal handlers.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0032E",
        define: "MSG_DIR_ERR",
        text: "failed to find %s='%s', check if the dir exists",
        reason: "Failed to find `<dir-type>`=`<dir-path>`, check if the directory exists.",
        action: "Verify `<dir-type>` is correctly defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0033E",
        define: "MSG_ROOT_DIR_ERR",
        text: "failed to get ROOT_DIR dir",
        reason: "Failed to get ROOT_DIR (`zowe.runtimeDirectory`).",
        action: "Review `zowe.runtimeDirectory` defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0034E",
        define: "MSG_ROOT_DIR_EMPTY",
        text: "ROOT_DIR is empty string",
        reason: "ROOT_DIR (`zowe.runtimeDirectory`) is empty string.",
        action: "Review `zowe.runtimeDirectory` defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0035E",
        define: "MSG_CMDLINE_INVALID",
        text: "invalid command line arguments, provide HA_INSTANCE_ID as a first argument",
        reason: "Invalid command line arguments, provide `<HA_INSTANCE_ID>` as a first argument.",
        action: "Review the command and provide `<HA_INSTANCE_ID>` as a first argument."
    },
    {
        id: "ZWEL0036E",
        define: "MSG_CTX_INIT_FAILED",
        text: "failed to initialize launcher context",
        reason: "Failed to initialize launcher context.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0037E",
        define: "MSG_MAX_COMP_REACHED",
        text: "max component number reached, ignoring the rest",
        reason: "Maximal number of components reached, ignoring the rest.",
        action: "Review the components defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0038E",
        define: "MSG_MAX_RETRIES_REACHED",
        text: "failed to restart component %s, max retries reached",
        reason: "The component `<component-name>` terminates, and the start limit of the launcher has been reached,\n  thereby preventing component restart.",
        action: "Review the logs to determine the cause of component terminations.\n  When the problem has been corrected, restart the main Zowe task or\n  restart the component manually, to continue using the component.\n  For more information on restarting Zowe or individual components,\n  see [Starting and stopping Zowe](https://docs.zowe.org/stable/user-guide/start-zowe-zos)."
    },
    {
        id: "ZWEL0039E",
        define: "MSG_COMP_ALREADY_RUN",
        text: "cannot start component %s - already running",
        reason: "Cannot start the component `<component-name>` because it is already running.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0040E",
        define: "MSG_COMP_START_FAILED",
        text: "failed to start component %s",
        reason: "Failed to start the component `<component-name>`.",
        action: "Review the component defined in configuration (identified by message `ZWEL0023I`)."
    },
    {
        id: "ZWEL0041E",
        define: "MSG_BAD_CMD_VAL",
        text: "bad value supplied, command ignored",
        reason: "Bad value for the command supplied, such command is ignored.",
        action: "Review the command."
    },
    {
        id: "ZWEL0042E",
        define: "MSG_CMD_UNKNOWN",
        text: "command not recognized",
        reason: "Command not recognized.",
        action: "Review the command."
    },
    {
        id: "ZWEL0043E",
        define: "MSG_CONS_START_ERR",
        text: "failed to start console thread",
        reason: "Failed to start the console thread.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0044E",
        define: "MSG_CONS_STOP_ERR",
        text: "failed to stop console thread",
        reason: "Failed to stop the console thread.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0045E",
        define: "MSG_YAML_CONV_ERR",
        text: "error converting zowe.yaml file - %s",
        reason: "Error converting zowe.yaml file - `<path-to-zowe-yaml>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0046E",
        define: "MSG_YAML_READ_ERR",
        text: "error reading zowe.yaml file - %s",
        reason: "There is an error while reading zowe.yaml file - `<path-to-zowe.yaml>`",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0047E",
        define: "MSG_YAML_PARSE_ERR",
        text: "failed to parse zowe.yaml - %s",
        reason: "Failed to parse the zowe.yaml - `<path-to-zowe-yaml>`.",
        action: "Verify that the YAML has no syntax errors."
    },
    {
        id: "ZWEL0048E",
        define: "MSG_YAML_OPEN_ERR",
        text: "failed to open zowe.yaml - %s: %s",
        reason: "Failed to open zowe.yaml - `<path-to-zowe-yaml>`",
        action: "Verify if the YAML provided exists and the user running Zowe has permission to read it."
    },
    {
        id: "ZWEL0049E",
        define: "MSG_COMP_RESTART_FAILED",
        text: "failed to restart component %s:",
        reason: "Failed to restart the component `<component-name>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0050E",
        define: "MSG_COMP_OUTPUT_ERR",
        text: "cannot read output from comp %s(%d) - %s",
        reason: "Cannot read the output from component `<component-name>` (`<process-id>`) - `<error-text>`",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0055E",
        define: "MSG_STDIN_CREATE_ERROR",
        text: "failed to create file for stdin(%s) - %s",
        reason: "Failed to create the file for stdin(`<file>`) - `<error-text>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0056E",
        define: "MSG_STDIN_OPEN_ERROR",
        text: "failed to open file for stdin(%s) - %s",
        reason: "Failed to open the file for stdin(`<file>`) - `<error-text>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0057E",
        define: "MSG_WORKSPACE_ERROR",
        text: "failed to create workspace dir '%s'",
        reason: "Failed to create the workspace directory `<path-to-workspace-dir>`.",
        action: "Verify that the directory is valid and the Zowe user has permission to create it."
    },
    {
        id: "ZWEL0059E",
        define: "MSG_WKSP_DIR_ERR",
        text: "failed to get WORKSPACE_DIR dir",
        reason: "Failed to get the WORKSPACE_DIR (`zowe.workspaceDirectory`) directory.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0060E",
        define: "MSG_WKSP_DIR_EMPTY",
        text: "WORKSPACE_DIR is empty string",
        reason: "The WORKSPACE_DIR (`zowe.workspaceDirectory`) is empty string.",
        action: "Correct the Zowe YAML to define the `zowe.workspaceDirectory` value."
    },
    {
        id: "ZWEL0061E",
        define: "MSG_FILE_ERR",
        text: "failed to find %s='%s', check if the file exists",
        reason: "Failed to find ?=?, check if the file exists.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0062E",
        define: "MSG_MKDIR_ERR",
        text: "failed to create dir '%s' - %s",
        reason: "Failed to create the directory `<directory>` - `<error-text>`.",
        action: "Review the error text to determine the action to take."
    },
    {
        id: "ZWEL0064E",
        define: "MSG_CMD_RUN_ERR",
        text: "failed to run command %s - %s",
        reason: "Failed to run the command `<command>` - `<error-text>`.",
        action: "Review the error text to determine the action to take."
    },
    {
        id: "ZWEL0065E",
        define: "MSG_CMD_OUT_ERR",
        text: "error reading output from command '%s' - %s",
        reason: "There is an error reading the output from command `<command>` - `<error-text>`",
        action: "Review the error text to determine the action to take."
    },
    {
        id: "ZWEL0068E",
        define: "MSG_MEMBER_MISSING",
        text: "PARMLIB() entries must have a member name",
        reason: "PARMLIB() entries must have a member name.",
        action: "Review the dataset name contains the member name in PARMLIB entry."
    },
    {
        id: "ZWEL0070E",
        define: "MSG_CFG_INVALID",
        text: "Configuration has validity exceptions:",
        reason: "Configuration has validity exceptions.",
        action: "Review the exceptions and correct the configuration."
    },
    {
        id: "ZWEL0071E",
        define: "MSG_CFG_INTERNAL_FAIL",
        text: "Internal failure during validation, please contact support",
        reason: "Internal failure during validation, please contact support.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0072E",
        define: "MSG_CFG_LOAD_FAIL",
        text: "Launcher could not load configurations",
        reason: "Launcher could not load the configurations.",
        action: "Review the configuration entries."
    },
    {
        id: "ZWEL0073E",
        define: "MSG_CFG_SCHEMA_FAIL",
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
        define: "MSG_NO_LOG_CONTEXT",
        text: "Log context was not created",
        reason: "The logging context was not created.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0075E",
        define: "MSG_NO_LOG_FILE",
        text: "Log file for %s not created",
        reason: "The log file for `<component-name>` was not created.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0076E",
        define: "MSG_LOG_DIR_PERM",
        text: "Log directory unreadable, rc=0x%x, rsn=0x%x, location=%s",
        reason: "The log directory is unreadable. Return code=`<rc>`, reason code=`<rsn>`, location=`<location>`.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0051W",
        define: "MSG_USE_DEFAULTS",
        text: "failed to read zowe.yaml, launcher will use default settings",
        reason: "Failed to read zowe.yaml, launcher will use default settings.",
        action: CONTACT_SUPPORT
    },
    {
        id: "ZWEL0052W",
        define: "MSG_NOT_ALL_STARTED",
        text: "not all components started",
        reason: "Not all components were started.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0053W",
        define: "MSG_NOT_ALL_STOPPED",
        text: "not all components stopped gracefully",
        reason: "Not all components were stopped gracefully.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0054W",
        define: "MSG_COMP_NOT_FOUND",
        text: "component %s not found",
        reason: "The component `<component-name>` was not found.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0063W",
        define: "MSG_NOT_SIGTERM_STOPPED",
        text: "Component %s(%d) will be terminated using SIGKILL",
        reason: "Component `<component-name>`(`<process-id>`) will be terminated using `SIGKILL`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0066W",
        define: "MSG_CMD_RCP_WARN",
        text: "command '%s' ended with code %d",
        reason: "The command `<command>` ended with return code `<return-code>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0077W",
        define: "MSG_LOG_DIR_CLOSE",
        text: "Log directory close issue, rc=0%x, rsn=0x%x, location=%s",
        reason: "There is an issue closing the log directory. Return code=`<rc>`, reason code=`<rsn>`, location=`<location>`.",
        action: NO_ACTION
    },
    {
        id: "ZWEL0078E",
        define: "MSG_CFGMGR_INIT_FAILED",
        text: "Configuration manager could not be created",
        reason: "The configuration manager could not be created, so the launcher cannot read `zowe.yaml`. This happens when its embedded JavaScript engine cannot be set up, for example when the launcher is started from a privileged environment rather than problem state and key 8, or when memory is short.",
        action: "Check the messages that precede this one and the environment the launcher was started from, then restart."
    }
]
