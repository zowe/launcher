
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
#define MSG_LAUNCHER_START      MSG_PREFIX "0021I" " Zowe Launcher starting\n"
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
#define MSG_MEMBER_NAME_BAD     MSG_PREFIX "0067E" " PARMLIB() entries must all have the same member name\n"
#define MSG_MEMBER_MISSING      MSG_PREFIX "0068E" " PARMLIB() entries must have a member name\n"
#define MSG_CFG_VALID           MSG_PREFIX "0069I" " Configuration is valid\n"
#define MSG_CFG_INVALID         MSG_PREFIX "0070E" " Configuration has validity exceptions:\n"
#define MSG_CFG_INTERNAL_FAIL   MSG_PREFIX "0071E" " Internal failure during validation, please contact support\n"
#define MSG_CFG_LOAD_FAIL       MSG_PREFIX "0072E" " Launcher Could not load configurations\n"
#define MSG_CFG_SCHEMA_FAIL     MSG_PREFIX "0073E" " Launcher Could not load schemas, status=%d\n"

#endif // MSG_H

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
