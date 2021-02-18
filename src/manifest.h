#ifndef MAINFEST_H
#define MAINFEST_H

#ifndef PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX
#endif

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

#define KEY_LEN 255

#endif // MANIFEST_H
