/*
This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.
*/

const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const test = require('node:test');

const source = path.resolve(__dirname, '../../.github/workflows/set-changelog.js');
const changelog = '# Zowe Launcher Changelog\n\n## 3.6.0\n\n- Existing entry.\n';

function fixture(t) {
  const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'launcher-pr-changelog-test-'));
  t.after(() => {
    assert.equal(path.dirname(directory), path.resolve(os.tmpdir()));
    assert.ok(path.basename(directory).startsWith('launcher-pr-changelog-test-'));
    fs.rmSync(directory, { recursive: true, force: true });
  });
  const workflowDirectory = path.join(directory, '.github/workflows');
  fs.mkdirSync(workflowDirectory, { recursive: true });
  const script = path.join(workflowDirectory, 'set-changelog.js');
  fs.copyFileSync(source, script);
  fs.writeFileSync(path.join(directory, 'CHANGELOG.md'), changelog);
  return {
    directory,
    read: name => fs.readFileSync(path.join(directory, name), 'utf8'),
    exists: name => fs.existsSync(path.join(directory, name)),
    run: (description, ...args) => {
      const env = { ...process.env };
      if (description === undefined) delete env.PR_DESCRIPTION;
      else env.PR_DESCRIPTION = description;
      return spawnSync(process.execPath, [script, ...args], {
        cwd: os.tmpdir(), // Locate the repository from the script, not the caller.
        env,
        encoding: 'utf8'
      });
    }
  };
}

function succeeds(result) {
  assert.equal(result.error, undefined);
  assert.equal(result.status, 0, result.stderr || result.stdout);
}

function fails(result) {
  assert.equal(result.error, undefined);
  assert.notEqual(result.status, 0, result.stderr || result.stdout);
}

test('creates a fragment from CHANGELOG alone without needing a version or existing directory', t => {
  const repository = fixture(t);

  succeeds(repository.run('## Description\nA change.\n\nCHANGELOG: Bugfix: Correct startup.\n', '234'));
  assert.equal(repository.read('.changelog/CHANGELOG.234'), '- Bugfix: Correct startup.\n');
  assert.equal(repository.read('CHANGELOG.md'), changelog);
  assert.equal(repository.exists('manifest.yaml'), false);
});

test('ignores VERSION and supports multiple CHANGELOG lines, CRLF, and optional bullets', t => {
  const repository = fixture(t);
  const description = 'VERSION: not-needed\r\n' +
    'CHANGELOG: Enhancement: First change.  \r\n' +
    '\tCHANGELOG: - Bugfix: Second change.\r\n' +
    'Unrelated description text.\r\n';

  succeeds(repository.run(description, '235'));
  assert.equal(repository.read('.changelog/CHANGELOG.235'),
    '- Enhancement: First change.\n- Bugfix: Second change.\n');
  assert.equal(repository.read('CHANGELOG.md'), changelog);
});

test('treats shell syntax and Markdown as literal text', t => {
  const repository = fixture(t);
  const text = 'Bugfix: Preserve `backticks`, $(commands), ${variables}, "quotes", \'apostrophes\', and [links](url).';

  succeeds(repository.run(`CHANGELOG: ${text}`, '236'));
  assert.equal(repository.read('.changelog/CHANGELOG.236'), `- ${text}\n`);
  assert.equal(repository.read('CHANGELOG.md'), changelog);
});

test('rejects missing or empty CHANGELOG input before creating anything', async t => {
  for (const description of [undefined, '', 'VERSION: 3.7.0', 'A mention of CHANGELOG: inside prose.',
    'CHANGELOG: \t', 'CHANGELOG: - ', 'CHANGELOG: Valid entry.\nCHANGELOG:']) {
    await t.test(JSON.stringify(description) || 'undefined', t => {
      const repository = fixture(t);

      fails(repository.run(description, '237'));
      assert.equal(repository.exists('.changelog'), false);
      assert.equal(repository.read('CHANGELOG.md'), changelog);
    });
  }
});

test('rejects invalid PR arguments without writing files', async t => {
  for (const args of [[], ['0'], ['0238'], ['-1'], ['238.md'], ['../238'], ['238', '239']]) {
    await t.test(JSON.stringify(args), t => {
      const repository = fixture(t);

      fails(repository.run('CHANGELOG: Valid entry.', ...args));
      assert.equal(repository.exists('.changelog'), false);
      assert.equal(repository.read('CHANGELOG.md'), changelog);
    });
  }
});

test('preserves an existing fragment and explains how to edit it', t => {
  const repository = fixture(t);
  fs.mkdirSync(path.join(repository.directory, '.changelog'));
  const manualEntry = '- Bugfix: Hand-written entry.\n  With additional detail.\n';
  fs.writeFileSync(path.join(repository.directory, '.changelog/CHANGELOG.239'), manualEntry);

  const result = repository.run('CHANGELOG: Replacement entry.', '239');

  fails(result);
  assert.match(result.stderr, /already exists; edit the fragment directly/);
  assert.equal(repository.read('.changelog/CHANGELOG.239'), manualEntry);
  assert.equal(repository.read('CHANGELOG.md'), changelog);
});
