
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
#include "yaml.h"

extern char ** environ;
/*
 * TODO:
 * - Better process monitoring and clean up. For example, can we find all the
 * child processes of a component? How do we clean up the forks of a
 * killed process?
 * - a REST endpoint? Zowe CLI?
 */

#define CONFIG_DEBUG_MODE_KEY     "ZLDEBUG"
#define CONFIG_DEBUG_MODE_VALUE   "ON"

#define COMP_ID "ZWELNCH"
#define MSG_PREFIX "ZWEL"
#define MSG_COMPONENT_STARTED MSG_PREFIX "0001I component %s started\n"
#define MSG_COMPONENT_STOPPED MSG_PREFIX "0002I component %s stopped\n"

#define MIN_UPTIME_SECS 90

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

typedef struct zl_yaml_config_t {
  yaml_document_t document;
  yaml_node_t *root;
} zl_yaml_config_t;

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

  char instance_dir[PATH_MAX + 1];
  char root_dir[PATH_MAX+1];
  
  char ha_instance_id[64];
  
  pid_t pid;
  char userid[9];

  zl_yaml_config_t yaml_config;
  
} zl_context = {.config = {.debug_mode = true}, .userid = "(NONE)", .yaml_config = {0}} ;



#define INFO(fmt, ...)  printf("%s <%s:%d> %s INFO "fmt, gettime().value, COMP_ID, zl_context.pid, zl_context.userid, ##__VA_ARGS__)
#define WARN(fmt, ...)  printf("%s <%s:%d> %s WARN "fmt, gettime().value, COMP_ID, zl_context.pid, zl_context.userid, ##__VA_ARGS__)
#define DEBUG(fmt, ...) if (zl_context.config.debug_mode) \
  printf("%s <%s:%d> %s DEBUG "fmt, gettime().value, COMP_ID, zl_context.pid, zl_context.userid, ##__VA_ARGS__)
#define ERROR(fmt, ...) printf("%s <%s:%d> %s ERROR "fmt, gettime().value, COMP_ID, zl_context.pid, zl_context.userid, ##__VA_ARGS__)

static int get_env(const char *name, char *buf, size_t buf_size) {
  const char *value = getenv(name);
  if (value == NULL) {
    ERROR("%s env variable not found\n", name);
    return -1;
  }
  
  const char *value_start = value;
  const char *value_end = value_start + strlen(value) - 1;
  while (*value_end == ' ' && value_end != value_start) {
    value_end--;
  }

  size_t value_len = value_end - value_start + 1;
  if (value_len > buf_size - 1) {
    ERROR("%s env too large\n", name);
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
    ERROR("failed to get properties for dir %s='%s' - %s\n", name, dir, strerror(errno));
    return -1;
  }
  if (!S_ISDIR(s.st_mode)) {
    ERROR("%s='%s' is not a directory\n", name, dir);
    return -1;
  }
  return 0;
}

static int init_context(int argc, char **argv, const struct zl_config_t *cfg) {

  if (get_env("INSTANCE_DIR", zl_context.instance_dir, sizeof(zl_context.instance_dir))) {
    return -1;
  }
  if (check_if_dir_exists(zl_context.instance_dir, "INSTANCE_DIR")) {
    return -1;
  }

  zl_context.config = *cfg;

  if (argc != 2) {
    ERROR("Invalid command line arguments, provide HA_INSTANCE_ID as a first argument\n");
    return -1;
  }
  snprintf (zl_context.ha_instance_id, sizeof(zl_context.ha_instance_id), "%s", argv[1]);
  to_lower(zl_context.ha_instance_id);
  INFO("HA_INSTANCE_ID='%s'\n", zl_context.ha_instance_id);

  /* do we really need to change work dir? */
  if (chdir(zl_context.instance_dir)) {
    ERROR("working directory not changed - %s\n", strerror(errno));
    return -1;
  }

  if (pthread_cond_init(&zl_context.event_cv, NULL) != 0) {
    ERROR("pthread_cond_init() error - %s\n", strerror(errno));
    return -1;
  }

  if (pthread_mutex_init(&zl_context.event_lock, NULL) != 0) {
    ERROR("pthread_mutex_init() error - %s\n", strerror(errno));
    return -1;
  }

  setenv("INSTANCE_DIR", zl_context.instance_dir, 1);
  INFO("instance directory is \'%s\'\n", zl_context.instance_dir);

  return 0;
}

static void get_yaml_pair_key(yaml_document_t *document, yaml_node_pair_t *pair, char *buf, size_t buf_size) {
  yaml_node_t *node = yaml_document_get_node(document, pair->key);
  if (node) {
    snprintf(buf, buf_size, "%.*s", (int)node->data.scalar.length, (const char *)node->data.scalar.value);
#ifdef __MVS__
    __atoe(buf);
#endif
  } else {
    snprintf(buf, buf_size, "");
    DEBUG ("key node not found\n");
  }
}

static void get_yaml_item(yaml_document_t *document, yaml_node_item_t item, char *buf, size_t buf_size) {
  yaml_node_t *node = yaml_document_get_node(document, item);
  if (node) {
    snprintf(buf, buf_size, "%.*s", (int)node->data.scalar.length, (const char *)node->data.scalar.value);
#ifdef __MVS__
    __atoe(buf);
#endif
  } else {
    snprintf(buf, buf_size, "");
    DEBUG ("no node for item %d\n", item);
  }
}

static void get_yaml_scalar(yaml_document_t *doc, yaml_node_t *node, char *buf, size_t buf_size) {
  char *value = (char *)node->data.scalar.value;
  snprintf(buf, buf_size, "%s", value);
#ifdef __MVS__
  __atoe(buf);
#endif
}

static yaml_node_t *get_child_node(yaml_document_t *doc, yaml_node_t *node, const char *name) {
  char key[ZL_YAML_KEY_LEN + 1];
  yaml_node_t *value_node = NULL;
  for (yaml_node_pair_t *pair = node->data.mapping.pairs.start; pair != node->data.mapping.pairs.top; pair++) {
    get_yaml_pair_key(doc, pair, key, sizeof(key));
    if (0 == strcmp(key, name)) {
      value_node = yaml_document_get_node(doc, pair->value);
      break;
    }
  }
  return value_node;
}

static void get_int_array_from_yaml_sequence(yaml_document_t *doc, yaml_node_t *node, zl_int_array_t *data) {
  char buf[ZL_YAML_KEY_LEN + 1];
  for (yaml_node_item_t *item = node->data.sequence.items.start; item != node->data.sequence.items.top; item++) {
    get_yaml_item(doc, *item, buf, sizeof(buf));
    if (data->count < ZL_INT_ARRAY_CAPACITY) {
      data->data[data->count++] = atoi(buf);
    } else {
      WARN ("yaml sequence is too large\n");
      break;
    }
  }
}

static yaml_node_t *get_node_by_path(yaml_document_t *doc, yaml_node_t *node, const char **path, size_t path_len) {
  for (size_t i = 0; i < path_len; i++) {
    node = get_child_node(doc, node, path[i]);
    if (!node) {
      break;
    }
  }
  return node;
}

static int get_int_by_path(yaml_document_t *doc, yaml_node_t *root, const char **path, size_t path_len, int *result) {
  yaml_node_t *node = get_node_by_path(doc, root, path, path_len);
  if (node && node->type == YAML_SCALAR_NODE) {
    char buf[128];
    get_yaml_scalar(doc, node, buf, sizeof(buf));
    *result = atoi(buf);
    return 0;
  }
  return -1;
}

static int get_string_by_yaml_path(yaml_document_t *doc, yaml_node_t *root, const char **path, size_t path_len, char *buf, int buf_size) {
  yaml_node_t *node = get_node_by_path(doc, root, path, path_len);
  if (node && node->type == YAML_SCALAR_NODE) {
    get_yaml_scalar(doc, node, buf, buf_size);
    return 0;
  }
  return -1;
}

static int get_int_array_by_path(yaml_document_t *doc, yaml_node_t *root, const char **path, size_t path_len, zl_int_array_t *arr) {
  yaml_node_t *node = get_node_by_path(doc, root, path, path_len);
  if (node && node->type == YAML_SEQUENCE_NODE) {
    get_int_array_from_yaml_sequence(doc, node, arr);
    return 0;
  }
  return -1;
}

static void snprint_int_array(zl_int_array_t *array, char *buf, size_t buf_size) {
  int pos = 0;
  for (int i = 0; i < array->count; i++) {
    pos += snprintf(buf + pos, buf_size - pos, "%d%s", array->data[i], i == array->count - 1 ? "" : " ");
  }
}

static void init_component_restart_intervals(zl_comp_t *comp) {
  zl_yaml_config_t *zowe_yaml_config = &zl_context.yaml_config;
  yaml_document_t *document = &zowe_yaml_config->document;
  DEBUG ("loading restart intervals for component '%s'\n", comp->name);
  yaml_node_t *root = zowe_yaml_config->root;
  const char *zowe_path[] = {"zowe", "launcher", "restartIntervals"};
  const char *component_path[] = {"components", comp->name, "launcher", "restartIntervals"};
  const char *instance_path[] = {"haInstances", zl_context.ha_instance_id, "components", comp->name, "launcher", "restartIntervals"};
  bool found = false;
  if (root) {
    if (!found && get_int_array_by_path(document, root, instance_path, sizeof(instance_path)/sizeof(instance_path[0]), &comp->restart_intervals) == 0) {
      found = true;
    }
    if (!found && get_int_array_by_path(document, root, component_path, sizeof(component_path)/sizeof(component_path[0]), &comp->restart_intervals) == 0) {
      found = true;
    }
    if (!found && get_int_array_by_path(document, root, zowe_path, sizeof(zowe_path)/sizeof(zowe_path[0]), &comp->restart_intervals) == 0) {
      found = true;
    }
  }
  if (!found) {
    memcpy(&comp->restart_intervals, restart_intervals_default, sizeof(restart_intervals_default));
    comp->restart_intervals.count = sizeof(restart_intervals_default)/sizeof(restart_intervals_default[0]);
  }
}

static void init_component_min_uptime(zl_comp_t *comp) {
  zl_yaml_config_t *zowe_yaml_config = &zl_context.yaml_config;
  yaml_document_t *document = &zowe_yaml_config->document;
  yaml_node_t *root = zowe_yaml_config->root;
  const char *zowe_path[] = {"zowe", "launcher", "minUptime"};
  const char *component_path[] = {"components", comp->name, "launcher", "minUptime"};
  const char *instance_path[] = {"haInstances", zl_context.ha_instance_id, "components", comp->name, "launcher", "minUptime"};
  bool found = false;
  int min_uptime = 0;
  if (root) {
    if (!found && get_int_by_path(document, root, instance_path, sizeof(instance_path)/sizeof(instance_path[0]), &min_uptime) == 0) {
      found = true;
    }
    if (!found && get_int_by_path(document, root, component_path, sizeof(component_path)/sizeof(component_path[0]), &min_uptime) == 0) {
      found = true;
    }
    if (!found && get_int_by_path(document, root, zowe_path, sizeof(zowe_path)/sizeof(zowe_path[0]), &min_uptime) == 0) {
      found = true;
    }
  }
  comp->min_uptime = found ? min_uptime : MIN_UPTIME_SECS;
}

static void init_component_shareas(zl_comp_t *comp) {
  zl_yaml_config_t *zowe_yaml_config = &zl_context.yaml_config;
  yaml_document_t *document = &zowe_yaml_config->document;
  yaml_node_t *root = zowe_yaml_config->root;
  const char *zowe_path[] = {"zowe", "launcher", "shareAs"};
  const char *component_path[] = {"components", comp->name, "launcher", "shareAs"};
  const char *instance_path[] = {"haInstances", zl_context.ha_instance_id, "components", comp->name, "launcher", "shareAs"};
  bool found = false;
  char share_as[128] = {0};
  if (root) {
    if (!found && get_string_by_yaml_path(document, root, instance_path, sizeof(instance_path)/sizeof(instance_path[0]), share_as, sizeof(share_as)) == 0) {
      found = true;
    }
    if (!found && get_string_by_yaml_path(document, root, component_path, sizeof(component_path)/sizeof(component_path[0]), share_as, sizeof(share_as)) == 0) {
      found = true;
    }
    if (!found && get_string_by_yaml_path(document, root, zowe_path, sizeof(zowe_path)/sizeof(zowe_path[0]), share_as, sizeof(share_as)) == 0) {
      found = true;
    }
  }
  if (!strcmp(share_as, "no")) {
    comp->share_as = ZL_COMP_AS_SHARE_NO;
  } else if (!strcmp(share_as, "yes")) {
    comp->share_as = ZL_COMP_AS_SHARE_YES;
  } else if (!strcmp(share_as, "must")) {
    comp->share_as = ZL_COMP_AS_SHARE_MUST;
  } else {
    comp->share_as = ZL_COMP_AS_SHARE_NO;
  }
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

static int init_component(const char *name, zl_comp_t *result) {
  snprintf(result->name, sizeof(result->name), "%s", name);
  result->pid = -1;
  init_component_shareas(result);
  init_component_restart_intervals(result);
  init_component_min_uptime(result);
  
  INFO("new component init'd \'%s\', restart_cnt=%d, min_uptime=%d seconds, share_as=%s\n", 
       result->name, result->restart_intervals.count,
       result->min_uptime, get_shareas_label(result));

  char restart_intervals_buf[2048];
  snprint_int_array(&result->restart_intervals, restart_intervals_buf, sizeof(restart_intervals_buf));
  INFO("restart_intervals for component '%s'= %s\n", result->name, restart_intervals_buf);
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

static int init_components(char *components) {
  if (!components) {
    ERROR("components to launch not set\n");
    return -1;
  }
  char *name = strtok(components, ",");

  while(name != NULL) {
    zl_comp_t comp = { 0 };
    init_component(name, &comp);
    if (zl_context.child_count != MAX_CHILD_COUNT) {
      zl_context.children[zl_context.child_count++] = comp;
    } else {
      ERROR("max component number reached, ignoring the rest\n");
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
      INFO("component %s(%d) terminated, status = %d\n",
           comp->name, comp->pid, comp_status);
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
          INFO("next attempt to restart component %s in %d seconds\n", comp->name, (int)delay);
          sleep(delay);
          send_event(ZL_EVENT_COMP_RESTART, comp);
        } else {
          ERROR("failed to restart component %s, max retries reached\n", comp->name);
        }
      }

      break;
    } else if (wait_rc == -1) {
      ERROR("waitpid failed for %s(%d) - %s\n",
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
        ERROR("cannot read output from comp %s(%d) failed - %s\n",
              comp->name, comp->pid, strerror(errno));
      }

    }

  }

  return NULL;
}

static int start_component(zl_comp_t *comp) {

  if (comp->pid != -1) {
    ERROR("cannot start component %s - already running\n", comp->name);
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
    ERROR("pipe() failed for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  if (fcntl(c_stdout[0], F_SETFL, O_NONBLOCK)) {
    ERROR("fcntl() failed for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  int fd_count = 3;
  int fd_map[3];
  char bin[PATH_MAX];

  snprintf(bin, sizeof(bin), "%s/bin/internal/start-component.sh", zl_context.root_dir);
  script = fopen(bin, "r");
  if (script == NULL) {
    ERROR("script not open for %s - %s\n", comp->name, strerror(errno));
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
    "-c", zl_context.instance_dir,
    "-r", zl_context.root_dir,
    "-i", zl_context.ha_instance_id,
    "-o", comp->name, 
    NULL
  };
  comp->pid = spawn(bin, fd_count, fd_map, &inherit, c_args, c_envp);
  if (comp->pid == -1) {
    ERROR("spawn() failed for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  comp->start_time = time(NULL);
  comp->output = c_stdout[0];
  close(c_stdout[1]);

  comp->clean_stop = false;

  INFO(MSG_COMPONENT_STARTED, comp->name);

  if (pthread_create(&comp->comm_thid, NULL, handle_comp_comm, comp) != 0) {
    ERROR("comm thread not started for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  return 0;
}

static int start_components(void) {

  INFO("starting components\n");

  int rc = 0;

  for (size_t i = 0; i < zl_context.child_count; i++) {
    if (start_component(&zl_context.children[i])) {
      rc = -1;
    }
  }

  if (rc) {
    WARN("not all components started\n");
  } else {
    INFO("components started\n");
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
  if (!kill(pgid, SIGTERM)) {

    if (pthread_join(comp->comm_thid, NULL) != 0) {
      ERROR("pthread_join() failed for %s comm thread - %s\n",
            comp->name, strerror(errno));
      return -1;
    }

  } else {
    ERROR("kill() failed for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  comp->pid = -1;
  INFO(MSG_COMPONENT_STOPPED, comp->name);

  return 0;
}

static int stop_components(void) {

  INFO("stopping components\n");

  int rc = 0;

  for (size_t i = 0; i < zl_context.child_count; i++) {
    if (stop_component(&zl_context.children[i])) {
      rc = -1;
    }
  }

  if (rc) {
    WARN("not all components stopped\n");
  } else {
    INFO("components stopped\n");
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
    WARN("component %s not found\n", comp_name);
    return -1;
  }

  comp->fail_cnt = 0;
  start_component(comp);

  return 0;
}

static int handle_stop(const char *comp_name) {

  zl_comp_t *comp = find_comp(comp_name);
  if (comp == NULL) {
    WARN("component %s not found\n", comp_name);
    return -1;
  }

  stop_component(comp);

  return 0;
}

static int handle_disp(void) {

  INFO("launcher has the following components:\n");
  for (size_t i = 0; i < zl_context.child_count; i++) {
    INFO("    name = %16.16s, PID = %d\n", zl_context.children[i].name,
         zl_context.children[i].pid);
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

  INFO("starting console listener\n");

  while (true) {

    struct __cons_msg2 cons = {0};
    cons.__cm2_format = __CONSOLE_FORMAT_3;

    char mod_cmd[128] = {0};
    int cmd_type = 0;

    if (__console2(&cons, mod_cmd, &cmd_type)) {
      ERROR("__console2() - %s\n", strerror(errno));
      pthread_exit(NULL);
    }

    if (cmd_type == _CC_modify) {

      INFO("command \'%s\' received\n", mod_cmd);

      char cmd_val[128] = {0};

      if (strstr(mod_cmd, CMD_START) == mod_cmd) {
        char *val = get_cmd_val(mod_cmd, cmd_val, sizeof(cmd_val));
        if (val != NULL) {
          handle_start(val);
        } else {
          ERROR("bad value, command ignored\n");
        }
      } else if (strstr(mod_cmd, CMD_STOP) == mod_cmd) {
        char *val = get_cmd_val(mod_cmd, cmd_val, sizeof(cmd_val));
        if (val != NULL) {
          handle_stop(val);
        } else {
          ERROR("bad value, command ignored\n");
        }
      } else if (strstr(mod_cmd, CMD_DISP) == mod_cmd) {
        handle_disp();
      } else {
        WARN("command not recognized\n");
      }

    } else if (cmd_type == _CC_stop) {
      INFO("termination command received\n");
      send_event(ZL_EVENT_TERM, NULL);
      break;
    }

  }

  INFO("console listener stopped\n");

  return NULL;
}

static int start_console_tread(void) {

  INFO("starting console thread\n");

  if (pthread_create(&zl_context.console_thid, NULL, handle_console, NULL) != 0) {
    ERROR("pthread_created() for console listener - %s\n", strerror(errno));
    return -1;
  }

  return 0;
}

static int stop_console_thread(void) {

  if (pthread_join(zl_context.console_thid, NULL) != 0) {
    ERROR("pthread_join() for console listener - %s\n", strerror(errno));
    return -1;
  }

  INFO("console thread stopped\n");

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
    ERROR("monitor_events: pthread_mutex_lock() error - %s\n", strerror(errno));
    return;
  }

  while (true) {

    while (zl_context.event_type == ZL_EVENT_NONE) {
      if (pthread_cond_wait(&zl_context.event_cv, &zl_context.event_lock) !=0) {
        ERROR("monitor_events: pthread_cond_wait() error - %s\n",
              strerror(errno));
        return;
      }
    }

    DEBUG("event with type %d and data 0x%p has been received\n",
          zl_context.event_type, zl_context.event_data);

    if (zl_context.event_type == ZL_EVENT_TERM) {
      break;
    } else if (zl_context.event_type == ZL_EVENT_COMP_RESTART) {
      int restart_rc = restart_component(zl_context.event_data);
      if (restart_rc) {
        ERROR("component not restarted, rc = %d\n", restart_rc);
      }
    } else {
      ERROR("unknown event type %d\n", zl_context.event_type);
      break;
    }

    zl_context.event_type = ZL_EVENT_NONE;
    zl_context.event_data = NULL;

  }

  if (pthread_mutex_unlock(&zl_context.event_lock) != 0) {
    ERROR("monitor_events: pthread_mutex_unlock() error - %s\n",
          strerror(errno));
    return;
  }

}

static int send_event(enum zl_event_t event_type, void *event_data) {

  if (pthread_mutex_lock(&zl_context.event_lock) != 0) {
    ERROR("send_event: pthread_mutex_lock() error - %s\n", strerror(errno));
    return -1;
  }

  zl_context.event_type = event_type;
  zl_context.event_data = event_data;

  if (pthread_cond_signal(&zl_context.event_cv) != 0) {
    ERROR("send_event: pthread_cond_signal() error - %s\n", strerror(errno));
    return -1;
  }

  DEBUG("event with type %d and data 0x%p has been sent\n",
        zl_context.event_type, zl_context.event_data);

  if (pthread_mutex_unlock(&zl_context.event_lock) != 0) {
    ERROR("send_event: pthread_mutex_unlock() error - %s\n", strerror(errno));
    return -1;
  }

  return 0;
}

typedef void (*handle_line_callback_t)(void *data, const char *line);

static int run_command(const char *command, handle_line_callback_t handle_line, void *data) {
  DEBUG("about to run command '%s'\n", command);
  FILE *fp = popen(command, "r");
  if (!fp) {
    ERROR("failed to run command %s - %s\n", command, strerror(errno));
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
    ERROR("error reading output from command '%s' - %s\n", command, strerror(errno));
    return -1;
  }
  int rc = pclose(fp);
  if (rc == -1) {
    ERROR("failed to run command '%s' - %s\n", command, strerror(errno));
  } else if (rc > 0) {
    ERROR("command '%s' ended with code %d\n", command, rc);
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
  snprintf (command, sizeof(command), "%s/bin/internal/get-launch-components.sh -c %s -r %s -i %s",
            zl_context.root_dir, zl_context.instance_dir,
            zl_context.root_dir, zl_context.ha_instance_id);
  DEBUG("about to get component list\n");
  char comp_list[COMP_LIST_SIZE] = {0};
  if (run_command(command, handle_get_component_line, (void*)comp_list)) {
    ERROR("failed to get component list\n");
  }
  if (strlen(comp_list) == 0) {
    ERROR("start component list is empty\n");
    return -1;
  }
  snprintf(buf, buf_size, "%s", comp_list);
  INFO("start component list: '%s'\n", buf);
  return 0;
}

static void handle_get_root_dir_line(void *data, const char *line) {
  char *root_dir = data;
  snprintf(root_dir, PATH_MAX+1, "%s", line);
  int len = strlen(root_dir);
  for (int i = len - 1; i >= 0; i--) {
    if (root_dir[i] != ' ' && root_dir[i] != '\n') {
      break;
    }
    root_dir[i] = '\0';
  }
}

static int get_root_dir(char *buf, size_t buf_size) {
  char command[2*PATH_MAX];
  snprintf (command, sizeof(command), ". %s/bin/internal/read-essential-vars.sh && echo $ROOT_DIR",
    zl_context.instance_dir);
  DEBUG("about to get root dir\n");
  char root_dir[PATH_MAX+1] = {0};
  if (run_command(command, handle_get_root_dir_line, (void*)root_dir)) {
    ERROR("failed to ROOT_DIR dir\n");
  }
  if (strlen(root_dir) == 0) {
    ERROR("ROOT_DIR is empty\n");
    return -1;
  }
  snprintf(buf, buf_size, "%s", root_dir);
  if (check_if_dir_exists(zl_context.root_dir, "ROOT_DIR")) {
    return -1;
  }
  setenv("ROOT_DIR", zl_context.root_dir, 1);
  INFO("ROOT_DIR found: '%s'\n", buf);
  return 0;
}

static void print_line(void *data, const char *line) {
  printf("%s", line);
}

static int prepare_instance() {
  char command[4*PATH_MAX];
  INFO("about to prepare Zowe instance\n");
  const char *script = "bin/internal/prepare-instance.sh";
  snprintf(command, sizeof(command), "%s/%s -c %s -r %s -i %s 2>&1",
           zl_context.root_dir, script, zl_context.instance_dir,
           zl_context.root_dir, zl_context.ha_instance_id);
  if (run_command(command, print_line, NULL)) {
    ERROR("failed to prepare Zowe instance\n");
    return -1;
  }
  INFO("Zowe instance prepared successfully\n");
  return 0;
}

static int init() {
  zl_context.pid = getpid();
  char *login = __getlogin1();
  snprintf(zl_context.userid, sizeof(zl_context.userid), "%s", login);
  return 0;
}

static void terminate(int sig) {
  INFO("Zowe Launcher stopping\n");
  stop_components();
  exit(EXIT_SUCCESS);
}

static int setup_signal_handlers() {
  struct sigaction sa;
  sa.sa_handler = terminate;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    ERROR("failed to set SIGINT handler - %s\n", strerror(errno));
    return -1;
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    ERROR("failed to set SIGTERM handler - %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

static int yaml_read_handler(void *data, unsigned char *buffer, size_t size, size_t *size_read) {
  FILE *fp = data;
  int rc = 1;
  size_t bytes_read = fread(buffer, 1, size, fp);
  if (bytes_read > 0) {
#ifdef __MVS__
    if (__etoa_l((char *)buffer, bytes_read) == -1) {
      ERROR("error converting yaml file - %s\n", strerror(errno));
      rc = 0;
    }
#endif
  }
  if (ferror(fp)) {
    ERROR("error reading yaml file - %s\n", strerror(errno));
    rc = 0;
  }
  *size_read = bytes_read;
  return rc;
}

static int read_zowe_yaml_config() {
  zl_yaml_config_t *config = &zl_context.yaml_config;
  FILE *fp = NULL;
  yaml_parser_t parser;
  yaml_document_t *document = &config->document;
  int rc;
  char zowe_yaml_file[PATH_MAX];

  snprintf(zowe_yaml_file, sizeof(zowe_yaml_file), "%s/zowe.yaml", zl_context.instance_dir); 
  
  INFO("loading '%s'\n", zowe_yaml_file);

  fp = fopen(zowe_yaml_file, "r");
  if (!fp) {
    ERROR("failed to open zowe.yaml - %s: %s\n", zowe_yaml_file, strerror(errno));
    return -1;
  }
  if (!yaml_parser_initialize(&parser)) {
    ERROR("failed to init YAML parser\n");
    fclose(fp);
    return -1;
  };
  yaml_parser_set_input(&parser, yaml_read_handler, fp);
  if (!yaml_parser_load(&parser, document)) {
    ERROR("failed to parse zowe.yaml %s\n", zowe_yaml_file);
    yaml_parser_delete(&parser);
    fclose(fp);
    return -1;
  }
  yaml_node_t *root = yaml_document_get_root_node(document);
  do {
    if (!root) {
      ERROR("failed to get root node in zowe.yaml %s\n", zowe_yaml_file);
      rc = -1;
      break;
    }
    if (root->type != YAML_MAPPING_NODE) {
      ERROR("failed to find mapping node in zowe.yaml %s\n", zowe_yaml_file);
      rc = -1;
      break;
    }
    config->root = root; 
  } while(0);
  yaml_parser_delete(&parser);
  fclose(fp);
  return 0;
}

int main(int argc, char **argv) {
  if (init()) {
    exit(EXIT_FAILURE);
  }

  INFO("Zowe Launcher starting\n");

  zl_config_t config = read_config(argc, argv);

  if (init_context(argc, argv, &config)) {
    exit(EXIT_FAILURE);
  }
  
  if (get_root_dir(zl_context.root_dir, sizeof(zl_context.root_dir))) {
    exit(EXIT_FAILURE);
  }
  
  if (setup_signal_handlers()) {
    exit(EXIT_FAILURE);
  }

  if (read_zowe_yaml_config()) {
    WARN ("failed to read zowe.yaml, launcher will use default settings");
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

  if (init_components(component_list)) {
    exit(EXIT_FAILURE);
  }

  start_components();

  if (start_console_tread()) {
    exit(EXIT_FAILURE);
  }

  monitor_events();

  if (stop_console_thread()) {
    exit(EXIT_FAILURE);
  }

  stop_components();

  INFO("Zowe Launcher stopped\n");

  exit(EXIT_SUCCESS);
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
