#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
#
# SPDX-License-Identifier: EPL-2.0
#
# Copyright Contributors to the Zowe Project.

# Start with any parameter -> prints output (diff style)
#   rc of this = number of errors found

print=
errors=0

# Get the launchers's version from manifest.yaml
VERSION=$(cat "../manifest.yaml" | grep -e "^version:" | awk -F: '{ print $2 }' | awk '{$1=$1};1')

if [ ! -z "${1}" ]; then
    print='1'
fi

LAUNCHER='../bin/zowe_launcher'
if [ ! -f "${LAUNCHER}" ]; then
    echo "Executable for Zowe launcher not found at: ${LAUNCHER}"
    exit 255
fi

ABS_PATH=$(cd .; pwd)

TEST_FILES='./files'
ZOWE="${TEST_FILES}/zowe.runtime.dev.null.yaml"
ZOWE_EMPTY="${TEST_FILES}/zowe.empty.yaml"
ABS_ZOWE="${ABS_PATH}/${ZOWE}"
ABS_ZOWE2="${ABS_PATH}/././././////./////${ZOWE}"

run_launcher() {
    # trim via awk
    config=$(echo "$1" | awk '{$1=$1};1')
    haInstance=$(echo "$2" | awk '{$1=$1};1')
    textMatch=$(echo "$3" | awk '{$1=$1};1')
    desc=$(echo "$4" | awk '{$1=$1};1')
    debug=$(echo "$5" | awk '{$1=$1};1')

    export ZLDEBUG=
    if [ ! -z "${debug}" ]; then
        export ZLDEBUG='ON'
    fi

    export CONFIG="${config}"
    # Filter out the long line when printing
    if [ -n "${print}" ]; then
        result=$($LAUNCHER "${haInstance}" 2>&1 | grep -v "INFO -- If you cant see '500'")
    else
        result=$($LAUNCHER "${haInstance}" 2>&1)
    fi

    [ ! -z "${desc}" ] && [ ! -z "${print}" ] && echo "+ >>> ${desc}"
    [ ! -z "${print}" ] && echo "+ >  CONFIG=${config}"
    [ ! -z "${haInstance}" ] && [ ! -z "${print}" ] && echo "+ >  haInstance=${haInstance}"

    [ ! -z "${print}" ] && echo "${result}"

    if [ ! -z "${textMatch}" ]; then
        findText=$(echo "${result}" | grep "${textMatch}" )
        if [ -z "${findText}" ]; then
            echo "- >  Not found: ${textMatch}"
            errors=`expr $errors + 1`
        else
            [ ! -z "${print}" ] && echo "+ >  Found: ${textMatch}"
        fi
    fi

    [ ! -z "${print}" ] && echo ""
}

IFS='|'
while read config haInstace textMatch desc debug; do
    # Skip the first line
    if [ "${config}" != "CONFIG " ]; then
        run_launcher $config $haInstace $textMatch $desc $debug
    fi
done <<EOF
CONFIG | HA-INSTANCE | TEXT-TO-FIND | DESCRIPTION | ZLDEBUG
 | | PANIC! readJson got null pathElement | No config leads to PANIC!
FILE(${ZOWE_EMPTY}) | | ZWEL0318E - failed to get root node in YAML | Empty config leads to ZWEL0318E
${ABS_ZOWE} | | ZWEL0021I Zowe Launcher starting, version is ${VERSION}+ | Check the basic message ZWEL0021I
${ZOWE} | | INFO ZWEL0023I Zowe YAML config file is '${ZOWE}' | ZWEL0023I
FILE(${ZOWE}) | | INFO ZWEL0023I Zowe YAML config file is 'FILE(${ZOWE})' | Same as previous test
${ABS_ZOWE} | | INFO ZWEL0023I Zowe YAML config file is 'FILE(${ABS_ZOWE})' | ZWEL0023I wrapped by FILE()
${ABS_ZOWE2} | | INFO ZWEL0023I Zowe YAML config file is 'FILE(${ABS_ZOWE2})' | Should be able to read the file
FILE(${ZOWE}) | LPAR123 | ZWEL0024I HA_INSTANCE_ID is 'lpar123' | HaInstance should be sanitazed
PARMLIB(ZOWE.TEST-1.A(A)) | hello | ZWEL0023I Zowe YAML config file is 'PARMLIB(ZOWE.TEST-1.A(A))' | Parmlib does not exist, but it should be in ZWEL00203I
PARMLIB(ZOWE.TEST-1.A) | world | ZWEL0068E PARMLIB() entries must have a member name | Should detect missing member
PARMLIB(ZOWE.TEST-1.A() | | ZWEL0068E PARMLIB() entries must have a member name | Should detect missing member
PARMLIB(ZOWE.TEST-1.A()) | | ZWEL0068E PARMLIB() entries must have a member name | Should detect missing member
${ABS_ZOWE} | valid,IGNORED | ZWEL0024I HA_INSTANCE_ID is 'valid' | Should ignore the second parameter
EOF

exit $errors
