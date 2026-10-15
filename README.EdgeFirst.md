# EdgeFirst eIQ Neutron SDK branch

This branch republishes the target-side contents of NXP's eIQ Neutron SDK so they can be fetched by a Yocto recipe. NXP distributes the newest SDK releases only as a zip behind a login on nxp.com, and mirrors older ones to `nxp-imx/neutron.git` some releases later. BitBake cannot fetch the zip, so our BSPs track this branch instead.

Tags are **eIQ Neutron SDK versions** (`v3.2.3`), not Yocto or kernel release names. The upstream branches are named for the BSP they shipped with (`lf-6.18.37_2.1.0`), which hides the version that actually matters: the driver, firmware and compiler must all come from the same SDK, and the recipe filename is not a reliable guide — upstream `neutron_3.1.1.bb` ships SDK 3.1.2.

## Layout

Identical to the layout `nxp-imx/neutron.git` publishes, so this branch is a drop-in source for the same recipes:

| Path | Source in the SDK zip |
|---|---|
| `<soc>/firmware/NeutronFirmware.elf` | `target/<soc>/<soc>/NeutronFirmware.elf` |
| `<soc>/library/libNeutronDriver.so` | `target/<soc>/<soc>/libNeutronDriver.so` |
| `<soc>/include/NeutronDriver.h` | `target/<soc>/driver/include/NeutronDriver.h` |
| `<soc>/include/NeutronDriverBmm.h` | `target/<soc>/driver/include/NeutronDriverBmm.h` |
| `<soc>/include/NeutronErrors.h` | `target/<soc>/common/include/NeutronErrors.h` |
| `LICENSE.txt` | `LA_OPT_NXP_Software_License.txt` |
| `SW-Content-Register.txt` | `Software-Content-Register.txt` |

The mapping was validated against SDK 3.2.1, which NXP publishes both ways: every file above is byte-identical between `eiq-neutron-sdk-linux-3.2.1.zip` and branch `lf-6.18.37_2.1.0`.

Host-side compiler tooling from the zip (`bin/`, `lib/libNeutronCompiler.a`, `include/NeutronCompiler.h`) is deliberately excluded, matching upstream's scope. Model compilation happens in EdgeFirst Studio, not on target.

The SDK's prebuilt `target/<soc>/delegate/libneutron_delegate.so` is also excluded; the TensorFlow Lite delegate is built from source so it can carry the dma-buf import path.

`SBOM.spdx.json` is not carried forward — the upstream file describes a different release and the SDK archive ships no replacement.

## Verifying a tag against the official archive

```sh
unzip -d /tmp/eiq eiq-neutron-sdk-linux-3.2.3.zip 'target/imx95/*'
git show v3.2.3:imx95/firmware/NeutronFirmware.elf | sha256sum
sha256sum /tmp/eiq/target/imx95/imx95/NeutronFirmware.elf
```

Both report `7d1d0deba9235c8841b16f19fd9642de2ba402db7852cbe158600e8809984fd9`.

## Kernel requirement

SDK 3.2.x firmware replies `0xA1` to a mailbox reset; the driver in BSP 6.18.20 expects `0x0` and will wait forever. Boards need the mailbox handshake from BSP 6.18.37 backported, which NXP wrote to negotiate both values and so stays compatible with 3.1.x firmware. Keep driver, firmware and compiler from one SDK: a mismatch reports `Microcode version mismatch!`.
