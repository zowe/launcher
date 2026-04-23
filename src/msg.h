/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#ifndef MSG_H
#define MSG_H

#define MSG_PREFIX "ZWEL"

#define MSG_COMP_STARTED        MSG_PREFIX "0001I" " component %s started\n"
#define MSG_COMP_STOPPED        MSG_PREFIX "0002I" " component %s stopped\n"
#define MSG_COMP_INITED         MSG_PREFIX "0003I" " new component initialized %s, restart_cnt=%d, min_uptime=%d seconds, share_as=%s\n"
#define MSG_COMP_TERMINATED     MSG_PREFIX "0004I" " component %s(%d) terminated, status = %d\n"
#define MSG_NEXT_RESTART        MSG_PREFIX "0005I" " next attempt to restart component %s in %d seconds\n"
#define MSG_STARTING_COMPS      MSG_PREFIX "0006I" " starting components\n"
#define MSG_COMPS_STARTED       MSG_PREFIX "0007I" " components started\n"
#define MSG_STOPING_COMPS       MSG_PREFIX "0008I" " stopping components\n"
#define MSG_COMPS_STOPPED       MSG_PREFIX "0009I" " components stopped\n"
#define MSG_LAUNCHER_COMPS      MSG_PREFIX "0010I" " launcher has the following components:\n"
#define MSG_LAUNCHER_COMP       MSG_PREFIX "0011I" "     name = %16.16s, PID = %d\n"
#define MSG_START_CONSOLE       MSG_PREFIX "0012I" " starting console listener\n"
#define MSG_CMD_RECV            MSG_PREFIX "0013I" " command \'%s\' received\n"
#define MSG_TERM_CMD_RECV       MSG_PREFIX "0014I" " termination command received\n"
#define MSG_CONSOLE_STOPPED     MSG_PREFIX "0015I" " console listener stopped\n"
#define MSG_START_COMP_LIST     MSG_PREFIX "0016I" " start component list: '%s'\n"
#define MSG_ROOT_DIR            MSG_PREFIX "0017I" " ROOT_DIR is '%s'\n"
#define MSG_INST_PREPARED       MSG_PREFIX "0018I" " Zowe instance prepared successfully\n"
#define MSG_LAUNCHER_STOPING    MSG_PREFIX "0019I" " Zowe Launcher stopping\n"
#define MSG_LOADING_YAML        MSG_PREFIX "0020I" " loading '%s'\n"
#define MSG_LAUNCHER_START      MSG_PREFIX "0021I" " Zowe Launcher starting, Launcher version %s, Zowe version %s\n"
#define MSG_LAUNCHER_STOPPED    MSG_PREFIX "0022I" " Zowe Launcher stopped\n"
#define MSG_YAML_FILE           MSG_PREFIX "0023I" " Zowe YAML config file is \'%s\'\n"
#define MSG_HA_INST_ID          MSG_PREFIX "0024I" " HA_INSTANCE_ID is '%s'\n"
#define MSG_RESTART_INTRVL      MSG_PREFIX "0025I" " restart_intervals for component '%s'= %s\n"
#define MSG_ENV_NOT_FOUND       MSG_PREFIX "0026E" " %s env variable not found\n"
#define MSG_ENV_TOO_LARGE       MSG_PREFIX "0027E" " %s env variable too large\n"
#define MSG_COMP_LIST_ERR       MSG_PREFIX "0028E" " failed to get component list\n"
#define MSG_COMP_LIST_EMPTY     MSG_PREFIX "0029E" " start component list is empty\n"
#define MSG_INST_PREP_ERR       MSG_PREFIX "0030E" " failed to prepare Zowe instance\n"
#define MSG_SIGNAL_ERR          MSG_PREFIX "0031E" " failed to setup signal handlers\n"
#define MSG_DIR_ERR             MSG_PREFIX "0032E" " failed to find %s='%s', check if the dir exists\n"
#define MSG_ROOT_DIR_ERR        MSG_PREFIX "0033E" " failed to get ROOT_DIR dir\n"
#define MSG_ROOT_DIR_EMPTY      MSG_PREFIX "0034E" " ROOT_DIR is empty string\n"
#define MSG_CMDLINE_INVALID     MSG_PREFIX "0035E" " invalid command line arguments, provide HA_INSTANCE_ID as a first argument\n"
#define MSG_CTX_INIT_FAILED     MSG_PREFIX "0036E" " failed to initialize launcher context\n"
#define MSG_MAX_COMP_REACHED    MSG_PREFIX "0037E" " max component number reached, ignoring the rest\n"
#define MSG_MAX_RETRIES_REACHED MSG_PREFIX "0038E" " failed to restart component %s, max retries reached\n"
#define MSG_COMP_ALREADY_RUN    MSG_PREFIX "0039E" " cannot start component %s - already running\n"
#define MSG_COMP_START_FAILED   MSG_PREFIX "0040E" " failed to start component %s\n"
#define MSG_BAD_CMD_VAL         MSG_PREFIX "0041E" " bad value supplied, command ignored\n"
#define MSG_CMD_UNKNOWN         MSG_PREFIX "0042E" " command not recognized\n"
#define MSG_CONS_START_ERR      MSG_PREFIX "0043E" " failed to start console thread\n"
#define MSG_CONS_STOP_ERR       MSG_PREFIX "0044E" " failed to stop console thread\n"
#define MSG_YAML_CONV_ERR       MSG_PREFIX "0045E" " error converting zowe.yaml file - %s\n"
#define MSG_YAML_READ_ERR       MSG_PREFIX "0046E" " error reading zowe.yaml file - %s\n"
#define MSG_YAML_PARSE_ERR      MSG_PREFIX "0047E" " failed to parse zowe.yaml - %s\n"
#define MSG_YAML_OPEN_ERR       MSG_PREFIX "0048E" " failed to open zowe.yaml - %s: %s\n"
#define MSG_COMP_RESTART_FAILED MSG_PREFIX "0049E" " failed to restart component %s\n"
#define MSG_COMP_OUTPUT_ERR     MSG_PREFIX "0050E" " cannot read output from comp %s(%d) - %s\n"
#define MSG_USE_DEFAULTS        MSG_PREFIX "0051W" " failed to read zowe.yaml, launcher will use default settings\n"
#define MSG_NOT_ALL_STARTED     MSG_PREFIX "0052W" " not all components started\n"
#define MSG_NOT_ALL_STOPPED     MSG_PREFIX "0053W" " not all components stopped gracefully\n"
#define MSG_COMP_NOT_FOUND      MSG_PREFIX "0054W" " component %s not found\n"
#define MSG_STDIN_CREATE_ERROR  MSG_PREFIX "0055E" " failed to create file for stdin(%s) - %s\n"
#define MSG_STDIN_OPEN_ERROR    MSG_PREFIX "0056E" " failed to open file for stdin(%s) - %s\n"
#define MSG_WORKSPACE_ERROR     MSG_PREFIX "0057E" " failed to create workspace dir '%s'\n"
#define MSG_WKSP_DIR            MSG_PREFIX "0058I" " WORKSPACE_DIR is '%s'\n"
#define MSG_WKSP_DIR_ERR        MSG_PREFIX "0059E" " failed to get WORKSPACE_DIR dir\n"
#define MSG_WKSP_DIR_EMPTY      MSG_PREFIX "0060E" " WORKSPACE_DIR is empty string\n"
#define MSG_FILE_ERR            MSG_PREFIX "0061E" " failed to find %s='%s', check if the file exists\n"
#define MSG_MKDIR_ERR           MSG_PREFIX "0062E" " failed to create dir '%s' - %s\n"
#define MSG_NOT_SIGTERM_STOPPED MSG_PREFIX "0063W" " Component %s(%d) will be terminated using SIGKILL\n"
#define MSG_CMD_RUN_ERR         MSG_PREFIX "0064E" " failed to run command %s - %s\n"
#define MSG_CMD_OUT_ERR         MSG_PREFIX "0065E" " error reading output from command '%s' - %s\n"
#define MSG_CMD_RCP_WARN        MSG_PREFIX "0066W" " command '%s' ended with code %d\n"
// 67E retired, parmlib member name restriction removed
#define MSG_MEMBER_MISSING      MSG_PREFIX "0068E" " PARMLIB() entries must have a member name\n"
#define MSG_CFG_VALID           MSG_PREFIX "0069I" " Configuration is valid\n"
#define MSG_CFG_INVALID         MSG_PREFIX "0070E" " Configuration has validity exceptions:\n"
#define MSG_CFG_INTERNAL_FAIL   MSG_PREFIX "0071E" " Internal failure during validation, please contact support\n"
#define MSG_CFG_LOAD_FAIL       MSG_PREFIX "0072E" " Launcher Could not load configurations\n"
#define MSG_CFG_SCHEMA_FAIL     MSG_PREFIX "0073E" " Launcher Could not load schemas, status=%d\n"
#define MSG_NO_LOG_CONTEXT      MSG_PREFIX "0074E" " Log context was not created\n"
#define MSG_MANIFEST_READ_WARN  MSG_PREFIX "0075W" " Could not read manifest.json version: %s\n"
#define MSG_LINE_LENGTH         "-- If you cant see '500' at the end of the line, your log is too short to read!80--------90------ 100----------------------125----------------------150----------------------175----------------------200----------------------225----------------------250----------------------275----------------------300----------------------325----------------------350----------------------375----------------------400----------------------425----------------------450----------------------475----------------------500\n"
#define MSG_NO_LOG_FILE         MSG_PREFIX "0075E" " Log file for %s not created\n"
#define MSG_LOG_DIR_PERM        MSG_PREFIX "0076E" " Log directory unreadable, rc=0x%x, rsn=0x%x, location=%s\n"
#define MSG_LOG_DIR_CLOSE       MSG_PREFIX "0077W" " Log directory close issue, rc=0%x, rsn=0x%x, location=%s\n"

#endif // MSG_H

//-----------------------------------------------------------------------------
// Markdown help for Launcher Error Codes
// https://github.com/zowe/docs-site/blob/master/docs/troubleshoot/launcher/launcher-error-codes.md?plain=1
//
// Line starting //@ is MD.
// When need to create new doc file, use for example:
//   cat msg.h | grep -e '^//@' | cut -c4- > launcher-error-codes.md
// The doc is structured:
//   Zowe Launcher informational messages
//   Zowe Launcher error messages
//   Zowe Launcher warning messages
//-----------------------------------------------------------------------------

//@# Error Message Codes
//@
//@The following error message codes may appear on Zowe Launcher SYSPRINT. Use the following message code references and the corresponding reasons and actions to help troubleshoot issues.
//@
//@## Zowe Launcher informational messages
//@
// #define MSG_COMP_STARTED        MSG_PREFIX "0001I" " component %s started\n"
//@### ZWEL0001I
//@
//@  component %s started
//@
//@  **Reason:**
//@
//@  The component `<component-name>` was started.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_COMP_STOPPED        MSG_PREFIX "0002I" " component %s stopped\n"
//@### ZWEL0002I
//@
//@  component %s stopped
//@
//@  **Reason:**
//@
//@  The component `<component-name>` was stopped.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_COMP_INITED         MSG_PREFIX "0003I" " new component initialized %s, restart_cnt=%d, min_uptime=%d seconds, share_as=%s\n"
//@### ZWEL0003I
//@
//@  new component initialized %s, restart_cnt=%d, min_uptime=%d seconds, share_as=%s
//@
//@  **Reason:**
//@
//@  The component `<component-name>` was initialized.
//@  - `restart_cnt` - Number of attempts to restart the component in case of failure
//@  - `min_uptime` - Minimum uptime that the component can be considered as successfully started
//@  - `share_as` - One of `<yes|no|must>` which indicates whether child processes of the component start in the same address space. See documentation for [_BPX_SHAREAS](https://www.ibm.com/docs/en/zos/2.4.0?topic=shell-setting-bpx-shareas-bpx-spawn-script) for details.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_COMP_TERMINATED     MSG_PREFIX "0004I" " component %s(%d) terminated, status = %d\n"
//@### ZWEL0004I
//@
//@  component %s(%d) terminated, status = %d
//@
//@  **Reason:**
//@
//@  The component `<component-name>`(`<process-id>`) terminated with the status `<code>`.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_NEXT_RESTART        MSG_PREFIX "0005I" " next attempt to restart component %s in %d seconds\n"
//@### ZWEL0005I
//@
//@  next attempt to restart component %s in %d seconds
//@
//@  **Reason:**
//@
//@  Next attempt to restart component `<component-name>` in `<n>` seconds.
//@
//@  **Action:**
//@
//@  No action required. The component `<component-name>` will be restarted in `<n>` seconds.
//@
// #define MSG_STARTING_COMPS      MSG_PREFIX "0006I" " starting components\n"
//@### ZWEL0006I
//@
//@  starting components
//@
//@  **Reason:**
//@
//@  Starting the components.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_COMPS_STARTED       MSG_PREFIX "0007I" " components started\n"
//@### ZWEL0007I
//@
//@  components started
//@
//@  **Reason:**
//@
//@  The components are started.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_STOPING_COMPS       MSG_PREFIX "0008I" " stopping components\n"
//@### ZWEL0008I
//@
//@  stopping components
//@
//@  **Reason:**
//@
//@  Stopping the components.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_COMPS_STOPPED       MSG_PREFIX "0009I" " components stopped\n"
//@### ZWEL0009I
//@
//@  components stopped
//@
//@  **Reason:**
//@
//@  The components are stopped.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_LAUNCHER_COMPS      MSG_PREFIX "0010I" " launcher has the following components:\n"
//@### ZWEL0010I
//@
//@  launcher has the following components:
//@
//@  **Reason:**
//@
//@  The launcher has the following components.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_LAUNCHER_COMP       MSG_PREFIX "0011I" " name = %16.16s, PID = %d\n"
//@### ZWEL0011I
//@
//@  name = %16.16s, PID = %d
//@
//@  **Reason:**
//@
//@  Name = `<component-name>`, PID = `<process-id>`.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_START_CONSOLE       MSG_PREFIX "0012I" " starting console listener\n"
//@### ZWEL0012I
//@
//@  starting console listener
//@
//@  **Reason:**
//@
//@  Starting the console listener.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_CMD_RECV            MSG_PREFIX "0013I" " command \'%s\' received\n"
//@### ZWEL0013I
//@
//@  command \'%s\' received
//@
//@  **Reason:**
//@
//@  The command `<command>` was received.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_TERM_CMD_RECV       MSG_PREFIX "0014I" " termination command received\n"
//@### ZWEL0014I
//@
//@  termination command received
//@
//@  **Reason:**
//@
//@  The termination command was received.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_CONSOLE_STOPPED     MSG_PREFIX "0015I" " console listener stopped\n"
//@### ZWEL0015I
//@
//@  console listener stopped
//@
//@  **Reason:**
//@
//@  The console listener was stopped.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_START_COMP_LIST     MSG_PREFIX "0016I" " start component list: '%s'\n"
//@### ZWEL0016I
//@
//@  start component list: '%s'
//@
//@  **Reason:**
//@
//@  Start the component list `<component-list>`
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_ROOT_DIR            MSG_PREFIX "0017I" " ROOT_DIR is '%s'\n"
//@### ZWEL0017I
//@
//@  ROOT_DIR is '%s'
//@
//@  **Reason:**
//@
//@  The ROOT_DIR (`zowe.runtimeDirectory`) is `<zowe-runtime-directory>`
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_INST_PREPARED       MSG_PREFIX "0018I" " Zowe instance prepared successfully\n"
//@### ZWEL0018I
//@
//@  Zowe instance prepared successfully
//@
//@  **Reason:**
//@
//@  Zowe instance prepared successfully.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_LAUNCHER_STOPING    MSG_PREFIX "0019I" " Zowe Launcher stopping\n"
//@### ZWEL0019I
//@
//@  Zowe Launcher stopping
//@
//@  **Reason:**
//@
//@  Zowe Launcher is stopping.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_LAUNCHER_START      MSG_PREFIX "0021I" " Zowe Launcher starting, Launcher version %s, Zowe version %s\n"
//@### ZWEL0021I
//@
//@  Zowe Launcher starting, Launcher version %s, Zowe version %s
//@
//@  **Reason:**
//@
//@  Zowe Launcher is starting.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_LAUNCHER_STOPPED    MSG_PREFIX "0022I" " Zowe Launcher stopped\n"
//@### ZWEL0022I
//@
//@  Zowe Launcher stopped
//@
//@  **Reason:**
//@
//@  Zowe Launcher was stopped.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_YAML_FILE           MSG_PREFIX "0023I" " Zowe YAML config file is \'%s\'\n"
//@### ZWEL0023I
//@
//@  Zowe YAML config file is \'%s\'
//@
//@  **Reason:**
//@
//@  Zowe YAML config file is `<path-to-zowe-yaml>`.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_HA_INST_ID          MSG_PREFIX "0024I" " HA_INSTANCE_ID is '%s'\n"
//@### ZWEL0024I
//@
//@  HA_INSTANCE_ID is '%s'
//@
//@  **Reason:**
//@
//@  The HA_INSTANCE_ID name is `<ha-instance-name>`.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_RESTART_INTRVL      MSG_PREFIX "0025I" " restart_intervals for component '%s'= %s\n"
//@### ZWEL0025I
//@
//@  restart_intervals for component '%s'= %s
//@
//@  **Reason:**
//@
//@  Restart intervals for component `<component-name>` = `<restart-intervals>.`
//@  Restart intervals is defined in `zowe.launcher.restartIntervals`.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_WKSP_DIR            MSG_PREFIX "0058I" " WORKSPACE_DIR is '%s'\n"
//@### ZWEL0058I
//@
//@  WORKSPACE_DIR is '%s'
//@
//@  **Reason:**
//@
//@  The WORKSPACE_DIR (`zowe.workspaceDirectory`) is `<path-to-workspace-dir>`.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_CFG_VALID           MSG_PREFIX "0069I" " Configuration is valid\n"
//@### ZWEL0069I
//@
//@  Configuration is valid
//@
//@  **Reason:**
//@
//@  The configuration is valid.
//@
//@  **Action:**
//@
//@  No action required.
//@
//@
//@## Zowe Launcher error messages
//@
// #define MSG_ENV_NOT_FOUND       MSG_PREFIX "0026E" " %s env variable not found\n"
//@### ZWEL0026E
//@
//@  %s env variable not found
//@
//@  **Reason:**
//@
//@  The environmental variable `<variable-name>` was not found.
//@
//@  **Action:**
//@
//@  In launcher's STC, under `DD` statement `STDENV`, review the `<variable-name>`.
//@
// #define MSG_ENV_TOO_LARGE       MSG_PREFIX "0027E" " %s env variable too large\n"
//@### ZWEL0027E
//@
//@  %s env variable too large
//@
//@  **Reason:**
//@
//@  `<variable-name>` environmental variable is too large.
//@
//@  **Action:**
//@
//@  In launcher's STC, under `DD` statement `STDENV`, review the `<variable-name>`.
//@
// #define MSG_COMP_LIST_ERR       MSG_PREFIX "0028E" " failed to get component list\n"
//@### ZWEL0028E
//@
//@  failed to get component list
//@
//@  **Reason:**
//@
//@  Failed to get the component list.
//@
//@  **Action:**
//@
//@  Review the components defined in the configuration (identified by message `ZWEL0023I`).
//@
// #define MSG_COMP_LIST_EMPTY     MSG_PREFIX "0029E" " start component list is empty\n"
//@### ZWEL0029E
//@
//@  start component list is empty
//@
//@  **Reason:**
//@
//@  Start component list is empty.
//@
//@  **Action:**
//@
//@  Review the components defined in the configuration (identified by message `ZWEL0023I`).
//@
// #define MSG_INST_PREP_ERR       MSG_PREFIX "0030E" " failed to prepare Zowe instance\n"
//@### ZWEL0030E
//@
//@  failed to prepare Zowe instance
//@
//@  **Reason:**
//@
//@  Failed to prepare the zowe instance.
//@
//@  **Action:**
//@
//@  Check previous messages in the Zowe Launcher `SYSPRINT` to find the reason and correct it.
//@
// #define MSG_SIGNAL_ERR          MSG_PREFIX "0031E" " failed to setup signal handlers\n"
//@### ZWEL0031E
//@
//@  failed to setup signal handlers
//@
//@  **Reason:**
//@
//@  Failed to setup signal handlers.
//@
//@  **Action:**
//@
//@  Contact Support.
//@
// #define MSG_DIR_ERR             MSG_PREFIX "0032E" " failed to find %s='%s', check if the dir exists\n"
//@### ZWEL0032E
//@
//@  failed to find %s='%s', check if the dir exists
//@
//@  **Reason:**
//@
//@  Failed to find `<dir-type>`=`<dir-path>`, check if the directory exists.
//@
//@  **Action:**
//@
//@  Verify `<dir-type>` is correctly defined in configuration (identified by message `ZWEL0023I`).
//@
// #define MSG_ROOT_DIR_ERR        MSG_PREFIX "0033E" " failed to get ROOT_DIR dir\n"
//@### ZWEL0033E
//@
//@  failed to get ROOT_DIR dir
//@
//@  **Reason:**
//@
//@  Failed to get ROOT_DIR (`zowe.runtimeDirectory`).
//@
//@  **Action:**
//@
//@  Review `zowe.runtimeDirectory` defined in configuration (identified by message `ZWEL0023I`).
//@
// #define MSG_ROOT_DIR_EMPTY      MSG_PREFIX "0034E" " ROOT_DIR is empty string\n"
//@### ZWEL0034E
//@
//@  ROOT_DIR is empty string
//@
//@  **Reason:**
//@
//@  ROOT_DIR (`zowe.runtimeDirectory`) is empty string.
//@
//@  **Action:**
//@
//@  Review `zowe.runtimeDirectory` defined in configuration (identified by message `ZWEL0023I`).
//@
// #define MSG_CMDLINE_INVALID     MSG_PREFIX "0035E" " invalid command line arguments, provide HA_INSTANCE_ID as a first argument\n"
//@### ZWEL0035E
//@
//@  invalid command line arguments, provide HA_INSTANCE_ID as a first argument
//@
//@  **Reason:**
//@
//@  Invalid command line arguments, provide `<HA_INSTANCE_ID>` as a first argument.
//@
//@  **Action:**
//@
//@  Review the command and provide `<HA_INSTANCE_ID>` as a first argument.
//@
// #define MSG_CTX_INIT_FAILED     MSG_PREFIX "0036E" " failed to initialize launcher context\n"
//@### ZWEL0036E
//@
//@  failed to initialize launcher context
//@
//@  **Reason:**
//@
//@  Failed to initialize launcher context.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_MAX_COMP_REACHED    MSG_PREFIX "0037E" " max component number reached, ignoring the rest\n"
//@### ZWEL0037E
//@
//@  max component number reached, ignoring the rest
//@
//@  **Reason:**
//@
//@  Maximal number of components reached, ignoring the rest.
//@
//@  **Action:**
//@
//@  Review the components defined in configuration (identified by message `ZWEL0023I`).
//@
// #define MSG_MAX_RETRIES_REACHED MSG_PREFIX "0038E" " failed to restart component %s, max retries reached\n"
//@### ZWEL0038E
//@
//@  failed to restart component %s, max retries reached
//@
//@  **Reason:**
//@
//@  The component `<component-name>` has been terminating and being restarted by the launcher,
//@  but the maximum restart limit has been reached and so the component remains stopped.
//@
//@  **Action:**
//@
//@  Review the logs to determine the cause of the component terminations.
//@  When the problem has been corrected, restart the main Zowe task or 
//@  restart the component manually, to continue using the component.
//@  Information on restarting Zowe or individual components can be found at
//@  https://docs.zowe.org/stable/user-guide/start-zowe-zos
//@
// #define MSG_COMP_ALREADY_RUN    MSG_PREFIX "0039E" " cannot start component %s - already running\n"
//@### ZWEL0039E
//@
//@  cannot start component %s - already running
//@
//@  **Reason:**
//@
//@  Cannot start the component `<component-name>` because it is already running.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_COMP_START_FAILED   MSG_PREFIX "0040E" " failed to start component %s\n"
//@### ZWEL0040E
//@
//@  failed to start component %s
//@
//@  **Reason:**
//@
//@  Failed to start the component `<component-name>`.
//@
//@  **Action:**
//@
//@  Review the component defined in configuration (identified by message `ZWEL0023I`).
//@
// #define MSG_BAD_CMD_VAL         MSG_PREFIX "0041E" " bad value supplied, command ignored\n"
//@### ZWEL0041E
//@
//@  bad value supplied, command ignored
//@
//@  **Reason:**
//@
//@  Bad value for the command supplied, such command is ignored.
//@
//@  **Action:**
//@
//@  Review the command.
//@
// #define MSG_CMD_UNKNOWN         MSG_PREFIX "0042E" " command not recognized\n"
//@### ZWEL0042E
//@
//@  command not recognized
//@
//@  **Reason:**
//@
//@  Command not recognized.
//@
//@  **Action:**
//@
//@  Review the command.
//@
// #define MSG_CONS_START_ERR      MSG_PREFIX "0043E" " failed to start console thread\n"
//@### ZWEL0043E
//@
//@  failed to start console thread
//@
//@  **Reason:**
//@
//@  Failed to start the console thread.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_CONS_STOP_ERR       MSG_PREFIX "0044E" " failed to stop console thread\n"
//@### ZWEL0044E
//@
//@  failed to stop console thread
//@
//@  **Reason:**
//@
//@  Failed to stop the console thread.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_YAML_CONV_ERR       MSG_PREFIX "0045E" " error converting zowe.yaml file - %s\n"
//@### ZWEL0045E
//@
//@  error converting zowe.yaml file - %s
//@
//@  **Reason:**
//@
//@  Error converting zowe.yaml file - `<path-to-zowe-yaml>`.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_YAML_READ_ERR       MSG_PREFIX "0046E" " error reading zowe.yaml file - %s\n"
//@### ZWEL0046E
//@
//@  error reading zowe.yaml file - %s
//@
//@  **Reason:**
//@
//@  There is an error while reading zowe.yaml file - `<path-to-zowe.yaml>`
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_YAML_PARSE_ERR      MSG_PREFIX "0047E" " failed to parse zowe.yaml - %s\n"
//@### ZWEL0047E
//@
//@  failed to parse zowe.yaml - %s
//@
//@  **Reason:**
//@
//@  Failed to parse the zowe.yaml - `<path-to-zowe-yaml>`.
//@
//@  **Action:**
//@
//@  Verify that the YAML has no syntax errors.
//@
// #define MSG_YAML_OPEN_ERR       MSG_PREFIX "0048E" " failed to open zowe.yaml - %s: %s\n"
//@### ZWEL0048E
//@
//@  failed to open zowe.yaml - %s: %s
//@
//@  **Reason:**
//@
//@  Failed to open zowe.yaml - `<path-to-zowe-yaml>`:?
//@
//@  **Action:**
//@
//@  Verify if the YAML provided exists and the user running Zowe has permission to read it.
//@
// #define MSG_COMP_RESTART_FAILED MSG_PREFIX "0049E" " failed to restart component %s\n"
//@### ZWEL0049E
//@
//@  failed to restart component %s
//@
//@  **Reason:**
//@
//@  Failed to restart the component `<component-name>`.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_COMP_OUTPUT_ERR     MSG_PREFIX "0050E" " cannot read output from comp %s(%d) - %s\n"
//@### ZWEL0050E
//@
//@  cannot read output from comp %s(%d) - %s
//@
//@  **Reason:**
//@
//@  Cannot read the output from component `<component-name>` (`<process-id>`) - `<error-text>`
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_STDIN_CREATE_ERROR  MSG_PREFIX "0055E" " failed to create file for stdin(%s) - %s\n"
//@### ZWEL0055E
//@
//@  failed to create file for stdin(%s) - %s
//@
//@  **Reason:**
//@
//@  Failed to create the file for stdin(`<file>`) - `<error-text>`.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_STDIN_OPEN_ERROR    MSG_PREFIX "0056E" " failed to open file for stdin(%s) - %s\n"
//@### ZWEL0056E
//@
//@  failed to open file for stdin(%s) - %s
//@
//@  **Reason:**
//@
//@  Failed to open the file for stdin(`<file>`) - `<error-text>`.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_WORKSPACE_ERROR     MSG_PREFIX "0057E" " failed to create workspace dir '%s'\n"
//@### ZWEL0057E
//@
//@  failed to create workspace dir '%s'
//@
//@  **Reason:**
//@
//@  Failed to create the workspace directory `<path-to-workspace-dir>`.
//@
//@  **Action:**
//@
//@  Verify that the directory is valid and the Zowe user has permission to create it.
//@
// #define MSG_WKSP_DIR_ERR        MSG_PREFIX "0059E" " failed to get WORKSPACE_DIR dir\n"
//@### ZWEL0059E
//@
//@  failed to get WORKSPACE_DIR dir
//@
//@  **Reason:**
//@
//@  Failed to get the WORKSPACE_DIR (`zowe.workspaceDirectory`) directory.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_WKSP_DIR_EMPTY      MSG_PREFIX "0060E" " WORKSPACE_DIR is empty string\n"
//@### ZWEL0060E
//@
//@  WORKSPACE_DIR is empty string
//@
//@  **Reason:**
//@
//@  The WORKSPACE_DIR (`zowe.workspaceDirectory`) is empty string.
//@
//@  **Action:**
//@
//@  Correct the Zowe YAML to define the `zowe.workspaceDirectory` value.
//@
// #define MSG_FILE_ERR            MSG_PREFIX "0061E" " failed to find %s='%s', check if the file exists\n"
//@### ZWEL0061E
//@
//@  failed to find %s='%s', check if the file exists
//@
//@  **Reason:**
//@
//@  Failed to find ?=?, check if the file exists.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_MKDIR_ERR           MSG_PREFIX "0062E" " failed to create dir '%s' - %s\n"
//@### ZWEL0062E
//@
//@  failed to create dir '%s' - %s
//@
//@  **Reason:**
//@
//@  Failed to create the directory `<directory>` - `<error-text>`.
//@
//@  **Action:**
//@
//@  Review the error text to determine the action to take.
//@
// #define MSG_CMD_RUN_ERR         MSG_PREFIX "0064E" " failed to run command %s - %s\n"
//@### ZWEL0064E
//@
//@  failed to run command %s - %s
//@
//@  **Reason:**
//@
//@  Failed to run the command `<command>` - `<error-text>`.
//@
//@  **Action:**
//@
//@  Review the error text to determine the action to take.
//@
// #define MSG_CMD_OUT_ERR         MSG_PREFIX "0065E" " error reading output from command '%s' - %s\n"
//@### ZWEL0065E
//@
//@  error reading output from command '%s' - %s
//@
//@  **Reason:**
//@
//@  There is an error reading the output from command `<command>` - `<error-text>`
//@
//@  **Action:**
//@
//@  Review the error text to determine the action to take.
//@
// #define MSG_MEMBER_MISSING      MSG_PREFIX "0068E" " PARMLIB() entries must have a member name\n"
//@### ZWEL0068E
//@
//@  PARMLIB() entries must have a member name
//@
//@  **Reason:**
//@
//@  PARMLIB() entries must have a member name.
//@
//@  **Action:**
//@
//@  Review the dataset name contains the member name in PARMLIB entry.
//@
// #define MSG_CFG_INVALID         MSG_PREFIX "0070E" " Configuration has validity exceptions:\n"
//@### ZWEL0070E
//@
//@  Configuration has validity exceptions:
//@
//@  **Reason:**
//@
//@  Configuration has validity exceptions.
//@
//@  **Action:**
//@
//@  Review the exceptions and correct the configuration.
//@
// #define MSG_CFG_INTERNAL_FAIL   MSG_PREFIX "0071E" " Internal failure during validation, please contact support\n"
//@### ZWEL0071E
//@
//@  Internal failure during validation, please contact support
//@
//@  **Reason:**
//@
//@  Internal failure during validation, please contact support.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_CFG_LOAD_FAIL       MSG_PREFIX "0072E" " Launcher could not load configurations\n"
//@### ZWEL0072E
//@
//@  Launcher could not load configurations
//@
//@  **Reason:**
//@
//@  Launcher could not load the configurations.
//@
//@  **Action:**
//@
//@  Review the configuration entries.
//@
// #define MSG_CFG_SCHEMA_FAIL     MSG_PREFIX "0073E" " Launcher could not load schemas, status=%d\n"
//@### ZWEL0073E
//@
//@  Launcher could not load schemas, status=%d
//@
//@  **Reason:**
//@
//@  Launcher could not load schemas, status=`<return-code>`
//@
//@  **Action:**
//@
//@  For the `status=5`, locate the `zowe.runtimeDirectory` in the configuration.
//@
//@  Check the `zowe.runtimeDirectory/schemas` contains four `.json` files shown below:
//@
//@  ```
//@  manifest-schema.json
//@  server-common-json
//@  trivial-component-schema.json
//@  zowe-yaml-schema.json
//@  ```
//@
//@  On occasion the error occurs because the `zowe.runtimeDirectory` is pointing to a valid directory, but one which doesn't contain a valid Zowe runtime environment is one of the first failures during a Zowe launch.
//@
// #define MSG_NO_LOG_CONTEXT      MSG_PREFIX "0074E" " Log context was not created\n"
//@### ZWEL0074E
//@
//@  Log context was not created
//@
//@  **Reason:**
//@
//@  The logging context was not created.
//@
//@  **Action:**
//@
//@  Contact support.
//@
//@## Zowe Launcher warning messages
//@
// #define MSG_USE_DEFAULTS        MSG_PREFIX "0051W" " failed to read zowe.yaml, launcher will use default settings\n"
//@### ZWEL0051W
//@
//@  failed to read zowe.yaml, launcher will use default settings
//@
//@  **Reason:**
//@
//@  Failed to read zowe.yaml, launcher will use default settings.
//@
//@  **Action:**
//@
//@  Contact support.
//@
// #define MSG_NOT_ALL_STARTED     MSG_PREFIX "0052W" " not all components started\n"
//@### ZWEL0052W
//@
//@  not all components started
//@
//@  **Reason:**
//@
//@  Not all components were started.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_NOT_ALL_STOPPED     MSG_PREFIX "0053W" " not all components stopped gracefully\n"
//@### ZWEL0053W
//@
//@  not all components stopped gracefully
//@
//@  **Reason:**
//@
//@  Not all components were stopped gracefully.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_COMP_NOT_FOUND      MSG_PREFIX "0054W" " component %s not found\n"
//@### ZWEL0054W
//@
//@  component %s not found
//@
//@  **Reason:**
//@
//@  The component `<component-name>` was not found.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_NOT_SIGTERM_STOPPED MSG_PREFIX "0063W" " Component %s(%d) will be terminated using SIGKILL\n"
//@### ZWEL0063W
//@
//@  Component %s(%d) will be terminated using SIGKILL
//@
//@  **Reason:**
//@
//@  Component `<component-name>`(`<process-id>`) will be terminated using `SIGKILL`.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_CMD_RCP_WARN        MSG_PREFIX "0066W" " command '%s' ended with code %d\n"
//@### ZWEL0066W
//@
//@  command '%s' ended with code %d
//@
//@  **Reason:**
//@
//@  The command `<command>` ended with return code `<return-code>`.
//@
//@  **Action:**
//@
//@  No action required.
//@
// #define MSG_CMD_RCP_WARN        MSG_PREFIX "0075W" " Could not read manifest.json version: %s\n"
//@### ZWEL0075W
//@
//@  Could not read manifest.json version: %s
//@
//@  **Reason:**
//@
//@  The manifest.json file within the Zowe runtime was unreadable for the reason stated in `<reason>`.
//@  The version will be printed as 'unknown'.
//@
//@  **Action:**
//@
//@  Verify that the runtime directory is unaltered, has necessary read permissions for the Zowe STC
//@  And has not become corrupt in some way.
//@



/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
