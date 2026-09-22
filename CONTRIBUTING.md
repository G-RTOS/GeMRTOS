# Contributing to GeMRTOS

Thank you for your interest in contributing to GeMRTOS.

## License and Contributor Agreement

GeMRTOS software is licensed under the
[PolyForm Noncommercial License 1.0.0](LICENSE.md). Because GeMRTOS reserves the
right to grant commercial licenses, contributors must agree that their
contributions may be used in commercial distributions. By submitting a pull
request or patch you confirm that:

1. Your contribution is your original work (or you have the right to submit it).
2. You grant the GeMRTOS project a perpetual, worldwide, royalty-free licence to
   use, reproduce, modify, and distribute your contribution, including in
   commercial distributions.

If you are contributing on behalf of an employer, ensure your employer approves
the above.

## Reporting Bugs

Open an issue in the project repository. Please include:

- GeMRTOS version or commit hash
- Quartus Prime edition and version, and target FPGA family
- Processor type (Nios II or Nios V) and processor count
- Minimal reproduction (task code + configuration that triggers the bug)
- Expected vs. actual behaviour
- Relevant output from the debug console or System Console

## Submitting Changes

1. Fork the repository and create a feature branch from `main`.
2. Make your changes following the coding style below.
3. Test on real hardware if possible; at minimum verify the change compiles
   without warnings for both Nios II and Nios V targets.
4. Open a pull request against `main` with a clear description of what changed
   and why.

## Coding Style

**C source:**
- Indentation: tabs (matching the existing codebase)
- Naming: `gk_` prefix for kernel-internal functions, `gu_` for public API,
  `gm_` for macros/inline hardware access, `GS_` for struct types, `G_` for
  constants and globals
- Every public `gu_` function must hold `gm_GeMRTOSCriticalSectionEnter` for
  the entire duration of any kernel state modification — never split a critical
  section across a function boundary
- New functions need Doxygen: `@brief`, `@param`, `@return`, `@note` for
  context constraints (ISR-safe? task context only? must hold critical section?)

**Documentation:**
- The User and Development Manual is not part of this repository. If you find an
  error in it, or in a `README.md` here, open an issue quoting the passage.

## Debug Flags

When developing, keep `G_DEBUG_SYSTEM_CHECK_ENABLE`, `G_DEBUG_WHILEFOREVER_ENABLE`,
`G_DEBUG_GET_POINTED_FIELD_CHECK` and `G_DEBUG_SET_POINTED_FIELD_CHECK` at `1` in
`GeMRTOS_controller/HAL/inc/gemrtos_config.h` (that is how the file ships).
Before submitting, verify your change does not introduce new `PRINT_ASSERT`
failures. Do not submit changes that require `G_DEBUG_GET_POINTED_FIELD_CHECK`
or `G_DEBUG_SET_POINTED_FIELD_CHECK` to be set to `0` in order to work correctly.
`G_DEBUG_WAITING_LOOP` is a different kind of flag — it ships at `0` and is set to `1`
only for a debugger session — so leave it alone.

## Questions

Open an issue labelled `question` for architecture or design questions before
implementing a large change — early discussion saves effort.
