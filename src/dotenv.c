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
#include <errno.h>
#include <time.h>
#include "launcher.h"

int load_instance_dot_env(const char *instance_dir) {
  
  char command[PATH_MAX];
  snprintf (command, sizeof(command), "%s/bin/internal/save-env.sh", getenv("ROOT_DIR"));
  int rc;
  if ((rc = system(command)) != 0) {
    ERROR("save-env exited with %d\n", rc);
    return -1;
  }
  INFO("save-env successful\n");
  char path[PATH_MAX];
  snprintf (path, sizeof(path), "%s/instance-saved.env", instance_dir);
  FILE *fp;

  if ((fp = fopen(path, "r")) == NULL) {
    ERROR("instace.env %s file not open - %s\n", path, strerror(errno));
    return -1;
  }
  INFO("%s opened\n", path);

  char *line;
  char buf[1024];
  char key[sizeof(buf)];
  char value[sizeof(buf)];

  while ((line = fgets(buf, sizeof(buf), fp)) != NULL) {
    size_t len = strlen(line);
    if (len > 0 && line[len-1] == '\n') {
      line[len-1] = '\0';
    }
    DEBUG("handling line \'%s\'\n", line);
    char *hash = strchr(line, '#');
    if (hash) {
      *hash = '\0';
    }
    for (int i = strlen(line) - 1; i >= 0; i--) {
      if (line[i] != ' ') {
        break;
      } else {
        line[i] = '\0';
      }
    }
    char *equal = strchr(line, '=');
    if (equal) {
      snprintf(key, sizeof(key), "%.*s", (int)(equal - line), line);
      snprintf(value, sizeof(value), "%s", equal + 1);
      INFO("set env %s=%s\n", key, value);
      setenv(key, value, 1);
      if (zl_context.env_var_count < (MAX_ENV_VAR_COUNT - 1)) {
        snprintf(
          zl_context.environment[zl_context.env_var_count],
          sizeof(zl_context.environment[0]),
          "%s=%s", key, value);
          zl_context.env_var_count++;
      } else {
        ERROR("max environment variable number reached, ignoring the rest\n");
        break;
      }
    }
    
  }

  DEBUG("reading instance-saved.env finished - %s\n", strerror(errno));

  fclose(fp);
  return 0;
}

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
