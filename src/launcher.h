/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <time.h>

#ifndef PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX
#endif

#define CONFIG_DEBUG_MODE_KEY     "ZLDEBUG"
#define CONFIG_DEBUG_MODE_VALUE   "ON"

#define MIN_UPTIME_SECS 90

#ifndef PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX
#endif

typedef struct zl_time_t {
  char value[32];
} zl_time_t;

zl_time_t gettime(void);

typedef struct zl_config_t {
  bool debug_mode;
} zl_config_t;

typedef struct zl_manifest_commands_t {
  char start[PATH_MAX+1];
  char configure[PATH_MAX+1];
  char validate[PATH_MAX+1];
} zl_manifest_commands_t;

typedef struct zl_manifest_build_t {
  char branch[15+1];
  char number[15+1];
  char commit_hash[40+1];
  char timestamp[20+1];
} zl_manifest_build_t;

typedef struct zl_manifest_t {
  char name[24+1];
  char id[16+1];
  char title[40+1];
  char description[128+1];
  char license[16+1];
  zl_manifest_commands_t commands;
  zl_manifest_build_t build;
} zl_manifest_t;

typedef struct zl_comp_t {

  char name[32];
  char bin[_POSIX_PATH_MAX + 1];
  pid_t pid;
  int output;

  bool clean_stop;
  int restart_cnt;
  int fail_cnt;
  time_t start_time;

  enum {
    ZL_COMP_AS_SHARE_NO,
    ZL_COMP_AS_SHARE_YES,
    ZL_COMP_AS_SHARE_MUST,
  } share_as;

  pthread_t comm_thid;
  zl_manifest_t manifest;

} zl_comp_t;

enum zl_event_t {
  ZL_EVENT_NONE = 0,
  ZL_EVENT_TERM,
  ZL_EVENT_COMP_RESTART,
};

struct zl_context_t {

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
  
#define MAX_ENV_VAR_COUNT 256
#define ENV_VAR_SIZE 128
  char environment[MAX_ENV_VAR_COUNT][ENV_VAR_SIZE];
  size_t env_var_count;
};

extern struct zl_context_t zl_context;

#define INFO(fmt, ...)  printf("%s INFO:  "fmt, gettime().value, ##__VA_ARGS__)
#define WARN(fmt, ...)  printf("%s WARN:  "fmt, gettime().value, ##__VA_ARGS__)
#define DEBUG(fmt, ...) if (zl_context.config.debug_mode) \
  printf("%s DEBUG: "fmt, gettime().value, ##__VA_ARGS__)
#define ERROR(fmt, ...) printf("%s ERROR: "fmt, gettime().value, ##__VA_ARGS__)

int load_instance_dot_env(const char *instance_dir);
int load_manifest(const char *filename, zl_manifest_t *manifest);

#endif // LAUNCHER_H

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
