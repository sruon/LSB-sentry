## Read first
Highly experimental, not supported. Not an official module. Don't @ me.

## Sentry handler for LandSandBoat
- Uploads source code, debug symbols to Sentry after build
- Uploads crash reports to Sentry, including:
    - Minidump
    - Log file

## Limitations
- Windows only at this time
- Focused on SaaS first. Self-hosted Sentry _should_ work with slight changes.
- Only captures crashes.
- You must be creating debug symbols in your build process.
- Debug symbols are large and may use a lot of disk space and bandwidth.
- May not catch all crashes.

## Data privacy
This uploads minidumps and log files to Sentry. They may contain information considered to be PII in some jurisdictions.

Minidump attachment is an opt-in feature on Sentry and not enabled out of the box.

## Requirements
- sentry-cli must be installed and available in PATH
    - `npm install -g @sentry/cli`
- An account with Sentry or a self-hosted Sentry instance.

## Getting started
### Clone
Clone or add this repository as a submodule in your LandSandBoat modules directory.

### Enable the module
Edit `modules/init.txt` and add `LSB-sentry` on a separate line.

### Install sentry-cli
Install `sentry-cli` globally using npm:

```bash
npm install -g @sentry/cli
```

Your IDE may need to be restarted for the command to be available.

### Configure sentry-cli
The `.sentryclirc` file must be updated with your Sentry project, organization and organization token.
  - See https://docs.sentry.io/product/cli/configuration/

You may either find the relevant information in your Sentry account or use:
```bash
sentry-cli login
```

Note that this command may not set the organization and project IDs. 
Double-check the file after running it.

### (Optional) Enable minidump attachment
Settings > Security & Privacy > Store Minidumps As Attachments

### Add Sentry collection endpoint to your config
Edit `settings/main.lua` and add the following option:

```lua
  SENTRY_DSN = "<url_from_sentry_project>"
```

### Compile
Recompiling xi_map should trigger sentry SDK compilation and the automated upload of debug symbols.
```
[2/2] Linking CXX executable lsb\xi_map.exe
INFO    2025-07-31 01:40:17.598009600 -06:00 Loaded config from modules\LSB-sentry\.sentryclirc
INFO    2025-07-31 01:40:17.598591100 -06:00 sentry-cli was invoked with the following command line: "\AppData\Roaming\npm\node_modules\@sentry\cli\node_modules\@sentry\cli-win32-x64\bin\sentry-cli.exe" "debug-files" "upload" "--include-sources" "--log-level=info" "xi_map.pdb" "xi_map.exe" "sentry.dll" "libluajit_64.dll" "libmariadb64.dll" "libzmq_64.dll"
> Found 3 debug information files
> Resolved source code for 1 debug information file
> Prepared debug information files for upload
> Uploaded 3 missing debug information files
> File upload complete:

UPLOADED 8c946c50-b00f-4b1b-9a38-7427546c4f24-35c (xi_map.pdb; x86_64 debug companion)
UPLOADED 8c946c50-b00f-4b1b-9a38-7427546c4f24-35c (xi_map.pdb; x86_64 sources)
UPLOADED 8c946c50-b00f-4b1b-9a38-7427546c4f24-35c (xi_map.exe; x86_64 executable)
```

### Test it!
Login and run the !crash command in-game.

The following logs should be generated, if you have debug logging enabled:
```
[sentry] INFO flushing session and queue before crashpad handler
[sentry] DEBUG sending envelope
[sentry] INFO handing control over to crashpad
```

## Ideas
- Hook this into Lua error handler to capture Lua errors and upload them to Sentry.
- Capture more context information such as PChar, PZone etc.
