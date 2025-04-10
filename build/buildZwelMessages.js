/*
  This program and the accompanying materials are made available
  under the terms of the Eclipse Public License v2.0 which
  accompanies this distribution, and is available at
  https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

import * as ZWEL from './zwelMessages.js';
import { parseArgs } from "node:util";
import * as fs from 'node:fs';

const options = {
    'check': { type: 'boolean', short: 'c' }
}

const { values: { check } } = parseArgs({ options });

// Documentation is made in oder of info, error and warning messages
const SEVERITY_LIST = 'IEW';
const DEBUG = false;
let sorted = [[],[],[]];

// Markdown template
const TEMPLATE = `### \${this.id}

\${this.text}

**Reason:**

\${this.reason}

**Action:**

\${this.action}

`

function resolveTemplate(templateString, data) {
    const template = new Function('return `' + templateString + '`;');
    return template.call(data);
}

function severityToIndex(severity) {
    const i = SEVERITY_LIST.indexOf(severity.toUpperCase());
    if (i === -1) {
        throw new Error(`Wrong severity code ${severity}`);
    }
    return i;
}

// Basic check and sort messages by severity and in numeric code order
function checkAndSort() {
    ZWEL.MESSAGES.forEach(element => {
        const severityIndex = severityToIndex(element.id.substring(element.id.length - 1));
        const codeString = element.id.substring(4, 8);
        if (!codeString.match(/[0-9]{4}/)) {
            throw new Error(`Wrong numeric code for ${element.id}`);
        }
        const code = Number(codeString);
        if (!element.action || !element.reason) {
            throw new Error(`Missing action or reason for ${element.id}`);
        }

        DEBUG && console.log(`${element.id} -> ${element.text}\nR=${element.reason}\nA=${element.action}\n`);

        sorted[severityIndex][code] = { id: element.id, text: element.text, reason: element.reason, action: element.action };
    });
}

// Print as MD using the template
function createMD() {
    for (let svr = 0; svr < SEVERITY_LIST.length; svr ++) {
        sorted[svr].forEach(msg => {
            DEBUG && console.log(`<!--\n${msg.id} -> ${msg.text}\nR=${msg.reason}\nA=${msg.action}\n-->`);
            console.log(resolveTemplate(TEMPLATE, { id: msg.id, text: msg.text, reason: msg.reason, action: msg.action }));
        })
    }
}

// Check header messages and compare with ZWEL.MESSAGES
function checkMessages() {
    const headerFile = fs.readFileSync('../src/msg.h', 'utf8');
    let header = [];
    let doc = [];

    headerFile.split('\n').forEach(line => {
        if (line.match(/^#define[\ ]+MSG_[A-Z0-9_]+[\ ]+MSG_PREFIX/)) {
            const firstApos = line.indexOf('"');
            const secondApos = line.indexOf('"', firstApos + 1);
            const headerCode = 'ZWEL' + line.substring(firstApos + 1, secondApos);
            header.push(headerCode);

            DEBUG && console.log(`${headerCode} -> ${line}`);
        }
    })

    ZWEL.MESSAGES.forEach(element => {
        doc.push(element.id);
    })

    DEBUG && console.dir(header.sort());
    DEBUG && console.dir(doc.sort());

    let difference = header.filter(zwelMsg => !doc.includes(zwelMsg));
    console.log(difference.length);
}

if (check) {
    checkMessages();
} else {
    checkAndSort();
    createMD();
}
