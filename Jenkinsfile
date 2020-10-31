#!groovy

/**
 * This program and the accompanying materials are
 * made available under the terms of the Eclipse Public License v2.0 which accompanies
 * this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Copyright Contributors to the Zowe Project.
 */

node("ibm-jenkins-slave-nvm") {

    def lib = library("jenkins-library@users/jack/version-text-file").org.zowe.jenkins_shared_library
    def pipeline = lib.pipelines.generic.GenericPipeline.new(this)

    // pipeline.admins.add("dnikolaev", "sgrady", "jackjia")
    pipeline.admins.add("jackjia")

    // setup will read package information from manifest
    pipeline.setup()

    pipeline.build(
        operation: {
            echo "Build will happen in pre-packaging step on z/OS"
        }
    )

    // define we need packaging stage, which processed in .pax folder
    pipeline.packaging(name: 'zowe-launcher', paxOptions: '-x os390')

    // define we need publish stage
    pipeline.publish(
        allowPublishWithoutTest: true,
        artifacts: [
            '.pax/zowe-launcher.pax',
        ]
    )

    pipeline.release()

    pipeline.end()

}
