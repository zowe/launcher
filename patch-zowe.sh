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

set -eu

. zowe.conf

cd ./patches

patch -i instance.patch $ZOWE_INSTANCE_DIR/instance.env
patch -i zowe-start.patch $ZOWE_INSTANCE_DIR/bin/zowe-start.sh
patch -i run-zowe.patch $ZOWE_INSTALL_DIR/bin/internal/run-zowe.sh
