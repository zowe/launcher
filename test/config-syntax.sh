#!/bin/sh

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.

# Start with_any_paramter -> prints output (diff style)
#   rc of this = number of errors found

print=
errors=0

if [ ! -z "${1}" ]; then
    print='1'
fi

LAUNCHER='../bin/zowe_launcher'

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
        fi
    fi

    [ ! -z "${print}" ] && echo ""
}

IFS='|'
while read config haInstace textMatch desc debug; do
    if [ "${config}" != "CONFIG " ]; then
        run_launcher $config $haInstace $textMatch $desc $debug 
    fi
done <<EOF #! /bin/sh
EOF
CONFIG | HA-INSTANCE | TEXT-TO-FIND | DESCRIPTION | ZLDEBUG
 | | PANIC! readJson got null pathElement | No config lead to PANIC!
/git/repos/launcher/bin/zowe.yaml | | ZWEL0021I Zowe Launcher starting | Check the basic message ZWEL0021I
/git/repos/launcher/bin/zowe.yaml | | INFO ZWEL0023I Zowe YAML config file is 'FILE(/git/repos/launcher/bin/zowe.yaml)' | ZWEL0023I wrapped by FILE()
FILE(/git/repos/launcher/bin/zowe.yaml) | | INFO ZWEL0023I Zowe YAML config file is 'FILE(/git/repos/launcher/bin/zowe.yaml)' | Same as previous test
FILE(/git/repos/launcher/bin/zowe.yaml) | LPAR123 | ZWEL0024I HA_INSTANCE_ID is 'lpar123' | HaInstance should be sanitazed
PARMLIB(ZOWE.PR4285.A(A)) | hello | ZWEL0023I Zowe YAML config file is 'PARMLIB(ZOWE.PR4285.A(A))' | Parmlib does not exist, but it should be in ZWEL00203I
PARMLIB(ZOWE.PR4285.A) | hello | ZWEL0068E PARMLIB() entries must have a member name | Should detect missing member
PARMLIB(ZOWE.PR4285.A() | hello | ZWEL0068E PARMLIB() entries must have a member name | Should detect missing member
PARMLIB(ZOWE.PR4285.A()) | hello | ZWEL0068E PARMLIB() entries must have a member name | Should detect missing member
EOF

exit $errors
EOF
