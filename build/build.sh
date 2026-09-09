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

LAUNCHER_DIR="$WORKING_DIR/.."

# Checks for and possibly downloads dependencies from env vars from above file
. $WORKING_DIR/dependencies.sh
check_dependencies "${WORKING_DIR}/.." "$WORKING_DIR/launcher.proj.env"
DEPS_DESTINATION=$(get_destination "${WORKING_DIR}/.." "${PROJECT}")

# These paths assume that the build is run from /launcher/builds

date_stamp=$(date +%Y%m%d%S)
launcher_date_stamp=$(date +%Y%m%d)

TMP_DIR="${WORKING_DIR}/tmp-${date_stamp}"

mkdir -p "${TMP_DIR}" && cd "${TMP_DIR}"

# Split LIBYAML version into parts
OLDIFS=$IFS
IFS="."
for part in ${LIBYAML_BRANCH}; do
  if [ -z "$LIBYAML_MAJOR" ]; then
    LIBYAML_MAJOR=$part
  elif [ -z "$LIBYAML_MINOR" ]; then
    LIBYAML_MINOR=$part
  else
    LIBYAML_PATCH=$part
  fi
done
IFS=$OLDIFS

rm -f "${LAUNCHER_DIR}/bin/zowe_launcher"
mkdir -p "${LAUNCHER_DIR}/bin"

GSKDIR=/usr/lpp/gskssl
GSKINC="${GSKDIR}/include"

echo "Compiling qascii libyaml \"${LIBYAML_BRANCH}\" and quickjs \"${QUICKJS_BRANCH}\""

xlclang \
  -c \
  ${ZWE_XLCLANG_FLAGS} \
  -q64 \
  -qascii \
  "-Wc,float(ieee),longname,langlvl(extc99),gonum,goff,ASM,asmlib('CEE.SCEEMAC','SYS1.MACLIB','SYS1.MODGEN')" \
  -DYAML_VERSION_MAJOR=${LIBYAML_MAJOR} \
  -DYAML_VERSION_MINOR=${LIBYAML_MINOR} \
  -DYAML_VERSION_PATCH=${LIBYAML_PATCH} \
  -DYAML_VERSION_STRING="\"${LIBYAML_BRANCH}\"" \
  -DYAML_DECLARE_STATIC=1 \
  -D_OPEN_SYS_FILE_EXT=1 \
  -D_XOPEN_SOURCE=600 \
  -D_OPEN_THREADS=1 \
  -DCONFIG_BIGNUM=1 \
  -DCONFIG_VERSION=\"2024-01-13\" \
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
  ${DEPS_DESTINATION}/${QUICKJS}/libbf.c \
  ${DEPS_DESTINATION}/${QUICKJS}/libregexp.c \
  ${DEPS_DESTINATION}/${QUICKJS}/porting/debugutil.c \
  ${DEPS_DESTINATION}/${QUICKJS}/porting/polyfill.c
rc=$?
if [ $rc -ne 0 ]; then
  echo "Build failed"
  exit 8
fi

# LAUNCHER_VERSION can be exported to specific version
# However C code is expecting numbers
# print $1/1 -> numeric input => numeric output, other input => "0"
if [ -z "${LAUNCHER_VERSION}" ]; then
  LAUNCHER_VERSION=$(cat "${LAUNCHER_DIR}/manifest.yaml" | grep -e "^version:" | awk -F: '{ print $2 }')
  LAUNCHER_VERSION=$(echo "${LAUNCHER_VERSION}" | awk '{$1=$1};1')  # awk = strip leading and trailing spaces
fi
if [ -n "${LAUNCHER_VERSION}" ]; then
  LAUNCHER_MAJOR=$(echo "${LAUNCHER_VERSION}" | awk -F. '{ print $1/1 }')
  LAUNCHER_MINOR=$(echo "${LAUNCHER_VERSION}" | awk -F. '{ print $2/1 }')
  LAUNCHER_PATCH=$(echo "${LAUNCHER_VERSION}" | awk -F. '{ print $3/1 }')
fi

echo "Compiling zowe_launcher ${LAUNCHER_MAJOR}.${LAUNCHER_MINOR}.${LAUNCHER_PATCH}+${launcher_date_stamp}"

xlclang \
  ${ZWE_XLCLANG_FLAGS} \
  -q64 \
  "-Wa,goff" \
  "-Wc,float(ieee),langlvl(extc99),agg,list()" \
  "-Wc,gonum,goff,xref,roconst,ASM,asmlib('CEE.SCEEMAC','SYS1.MACLIB','SYS1.MODGEN')" \
  -D_OPEN_SYS_FILE_EXT=1 \
  -D_XOPEN_SOURCE=600 \
  -D_OPEN_THREADS=1 \
  -DNOIBMHTTP=1 \
  -DNEW_CAA_LOCATIONS=1 \
  -DUSE_ZOWE_TLS=1 \
  -DLAUNCHER_VERSION_MAJOR="${LAUNCHER_MAJOR}" \
  -DLAUNCHER_VERSION_MINOR="${LAUNCHER_MINOR}" \
  -DLAUNCHER_VERSION_PATCH="${LAUNCHER_PATCH}" \
  -DLAUNCHER_VERSION_DATE_STAMP="${launcher_date_stamp}" \
  -I "${LAUNCHER_DIR}/src/msg.h" \
  -I "${DEPS_DESTINATION}/${COMMON}/h" \
  -I "${DEPS_DESTINATION}/${COMMON}/platform/posix" \
  -I ${GSKINC} \
  -I "${DEPS_DESTINATION}/${LIBYAML}/include" \
  -I "${DEPS_DESTINATION}/${QUICKJS}" \
  -o "${LAUNCHER_DIR}/bin/zowe_launcher" \
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
  libbf.o \
  debugutil.o \
  libregexp.o \
  polyfill.o \
  ${LAUNCHER_DIR}/src/main.c \
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
