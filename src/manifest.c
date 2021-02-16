
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

void printl_utf8(unsigned char *str, size_t length, FILE *stream) {
  __atoe_l((char *)str, length);
  fwrite(str, 1, length, stream);
}

char *yaml_pair_get_key(yaml_document_t *document, yaml_node_pair_t *pair) {
  yaml_node_t *key_node = yaml_document_get_node(document, pair->key);
  if (key_node->type == YAML_SCALAR_NODE) {
    return (char *)key_node->data.scalar.value;
  }
  return NULL;
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

void print_yaml_node(yaml_document_t *document_p, yaml_node_t *node) {
  static int x = 0;
  x++;
  int node_n = x;

  yaml_node_t *next_node_p;

  switch (node->type) {
    case YAML_NO_NODE:
      printf("Empty node(%d):\n", node_n);
      break;
    case YAML_SCALAR_NODE:
      printf("Scalar node(%d):\n", node_n);
      printl_utf8(node->data.scalar.value, node->data.scalar.length, stdout);
      puts("");
      break;
    case YAML_SEQUENCE_NODE:
      printf("Sequence node(%d):\n", node_n);
      yaml_node_item_t *i_node;
      for (i_node = node->data.sequence.items.start; i_node < node->data.sequence.items.top; i_node++) {
        next_node_p = yaml_document_get_node(document_p, *i_node);
        if (next_node_p) print_yaml_node(document_p, next_node_p);
      }
      break;
    case YAML_MAPPING_NODE:
      printf("Mapping node(%d):\n", node_n);

      yaml_node_pair_t *i_node_p;
      for (i_node_p = node->data.mapping.pairs.start; i_node_p < node->data.mapping.pairs.top; i_node_p++) {
        next_node_p = yaml_document_get_node(document_p, i_node_p->key);
        if (next_node_p) {
          puts("Key:");
          if (next_node_p->type == YAML_SCALAR_NODE) {
            printf("key is always scalar\n");
          } else {
            printf("key is not scalar\n");
            exit(1);
          }
          print_yaml_node(document_p, next_node_p);
        } else {
          fputs("Couldn't find next node\n", stderr);
          exit(1);
        }

        next_node_p = yaml_document_get_node(document_p, i_node_p->value);
        if (next_node_p) {
          puts("Value:");
          print_yaml_node(document_p, next_node_p);
        } else {
          fputs("Couldn't find next node\n", stderr);
          exit(1);
        }
      }
      break;
    default:
      fputs("Unknown node type\n", stderr);
      exit(1);
      break;
  }

  printf("END NODE(%d)\n", node_n);
}

#define copy_property(target_key, object, doc, pair)        \
  {                                                         \
    char *key = yaml_pair_get_key(doc, pair);               \
    if (key && compare_with_ascii(#target_key, key) == 0) { \
      char *value = yaml_pair_get_value_string(doc, pair);  \
      if (value) {                                          \
        char *copy = strdup(value);                         \
        __atoe(copy);                                       \
        object->target_key = copy;                          \
      }                                                     \
      continue;                                             \
    }                                                       \
  }

inline yaml_node_pair_t *start_pair(yaml_node_t *node) {
  return node->data.mapping.pairs.start;
}

inline yaml_node_pair_t *end_pair(yaml_node_t *node) {
  return node->data.mapping.pairs.top;
}

void copy_commands(zl_manifest_commands_t *commands, yaml_document_t *doc, yaml_node_t *node) {
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    copy_property(start, commands, doc, pair);
    copy_property(validate, commands, doc, pair);
  }
}

void copy_build(zl_manifest_build_t *build, yaml_document_t *doc, yaml_node_t *node) {
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    copy_property(branch, build, doc, pair);
    copy_property(number, build, doc, pair);
    copy_property(commitHash, build, doc, pair);
    copy_property(timestamp, build, doc, pair);
  }
}

void top(zl_manifest_t *manifest, yaml_document_t *doc, yaml_node_t *node) {
  for (yaml_node_pair_t *pair = start_pair(node); pair != end_pair(node); pair++) {
    copy_property(name, manifest, doc, pair);
    copy_property(id, manifest, doc, pair);
    copy_property(version, manifest, doc, pair);
    copy_property(title, manifest, doc, pair);
    copy_property(description, manifest, doc, pair);
    copy_property(license, manifest, doc, pair);
    char *key = yaml_pair_get_key(doc, pair);
    if (key && compare_with_ascii("commands", key) == 0) {
      yaml_node_t *value_node = yaml_pair_get_value_node(doc, pair);
      copy_commands(&manifest->commands, doc, value_node);
    }
    if (key && compare_with_ascii("build", key) == 0) {
      yaml_node_t *value_node = yaml_pair_get_value_node(doc, pair);
      copy_build(&manifest->build, doc, value_node);
    }
  }
}

int test_document_parser() {
  yaml_parser_t   parser;
  yaml_document_t document;
  int             error = 0;
  FILE *          fp = NULL;
  const char *    filename = "/u/ts3105/zowe-installation/components/explorer-mvs/manifest.yaml";

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
    top(&manifest, &document, root);

    printf("name: %s, id: %s, version: %s, title: %s, description: %s license: %s\n", manifest.name, manifest.id,
           manifest.version, manifest.title, manifest.description, manifest.license);
    printf("start: %s, validate: %s\n", manifest.commands.start, manifest.commands.validate);
    printf("branch: %s, number: %s, commitHash: %s, timestamp %s\n", manifest.build.branch, manifest.build.number,
           manifest.build.commitHash, manifest.build.timestamp);
  }
  //  print_yaml_node(&document, root);

  yaml_document_delete(&document);

  yaml_parser_delete(&parser);

  fclose(fp);

  return !error;
}

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
