
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
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "yaml.h"

void dump_mapping(yaml_node_t *node) {
  yaml_node_pair_t *start = node->data.mapping.pairs.start;
  yaml_node_pair_t *end = node->data.mapping.pairs.end;
  printf("[\n");
  printf(" mapping start %d %d\n", start->key, start->value);
  printf(" mapping end %d %d\n", end->key, end->value);
  printf("]\n");
}

void indent(int margin, char *format, ...) {
  for (int i = 0; i < margin; i++) {
    printf(" ");
  }
  va_list ptr;
  va_start(ptr, format);
  vprintf(format, ptr);
  va_end(ptr);
}

static int yaml_read_handler(void *data, unsigned char *buffer, size_t size, size_t *size_read) {
  FILE *fp = data;
  int rc = 1;
  size_t bytes_read = fread(buffer, 1, size, fp);
  if (bytes_read > 0) {
    if (__etoa_l((char*)buffer, bytes_read) == -1) {
      printf ("error etoa %s\n", strerror(errno));
      rc = 0;
    }
  }
  if (ferror(fp)) {
    rc = 0;
  }
  *size_read = bytes_read;
  return rc;
}

int test() {
  yaml_parser_t   parser = {0};
  yaml_emitter_t  emitter = {0};
  yaml_event_t    event = {0};
  FILE *          fp = NULL;
  const char *filename = "/u/ts3105/zowe-installation/components/explorer-jes/manifest.yaml";

  do {
    if (!yaml_parser_initialize(&parser)) {
      printf("unable to init yaml parser\n");
      break;
    }
    if (!yaml_emitter_initialize(&emitter)) {
      printf("unable to init yaml emitter\n");
      break;
    }
    fp = fopen(filename, "r");
    if (!fp) {
      printf("unable to open %s: %s\n", filename, strerror(errno));
      break;
    }
    yaml_parser_set_input(&parser, yaml_read_handler, fp);
    yaml_emitter_set_output_file(&emitter, stdout);
    yaml_emitter_set_canonical(&emitter, 1);
    yaml_emitter_set_unicode(&emitter, 0);
    printf("about to parse yaml\n");
    int done = 0;
    int margin = 0;
    bool isKey = true;
    bool inMap = false;
    while (!done) {
      if (!yaml_parser_parse(&parser, &event)) {
        printf("error parsing yaml\n");
        break;
      }
      switch (event.type) {
        case YAML_STREAM_START_EVENT:
          indent(margin+=2, "STREAM_START\n");
          break;
        case YAML_STREAM_END_EVENT:
          indent(margin-=2, "STREAM_END\n");
          break;
        case YAML_DOCUMENT_START_EVENT:
          indent(margin+=2, "DOCUMENT_START\n");
          break;
        case YAML_DOCUMENT_END_EVENT:
          indent(margin-=2, "DOCUMENT_END\n");
          break;
        case YAML_MAPPING_START_EVENT:
          indent(margin+=2, "MAPPING_START\n");
          inMap = true;
          isKey = true;
          break;
        case YAML_MAPPING_END_EVENT:
          indent(margin-=2, "MAPPING_END\n");
          inMap = false;
          break;
        case YAML_SEQUENCE_START_EVENT:
          indent(margin+=2, "SEQUENCE_START\n");
          break;
        case YAML_SEQUENCE_END_EVENT:
          indent(margin-=2, "SEQUENCE_END\n");
          break;
        case YAML_SCALAR_EVENT: {
          char *value = (char*)event.data.scalar.value;
          __atoe(value);
          if (inMap) {
          indent(isKey ? margin : 0, "%s '%s'%c", isKey ? "key" : "value", value,isKey ? ' ' : '\n');
          isKey = !isKey;
          } else {
            indent(margin, "not in map '%s'\n", value);
          }
          if (strcmp((const char*)value, "start") == 0) {
            printf ("start found\n");
          }
        }
          break;
          
        default:
          indent(margin, "event type %d\n", event.type);
          break;
      }

      done = (event.type == YAML_STREAM_END_EVENT);
      yaml_event_delete(&event);
    }

  } while (0);

  yaml_parser_delete(&parser);
  yaml_emitter_delete(&emitter);
  if (fp) {
    fclose(fp);
  }

  return 0;
}

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
