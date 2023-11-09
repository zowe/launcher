# Zowe Launcher Changelog

All notable changes to the Zowe Launcher package will be documented in this file..
This repo is part of the app-server Zowe Component, and the change logs here may appear on Zowe.org in that section.

## 2.13.0

- Bugfix: Changed timestamp to UTC to match the server timestamps (#103)
- Bugfix: Removed server timestamps from syslog to avoid duplicate logging of time (#103)
- Bugfix: Avoided hang on components when components were logging messages longer than 1024 characters. (#103)
- Enhancement: syslog output per line is capped at 512 bytes, extra characters will be omitted (#103)
- Enhancement: Added milliseconds logging to match the server timestamps (#103)

## 2.12.0
- Added a wrapper for wtoPrintf3
- Bugfix: Fixed a gap in WTO syslog checking