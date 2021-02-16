
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
#include <dirent.h>
#include <sys/stat.h>

#include "yaml.h"
#include "manifest.h"

int compare_with_ascii(const char *ebcdic, const char *ascii) {
  char dup[strlen(ebcdic) + 1];
  strcpy(dup, ebcdic);
  __etoa(dup);
  return strcmp(dup, ascii);
}

static int yaml_read_handler(void *data, unsigned char *buffer, size_t size, size_t *size_read) {
  FILE * fp = data;
  int    rc = 1;
  size_t bytes_read = fread(buffer, 1, size, fp);
  if (bytes_read > 0) {
    if (__etoa_l((char *)buffer, bytes_read) == -1) {
      printf("error etoa %s\n", strerror(errno));
      rc = 0;
    }
  }
  if (ferror(fp)) {
    rc = 0;
  }
  *size_read = bytes_read;
  return rc;
}

char *yaml_pair_get_key(yaml_document_t *document, yaml_node_pair_t *pair) {
  yaml_node_t *key_node = yaml_document_get_node(document, pair->key);
  return (char *)key_node->data.scalar.value;
}

inline bool yaml_is_mapping_node(yaml_node_t *node) {
  return node->type == YAML_MAPPING_NODE;
}

inline bool yaml_is_scalar_node(yaml_node_t *node) {
  return node->type == YAML_SCALAR_NODE;
}

inline bool yaml_is_sequence_node(yaml_node_t *node) {
  return node->type == YAML_SEQUENCE_NODE;
}

yaml_node_t *yaml_pair_get_value_node(yaml_document_t *document, yaml_node_pair_t *pair) {
  yaml_node_t *node = yaml_document_get_node(document, pair->value);
  return node;
}

char *yaml_pair_get_value_string(yaml_document_t *document, yaml_node_pair_t *pair) {
  yaml_node_t *node = yaml_document_get_node(document, pair->value);
  if (yaml_is_scalar_node(node)) {
    return (char *)node->data.scalar.value;
  }
  return NULL;
}

void copy_string_property_as_ebcdic(yaml_document_t *doc, yaml_node_pair_t *pair, char **out) {
  char *value = yaml_pair_get_value_string(doc, pair);
  if (value) {
    char *copy = strdup(value);
    __atoe(copy);
    *out = copy;
  }
}

inline yaml_node_pair_t *start_pair(yaml_node_t *node) {
  return node->data.mapping.pairs.start;
}

inline yaml_node_pair_t *end_pair(yaml_node_t *node) {
  return node->data.mapping.pairs.top;
}

void copy_manifest_commands(zl_manifest_commands_t *commands, yaml_document_t *doc, yaml_node_t *node) {
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    char *key = yaml_pair_get_key(doc, pair);
    char  ebcdic_key[strlen(key) + 1];
    strcpy(ebcdic_key, key);
    __atoe(ebcdic_key);
    if (0 == strcmp(ebcdic_key, "start")) {
      copy_string_property_as_ebcdic(doc, pair, &commands->start);
    } else if (0 == strcmp(ebcdic_key, "validate")) {
      copy_string_property_as_ebcdic(doc, pair, &commands->validate);
    }
  }
}

void copy_manifest_build(zl_manifest_build_t *build, yaml_document_t *doc, yaml_node_t *node) {
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    char *key = yaml_pair_get_key(doc, pair);
    char  ebcdic_key[strlen(key) + 1];
    strcpy(ebcdic_key, key);
    __atoe(ebcdic_key);
    if (0 == strcmp(ebcdic_key, "branch")) {
      copy_string_property_as_ebcdic(doc, pair, &build->branch);
    } else if (0 == strcmp(ebcdic_key, "number")) {
      copy_string_property_as_ebcdic(doc, pair, &build->number);
    } else if (0 == strcmp(ebcdic_key, "commitHash")) {
      copy_string_property_as_ebcdic(doc, pair, &build->commit_hash);
    } else if (0 == strcmp(ebcdic_key, "timestamp")) {
      copy_string_property_as_ebcdic(doc, pair, &build->timestamp);
    }
  }
}

void fill_manifest(zl_manifest_t *manifest, yaml_document_t *doc, yaml_node_t *node) {
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    char *key = yaml_pair_get_key(doc, pair);
    char  ebcdic_key[strlen(key) + 1];
    strcpy(ebcdic_key, key);
    __atoe(ebcdic_key);
    if (0 == strcmp(ebcdic_key, "name")) {
      copy_string_property_as_ebcdic(doc, pair, &manifest->name);
    } else if (0 == strcmp(ebcdic_key, "id")) {
      copy_string_property_as_ebcdic(doc, pair, &manifest->id);
    } else if (0 == strcmp(ebcdic_key, "title")) {
      copy_string_property_as_ebcdic(doc, pair, &manifest->title);
    } else if (0 == strcmp(ebcdic_key, "description")) {
      copy_string_property_as_ebcdic(doc, pair, &manifest->description);
    } else if (0 == strcmp(ebcdic_key, "license")) {
      copy_string_property_as_ebcdic(doc, pair, &manifest->license);
    } else if (0 == strcmp(ebcdic_key, "commands")) {
      yaml_node_t *value_node = yaml_pair_get_value_node(doc, pair);
      copy_manifest_commands(&manifest->commands, doc, value_node);
    } else if (0 == strcmp(ebcdic_key, "build")) {
      yaml_node_t *value_node = yaml_pair_get_value_node(doc, pair);
      copy_manifest_build(&manifest->build, doc, value_node);
    }
  }
}

int test_document_parser(const char *filename) {
  yaml_parser_t   parser;
  yaml_document_t document;
  int             error = 0;
  FILE *          fp = NULL;

  fp = fopen(filename, "r");
  if (!fp) {
    printf("unable to open %s: %s\n", filename, strerror(errno));
    return -1;
  }
  printf("Loading '%s': \n", filename);

  yaml_parser_initialize(&parser);

  yaml_parser_set_input(&parser, yaml_read_handler, fp);

  int done = 0;
  if (!yaml_parser_load(&parser, &document)) {
    fprintf(stderr, "Failed to load document in %s\n", filename);
    return -1;
  }

  yaml_node_t *root = yaml_document_get_root_node(&document);
  done = !root;

  if (!done) {
    struct zl_manifest_t manifest = {0};
    fill_manifest(&manifest, &document, root);

    printf("name: %s, id: %s, title: %s, description: %s, license: %s\n", manifest.name, manifest.id, manifest.title,
           manifest.description, manifest.license);
    printf("start: %s, validate: %s\n", manifest.commands.start, manifest.commands.validate);
    printf("branch: %s, number: %s, commitHash: %s, timestamp %s\n", manifest.build.branch, manifest.build.number,
           manifest.build.commit_hash, manifest.build.timestamp);
  }

  yaml_document_delete(&document);

  yaml_parser_delete(&parser);

  fclose(fp);

  return !error;
}

void read_manifests() {
  char *workdir = getenv("WORKDIR");
  if (!workdir) {
    printf("WORKDIR not found\n");
    return;
  }
  char path[strlen(workdir) + strlen("/components") + 1];
  strcpy(path, workdir);
  strcat(path, "/components");
  DIR *components_dir = opendir(path);
  if (!components_dir) {
    printf("unable to open components directory %s\n", path);
    return;
  }
  printf("components dir %s opened\n", path);
  struct dirent *entry;
  struct stat    file_stat;
  char           dir_path[_POSIX_PATH_MAX + 1];
  char           manifest_path[_POSIX_PATH_MAX + 1];
  while ((entry = readdir(components_dir)) != NULL) {
    if (entry->d_name[0] == '.') {
      continue;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/%s", path, entry->d_name);
    stat(dir_path, &file_stat);
    if (S_ISDIR(file_stat.st_mode)) {
      snprintf(manifest_path, sizeof(manifest_path), "%s/manifest.yaml", dir_path);
      test_document_parser(manifest_path);
    }
  }
  closedir(components_dir);
}

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
