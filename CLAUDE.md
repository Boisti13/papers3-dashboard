# Working on papers3-dashboard

- `git pull` first — the owner works from more than one PC.
- Work on **`dev`**, push to `dev`. `main` is only for releases, and only when
  the owner asks ("merge to main and make a new release"): merge `dev` into
  `main`, tag `vX.Y.Z`, GitHub release with the CHANGELOG section as notes.
  (Pull request #4 was squash-merged into `main` on 2026-05-22 without a tag;
  `dev` was fast-forwarded to it on 2026-10-16. Its changes sit under
  *Unreleased* and become v1.4.0.)
- Every user-visible change: an entry under *Unreleased* in `CHANGELOG.md`
  (Added / Changed / Fixed / Under the hood) and the README updated to match
  (Highlights, Pages, entity tables).
- **Build** with PlatformIO: `pio run` (on Windows
  `~/.platformio/penv/Scripts/pio run`). Needs `src/secrets.h` — copy
  `src/secrets.h.template`; never commit `secrets.h`. The owner tests on the
  real device; flash over USB (`pio run --target upload`) or Wi-Fi
  (`./ota_upload.sh <ip>` after tapping OTA on the Settings page) only when
  the owner says so.
- **Home Assistant side**: the live files are `/config/packages/papers3dash.yaml`
  (commands, push on connect) and `/config/packages/mqtt_statestream.yaml`
  (the entities mirrored to MQTT for the
  PaperS3). The repo's copy is `ha/papers3dash.yaml`. They can drift —
  diff over `ssh ha` before changing either, and follow the homelab rules
  for editing HA YAML (backup to `/config/project-backups/`, `ha core check`;
  `mqtt_statestream` changes need a restart).
- A new entity on the dashboard needs three things: its topic in
  `src/config.h`, the entity in `mqtt_statestream.yaml`, and — if the page
  sends commands — an automation in the package.
- Public repo: no secrets or passwords in committed files. The entity IDs and
  topics in `config.h` are the owner's own setup by design.
