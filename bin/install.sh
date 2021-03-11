#!/bin/sh

################################################################################
#  This program and the accompanying materials are
#  made available under the terms of the Eclipse Public License v2.0 which accompanies
#  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
#  SPDX-License-Identifier: EPL-2.0
#
#  Copyright Contributors to the Zowe Project.
################################################################################

if [ -z "${ZOWE_ROOT_DIR}" ]; then
  >&2 echo "Error: ZOWE_ROOT_DIR is required"
  exit 1
fi
if [ -z "${ZOWE_DSN_PREFIX}" ]; then
  >&2 echo "Error: ZOWE_DSN_PREFIX is required"
  exit 1
fi

# we should already in component directory
if [ ! -f bin/zowe_launcher ]; then
  >&2 echo "Error: Zowe launcher loadlib is missing"
  exit 1
fi
if [ ! -f samplib/ZWESLNCH ]; then
  >&2 echo "Error: ZWESLNCH sampllib is missing"
  exit 1
fi

script=launcher/install.sh

# copy samplib to SZWESAMP
if [ -z "${LOG_FILE}" ]; then
  cp -v samplib/ZWESLNCH "//'${ZOWE_DSN_PREFIX}.SZWESAMP'"
  rc=$?
else
  cp -v samplib/ZWESLNCH "//'${ZOWE_DSN_PREFIX}.SZWESAMP'" >> ${LOG_FILE}
  rc=$?
fi
if [ "${rc}" = "0" ] ; then
  print_and_log_message "$script ZWESLNCH copied to ${ZOWE_DSN_PREFIX}.SZWESAMP"
else
  print_and_log_message "$script ZWESLNCH not copied to ${ZOWE_DSN_PREFIX}.SZWESAMP, RC=$rc"
fi

# copy launcher program to 
if [ -z "${LOG_FILE}" ]; then
  cp -X bin/zowe_launcher "//'${ZOWE_DSN_PREFIX}.SZWEAUTH(ZWELNCH)'"
  rc=$?
else
  cp -X bin/zowe_launcher "//'${ZOWE_DSN_PREFIX}.SZWEAUTH(ZWELNCH)'" >> ${LOG_FILE}
  rc=$?
fi
if [ "${rc}" = "0" ] ; then
  print_and_log_message "$script ZWELNCH program copied to ${ZOWE_DSN_PREFIX}.SZWEAUTH"
else
  print_and_log_message "$script ZWELNCH program not copied to ${ZOWE_DSN_PREFIX}.SZWEAUTH, RC=$rc"
fi
