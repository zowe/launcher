# Zowe Launcher Changelog

All notable changes to the Zowe Launcher package will be documented in this file.
This repo is part of the app-server Zowe Component, and the change logs here may appear on Zowe.org in that section.

## 3.4.0
- Bugfix: Escape backslash when used in environment variable ([#168](https://github.com/zowe/launcher/pull/168))

## 3.3.0
- Bugfix: `zowe.sysMessages` feature was ignoring messages where the matching string was after the position of 126 ([#157](https://github.com/zowe/launcher/pull/157))
- Enhancement: Launcher can now accept PARMLIB CONFIG entries that have more than one member name ([#146](https://github.com/zowe/launcher/pull/146))
- Enhancement: Trimming the sys messages to print from the sys-message-id as optional based on the zowe.sysMessageTrim=true/false. (#147)
- Enhancement: Avoid starting individual apiml components when apiml modulith is enabled ([#160](https://github.com/zowe/launcher/pull/160))

## 3.1.0
- Bugfix: HEAPPOOLS and HEAPPOOLS64 no longer need to be set to OFF for launcher (#133)

## 2.17.0
- Using configmgr to create the component list rather than zwe. (#117)

## 2.13.0
- Bugfix: Changed timestamp to UTC to match the server timestamps (#103)
- Bugfix: Removed server timestamps from syslog to avoid duplicate logging of time (#103)
- Bugfix: Avoided hang on components when components were logging messages longer than 1024 characters. (#103)
- Enhancement: syslog output per line is capped at 512 bytes, extra characters will be omitted (#103)
- Enhancement: Added milliseconds logging to match the server timestamps (#103)
- Enhancement: Launcher prints a message at the beginning of startup to alert users whether or not their log output has long enough lines to be readable if sent to support. (#101)
- Added a wrapper for wtoPrintf3

## 2.12.0
- Added a wrapper for wtoPrintf3
- Bugfix: Fixed a gap in WTO syslog checking
