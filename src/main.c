
/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <time.h>

#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/__messag.h>
#include <unistd.h>
#include "msg.h"

#include "collections.h"
#include "alloc.h"
#include "utils.h"
#include "json.h"
#include "configmgr.h"
#include "logging.h"
#include "stcbase.h"

extern char ** environ;
/*
 * TODO:
 * - Better process monitoring and clean up. For example, can we find all the
 * child processes of a component? How do we clean up the forks of a
 * killed process?
 * - a REST endpoint? Zowe CLI?
 */

#define CONFIG_DEBUG_MODE_KEY     "ZLDEBUG"
#define ZOWE_CONFIG_NAME          "ZOWEYAML"
#define CONFIG_DEBUG_MODE_VALUE   "ON"

#define COMP_ID "ZWELNCH"

#define MIN_UPTIME_SECS 90

#define SHUTDOWN_GRACEFUL_PERIOD (20 * 1000)

#define SHUTDOWN_POLLING_INTERVAL 300

#define COMP_LIST_SIZE 1024

#ifndef PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX
#endif

// Progressive restart internals in seconds
static int restart_intervals_default[] = {1, 1, 1, 5, 5, 10, 20, 60, 120, 240};

typedef struct zl_time_t {
  char value[32];
} zl_time_t;

static zl_time_t gettime(void) {

  time_t t = time(NULL);
  const char *format = "%Y-%m-%d %H:%M:%S";

  struct tm lt;
  zl_time_t result;

  localtime_r(&t, &lt);

  strftime(result.value, sizeof(result.value), format, &lt);

  return result;
}
typedef struct zl_int_array_t {
  int count;
#define ZL_INT_ARRAY_CAPACITY 100
  int data[ZL_INT_ARRAY_CAPACITY];
} zl_int_array_t;

#define ZL_YAML_KEY_LEN 255

typedef struct zl_config_t {
  bool debug_mode;
} zl_config_t;

typedef struct zl_comp_t {

  char name[32];
  char bin[_POSIX_PATH_MAX + 1];
  pid_t pid;
  int output;

  bool clean_stop;
  int fail_cnt;
  time_t start_time;

  enum {
    ZL_COMP_AS_SHARE_NO,
    ZL_COMP_AS_SHARE_YES,
    ZL_COMP_AS_SHARE_MUST,
  } share_as;

  pthread_t comm_thid;

  zl_int_array_t restart_intervals;
  int min_uptime; // secs

} zl_comp_t;

enum zl_event_t {
  ZL_EVENT_NONE = 0,
  ZL_EVENT_TERM,
  ZL_EVENT_COMP_RESTART,
};

struct {

  pthread_t console_thid;

#define MAX_CHILD_COUNT 128

  zl_comp_t children[MAX_CHILD_COUNT];
  size_t child_count;

  zl_config_t config;

  bool is_term;

  enum zl_event_t event_type;
  void *event_data;
  pthread_cond_t event_cv;
  pthread_mutex_t event_lock;

  //Room for at least 16 paths
  char yaml_file[PATH_MAX*17];
  char parm_member[8+1];
  char *root_dir;
  char *workspace_dir;
  
  char ha_instance_id[64];
  
  pid_t pid;
  char userid[9];
  
} zl_context = {.config = {.debug_mode = true}, .userid = "(NONE)"} ;



#define INFO(fmt, ...)  printf("%s <%s:%d> %s INFO "fmt, gettime().value, COMP_ID, zl_context.pid, zl_context.userid, ##__VA_ARGS__)
#define WARN(fmt, ...)  printf("%s <%s:%d> %s WARN "fmt, gettime().value, COMP_ID, zl_context.pid, zl_context.userid, ##__VA_ARGS__)
#define DEBUG(fmt, ...) if (zl_context.config.debug_mode) \
  printf("%s <%s:%d> %s DEBUG "fmt, gettime().value, COMP_ID, zl_context.pid, zl_context.userid, ##__VA_ARGS__)
#define ERROR(fmt, ...) printf("%s <%s:%d> %s ERROR "fmt, gettime().value, COMP_ID, zl_context.pid, zl_context.userid, ##__VA_ARGS__)

static int mkdir_all(const char *path, mode_t mode) {
  int path_len = strlen(path);
  int curr_path_len = 0;
  do {
    const char *slash = strchr(path + curr_path_len + 1, '/');
    char curr_path[PATH_MAX] = {0};
    curr_path_len = slash ? (int)(slash - path) : path_len;
    snprintf(curr_path, sizeof(curr_path), "%.*s", curr_path_len, path);
    if (mkdir(curr_path, mode) != 0) {
      if (errno != EEXIST) {
        ERROR(MSG_MKDIR_ERR, curr_path, strerror(errno));
        return -1;
      }
    }
  } while (curr_path_len < path_len - 1);
  return 0;
}

static int get_env(const char *name, char *buf, size_t buf_size) {
  const char *value = getenv(name);
  if (value == NULL) {
    ERROR(MSG_ENV_NOT_FOUND, name);
    return -1;
  }
  
  const char *value_start = value;
  const char *value_end = value_start + strlen(value) - 1;
  while (*value_end == ' ' && value_end != value_start) {
    value_end--;
  }

  size_t value_len = value_end - value_start + 1;
  if (value_len > buf_size - 1) {
    ERROR(MSG_ENV_TOO_LARGE, name);
    return -1;
  }
  
  memset(buf, 0, buf_size);
  memcpy(buf, value_start, value_len);
  return 0;
}

static void to_lower(char *s) {
  for ( ; *s; s++) {
    *s = tolower(*s);
  }
}

static int check_if_dir_exists(const char *dir, const char *name) {
  struct stat s;
  if (stat(dir, &s) != 0) {
    DEBUG("failed to get properties for dir %s='%s' - %s\n", name, dir, strerror(errno));
    return -1;
  }
  if (!S_ISDIR(s.st_mode)) {
    DEBUG("%s='%s' is not a directory\n", name, dir);
    return -1;
  }
  return 0;
}

static int init_context(int argc, char **argv, const struct zl_config_t *cfg, ConfigManager *configmgr) {

  if (get_env("CONFIG", zl_context.yaml_file, sizeof(zl_context.yaml_file))) {
    return -1;
  }

  int config_len = strlen(zl_context.yaml_file);
  bool hasMember = false;
  char member[9] = {0};
  char config_line[PATH_MAX*17] = {0};
  if (zl_context.yaml_file[0] == '/') { // simple file case, must be absolute path.
    snprintf(config_line, config_len+7, "FILE(%s)", zl_context.yaml_file);
    snprintf(zl_context.yaml_file, config_len+7, "%s", config_line);
  } else { //HERE loop over input to construct new string for configmgr use.
    // It needs to strip out the (member) within each occurrence of PARMLIB()
    int parmIndex = indexOfString(zl_context.yaml_file, config_len, "PARMLIB(", 0);
    int destPos = 0;
    int srcPos = 0;
    DEBUG("Handling config=%s\n",zl_context.yaml_file);
    while (parmIndex != -1) {
      int parenStartIndex = indexOf(zl_context.yaml_file, config_len, '(', parmIndex+9);
      int parenEndIndex = indexOf(zl_context.yaml_file, config_len, ')', parmIndex+9);
      DEBUG("pStart=%d, pEnd=%d\n", parenStartIndex, parenEndIndex);
      if (parenStartIndex != -1 && parenEndIndex != -1 && (parenStartIndex < parenEndIndex)) {
        memcpy(zl_context.parm_member, zl_context.yaml_file+parenStartIndex+1, parenEndIndex-parenStartIndex-1);
        if (hasMember && strcmp(zl_context.parm_member, member) != 0) {
          ERROR(MSG_MEMBER_NAME_BAD);
          return -1;
        }
        hasMember = true;
        memcpy(member, zl_context.yaml_file+parenStartIndex+1, parenEndIndex-parenStartIndex-1);
        DEBUG("Found member=%s\n",member);
        memcpy(config_line+destPos, zl_context.yaml_file+srcPos, parenStartIndex-srcPos);
        destPos+= parenStartIndex-srcPos;
        srcPos=parenEndIndex+1;
        parmIndex = indexOfString(zl_context.yaml_file, config_len, "PARMLIB(", parenEndIndex+2);
      } else {
        ERROR(MSG_MEMBER_MISSING);
        return -1;
      }
      DEBUG("config_line now=%s\n", config_line);
      DEBUG("src=%d, dst=%d, pNext=%d\n",srcPos,destPos,parmIndex);
    }

    if (destPos > 0) {
      memcpy(config_line+destPos, zl_context.yaml_file+srcPos, config_len - srcPos);
      destPos+= config_len - srcPos;
      memcpy(zl_context.yaml_file, config_line, destPos);
      zl_context.yaml_file[destPos]='\0';
    } else {
      zl_context.parm_member[0] = '\0';
    }
  
  }


  setenv("CONFIG", zl_context.yaml_file, 1);
  INFO(MSG_YAML_FILE, zl_context.yaml_file);

  zl_context.config = *cfg;

  if (argc != 2) {
    ERROR(MSG_CMDLINE_INVALID);
    return -1;
  }
  snprintf (zl_context.ha_instance_id, sizeof(zl_context.ha_instance_id), "%s", argv[1]);
  to_lower(zl_context.ha_instance_id);
  INFO(MSG_HA_INST_ID, zl_context.ha_instance_id);

  if (pthread_cond_init(&zl_context.event_cv, NULL) != 0) {
    DEBUG("pthread_cond_init() error - %s\n", strerror(errno));
    return -1;
  }

  if (pthread_mutex_init(&zl_context.event_lock, NULL) != 0) {
    DEBUG("pthread_mutex_init() error - %s\n", strerror(errno));
    return -1;
  }

  return 0;
}

static void snprint_int_array(zl_int_array_t *array, char *buf, size_t buf_size) {
  int pos = 0;
  for (int i = 0; i < array->count; i++) {
    pos += snprintf(buf + pos, buf_size - pos, "%d%s", array->data[i], i == array->count - 1 ? "" : " ");
  }
}

static void init_component_restart_intervals(zl_comp_t *comp, ConfigManager *configmgr) {
  DEBUG ("loading restart intervals for component '%s'\n", comp->name);
  Json *restartIntArray;
  int getStatus = cfgGetAnyC(configmgr, ZOWE_CONFIG_NAME, &restartIntArray, 6, "haInstances", zl_context.ha_instance_id, "components", comp->name, "launcher", "restartIntervals");
  if (getStatus != ZCFG_SUCCESS) {
    getStatus = cfgGetAnyC(configmgr, ZOWE_CONFIG_NAME, &restartIntArray, 4, "components", comp->name, "launcher", "restartIntervals");
    if (getStatus != ZCFG_SUCCESS) {
      getStatus = cfgGetAnyC(configmgr, ZOWE_CONFIG_NAME, &restartIntArray, 3, "zowe", "launcher", "restartIntervals");
    } else {
      memcpy(&comp->restart_intervals.data, restart_intervals_default, sizeof(restart_intervals_default));
      comp->restart_intervals.count = sizeof(restart_intervals_default)/sizeof(restart_intervals_default[0]);      
      return;
    }
  }

  JsonArray *intArray = jsonAsArray(restartIntArray);
  int count = jsonArrayGetCount(intArray);
  comp->restart_intervals.count = count;
  for (int i = 0; i < count; i++) {
    comp->restart_intervals.data[i] = jsonArrayGetNumber(intArray, i);
  }
}

static void init_component_min_uptime(zl_comp_t *comp, ConfigManager *configmgr) {
  int minUptime = MIN_UPTIME_SECS;
  int getStatus = cfgGetIntC(configmgr, ZOWE_CONFIG_NAME, &minUptime, 6, "haInstances", zl_context.ha_instance_id, "components", comp->name, "launcher", "minUptime");
  if (getStatus != ZCFG_SUCCESS) {
    getStatus = cfgGetIntC(configmgr, ZOWE_CONFIG_NAME, &minUptime, 4, "components", comp->name, "launcher", "minUptime");
    if (getStatus != ZCFG_SUCCESS) {
      getStatus = cfgGetIntC(configmgr, ZOWE_CONFIG_NAME, &minUptime, 3, "zowe", "launcher", "minUptime");
      if (getStatus != ZCFG_SUCCESS) {
        comp->min_uptime = MIN_UPTIME_SECS;
      } else {
        comp->min_uptime = minUptime;
      }
    } else {
      comp->min_uptime = minUptime;
    }
  } else {
    comp->min_uptime = minUptime;
  }
}

static void init_component_shareas(zl_comp_t *comp, ConfigManager *configmgr) {
  char *share_as = NULL;
  int getStatus = cfgGetStringC(configmgr, ZOWE_CONFIG_NAME, &share_as, 6, "haInstances", zl_context.ha_instance_id, "components", comp->name, "launcher", "shareAs");
  if (getStatus != ZCFG_SUCCESS) {
    getStatus = cfgGetStringC(configmgr, ZOWE_CONFIG_NAME, &share_as, 4, "components", comp->name, "launcher", "shareAs");
    if (getStatus != ZCFG_SUCCESS) {
      getStatus = cfgGetStringC(configmgr, ZOWE_CONFIG_NAME, &share_as, 3, "zowe", "launcher", "shareAs");
      if (getStatus != ZCFG_SUCCESS) {
        share_as = "yes";
      }
    }
  }
  
  if (!strcmp(share_as, "no")) {
    comp->share_as = ZL_COMP_AS_SHARE_NO;
  } else if (!strcmp(share_as, "yes")) {
    comp->share_as = ZL_COMP_AS_SHARE_YES;
  } else if (!strcmp(share_as, "must")) {
    comp->share_as = ZL_COMP_AS_SHARE_MUST;
  } else {
    comp->share_as = ZL_COMP_AS_SHARE_YES;
  }
  safeFree(share_as, strlen(share_as));
}

static const char *get_shareas_label(const zl_comp_t *comp) {
  switch (comp->share_as) {
  case ZL_COMP_AS_SHARE_NO:
    return "no";
  case ZL_COMP_AS_SHARE_YES:
    return "yes";
  case ZL_COMP_AS_SHARE_MUST:
    return "must";
  default:
    return "no";
  }
}

static int init_component(const char *name, zl_comp_t *result, ConfigManager *configmgr) {
  snprintf(result->name, sizeof(result->name), "%s", name);
  result->pid = -1;
  init_component_shareas(result, configmgr);
  init_component_restart_intervals(result, configmgr);
  init_component_min_uptime(result, configmgr);
  
  INFO(MSG_COMP_INITED, result->name, result->restart_intervals.count, result->min_uptime, get_shareas_label(result));

  char restart_intervals_buf[2048];
  snprint_int_array(&result->restart_intervals, restart_intervals_buf, sizeof(restart_intervals_buf));
  INFO(MSG_RESTART_INTRVL, result->name, restart_intervals_buf);
  return 0;
}

static const char *get_shareas_env(const zl_comp_t *comp) {

  switch (comp->share_as) {
  case ZL_COMP_AS_SHARE_NO:
    return "_BPX_SHAREAS=NO";
  case ZL_COMP_AS_SHARE_YES:
    return "_BPX_SHAREAS=YES";
  case ZL_COMP_AS_SHARE_MUST:
    return "_BPX_SHAREAS=MUST";
  default:
    return "_BPX_SHAREAS=NO";
  }

}

static int init_components(char *components, ConfigManager *configmgr) {
  if (!components) {
    DEBUG("components to launch not set\n");
    return -1;
  }
  char *name = strtok(components, ",");

  while(name != NULL) {
    zl_comp_t comp = { 0 };
    init_component(name, &comp, configmgr);
    if (zl_context.child_count != MAX_CHILD_COUNT) {
      zl_context.children[zl_context.child_count++] = comp;
    } else {
      ERROR(MSG_MAX_COMP_REACHED);
      break;
    }
    name = strtok(NULL, ",");
	}
  return 0;
}

static int send_event(enum zl_event_t event_type, void *event_data);

static void *handle_comp_comm(void *args) {

  DEBUG("starting a component communication thread\n");

  zl_comp_t *comp = args;

  while (true) {

    int comp_status = 0;
    int wait_rc = waitpid(comp->pid, &comp_status, WNOHANG);
    if (wait_rc == comp->pid) {
      INFO(MSG_COMP_TERMINATED, comp->name, comp->pid, comp_status);
      comp->pid = -1;
      time_t uptime = time(NULL) - comp->start_time;
      if (uptime > MIN_UPTIME_SECS) {
        comp->fail_cnt = 1;
      } else {
        comp->fail_cnt++;
      }
      if (!comp->clean_stop) {
        if (comp->fail_cnt <= comp->restart_intervals.count) {
          size_t delay = comp->restart_intervals.data[comp->fail_cnt - 1];
          INFO(MSG_NEXT_RESTART, comp->name, (int)delay);
          sleep(delay);
          send_event(ZL_EVENT_COMP_RESTART, comp);
        } else {
          ERROR(MSG_MAX_RETRIES_REACHED, comp->name);
        }
      }else{
        INFO(MSG_COMP_STOPPED, comp->name);
      }

      break;
    } else if (wait_rc == -1) {
      DEBUG("waitpid failed for %s(%d) - %s\n",
            comp->name, comp->pid, strerror(errno));
      break;
    } else {
      DEBUG("waitpid RC = 0 for %s(%d)\n", comp->name, comp->pid);
    }

    char msg[4096];
    int retries_left = 3;
    while (retries_left > 0) {

      int msg_len = read(comp->output, msg, sizeof(msg));
      if (msg_len > 0) {
        msg[msg_len] = '\0';

        char *next_line = strtok(msg, "\n");

        while (next_line) {
          printf("%s\n", next_line);
          next_line = strtok(NULL, "\n");
        }

        retries_left = 3;
      } else if (msg_len == -1 && errno == EAGAIN) {
        sleep(1);
        retries_left--;
        DEBUG("waiting for next message from %s(%d)\n", comp->name, comp->pid);
      } else {
        ERROR(MSG_COMP_OUTPUT_ERR, comp->name, comp->pid, strerror(errno));
      }

    }

  }

  return NULL;
}

static int start_component(zl_comp_t *comp) {

  if (comp->pid != -1) {
    ERROR(MSG_COMP_ALREADY_RUN, comp->name);
    return -1;
  }

  DEBUG("about to start component %s\n", comp->name);

  // ensure the new process has its own process group ID so we can terminate
  // the entire process tree
  struct inheritance inherit = {
      .flags = (short) SPAWN_SETGROUP,
      .pgroup = SPAWN_NEWPGROUP,
  };

  FILE *script = NULL;
  int c_stdout[2];
  if (pipe(c_stdout)) {
    DEBUG("pipe() failed for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  if (fcntl(c_stdout[0], F_SETFL, O_NONBLOCK)) {
    DEBUG("fcntl() failed for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  int fd_count = 3;
  int fd_map[3];
  char bin[PATH_MAX];

  snprintf(bin, sizeof(bin), "%s/bin/zwe", zl_context.root_dir);
  script = fopen(bin, "r");
  if (script == NULL) {
    DEBUG("script not open for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }
  fd_map[0] = dup(fileno(script));
  fclose(script);
  fd_map[1] = dup(c_stdout[1]);
  fd_map[2] = dup(c_stdout[1]);

  DEBUG("%s fd_map[0]=%d, fd_map[1]=%d, fd_map[2]=%d\n",
        comp->name, fd_map[0], fd_map[1], fd_map[2]);

  const char *c_envp[2] = {get_shareas_env(comp), NULL};
  const char *c_args[] = {
    bin,
    "internal",
    "start",
    "component",
    "--config", zl_context.yaml_file,
    "--ha-instance", zl_context.ha_instance_id,
    "--component", comp->name, 
    NULL
  };
  comp->pid = spawn(bin, fd_count, fd_map, &inherit, c_args, c_envp);
  if (comp->pid == -1) {
    DEBUG("spawn() failed for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  comp->start_time = time(NULL);
  comp->output = c_stdout[0];
  close(c_stdout[1]);

  comp->clean_stop = false;

  INFO(MSG_COMP_STARTED, comp->name);

  if (pthread_create(&comp->comm_thid, NULL, handle_comp_comm, comp) != 0) {
    DEBUG("comm thread not started for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  return 0;
}

static int start_components(void) {

  INFO(MSG_STARTING_COMPS);

  int rc = 0;

  for (size_t i = 0; i < zl_context.child_count; i++) {
    if (start_component(&zl_context.children[i])) {
      ERROR(MSG_COMP_START_FAILED, zl_context.children[i].name);
      rc = -1;
    }
  }

  if (rc) {
    WARN(MSG_NOT_ALL_STARTED);
  } else {
    INFO(MSG_COMPS_STARTED);
  }

  return rc;
}

static int stop_component(zl_comp_t *comp) {

  if (comp->pid == -1) {
    return 0;
  }

  comp->clean_stop = true;

  DEBUG("about to stop component %s(%d) and its children\n",
        comp->name, comp->pid);

  pid_t pgid = -comp->pid;
  if (kill(pgid, SIGTERM)) {
    ERROR("kill() failed for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  int wait_time = 0;
  while (comp->pid > 0 && wait_time < SHUTDOWN_GRACEFUL_PERIOD) {
    usleep(SHUTDOWN_POLLING_INTERVAL * 1000);
    wait_time += SHUTDOWN_POLLING_INTERVAL;
  }
  
  if (comp->pid > 0) {
    DEBUG("Component %s(%d) is not shutting down within %d milliseconds\n", 
          comp->name, comp->pid, SHUTDOWN_GRACEFUL_PERIOD);
    WARN(MSG_NOT_SIGTERM_STOPPED, comp->name, comp->pid);
    pid_t pgid = -comp->pid;
    if (kill(pgid, SIGKILL)) {
      ERROR("kill() failed for %s - %s\n", comp->name, strerror(errno));
      return -1;
    }
  }

  comp->pid = -1;
  INFO(MSG_COMP_STOPPED, comp->name);

  return 0;
}

static int stop_components(void) {

  INFO(MSG_STOPING_COMPS);

  int rc = 0;

  for (size_t i = 0; i < zl_context.child_count; i++) {
    zl_comp_t *comp = &zl_context.children[i];
    comp->clean_stop = true;
    if (comp->pid != -1) {
      DEBUG("about to send SIGTERM to component %s(%d)\n", comp->name, comp->pid); 
      pid_t pgid = -comp->pid;
      if (kill(pgid, SIGTERM)) {
        WARN("kill() failed for %s - %s\n", comp->name, strerror(errno));
      }
    }
  }

  int wait_time = 0;
  bool all_exit = false;
  while (!all_exit  && wait_time < SHUTDOWN_GRACEFUL_PERIOD) {
    all_exit = true;
    for (size_t i = 0; i < zl_context.child_count; i++) {
      zl_comp_t *comptout = &zl_context.children[i];
      if (comptout->pid > 0) {
        all_exit = false;
      }
    }
    usleep(SHUTDOWN_POLLING_INTERVAL * 1000);
    wait_time += SHUTDOWN_POLLING_INTERVAL;
  }
  
  for (size_t i = 0; i < zl_context.child_count; i++) {
    zl_comp_t *compkill = &zl_context.children[i];
    if (compkill->pid > 0) {
      pid_t pgid = -compkill->pid;
      DEBUG("Component %s(%d) is not shutting down within %d milliseconds\n", 
            compkill->name, compkill->pid, SHUTDOWN_GRACEFUL_PERIOD);
      WARN(MSG_NOT_SIGTERM_STOPPED, compkill->name, compkill->pid);
      if (kill(pgid, SIGKILL)) {
        WARN("kill() failed for %s - %s\n", compkill->name, strerror(errno));
      }
      rc = -1;
    }  
  }

  if (rc) {
    WARN(MSG_NOT_ALL_STOPPED);
  } else {
    INFO(MSG_COMPS_STOPPED);
  }
  
  return 0;
}

static zl_comp_t *find_comp(const char *name) {

  for (size_t i = 0; i < zl_context.child_count; i++) {
    if (!strcasecmp(name, zl_context.children[i].name)) {
      return &zl_context.children[i];
    }
  }

  return NULL;
}

#define CMD_START "START"
#define CMD_STOP  "STOP"
#define CMD_DISP  "DISP"

static int handle_start(const char *comp_name) {

  zl_comp_t *comp = find_comp(comp_name);
  if (comp == NULL) {
    WARN(MSG_COMP_NOT_FOUND, comp_name);
    return -1;
  }

  comp->fail_cnt = 0;
  start_component(comp);

  return 0;
}

static int handle_stop(const char *comp_name) {

  zl_comp_t *comp = find_comp(comp_name);
  if (comp == NULL) {
    WARN(MSG_COMP_NOT_FOUND, comp_name);
    return -1;
  }

  stop_component(comp);

  return 0;
}

static int handle_disp(void) {

  INFO(MSG_LAUNCHER_COMPS);
  for (size_t i = 0; i < zl_context.child_count; i++) {
    INFO(MSG_LAUNCHER_COMP, zl_context.children[i].name, zl_context.children[i].pid);
  }

  return 0;
}

static char *get_cmd_val(const char *cmd, char *buff, size_t buff_len) {

  const char *lb = strchr(cmd, '(');
  if (lb == NULL) {
    return NULL;
  }

  const char *rb = strchr(cmd, ')');
  if (rb == NULL) {
    return NULL;
  }

  if (lb > rb) {
    return NULL;
  }

  size_t val_len = rb - lb - 1;
  if (val_len >= buff_len) {
    return NULL;
  }

  memcpy(buff, lb + 1, val_len);
  buff[val_len] = '\0';

  return buff;
}

static void *handle_console(void *args) {

  INFO(MSG_START_CONSOLE);

  while (true) {

    struct __cons_msg2 cons = {0};
    cons.__cm2_format = __CONSOLE_FORMAT_3;

    char mod_cmd[128] = {0};
    int cmd_type = 0;

    if (__console2(&cons, mod_cmd, &cmd_type)) {
      DEBUG("__console2() - %s\n", strerror(errno));
      pthread_exit(NULL);
    }

    if (cmd_type == _CC_modify) {

      INFO(MSG_CMD_RECV, mod_cmd);

      char cmd_val[128] = {0};

      if (strstr(mod_cmd, CMD_START) == mod_cmd) {
        char *val = get_cmd_val(mod_cmd, cmd_val, sizeof(cmd_val));
        if (val != NULL) {
          handle_start(val);
        } else {
          ERROR(MSG_BAD_CMD_VAL);
        }
      } else if (strstr(mod_cmd, CMD_STOP) == mod_cmd) {
        char *val = get_cmd_val(mod_cmd, cmd_val, sizeof(cmd_val));
        if (val != NULL) {
          handle_stop(val);
        } else {
          ERROR(MSG_BAD_CMD_VAL);
        }
      } else if (strstr(mod_cmd, CMD_DISP) == mod_cmd) {
        handle_disp();
      } else {
        ERROR(MSG_CMD_UNKNOWN);
      }

    } else if (cmd_type == _CC_stop) {
      INFO(MSG_TERM_CMD_RECV);
      send_event(ZL_EVENT_TERM, NULL);
      break;
    }

  }

  INFO(MSG_CONSOLE_STOPPED);

  return NULL;
}

static int start_console_tread(void) {

  DEBUG("starting console thread\n");

  if (pthread_create(&zl_context.console_thid, NULL, handle_console, NULL) != 0) {
    DEBUG("pthread_created() for console listener - %s\n", strerror(errno));
    return -1;
  }

  return 0;
}

static int stop_console_thread(void) {

  if (pthread_join(zl_context.console_thid, NULL) != 0) {
    DEBUG("pthread_join() for console listener - %s\n", strerror(errno));
    return -1;
  }

  DEBUG("console thread stopped\n");

  return 0;
}

/**
 * @brief Compare space padded strings
 * @param s1 String 1
 * @param s2 String 2
 * @return 0 if equal, otherwise difference between the first non blank
 * characters
 */
static int strcmp_pad(const char *s1, const char *s2) {

  for (; *s1 == *s2; s1++, s2++) {
    if (*s1 == '\0') {
      return 0;
    }
  }

  if (*s1 == '\0') {
    while (*s2 == ' ') { s2++; }
    return -(unsigned) *s2;
  } else if (*s2 == '\0') {
    while (*s1 == ' ') { s1++; }
    return (unsigned) *s1;
  } else {
    return (unsigned) *s1 - (unsigned) *s2;
  }

}

static zl_config_t read_config(int argc, char **argv) {

  zl_config_t result = {0};

  char *debug_value = getenv(CONFIG_DEBUG_MODE_KEY);

  if (debug_value && !strcmp_pad(debug_value, CONFIG_DEBUG_MODE_VALUE)) {
    result.debug_mode = true;
  }

  return result;
}

static int restart_component(zl_comp_t *comp) {

  int stop_rc = stop_component(comp);
  if (stop_rc) {
    return stop_rc;
  }

  return start_component(comp);
}

static void monitor_events(void) {

  if (pthread_mutex_lock(&zl_context.event_lock) != 0) {
    DEBUG("monitor_events: pthread_mutex_lock() error - %s\n", strerror(errno));
    return;
  }

  while (true) {

    while (zl_context.event_type == ZL_EVENT_NONE) {
      if (pthread_cond_wait(&zl_context.event_cv, &zl_context.event_lock) !=0) {
        DEBUG("monitor_events: pthread_cond_wait() error - %s\n",
              strerror(errno));
        return;
      }
    }

    DEBUG("event with type %d and data 0x%p has been received\n",
          zl_context.event_type, zl_context.event_data);

    if (zl_context.event_type == ZL_EVENT_TERM) {
      break;
    } else if (zl_context.event_type == ZL_EVENT_COMP_RESTART) {
      zl_comp_t* comp = zl_context.event_data;
      int restart_rc = restart_component(comp);
      if (restart_rc) {
        ERROR(MSG_COMP_RESTART_FAILED, comp->name);
      }
    } else {
      DEBUG("unknown event type %d\n", zl_context.event_type);
      break;
    }

    zl_context.event_type = ZL_EVENT_NONE;
    zl_context.event_data = NULL;

  }

  if (pthread_mutex_unlock(&zl_context.event_lock) != 0) {
    DEBUG("monitor_events: pthread_mutex_unlock() error - %s\n",
          strerror(errno));
    return;
  }

}

static int send_event(enum zl_event_t event_type, void *event_data) {

  if (pthread_mutex_lock(&zl_context.event_lock) != 0) {
    DEBUG("send_event: pthread_mutex_lock() error - %s\n", strerror(errno));
    return -1;
  }

  zl_context.event_type = event_type;
  zl_context.event_data = event_data;

  if (pthread_cond_signal(&zl_context.event_cv) != 0) {
    DEBUG("send_event: pthread_cond_signal() error - %s\n", strerror(errno));
    return -1;
  }

  DEBUG("event with type %d and data 0x%p has been sent\n",
        zl_context.event_type, zl_context.event_data);

  if (pthread_mutex_unlock(&zl_context.event_lock) != 0) {
    DEBUG("send_event: pthread_mutex_unlock() error - %s\n", strerror(errno));
    return -1;
  }

  return 0;
}

typedef void (*handle_line_callback_t)(void *data, const char *line);

static int run_command(const char *command, handle_line_callback_t handle_line, void *data) {
  DEBUG("about to run command '%s'\n", command);
  FILE *fp = popen(command, "r");
  if (!fp) {
    ERROR(MSG_CMD_RUN_ERR, command, strerror(errno));
    return -1;
  }
  char *line;
  char buf[1024] = {0};
  while((line = fgets(buf, sizeof(buf) - 1, fp)) != NULL) {
    handle_line(data, line);
    memset(buf, '\0', sizeof(buf));
  }
  if (ferror(fp)) {
    pclose(fp);
    ERROR(MSG_CMD_OUT_ERR, command, strerror(errno));
    return -1;
  }
  int rc = pclose(fp);
  if (rc == -1) {
    ERROR(MSG_CMD_RUN_ERR, command, strerror(errno));
  } else if (rc > 0) {
    WARN(MSG_CMD_RCP_WARN, command, rc);
    return -1;
  }
  DEBUG("command '%s' ran successfully\n", command);
  return 0;
}

static void handle_get_component_line(void *data, const char *line) {
  char *comp_list = data;
  snprintf(comp_list, COMP_LIST_SIZE, "%s", line);
  int len = strlen(comp_list);
  for (int i = len - 1; i >= 0; i--) {
    if (comp_list[i] != ' ' && comp_list[i] != '\n' && comp_list[i] != ',') {
      break;
    }
    comp_list[i] = '\0';
  }
}

static int get_component_list(char *buf, size_t buf_size) {
  char command[4*PATH_MAX];
  snprintf (command, sizeof(command), "%s/bin/zwe internal get-launch-components --config \"%s\" --ha-instance %s",
            zl_context.root_dir, zl_context.yaml_file, zl_context.ha_instance_id);
  DEBUG("about to get component list\n");
  char comp_list[COMP_LIST_SIZE] = {0};
  if (run_command(command, handle_get_component_line, (void*)comp_list)) {
    ERROR(MSG_COMP_LIST_ERR);
  }
  if (strlen(comp_list) == 0) {
    ERROR(MSG_COMP_LIST_EMPTY);
    return -1;
  }
  snprintf(buf, buf_size, "%s", comp_list);
  INFO(MSG_START_COMP_LIST, buf);
  return 0;
}

static int check_root_dir() {
  if (strlen(zl_context.root_dir) == 0) {
    ERROR(MSG_ROOT_DIR_EMPTY);
    return -1;
  }
  if (check_if_dir_exists(zl_context.root_dir, "ROOT_DIR")) {
    ERROR(MSG_DIR_ERR, "ROOT_DIR", zl_context.root_dir);
    return -1;
  }
  setenv("ROOT_DIR", zl_context.root_dir, 1);
  INFO(MSG_ROOT_DIR, zl_context.root_dir);

  /* do we really need to change work dir? */
  if (chdir(zl_context.root_dir)) {
    DEBUG("working directory not changed - %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

/* unfortunate bootstrapping: we cannot find configmgr until we find runtimedir
  we must iterate through every yaml file until we find a valid runtimedir
  
  TODO: resolve template... right now we take the first value for runtimeDirectory we find and assume it to be a path
  TODO: allow parmlib to be the one that has runtimeDirectory. right now a FILE must be found prior to encountering a LIB entry, or the code will attempt an fopen() and fail.
*/
static int process_root_dir(ConfigManager *configmgr) {
  int getStatus = cfgGetStringC(configmgr, ZOWE_CONFIG_NAME, &zl_context.root_dir, 2, "zowe", "runtimeDirectory");
  if (getStatus) {
    ERROR(MSG_ROOT_DIR_ERR);
    return -1;
  }
  return check_root_dir();
}

static int process_workspace_dir(ConfigManager *configmgr) {
  DEBUG("about to get workspace dir\n");

  int getStatus = cfgGetStringC(configmgr, ZOWE_CONFIG_NAME, &zl_context.workspace_dir, 2, "zowe", "workspaceDirectory");
  if (getStatus) {
    ERROR(MSG_WKSP_DIR_ERR);
    return -1;
  }
  
  if (strlen(zl_context.workspace_dir) == 0) {
    ERROR(MSG_WKSP_DIR_EMPTY);
    return -1;
  }

  // create folder if it doesn't exist
  // FIXME: what's the proper permission?
  if (mkdir_all(zl_context.workspace_dir, 0750) != 0) {
    ERROR(MSG_WORKSPACE_ERROR, zl_context.workspace_dir);
    return -1;
  }
  
  // we really created
  if (check_if_dir_exists(zl_context.workspace_dir, "WORKSPACE_DIR")) {
    ERROR(MSG_DIR_ERR, "WORKSPACE_DIR", zl_context.workspace_dir);
    return -1;
  }
  setenv("WORKSPACE_DIR", zl_context.workspace_dir, 1);
  INFO(MSG_WKSP_DIR, zl_context.workspace_dir);

  // define stdin as a file in workspace directory
  char stdin_file[PATH_MAX+1] = {0};
  snprintf (stdin_file, sizeof(stdin_file), "%s/launcher.stdin.txt", zl_context.workspace_dir);
  FILE *stdin_fp = fopen(stdin_file, "w");
  if (!stdin_fp) {
    ERROR(MSG_STDIN_CREATE_ERROR, stdin_file, strerror(errno));
    return -1;
  }
  fclose(stdin_fp);
  stdin_fp = fopen(stdin_file, "r");
  if (!stdin_fp) {
    ERROR(MSG_STDIN_OPEN_ERROR, stdin_file, strerror(errno));
    return -1;
  }
  stdin = stdin_fp;

  return 0;
}

static void print_line(void *data, const char *line) {
  printf("%s", line);
}

static int prepare_instance() {
  char command[4*PATH_MAX];
  DEBUG("about to prepare Zowe instance\n");
  snprintf(command, sizeof(command), "%s/bin/zwe internal start prepare --config \"%s\" --ha-instance %s 2>&1",
           zl_context.root_dir, zl_context.yaml_file, zl_context.ha_instance_id);
  if (run_command(command, print_line, NULL)) {
    ERROR(MSG_INST_PREP_ERR);
    return -1;
  }
  INFO(MSG_INST_PREPARED);
  return 0;
}

static int init() {
  zl_context.pid = getpid();
  char *login = __getlogin1();
  snprintf(zl_context.userid, sizeof(zl_context.userid), "%s", login);
  return 0;
}

static void terminate(int sig) {
  INFO(MSG_LAUNCHER_STOPING);
  stop_components();
  exit(EXIT_SUCCESS);
}

static int setup_signal_handlers() {
  struct sigaction sa;
  sa.sa_handler = terminate;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    DEBUG("failed to set SIGINT handler - %s\n", strerror(errno));
    return -1;
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    DEBUG("failed to set SIGTERM handler - %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

static void displayValidityException(FILE *out, int depth, ValidityException *exception){
  for (int i=0; i<depth; i++){
    fprintf(out,"  ");
  }
  fprintf(out,"%s\n",exception->message);
  ValidityException *child = exception->firstChild;
  while (child){
    displayValidityException(out,depth+1,child);
    child = child->nextSibling;
  }
}


static bool validateConfiguration(ConfigManager *cmgr, FILE *out){
  bool ok = false;
  JsonValidator *validator = makeJsonValidator();
  validator->traceLevel = cmgr->traceLevel;
  int validateStatus = cfgValidate(cmgr,validator,ZOWE_CONFIG_NAME);

  switch (validateStatus){
  case JSON_VALIDATOR_NO_EXCEPTIONS:
    INFO(MSG_CFG_VALID);
    ok = true;
    break;
  case JSON_VALIDATOR_HAS_EXCEPTIONS:
    ERROR(MSG_CFG_INVALID);
    displayValidityException(out,0,validator->topValidityException);
    break;
  case JSON_VALIDATOR_INTERNAL_FAILURE:
    ERROR(MSG_CFG_INTERNAL_FAIL);
    break;
  }
  freeJsonValidator(validator);
  return ok;
}

int main(int argc, char **argv) {
  if (init()) {
    exit(EXIT_FAILURE);
  }

  INFO(MSG_LAUNCHER_START);

  zl_config_t config = read_config(argc, argv);

  LoggingContext *logContext = makeLoggingContext();
  logConfigureStandardDestinations(logContext);

  ConfigManager *configmgr = makeConfigManager(); /* configs,schemas,1,stderr); */
  CFGConfig *theConfig = addConfig(configmgr,ZOWE_CONFIG_NAME);
  cfgSetTraceStream(configmgr,stderr);
  cfgSetTraceLevel(configmgr, zl_context.config.debug_mode ? 2 : 0);
  
  if (init_context(argc, argv, &config, configmgr)) {
    ERROR(MSG_CTX_INIT_FAILED);
    exit(EXIT_FAILURE);
  }

  cfgSetConfigPath(configmgr, ZOWE_CONFIG_NAME, zl_context.yaml_file);
  int parm_member_len = strlen(zl_context.parm_member);
  if (parm_member_len > 0 && parm_member_len < 9) {
    cfgSetParmlibMemberName(configmgr, ZOWE_CONFIG_NAME, zl_context.parm_member);
  }

  if (cfgLoadConfiguration(configmgr, ZOWE_CONFIG_NAME) != 0){
    ERROR(MSG_CFG_LOAD_FAIL);
    exit(EXIT_FAILURE);
  }
  
  if (setup_signal_handlers()) {
    ERROR(MSG_SIGNAL_ERR);
    exit(EXIT_FAILURE);
  }

  if (process_root_dir(configmgr)) {
    exit(EXIT_FAILURE);
  }

  //got root dir, can now load up the schemas from it
  char schemaList[PATH_MAX*2 + 4] = {0};
  snprintf(schemaList, PATH_MAX*2 + 1, "%s/schemas/zowe-yaml-schema.json:%s/schemas/server-common.json", zl_context.root_dir, zl_context.root_dir);  
  int schemaLoadStatus = cfgLoadSchemas(configmgr, ZOWE_CONFIG_NAME, schemaList);
  if (schemaLoadStatus){
    ERROR(MSG_CFG_SCHEMA_FAIL, schemaLoadStatus);
    exit(EXIT_FAILURE);
  }


  if (!validateConfiguration(configmgr, stdout)){
    exit(EXIT_FAILURE);
  }


  if (process_workspace_dir(configmgr)) {
    exit(EXIT_FAILURE);
  }
  
  char comp_buf[COMP_LIST_SIZE];
  char *component_list = NULL;

  if (prepare_instance()) {
    exit(EXIT_FAILURE);
  }
  if (get_component_list(comp_buf, sizeof(comp_buf))) {
    exit(EXIT_FAILURE);
  }
  component_list = comp_buf;

  if (init_components(component_list, configmgr)) {
    exit(EXIT_FAILURE);
  }

  start_components();

  if (start_console_tread()) {
    ERROR(MSG_CONS_START_ERR);
    exit(EXIT_FAILURE);
  }

  monitor_events();

  if (stop_console_thread()) {
    ERROR(MSG_CONS_STOP_ERR);
    exit(EXIT_FAILURE);
  }

  stop_components();

  INFO(MSG_LAUNCHER_STOPPED);

  exit(EXIT_SUCCESS);
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
