# Changelog fragments

Each PR has a separate file named `CHANGELOG.<PR number>` in this directory,
for example `.changelog/CHANGELOG.234`. Keep these files tracked in Git; the
existing build packaging excludes this dot directory.

## From the PR description

Add a release note on a line starting with `CHANGELOG:`:

```text
CHANGELOG: Enhancement: Describe the change visible to users.
```

CI runs `.github/workflows/set-changelog.js` to create the PR's fragment and
commit it to the PR branch if the file is missing. No `VERSION:` field is needed;
the release version is chosen when fragments are merged. Repeat `CHANGELOG:` on
separate lines for multiple entries. Keep each entry on the same line as its keyword.

Opening or editing the PR description, pushing a commit, or changing a label
reruns the check. An existing fragment takes precedence over the description:
edit that file directly to change its entries after it has been created.

Automatic commits use the repository's existing `ZOWE_ROBOT_TOKEN` secret so
checks run on the new commit. For fork PRs, or when that token is unavailable,
commit the fragment manually as described below. Maintainers can apply the
`no-changelog` label when an entry is unnecessary.

## Manual fragments

Create `.changelog/CHANGELOG.<PR number>` with one or more Markdown list entries,
without a version or heading:

```markdown
- Enhancement: Describe the change visible to users.
- Bugfix: Describe the corrected behavior.
  Indent any continuation lines or nested lists.
```

Each entry starts with `- `. The merge script adds a link to the PR on the
first line of every entry, so the fragment does not need a PR link. CI checks
that the PR adds or updates its own fragment and validates all fragments.

## Merging for a release

Run these commands from the repository root with Node.js; no packages need
to be installed:

```shell
node .changelog/merge.js --check
node .changelog/merge.js --dry-run
node .changelog/merge.js
```

`--check` validates fragments without changing files; an optional PR number,
such as `--check 234`, also requires that PR's fragment. `--dry-run` prints the
prospective changelog without changing files.

The merge uses the version from `manifest.yaml`. To specify a release version,
use `node .changelog/merge.js 3.7.0` or preview it with
`node .changelog/merge.js --dry-run 3.7.0`.

All fragments are merged in ascending PR-number order into the matching version
section of `CHANGELOG.md`, or a new section is added at the top. Successfully
merged fragments are deleted. Review and commit `CHANGELOG.md` together with
the fragment deletions. Apply `no-changelog` to this release PR, since it merges
the existing entries. Run `node .changelog/merge.js --help` for usage.
