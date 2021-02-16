#ifndef MAINFEST_H
#define MAINFEST_H

typedef struct zl_manifest_commands_t {
  char *start;
  char *validate;
} zl_manifest_commands_t;

typedef struct zl_manifest_build_t {
  char *branch;
  char *number;
  char *commit_hash;
  char *timestamp;
} zl_manifest_build_t;

typedef struct zl_manifest_t {
  char *name;
  char *id;
  char *title;
  char *description;
  char *license;
  zl_manifest_commands_t commands;
  zl_manifest_build_t build;
} zl_manifest_t;

#endif // MANIFEST_H
