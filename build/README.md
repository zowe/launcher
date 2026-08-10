# Zowe Launcher build directory

## Launcher

* `build.sh` - main script for building the Zowe Launcher
* `dependencies.sh` - check and resolve the dependencies
* `launcher.proj.env` - defines the environment variables and dependencies for building

## Documentation

* `zwelMessages.js` - messages definitions with reason and action
* `buildZwelMessages.js` - node script for printing messages in MD format or check the status of defined and documented messages via `--check` flag

When adding a new message:
* Update the `zwelMessages.js` with proper text, reason and action.
* Run `node buildZwelMessages.js` to review it
* Run `node buildZwelMessages.js > launcher-error-codes.md` and use updated file [here](https://github.com/zowe/docs-site/blob/master/docs/troubleshoot/launcher/launcher-error-codes.md)

Note: if you need a testing message, do not use `MSG_` as prefix, for example:
```c
#define MDEBUG      MY_DEBUG    "7FFFI" "This message will be ignored by check\n"
#define TEST        MSG_TEST    "1234I" "This message will NOT be ignored by check\n"
```
