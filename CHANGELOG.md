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
