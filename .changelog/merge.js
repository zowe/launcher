/*
This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.
*/

const fs = require('node:fs');
const path = require('node:path');

const root = path.resolve(__dirname, '..');
const usage = `Usage: node .changelog/merge.js [X.Y.Z]
       node .changelog/merge.js --dry-run [X.Y.Z]
       node .changelog/merge.js --check [PR_NUMBER]

Merge .changelog/CHANGELOG.<PR_NUMBER> into CHANGELOG.md and remove merged files.
The version defaults to manifest.yaml. --dry-run prints the result without changes.
--check validates fragments and optionally requires a fragment for the specified PR.`;

function readFragments() {
  return fs.readdirSync(__dirname, { withFileTypes: true })
    .filter(file => file.name.startsWith('CHANGELOG.'))
    .map(file => {
      const match = /^CHANGELOG\.([1-9]\d*)$/.exec(file.name);
      if (!match || !file.isFile()) {
        throw new Error(`Invalid fragment: ${file.name}. Expected a file named CHANGELOG.<PR_NUMBER>.`);
      }

      const pr = match[1];
      const url = `https://github.com/zowe/launcher/pull/${pr}`;
      const content = fs.readFileSync(path.join(__dirname, file.name), 'utf8')
        .replace(/\r\n/g, '\n').trim();
      const entries = [];
      for (const line of content.split('\n')) {
        if (/^- \S/.test(line)) {
          entries.push([`${line.trimEnd()} ([#${pr}](${url}))`]);
        } else if (entries.length && (/^\s*$/.test(line) || /^[ \t]+\S/.test(line))) {
          entries[entries.length - 1].push(line);
        } else {
          throw new Error(`${file.name}: use nonempty Markdown entries starting with "- " and indent continuation lines.`);
        }
      }
      return { name: file.name, pr, url, text: entries.map(entry => entry.join('\n').trimEnd()).join('\n') };
    })
    .sort((a, b) => a.pr.length - b.pr.length || a.pr.localeCompare(b.pr));
}

function merge(changelog, fragments, version) {
  const eol = changelog.includes('\r\n') ? '\r\n' : '\n';
  const lines = changelog.split(/\r?\n/);
  const heading = new RegExp(`^## v?${version.replace(/\./g, '\\.')}[ \\t]*$`);
  const index = lines.findIndex(line => heading.test(line));
  const entries = fragments.map(fragment => fragment.text).join('\n').split('\n').join(eol);

  if (index !== -1) {
    let end = index + 1;
    while (end < lines.length && !/^## /.test(lines[end])) end++;
    const section = lines.slice(index + 1, end).join('\n');
    for (const fragment of fragments) {
      if (section.includes(`${fragment.url})`)) {
        throw new Error(`PR #${fragment.pr} is already in version ${version}; check ${fragment.name} before merging.`);
      }
    }
    let body = index + 1;
    while (body < end && !lines[body].trim()) body++;
    return lines.slice(0, index + 1).join(eol) + eol + eol + entries + eol + eol
      + lines.slice(body).join(eol);
  }

  const firstSection = lines.findIndex(line => /^## /.test(line));
  const intro = firstSection === -1 ? changelog : lines.slice(0, firstSection).join(eol);
  const history = firstSection === -1 ? '' : lines.slice(firstSection).join(eol);
  return intro.trimEnd() + eol + eol + `## ${version}` + eol + eol + entries + eol
    + (history ? eol + history : '');
}

function main() {
  const args = process.argv.slice(2);
  if (args.length === 1 && (args[0] === '--help' || args[0] === '-h')) {
    console.log(usage);
    return;
  }
  const mode = args[0] === '--check' || args[0] === '--dry-run' ? args.shift() : 'merge';
  if (args.length > 1 || args.some(arg => arg.startsWith('-'))) {
    throw new Error(usage);
  }
  const value = args[0];
  if (mode === '--check') {
    if (value !== undefined && !/^[1-9]\d*$/.test(value)) {
      throw new Error('PR_NUMBER must be a positive integer.');
    }
    const fragments = readFragments();
    if (value && !fragments.some(fragment => fragment.pr === value)) {
      throw new Error(`Missing .changelog/CHANGELOG.${value}. See .changelog/README.md.`);
    }
    console.log(`Validated ${fragments.length} changelog fragment(s).`);
    return;
  }

  const version = value === undefined
    ? fs.readFileSync(path.join(root, 'manifest.yaml'), 'utf8').match(/^version:\s*([^\r\n]+)$/m)?.[1].trim()
    : value;
  if (!/^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)$/.test(version || '')) {
    throw new Error('Version must be X.Y.Z; supply it as an argument or set version in manifest.yaml.');
  }

  const fragments = readFragments();
  const changelogPath = path.join(root, 'CHANGELOG.md');
  if (!fragments.length) {
    if (mode === '--dry-run') process.stdout.write(fs.readFileSync(changelogPath, 'utf8'));
    else console.log('No changelog fragments to merge.');
    return;
  }
  const result = merge(fs.readFileSync(changelogPath, 'utf8'), fragments, version);
  if (mode === '--dry-run') {
    process.stdout.write(result);
    return;
  }

  fs.writeFileSync(changelogPath, result);
  for (const fragment of fragments) fs.unlinkSync(path.join(__dirname, fragment.name));
  console.log(`Merged ${fragments.length} changelog fragment(s) into version ${version}. Commit CHANGELOG.md and the fragment deletions together.`);
}

try {
  main();
} catch (error) {
  console.error(error.message);
  process.exitCode = 1;
}
