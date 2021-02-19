
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
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/__messag.h>
#include <unistd.h>

#include "launcher.h"

/*
 * TODO:
 * - Better process monitoring and clean up. For example, can we find all the
 * child processes of a component? How do we clean up the forks of a
 * killed process?
 * - a REST endpoint? Zowe CLI?
 */

zl_time_t gettime(void) {

  time_t t = time(NULL);
  const char *format = "%Y-%m-%d %H:%M:%S";

  struct tm lt;
  zl_time_t result;

  localtime_r(&t, &lt);

  strftime(result.value, sizeof(result.value), format, &lt);

  return result;
}

struct zl_context_t zl_context = {.config = {.debug_mode = true}};

static int init_context(const struct zl_config_t *cfg) {

  const char *instance_dir = getenv("INSTANCE_DIR");
  if (instance_dir == NULL) {
    ERROR("INSTANCE_DIR env variable not found\n");
    return -1;
  }

  const char *dir_start = instance_dir;
  const char *dir_end = dir_start + strlen(instance_dir) - 1;
  while (*dir_end == ' ' && dir_end != dir_start) {
    dir_end--;
  }

  size_t dir_len = dir_end - dir_start + 1;
  if (dir_len > sizeof(zl_context.instance_dir) - 1) {
    ERROR("INSTANCE_DIR env too large\n");
    return -1;
  }

  memset(zl_context.instance_dir, 0, sizeof(zl_context.instance_dir));
  memcpy(zl_context.instance_dir, dir_start, dir_len);
  zl_context.config = *cfg;

  size_t env_var_count = zl_context.env_var_count;
  snprintf(zl_context.environment[env_var_count],
           sizeof(zl_context.environment[0]),
           "INSTANCE_DIR=%s",
           zl_context.instance_dir);
  zl_context.env_var_count++;

  if (load_instance_dot_env(zl_context.instance_dir)) {
    ERROR("failed to load instance.env\n");
    return -1;
  }
  
  char *root_dir = getenv("ROOT_DIR");
  if (!root_dir) {
    ERROR("ROOT_DIR not set by instance.env\n");
    return -1;
  }
  snprintf(zl_context.root_dir, sizeof(zl_context.root_dir), "%s", root_dir);

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

  DEBUG("instance directory is \'%s\'\n", zl_context.instance_dir);
  DEBUG("root directory is \'%s\'\n", zl_context.root_dir);

  return 0;
}

static int init_component_from_manifest(zl_manifest_t *manifest, const char *component_dir, zl_comp_t *result) {
  if (strlen(manifest->commands.start) == 0) {
    WARN("component %s doesn't have start command use default start command\n", manifest->name);
    snprintf (manifest->commands.start, sizeof(manifest->commands.start), "%s", "bin/start.sh");
  }
  snprintf(result->bin, sizeof(result->bin), "%s/%s", component_dir, manifest->commands.start);
  snprintf(result->name, sizeof(result->name), "%s", manifest->name);
  result->pid = -1;
  result->share_as = ZL_COMP_AS_SHARE_NO;
  result->manifest = *manifest;
  result->restart_cnt = 5;
  
  INFO("new component init'd \'%s\', \'%s\', restart_cnt=%d, share_as=%d\n",
       result->name, result->bin, result->restart_cnt, result->share_as);

  return 0;
}

static int init_component(const char *name, zl_comp_t *result) {
  snprintf(result->name, sizeof(result->name), "%s", name);
  result->pid = -1;
  result->share_as = ZL_COMP_AS_SHARE_NO;
  result->restart_cnt = 5;
  
  INFO("new component init'd \'%s\', restart_cnt=%d, share_as=%d\n",
       result->name, result->restart_cnt, result->share_as);

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

/*static */int read_component_manifests() {
  char *root_dir = zl_context.root_dir;
  char path[strlen(root_dir) + strlen("/components") + 1];
  strcpy(path, root_dir);
  strcat(path, "/components");
  DIR *components_dir = opendir(path);
  if (!components_dir) {
    ERROR("unable to open components directory %s\n", path);
    return -1;
  }
  DEBUG("components dir %s opened\n", path);
  struct dirent *entry;
  struct stat    file_stat;
  char           dir_path[PATH_MAX + 1];
  char           manifest_path[PATH_MAX + 1];
  while ((entry = readdir(components_dir)) != NULL) {
    if (entry->d_name[0] == '.') {
      continue;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/%s", path, entry->d_name);
    if (stat(dir_path, &file_stat)) {
      WARN("unable get info about %s\n", dir_path);
      continue;
    };
    if (!S_ISDIR(file_stat.st_mode)) {
      continue;
    }
    snprintf(manifest_path, sizeof(manifest_path), "%s/manifest.yaml", dir_path);
    zl_manifest_t manifest = {0};
    if (load_manifest(manifest_path, &manifest)) {
      WARN("failed to load component manifest file %s\n", manifest_path);
      continue;
    }
    zl_comp_t comp = {0};
    if (!init_component_from_manifest(&manifest, dir_path, &comp)) {
      if (zl_context.child_count != MAX_CHILD_COUNT) {
        zl_context.children[zl_context.child_count++] = comp;
      } else {
        ERROR("max component number reached, ignoring the rest\n");
        break;
      }
    }
  }
  closedir(components_dir);
  return 0;
}
static int init_components() {
  char *components = getenv("LAUNCH_COMPONENTS");
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

  INFO("starting a component communication thread\n");

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
      if (!comp->clean_stop && (comp->fail_cnt < comp->restart_cnt)) {
        send_event(ZL_EVENT_COMP_RESTART, comp);
      }

      break;
    } else if (wait_rc == -1) {
      ERROR("waitpid failed for %s(%d) - %s\n",
            comp->name, comp->pid, strerror(errno));
      break;
    } else {
      DEBUG("waitpid RC = 0 for %s(%d)\n", comp->name, comp->pid);
    }

    char msg[1024];
    int retries_left = 3;
    while (retries_left > 0) {

      int msg_len = read(comp->output, msg, sizeof(msg));
      if (msg_len > 0) {
        msg[msg_len] = '\0';

        char *next_line = strtok(msg, "\n");

        while (next_line) {
          char *tm = gettime().value;
          printf("%s CMSG:  %s(%d) - %s\n", tm, comp->name, comp->pid, next_line);
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

  DEBUG("about to start component %s at \'%s\'\n", comp->name, comp->bin);

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
  snprintf(bin, sizeof(bin), "%s/bin/internal/start-component-with-launcher.sh", getenv("ROOT_DIR"));
  // const char *bin = comp->bin;
  int bin_len = strlen(bin);
  
  if (strcmp(&bin[bin_len - 3], ".sh") == 0) {
    script = fopen(bin, "r");
    if (script == NULL) {
      ERROR("script not open for %s - %s\n", comp->name, strerror(errno));
      return -1;
    }
    fd_map[0] = dup(fileno(script));
    fclose(script);
  } else {
    fd_map[0] = dup(STDIN_FILENO);
  }
  fd_map[1] = dup(c_stdout[1]);
  fd_map[2] = dup(c_stdout[1]);

  DEBUG("%s fd_map[0]=%d, fd_map[1]=%d, fd_map[2]=%d\n",
        comp->name, fd_map[0], fd_map[1], fd_map[2]);

  const char *c_envp[zl_context.env_var_count+2];
  size_t i;
  for (i = 0; i < zl_context.env_var_count; i++) {
    c_envp[i] = zl_context.environment[i];
  }
  c_envp[i++] = get_shareas_env(comp);
  c_envp[i++] = NULL;
  const char *c_args[] = {
    bin,
    "-c", getenv("INSTANCE_DIR"),
    "-r", getenv("ROOT_DIR"), 
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

  INFO("process with PID = %d started for comp %s\n", comp->pid, comp->name);

  if (pthread_create(&comp->comm_thid, NULL, handle_comp_comm, comp) != 0) {
    ERROR("comm thread not started for %s - %s\n", comp->name, strerror(errno));
    return -1;
  }

  return 0;
}

static int start_components(void) {

  INFO("starting components\n");

  int rc = 0;
  char *components_to_start = getenv("LAUNCH_COMPONENTS");
  INFO("components to start %s\n", components_to_start);
  for (size_t i = 0; i < zl_context.child_count; i++) {
    // zl_comp_t *comp = &zl_context.children[i];
    // if (!strstr(components_to_start, comp->name)) {
    //   INFO("skip component %s\n", comp->name);
    //   continue;
    // }
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
  INFO("component %s stopped\n", comp->name);

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

  printf ("handle start %s\n", comp_name);
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

int main(int argc, char **argv) {

  INFO("Zowe Launcher starting\n");

  zl_config_t config = read_config(argc, argv);

  if (init_context(&config)) {
    exit(EXIT_FAILURE);
  }
  
  if (init_components()) {
    exit(EXIT_FAILURE);
  }
  
  // if (read_component_manifests()) {
  //   exit(EXIT_FAILURE);
  // }

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
