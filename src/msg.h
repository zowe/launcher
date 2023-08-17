
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

// @DocStart, @DocEnd - start/end for documentation reader
// @Action with no text => "No action required."
// @Reason with no text = forbidden!
// @! Comment, line is ignored by documentaion reader
// Other lines are copied as it is, use markdown
// Documentation guide lines:
//  - messages are added in ascending order for each section
//  - sections are informational, error and warning messages
//@DocStart
//@!-------------------------------------------------------------------
//
//## Zowe Launcher informational messages
//
//@!-------------------------------------------------------------------
#define MSG_COMP_STARTED        MSG_PREFIX "0001I" " component %s started\n"
//@Reason: The component `<component-name>` was started.
//@Action:
#define MSG_COMP_STOPPED        MSG_PREFIX "0002I" " component %s stopped\n"
//@Reason: The component `<component-name>` was stopped.
//@Action:
#define MSG_COMP_INITED         MSG_PREFIX "0003I" " new component initialized %s, restart_cnt=%d, min_uptime=%d seconds, share_as=%s\n"
//@Reason: The component `<component-name>` was initialized.
//  - `restart_cnt` - Number of attempts to restart the component in case of failure
//  - `min_uptime` - Minimum uptime that the component can be considered as successfully started
//  - `share_as` - One of `<yes|no|must>` which indicates whether child processes of the component start in the same address space. See documentation for [_BPX_SHAREAS](https://www.ibm.com/docs/en/zos/2.4.0?topic=shell-setting-bpx-shareas-bpx-spawn-script) for details.
//@Action:
#define MSG_COMP_TERMINATED     MSG_PREFIX "0004I" " component %s(%d) terminated, status = %d\n"
//@Reason: The component `<component-name>`(`<process-id>`) terminated with the status `<code>`.
//@Action:
#define MSG_NEXT_RESTART        MSG_PREFIX "0005I" " next attempt to restart component %s in %d seconds\n"
//@Reason: Next attempt to restart component `<component-name>` in `<n>` seconds."
//@Action: No action required. The component `<component-name>` will be restarted in `<n>` seconds.
#define MSG_STARTING_COMPS      MSG_PREFIX "0006I" " starting components\n"
//@Reason: Starting the components.
//@Action:
#define MSG_COMPS_STARTED       MSG_PREFIX "0007I" " components started\n"
//@Reason: The components are started.
//@Action:
#define MSG_STOPING_COMPS       MSG_PREFIX "0008I" " stopping components\n"
//@Reason: Stopping the components.
//@Action:
#define MSG_COMPS_STOPPED       MSG_PREFIX "0009I" " components stopped\n"
//@Reason: The components are stopped.
//@Action:
#define MSG_LAUNCHER_COMPS      MSG_PREFIX "0010I" " launcher has the following components:\n"
//@Reason: The launcher has the following components.
//@Action:
#define MSG_LAUNCHER_COMP       MSG_PREFIX "0011I" " name = %16.16s, PID = %d\n"
//@Reason: Name = `<component-name>`, PID = `<process-id>`.
//@Action:
#define MSG_START_CONSOLE       MSG_PREFIX "0012I" " starting console listener\n"
//@Reason: Starting the console listener.
//@Action:
#define MSG_CMD_RECV            MSG_PREFIX "0013I" " command \'%s\' received\n"
//@Reason: The command `<commnad>` was received.
//@Action:
#define MSG_TERM_CMD_RECV       MSG_PREFIX "0014I" " termination command received\n"
//@Reason: The termination command was received.
//@Action:
#define MSG_CONSOLE_STOPPED     MSG_PREFIX "0015I" " console listener stopped\n"
//@Reason: The console listener was stopped.
//@Action:
#define MSG_START_COMP_LIST     MSG_PREFIX "0016I" " start component list: '%s'\n"
//@Reason: Start the component list `<component-list>`
//@Action:
#define MSG_ROOT_DIR            MSG_PREFIX "0017I" " ROOT_DIR is '%s'\n"
//@Reason: The ROOT_DIR (`zowe.runtimeDirectory`) is `<zowe-runtime-directory>`
//@Action:
#define MSG_INST_PREPARED       MSG_PREFIX "0018I" " Zowe instance prepared successfully\n"
//@Reason: Zowe instance prepared successfully.
//@Action:
#define MSG_LAUNCHER_STOPING    MSG_PREFIX "0019I" " Zowe Launcher stopping\n"
//@Reason: Zowe Launcher is stopping.
//@Action:
#define MSG_LOADING_YAML        MSG_PREFIX "0020I" " loading '%s'\n"    //@!
//@!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//@!Reason: Loading `<yaml-file>`
//@!Action:
#define MSG_LAUNCHER_START      MSG_PREFIX "0021I" " Zowe Launcher starting\n"
//@Reason: Zowe Launcher is starting.
//@Action:
#define MSG_LAUNCHER_STOPPED    MSG_PREFIX "0022I" " Zowe Launcher stopped\n"
//@Reason: Zowe Launcher was stopped.
//@Action:
#define MSG_YAML_FILE           MSG_PREFIX "0023I" " Zowe YAML config file is \'%s\'\n"
//@Reason: Zowe YAML config file is `<path-to-zowe-yaml>`.
//@Action:
#define MSG_HA_INST_ID          MSG_PREFIX "0024I" " HA_INSTANCE_ID is '%s'\n"
//@Reason: The HA_INSTANCE_ID name is `<ha-instance-name>`.
//@Action:
#define MSG_RESTART_INTRVL      MSG_PREFIX "0025I" " restart_intervals for component '%s'= %s\n"
//@Reason:  Restart intervals for component `<component-name>` = `<restart-intervals>.`
//Restart intervals is defined in `zowe.launcher.restartIntervals`.
//@Action:
#define MSG_WKSP_DIR            MSG_PREFIX "0058I" " WORKSPACE_DIR is '%s'\n"
//@Reason: The WORKSPACE_DIR (`zowe.workspaceDirectory`) is `<path-to-workspace-dir>`.
//@Action:
#define MSG_CFG_VALID           MSG_PREFIX "0069I" " Configuration is valid\n"
//@Reason: The configuration is valid.
//@Action:
//@!-------------------------------------------------------------------
//
//## Zowe Launcher error messages
//
//@!-------------------------------------------------------------------
#define MSG_ENV_NOT_FOUND       MSG_PREFIX "0026E" " %s env variable not found\n"
//@Reason: The environmental variable `<variable-name>` was not found.
//@Action: In launcher's STC, under `DD` statement `STDENV`, review the `<variable-name>`.
#define MSG_ENV_TOO_LARGE       MSG_PREFIX "0027E" " %s env variable too large\n"
//@Reason: `<variable-name>` environmental variable is too large.
//@Action: In launcher's STC, under `DD` statement `STDENV`, review the `<variable-name>`.
#define MSG_COMP_LIST_ERR       MSG_PREFIX "0028E" " failed to get component list\n"
//@Reason: Failed to get the component list.
//@Action: Review the components defined in configuration (identified by message `ZWEL0023I`).
#define MSG_COMP_LIST_EMPTY     MSG_PREFIX "0029E" " start component list is empty\n"
//@Reason: Start component list is empty.
//@Action: Review the components defined in configuration (identified by message `ZWEL0023I`).
#define MSG_INST_PREP_ERR       MSG_PREFIX "0030E" " failed to prepare Zowe instance\n"
//@Reason: Failed to prepare the zowe instance.
//@Action: Check previous messages in the Zowe Launcher `SYSPRINT` to find the reason and correct it.
#define MSG_SIGNAL_ERR          MSG_PREFIX "0031E" " failed to setup signal handlers\n"
//@Reason: Failed to setup signal handlers.
//@Action: Contact Support.
#define MSG_DIR_ERR             MSG_PREFIX "0032E" " failed to find %s='%s', check if the dir exists\n"
//@Reason: Failed to find `<dir-type>`=`<dir-path>`, check if the directory exists.
//@Action: Verify `<dir-type>` is correctly defined in configuration (identified by message `ZWEL0023I`).
#define MSG_ROOT_DIR_ERR        MSG_PREFIX "0033E" " failed to get ROOT_DIR dir\n"
//@Reason: Failed to get ROOT_DIR (`zowe.runtimeDirectory`).
//@Action: Review `zowe.runtimeDirectory` defined in configuration (identified by message `ZWEL0023I`).
#define MSG_ROOT_DIR_EMPTY      MSG_PREFIX "0034E" " ROOT_DIR is empty string\n"
//@Reason: ROOT_DIR (`zowe.runtimeDirectory`) is empty string.
//@Action: Review `zowe.runtimeDirectory` defined in configuration (identified by message `ZWEL0023I`).
#define MSG_CMDLINE_INVALID     MSG_PREFIX "0035E" " invalid command line arguments, provide HA_INSTANCE_ID as a first argument\n"
//@Reason: Invalid command line arguments, provide `<HA_INSTANCE_ID>` as a first argument
//@Action: Review the command and provide `<HA_INSTANCE_ID>` as a first argument.
#define MSG_CTX_INIT_FAILED     MSG_PREFIX "0036E" " failed to initialize launcher context\n"
//@Reason: Failed to initialize launcher context.
//@Action: ?
#define MSG_MAX_COMP_REACHED    MSG_PREFIX "0037E" " max component number reached, ignoring the rest\n"
//@Reason: Maximal number of components reached, ignoring the rest.
//@Action: Review the components defined in configuration (identified by message `ZWEL0023I`).
#define MSG_MAX_RETRIES_REACHED MSG_PREFIX "0038E" " failed to restart component %s, max retries reached\n"
//@Reason: Failed to restart the component `<component-name>`, maximum number of retries reached.
//@Action: ?
#define MSG_COMP_ALREADY_RUN    MSG_PREFIX "0039E" " cannot start component %s - already running\n"
//@Reason: Cannot start the component `<component-name>` because it is already running.
//@Action:
#define MSG_COMP_START_FAILED   MSG_PREFIX "0040E" " failed to start component %s\n"
//@Reason: Failed to start the component `<component-name>`.
//@Action: Review the component defined in configuration (identified by message `ZWEL0023I`).
#define MSG_BAD_CMD_VAL         MSG_PREFIX "0041E" " bad value supplied, command ignored\n"
//@Reason: Bad value for the command supplied, such command is ignored.
//@Action: Review the command.
#define MSG_CMD_UNKNOWN         MSG_PREFIX "0042E" " command not recognized\n"
//@Reason: Command not recognized.
//@Action: Review the command.
#define MSG_CONS_START_ERR      MSG_PREFIX "0043E" " failed to start console thread\n"
//@Reason: Failed to start the console thread.
//@Action: ?
#define MSG_CONS_STOP_ERR       MSG_PREFIX "0044E" " failed to stop console thread\n"
//@Reason: Failed to stop the console thread.
//@Action: ?
#define MSG_YAML_CONV_ERR       MSG_PREFIX "0045E" " error converting zowe.yaml file - %s\n"    //@!
//@!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!@Reason: Error converting zowe.yaml file - `<path-to-zowe-yaml>`.
//!@Action: ?
#define MSG_YAML_READ_ERR       MSG_PREFIX "0046E" " error reading zowe.yaml file - %s\n"   //@!
//@!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//@!Reason: There is an error while reading zowe.yaml file - `<path-to-zowe.yaml>`
//@!Action: ?
#define MSG_YAML_PARSE_ERR      MSG_PREFIX "0047E" " failed to parse zowe.yaml - %s\n"    //@!
//@!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//@!Reason: Failed to parse the zowe.yaml - `<path-to-zowe-yaml>`.
//@!Action: ?
#define MSG_YAML_OPEN_ERR       MSG_PREFIX "0048E" " failed to open zowe.yaml - %s: %s\n"   //@!
//@!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//@!Reason: Failed to open zowe.yaml - `<path-to-zowe-yaml>`:?"
//@!Action: ?
#define MSG_COMP_RESTART_FAILED MSG_PREFIX "0049E" " failed to restart component %s\n"
//@Reason: Failed to restart the component `<component-name>`."
//@Action: ?
#define MSG_COMP_OUTPUT_ERR     MSG_PREFIX "0050E" " cannot read output from comp %s(%d) - %s\n"
//@Reason: Cannot read the output from component `<component-name>` (`<process-id>`) - `<error-text>`"
//@Action: ?
#define MSG_STDIN_CREATE_ERROR  MSG_PREFIX "0055E" " failed to create file for stdin(%s) - %s\n"
//@Reason: Failed to create the file for stdin(`<file>`) - <error-text>."
//@Action: ?
#define MSG_STDIN_OPEN_ERROR    MSG_PREFIX "0056E" " failed to open file for stdin(%s) - %s\n"
//@Reason: Failed to open the file for stdin(`<file>`) - <error-text>."
//@Action: ?
#define MSG_WORKSPACE_ERROR     MSG_PREFIX "0057E" " failed to create workspace dir '%s'\n"
//@Reason: Failed to create the workspace directory `<path-to-workspace-dir>`.
//@Action: ?
#define MSG_WKSP_DIR_ERR        MSG_PREFIX "0059E" " failed to get WORKSPACE_DIR dir\n"
//@Reason: Failed to get the WORKSPACE_DIR (`zowe.workspaceDirectory`) directory."
//@Action: ?
#define MSG_WKSP_DIR_EMPTY      MSG_PREFIX "0060E" " WORKSPACE_DIR is empty string\n"
//@Reason: The WORKSPACE_DIR (`zowe.workspaceDirectory`) is empty string.
//@Action: ?
#define MSG_FILE_ERR            MSG_PREFIX "0061E" " failed to find %s='%s', check if the file exists\n"    //@!
//@!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//@!Reason: Failed to find ?=?, check if the file exists.
//@!Action: ?
#define MSG_MKDIR_ERR           MSG_PREFIX "0062E" " failed to create dir '%s' - %s\n"
//@Reason: Failed to create the directory `<directory>` - `<error-text>`.
//@Action: ?
#define MSG_CMD_RUN_ERR         MSG_PREFIX "0064E" " failed to run command %s - %s\n"
//@Reason: Failed to run the command `<command>` - `<error-text>`.
//@Action: ?
#define MSG_CMD_OUT_ERR         MSG_PREFIX "0065E" " error reading output from command '%s' - %s\n"
//@Reason: There is an error reading the output from command `<command>` - `<error-text>`
//@Action: ?
#define MSG_MEMBER_NAME_BAD     MSG_PREFIX "0067E" " PARMLIB() entries must all have the same member name\n"
//@Reason: PARMLIB() entries must all have the same member name.
//@Action: Review the member names are identical for all PARMLIB() entries.
#define MSG_MEMBER_MISSING      MSG_PREFIX "0068E" " PARMLIB() entries must have a member name\n"
//@Reason: PARMLIB() entries must have a member name.
//@Action: Review the dataset name contains the member name in PARMLIB entry.
#define MSG_CFG_INVALID         MSG_PREFIX "0070E" " Configuration has validity exceptions:\n"
//@Reason: Configuration has validity exceptions.
//@Action: Review the exceptions and correct the configuration.
#define MSG_CFG_INTERNAL_FAIL   MSG_PREFIX "0071E" " Internal failure during validation, please contact support\n"
//@Reason: Internal failure during validation, please contact support.
//@Action: Contact support.
#define MSG_CFG_LOAD_FAIL       MSG_PREFIX "0072E" " Launcher could not load configurations\n"
//@Reason: Launcher could not load the configurations.
//@Action: Review the configuration entries.
#define MSG_CFG_SCHEMA_FAIL     MSG_PREFIX "0073E" " Launcher could not load schemas, status=%d\n"
//@Reason: Launcher could not load schemas, status=`<return-code>`
//@Action: Review the runtime directory
#define MSG_NO_LOG_CONTEXT      MSG_PREFIX "0074E" " Log context was not created\n"
//@Reason: The logging context was not created.
//@Action: Contact support.
//@!-------------------------------------------------------------------
//
//## Zowe Launcher warning messages
//
//@!-------------------------------------------------------------------
#define MSG_USE_DEFAULTS        MSG_PREFIX "0051W" " failed to read zowe.yaml, launcher will use default settings\n"    //@!
//@!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!@Reason: Failed to read zowe.yaml, launcher will use default settings.
//!@Action: ?
#define MSG_NOT_ALL_STARTED     MSG_PREFIX "0052W" " not all components started\n"
//@Reason: Not all components were started.
//@Action:
#define MSG_NOT_ALL_STOPPED     MSG_PREFIX "0053W" " not all components stopped gracefully\n"
//@Reason: Not all components were stopped gracefully.
//@Action:
#define MSG_COMP_NOT_FOUND      MSG_PREFIX "0054W" " component %s not found\n"
//@Reason: The component `<component-name>` was not found.
//@Action:
#define MSG_NOT_SIGTERM_STOPPED MSG_PREFIX "0063W" " Component %s(%d) will be terminated using SIGKILL\n"        
//@Reason: Component `<component-name>`(`<process-id>`) will be terminated using `SIGKILL`.
//@Action:
#define MSG_CMD_RCP_WARN        MSG_PREFIX "0066W" " command '%s' ended with code %d\n"
//@Reason:  The command `<command>` ended with return code `<return-code>`."
//@Action:
//@DocEnd

#endif // MSG_H

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
