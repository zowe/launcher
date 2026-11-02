/*
This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.
*/

const fs = require('node:fs');
const path = require('node:path');

function main() {
  const args = process.argv.slice(2);
  if (args.length !== 1 || !/^[1-9]\d*$/.test(args[0])) {
    throw new Error('Usage: node .github/workflows/set-changelog.js PR_NUMBER (positive integer).');
  }

  const entries = [];
  for (const line of (process.env.PR_DESCRIPTION || '').split(/\r?\n/)) {
    const match = /^[ \t]*CHANGELOG:[ \t]*(.*)$/.exec(line);
    if (!match) continue;
    const text = match[1].trim().replace(/^-(?:[ \t]+|$)/, '').trim();
    if (!text) {
      throw new Error('CHANGELOG: must have text on the same line in the PR description.');
    }
    entries.push(`- ${text}`);
  }
  if (!entries.length) {
    throw new Error('Add CHANGELOG: followed by the change description on the same line in the PR description.');
  }

  const directory = path.resolve(__dirname, '../../.changelog');
  const name = `CHANGELOG.${args[0]}`;
  fs.mkdirSync(directory, { recursive: true });
  try {
    fs.writeFileSync(path.join(directory, name), entries.join('\n') + '\n', { flag: 'wx' });
  } catch (error) {
    if (error.code === 'EEXIST') {
      throw new Error(`.changelog/${name} already exists; edit the fragment directly to update its entries.`);
    }
    throw error;
  }
  console.log(`Created .changelog/${name} from the PR description.`);
}

try {
  main();
} catch (error) {
  console.error(error.message);
  process.exitCode = 1;
}
