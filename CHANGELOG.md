# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.3] - 2020-12-25
### Added
- This file.
- LICENSE file to make GitHub happy ([#39](https://github.com/gkdr/carbons/issues/39)).
- The implemented XEP version to the README ([#41](https://github.com/gkdr/carbons/issues/41)).
- A `carbons_internal.h` file declaring internal functions ([#33](https://github.com/gkdr/carbons/pull/33)) (needed and added by [@shtrom](https://github.com/shtrom) :) )

### Fixed
- AppVeyor will now only push tagged builds to GitHub releases.
- Set `rpath` for regular build instead of just for tests ([#38](https://github.com/gkdr/carbons/pull/38)) (thanks, [@wladmis](https://github.com/wladmis)!).
- Stop leaking the retrieved message body ([#42](https://github.com/gkdr/carbons/pull/42)) (thanks, [@henry-nicolas](https://github.com/henry-nicolas)!).
- Preserve `CPPFLAGS` and `LDFLAGS` from env ([#43](https://github.com/gkdr/carbons/pull/43)) (thanks, [@henry-nicolas](https://github.com/henry-nicolas)!).

## [0.2.2] and below
Lost to git commit logs and the GitHub releases page.
