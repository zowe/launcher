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

const script = path.resolve(__dirname, '../../.changelog/merge.js');
const introduction = '# Zowe Launcher Changelog\n\nExisting introduction.\n\n';
const history = '## 3.6.0\n\n- Existing release entry.\n';
const initialChangelog = introduction + history;
const prLink = number => `([#${number}](https://github.com/zowe/launcher/pull/${number}))`;

function fixture(t, fragments = {}, changelog = initialChangelog) {
    const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'launcher-changelog-test-'));
    t.after(() => {
        // Only remove the exact temporary fixture created above.
        assert.equal(path.dirname(directory), path.resolve(os.tmpdir()));
        assert.ok(path.basename(directory).startsWith('launcher-changelog-test-'));
        fs.rmSync(directory, { recursive: true, force: true });
    });
    const fragmentDirectory = path.join(directory, '.changelog');
    fs.mkdirSync(fragmentDirectory);
    fs.copyFileSync(script, path.join(fragmentDirectory, 'merge.js'));
    fs.writeFileSync(path.join(fragmentDirectory, 'README.md'), 'Fragment instructions.\n');
    fs.writeFileSync(path.join(directory, 'CHANGELOG.md'), changelog);
    fs.writeFileSync(path.join(directory, 'manifest.yaml'), 'name: launcher\nversion: 3.7.0\n');
    for (const [name, contents] of Object.entries(fragments)) {
        fs.writeFileSync(path.join(fragmentDirectory, name), contents);
    }
    return {
        directory,
        read: name => fs.readFileSync(path.join(directory, name), 'utf8'),
        exists: name => fs.existsSync(path.join(directory, name)),
        run: (...args) => spawnSync(process.execPath, [path.join(fragmentDirectory, 'merge.js'), ...args], {
            cwd: os.tmpdir(), // The script must resolve paths relative to its own location.
            encoding: 'utf8'
        })
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

test('merges fragments in numeric PR order, preserves Markdown, and consumes only fragments', t => {
    const repository = fixture(t, {
        'CHANGELOG.10': '- Enhancement: Second PR.\n',
        'CHANGELOG.2': '- Bugfix: First PR.\n  Continued explanation.\n\n  - Nested detail.\n- Enhancement: Another change.\n'
    });

    succeeds(repository.run());
    assert.equal(repository.read('CHANGELOG.md'), introduction +
        '## 3.7.0\n\n' +
        `- Bugfix: First PR. ${prLink(2)}\n` +
        '  Continued explanation.\n\n  - Nested detail.\n' +
        `- Enhancement: Another change. ${prLink(2)}\n` +
        `- Enhancement: Second PR. ${prLink(10)}\n\n` + history);
    assert.equal(repository.exists('.changelog/CHANGELOG.2'), false);
    assert.equal(repository.exists('.changelog/CHANGELOG.10'), false);
    assert.equal(repository.read('.changelog/README.md'), 'Fragment instructions.\n');
    assert.equal(repository.read('.changelog/merge.js'), fs.readFileSync(script, 'utf8'));
});

for (const heading of ['## 3.7.0', '## v3.7.0']) {
    test(`inserts into the exact existing release heading ${heading}`, t => {
        const previous = '## 3.7.01\n\n- Similar version stays unchanged.\n\n';
        const existing = heading + '\n- Existing current entry.\n\n';
        const repository = fixture(t, { 'CHANGELOG.23': '- Bugfix: New entry.\n' },
            introduction + previous + existing + history);

        succeeds(repository.run('3.7.0'));
        const merged = repository.read('CHANGELOG.md');
        assert.ok(merged.startsWith(introduction + previous + heading));
        assert.match(merged, /- Existing current entry\./);
        assert.ok(merged.endsWith(history));
        assert.ok(merged.indexOf(`- Bugfix: New entry. ${prLink(23)}`) > merged.indexOf(heading + '\n'));
        assert.ok(merged.indexOf('- Bugfix: New entry.') < merged.indexOf('- Existing current entry.'));
        assert.equal(merged.split(heading + '\n').length, 2);
    });
}

test('dry run previews an explicitly selected version without modifying files', t => {
    const fragment = '- Enhancement: Preview.\n';
    const repository = fixture(t, { 'CHANGELOG.11': fragment });
    const result = repository.run('--dry-run', '3.8.0');

    succeeds(result);
    assert.equal(result.stdout, introduction + '## 3.8.0\n\n' +
        `- Enhancement: Preview. ${prLink(11)}\n\n` + history);
    assert.equal(repository.read('CHANGELOG.md'), initialChangelog);
    assert.equal(repository.read('.changelog/CHANGELOG.11'), fragment);
});

test('preserves CRLF changelog line endings while merging LF fragments', t => {
    const repository = fixture(t, { 'CHANGELOG.9': '- Bugfix: New entry.\n  More detail.\n' },
        initialChangelog.replace(/\n/g, '\r\n'));

    succeeds(repository.run());
    const merged = repository.read('CHANGELOG.md');
    assert.ok(merged.includes(`- Bugfix: New entry. ${prLink(9)}\r\n  More detail.\r\n`));
    assert.equal(merged.replace(/\r\n/g, '').includes('\n'), false);
    assert.ok(merged.endsWith(history.replace(/\n/g, '\r\n')));
});

test('running again after a merge is a successful no-op', t => {
    const repository = fixture(t, { 'CHANGELOG.7': '- Bugfix: One change.\n' });
    succeeds(repository.run());
    const merged = repository.read('CHANGELOG.md');

    succeeds(repository.run());
    assert.equal(repository.read('CHANGELOG.md'), merged);
});

test('invalid fragments prevent all changelog writes and fragment removals', async t => {
    const invalidFragments = [
        ['CHANGELOG.20', ''],
        ['CHANGELOG.20', ' \n\t\n'],
        ['CHANGELOG.20', '# Release heading\n'],
        ['CHANGELOG.20', '- Valid start.\nUnindented prose.\n'],
        ['CHANGELOG.20extra', '- Bad filename.\n'],
        ['CHANGELOG.020', '- Leading zero.\n']
    ];
    for (const [name, contents] of invalidFragments) {
        await t.test(`${name}: ${JSON.stringify(contents)}`, t => {
            const valid = '- Bugfix: Must remain pending.\n';
            const repository = fixture(t, { 'CHANGELOG.1': valid, [name]: contents });

            fails(repository.run());
            assert.equal(repository.read('CHANGELOG.md'), initialChangelog);
            assert.equal(repository.read('.changelog/CHANGELOG.1'), valid);
            assert.equal(repository.read(`.changelog/${name}`), contents);
        });
    }
});

test('refuses to duplicate a PR already present in the target release', t => {
    const changelog = introduction + `## 3.7.0\n\n- Already merged. ${prLink(12)}\n\n` + history;
    const fragment = '- Bugfix: Same PR again.\n';
    const repository = fixture(t, { 'CHANGELOG.12': fragment }, changelog);

    fails(repository.run());
    assert.equal(repository.read('CHANGELOG.md'), changelog);
    assert.equal(repository.read('.changelog/CHANGELOG.12'), fragment);
});

test('check validates fragments and the requested PR without needing release files', t => {
    const fragment = '- Enhancement: Check only.\n';
    const repository = fixture(t, { 'CHANGELOG.33': fragment });
    fs.unlinkSync(path.join(repository.directory, 'CHANGELOG.md'));
    fs.unlinkSync(path.join(repository.directory, 'manifest.yaml'));

    succeeds(repository.run('--check'));
    succeeds(repository.run('--check', '33'));
    fails(repository.run('--check', '34'));
    assert.equal(repository.exists('CHANGELOG.md'), false);
    assert.equal(repository.read('.changelog/CHANGELOG.33'), fragment);
});

test('check rejects invalid pending fragments even when the requested PR is valid', t => {
    const repository = fixture(t, {
        'CHANGELOG.33': '- Enhancement: Valid PR.\n',
        'CHANGELOG.34': 'Invalid fragment.\n'
    });

    fails(repository.run('--check', '33'));
    assert.equal(repository.read('CHANGELOG.md'), initialChangelog);
    assert.equal(repository.exists('.changelog/CHANGELOG.33'), true);
});

test('invalid arguments fail without consuming fragments', async t => {
    for (const args of [['3.7'], ['3.7.0extra'], ['--unknown'], ['3.7.0', '3.8.0']]) {
        await t.test(args.join(' '), t => {
            const fragment = '- Bugfix: Pending entry.\n';
            const repository = fixture(t, { 'CHANGELOG.4': fragment });

            fails(repository.run(...args));
            assert.equal(repository.read('CHANGELOG.md'), initialChangelog);
            assert.equal(repository.read('.changelog/CHANGELOG.4'), fragment);
        });
    }
});

test('help succeeds without changing the repository', t => {
    const repository = fixture(t, { 'CHANGELOG.8': '- Bugfix: Pending.\n' });
    const result = repository.run('--help');

    succeeds(result);
    assert.match(result.stdout, /--dry-run/);
    assert.equal(repository.read('CHANGELOG.md'), initialChangelog);
    assert.equal(repository.exists('.changelog/CHANGELOG.8'), true);
});
