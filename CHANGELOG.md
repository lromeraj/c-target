## v0.6.5
`[NEW]`
- Added interactive bash auto completions.
- Added flag `--update-ascii-version`, this will read your changelog and check for new version.

`[IMPROVEMENTS]`
- Improved help info.

`[FIXES]`
- Fixed a bug that was causing problems when loading default environment.
- Help info is now adapted to the current terminal size.
- Errors now are redirected to `stderr`.

## v0.6.1
`[FIXES]`
- Fixed core-dumped when `-m` flag was given and $TARGET was not defined.
- Improved stability.

`[NEW]`
- Added flag `-v` to see current version.

## v0.5.4
`[FIXES]`
- Minor bug fixes.

## v0.5.3
`[FIXES]`
- Fixed a bug that caused parsing errors
	when comments were present in the configuration file.

`[IMPROVEMENTS]`
- Improved `_comp()` function. This function now detects if it is necesary
	to recompile other files ehn modifying others.
