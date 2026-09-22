# 0.3.5 release preparation

- Keep the user-verified 0.3.4 Chinese/English input and F8 fixes.
- Remove Meridian's exact 1.5.0 release whitelist; negotiate the public Meridian.View/1 interface. Retain its ABI guard test.
- Prisma negotiates V1; Menu Framework checks required exports; SkyUI and Modex have no release whitelist.
- Skyrim runtime restrictions remain 1.5.97 / 1.6.1170, and SimpleIME must remain disabled.
- Publish original source under GPL-3.0-only, with third-party source/API headers and notices for rebuilding.

Validation: local build, existing Unicode/IME/DOM/settings tests, public API negotiation checks and SKSE metadata verification. The user confirmed 0.3.4 works on their 1.6.1170 setup; the new Meridian whitelist removal still needs game verification with future Meridian releases. SE 1.5.97 remains unverified in game.
