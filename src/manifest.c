
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
#include "launcher.h"

#define KEY_LEN 255

static int yaml_read_handler(void *data, unsigned char *buffer, size_t size, size_t *size_read) {
  FILE *fp = data;
  int rc = 1;
  size_t bytes_read = fread(buffer, 1, size, fp);
  if (bytes_read > 0) {
#ifdef __MVS__
    if (__etoa_l((char *)buffer, bytes_read) == -1) {
      ERROR("error converting manifest -%s\n", strerror(errno));
      rc = 0;
    }
#endif
  }
  if (ferror(fp)) {
    ERROR("error reading manifest %s\n", strerror(errno));
    rc = 0;
  }
  *size_read = bytes_read;
  return rc;
}

static void yaml_pair_copy_key(yaml_document_t *document, yaml_node_pair_t *pair, char *buf, size_t buf_size) {
  yaml_node_t *key_node = yaml_document_get_node(document, pair->key);
  snprintf(buf, buf_size, "%s", (const char *)key_node->data.scalar.value);
#ifdef __MVS__
  __atoe(buf);
#endif
}

inline static bool yaml_is_mapping_node(yaml_node_t *node) {
  return node && node->type == YAML_MAPPING_NODE;
}

inline static bool yaml_is_scalar_node(yaml_node_t *node) {
  return node && node->type == YAML_SCALAR_NODE;
}

static void copy_string_property(yaml_document_t *doc, yaml_node_pair_t *pair, char *buf, size_t buf_size) {
  yaml_node_t *node = yaml_document_get_node(doc, pair->value);
  char *value = NULL;
  if (yaml_is_scalar_node(node)) {
    value = (char *)node->data.scalar.value;
  }
  if (value) {
    snprintf(buf, buf_size, "%s", value);
#ifdef __MVS__
    __atoe(buf);
#endif
  }
}

static inline yaml_node_pair_t *start_pair(yaml_node_t *node) {
  return node->data.mapping.pairs.start;
}

static inline yaml_node_pair_t *end_pair(yaml_node_t *node) {
  return node->data.mapping.pairs.top;
}

static void copy_manifest_commands(zl_manifest_commands_t *commands, yaml_document_t *doc, yaml_node_t *node) {
  char key[KEY_LEN + 1];
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    yaml_pair_copy_key(doc, pair, key, sizeof(key));
    if (0 == strcmp(key, "start")) {
      copy_string_property(doc, pair, commands->start, sizeof(commands->start));
    } else if (0 == strcmp(key, "configure")) {
      copy_string_property(doc, pair, commands->configure, sizeof(commands->configure));
    } else if (0 == strcmp(key, "validate")) {
      copy_string_property(doc, pair, commands->validate, sizeof(commands->validate));
    }
  }
}

static void copy_manifest_build(zl_manifest_build_t *build, yaml_document_t *doc, yaml_node_t *node) {
  char key[KEY_LEN + 1];
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    yaml_pair_copy_key(doc, pair, key, sizeof(key));
    if (0 == strcmp(key, "branch")) {
      copy_string_property(doc, pair, build->branch, sizeof(build->branch));
    } else if (0 == strcmp(key, "number")) {
      copy_string_property(doc, pair, build->number, sizeof(build->number));
    } else if (0 == strcmp(key, "commitHash")) {
      copy_string_property(doc, pair, build->commit_hash, sizeof(build->commit_hash));
    } else if (0 == strcmp(key, "timestamp")) {
      copy_string_property(doc, pair, build->timestamp, sizeof(build->timestamp));
    }
  }
}

static void fill_manifest(zl_manifest_t *manifest, yaml_document_t *doc, yaml_node_t *node) {
  char key[KEY_LEN + 1];
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    yaml_pair_copy_key(doc, pair, key, sizeof(key));
    if (0 == strcmp(key, "name")) {
      copy_string_property(doc, pair, manifest->name, sizeof(manifest->name));
    } else if (0 == strcmp(key, "id")) {
      copy_string_property(doc, pair, manifest->id, sizeof(manifest->id));
    } else if (0 == strcmp(key, "title")) {
      copy_string_property(doc, pair, manifest->title, sizeof(manifest->title));
    } else if (0 == strcmp(key, "description")) {
      copy_string_property(doc, pair, manifest->description, sizeof(manifest->description));
    } else if (0 == strcmp(key, "license")) {
      copy_string_property(doc, pair, manifest->license, sizeof(manifest->license));
    } else if (0 == strcmp(key, "commands")) {
      yaml_node_t *value_node = yaml_document_get_node(doc, pair->value);
      copy_manifest_commands(&manifest->commands, doc, value_node);
    } else if (0 == strcmp(key, "build")) {
      yaml_node_t *value_node = yaml_document_get_node(doc, pair->value);
      copy_manifest_build(&manifest->build, doc, value_node);
    }
  }
}

int load_manifest(const char *manifest_file, zl_manifest_t *manifest) {
  FILE *fp = NULL;
  yaml_parser_t parser;
  yaml_document_t document;
  int rc;
  
  INFO("loading manifest '%s'\n", manifest_file);

  fp = fopen(manifest_file, "r");
  if (!fp) {
    ERROR("failed to open manifest - %s: %s\n", manifest_file, strerror(errno));
    return -1;
  }
  if (!yaml_parser_initialize(&parser)) {
    ERROR("failed to init YAML parser\n");
    fclose(fp);
    return -1;
  };
  yaml_parser_set_input(&parser, yaml_read_handler, fp);
  if (!yaml_parser_load(&parser, &document)) {
    ERROR("failed to parse manifest %s\n", manifest_file);
    yaml_parser_delete(&parser);
    fclose(fp);
    return -1;
  }
  yaml_node_t *root = yaml_document_get_root_node(&document);
  do {
    if (!root) {
      ERROR("failed to get root node in manifest %s\n", manifest_file);
      rc = -1;
      break;
    }
    if (!yaml_is_mapping_node(root)) {
      ERROR("failed to find mapping node in manifest %s\n", manifest_file);
      rc = -1;
      break;
    }
    fill_manifest(manifest, &document, root);
    INFO("manifest %s loaded - name: %s, id: %s, title: %s, description: %s, license: %s\n", manifest_file,
         manifest->name, manifest->id, manifest->title, manifest->description, manifest->license);
    DEBUG("manifest scripts - start: %s, validate: %s\n", manifest->commands.start, manifest->commands.validate);
    DEBUG("manifest build - branch: %s, number: %s, commitHash: %s, timestamp %s\n", manifest->build.branch,
          manifest->build.number, manifest->build.commit_hash, manifest->build.timestamp);
  } while(0);
  yaml_document_delete(&document);
  yaml_parser_delete(&parser);
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
