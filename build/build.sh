#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.

WORKING_DIR=$(cd $(dirname "$0") && pwd)

# set -v

# Loads project info like name and version
. $WORKING_DIR/launcher.proj.env

echo "********************************************************************************"
echo "Building $PROJECT..."

COMMON="$WORKING_DIR/.."

# Checks for and possibly downloads dependencies from env vars from above file
. $WORKING_DIR/dependencies.sh
check_dependencies "${WORKING_DIR}/.." "$WORKING_DIR/launcher.proj.env"
DEPS_DESTINATION=$(get_destination "${WORKING_DIR}/.." "${PROJECT}")

# These paths assume that the build is run from /launcher/builds

date_stamp=$(date +%Y%m%d%S)

TMP_DIR="${WORKING_DIR}/tmp-${date_stamp}"

mkdir -p "${TMP_DIR}" && cd "${TMP_DIR}"


# Split version into parts
OLDIFS=$IFS
IFS="."
for part in ${VERSION}; do
  if [ -z "$MAJOR" ]; then
    MAJOR=$part
  elif [ -z "$MINOR" ]; then
    MINOR=$part
  else
    PATCH=$part
  fi
done
IFS=$OLDIFS

VERSION="\"${VERSION}\""

rm -f "${WORKING_DIR}/../bin/zowe_launcher"

GSKDIR=/usr/lpp/gskssl
GSKINC="${GSKDIR}/include"

echo "Compiling qascii libyaml and quickjs"

xlclang \
  -c \
  -q64 \
  -qascii \
  "-Wc,float(ieee),longname,langlvl(extc99),gonum,goff,ASM,asmlib('CEE.SCEEMAC','SYS1.MACLIB','SYS1.MODGEN')" \
  -DYAML_VERSION_MAJOR=${MAJOR} \
  -DYAML_VERSION_MINOR=${MINOR} \
  -DYAML_VERSION_PATCH=${PATCH} \
  -DYAML_VERSION_STRING="${VERSION}" \
  -DYAML_DECLARE_STATIC=1 \
  -D_OPEN_SYS_FILE_EXT=1 \
  -D_XOPEN_SOURCE=600 \
  -D_OPEN_THREADS=1 \
  -DCONFIG_VERSION=\"2021-03-27\" \
  -I "${DEPS_DESTINATION}/${LIBYAML}/include" \
  -I "${DEPS_DESTINATION}/${QUICKJS}" \
  ${DEPS_DESTINATION}/${LIBYAML}/src/api.c \
  ${DEPS_DESTINATION}/${LIBYAML}/src/reader.c \
  ${DEPS_DESTINATION}/${LIBYAML}/src/scanner.c \
  ${DEPS_DESTINATION}/${LIBYAML}/src/parser.c \
  ${DEPS_DESTINATION}/${LIBYAML}/src/loader.c \
  ${DEPS_DESTINATION}/${LIBYAML}/src/writer.c \
  ${DEPS_DESTINATION}/${LIBYAML}/src/emitter.c \
  ${DEPS_DESTINATION}/${LIBYAML}/src/dumper.c \
  ${DEPS_DESTINATION}/${QUICKJS}/cutils.c \
  ${DEPS_DESTINATION}/${QUICKJS}/quickjs.c \
  ${DEPS_DESTINATION}/${QUICKJS}/quickjs-libc.c \
  ${DEPS_DESTINATION}/${QUICKJS}/libunicode.c \
  ${DEPS_DESTINATION}/${QUICKJS}/libregexp.c \
  ${DEPS_DESTINATION}/${QUICKJS}/porting/polyfill.c
rc=$?
if [ $rc -ne 0 ]; then
  echo "Build failed"
  exit 8
fi

echo "Compiling zowe_launcher"

xlclang \
  -q64 \
  "-Wa,goff" \
  "-Wc,float(ieee),langlvl(extc99),agg,list()" \
  "-Wc,gonum,goff,xref,roconst,ASM,asmlib('CEE.SCEEMAC','SYS1.MACLIB','SYS1.MODGEN')" \
  -D_OPEN_SYS_FILE_EXT=1 \
  -D_XOPEN_SOURCE=600 \
  -D_OPEN_THREADS=1 \
  -DNOIBMHTTP=1 \
  -DUSE_ZOWE_TLS=1 \
  -I "${DEPS_DESTINATION}/${COMMON}/h" \
  -I "${DEPS_DESTINATION}/${COMMON}/platform/posix" \
  -I ${GSKINC} \
  -I "${DEPS_DESTINATION}/${LIBYAML}/include" \
  -I "${DEPS_DESTINATION}/${QUICKJS}" \
  -o "${DEPS_DESTINATION}/${COMMON}/bin/zowe_launcher" \
  api.o \
  reader.o \
  scanner.o \
  parser.o \
  loader.o \
  writer.o \
  emitter.o \
  dumper.o \
  cutils.o \
  quickjs.o \
  quickjs-libc.o \
  libunicode.o \
  libregexp.o \
  polyfill.o \
  ${DEPS_DESTINATION}/${COMMON}/c/alloc.c \
  ${DEPS_DESTINATION}/${COMMON}/c/bpxskt.c \
  ${DEPS_DESTINATION}/${COMMON}/c/charsets.c \
  ${DEPS_DESTINATION}/${COMMON}/c/collections.c \
  ${DEPS_DESTINATION}/${COMMON}/c/configmgr.c \
  ${DEPS_DESTINATION}/${COMMON}/c/embeddedjs.c \
  ${DEPS_DESTINATION}/${COMMON}/c/fdpoll.c \
  ${DEPS_DESTINATION}/${COMMON}/c/http.c \
  ${DEPS_DESTINATION}/${COMMON}/c/httpclient.c \
  ${DEPS_DESTINATION}/${COMMON}/c/json.c \
  ${DEPS_DESTINATION}/${COMMON}/c/jcsi.c \
  ${DEPS_DESTINATION}/${COMMON}/c/jsonschema.c \
  ${DEPS_DESTINATION}/${COMMON}/c/le.c \
  ${DEPS_DESTINATION}/${COMMON}/c/logging.c \
  ${DEPS_DESTINATION}/${COMMON}/c/microjq.c \
  ${DEPS_DESTINATION}/${COMMON}/c/parsetools.c \
  ${DEPS_DESTINATION}/${COMMON}/c/pdsutil.c \
  ${DEPS_DESTINATION}/${COMMON}/c/qjsnet.c \
  ${DEPS_DESTINATION}/${COMMON}/c/qjszos.c \
  ${DEPS_DESTINATION}/${COMMON}/platform/posix/psxregex.c \
  ${DEPS_DESTINATION}/${COMMON}/c/recovery.c \
  ${DEPS_DESTINATION}/${COMMON}/c/scheduling.c \
  ${DEPS_DESTINATION}/${COMMON}/c/socketmgmt.c \
  ${DEPS_DESTINATION}/${COMMON}/c/timeutls.c \
  ${DEPS_DESTINATION}/${COMMON}/c/tls.c \
  ${DEPS_DESTINATION}/${COMMON}/c/utils.c \
  ${DEPS_DESTINATION}/${COMMON}/c/xlate.c \
  ${DEPS_DESTINATION}/${COMMON}/c/yaml2json.c \
  ${DEPS_DESTINATION}/${COMMON}/c/zos.c \
  ${DEPS_DESTINATION}/${COMMON}/c/zosfile.c \
  ${GSKDIR}/lib/GSKSSL64.x \
  ${GSKDIR}/lib/GSKCMS64.x
rc=$?
rm -rf "${TMP_DIR}"
if [ $rc -ne 0 ]; then
  echo "Build failed"
  exit 8
fi




# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.
