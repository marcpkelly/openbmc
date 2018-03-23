# OpenBMC

OpenBMC is an open software framework to build a complete Linux image for a Board Management Controller (BMC).

OpenBMC uses the [Yocto Project](https://www.yoctoproject.org) as the underlying building and distro generation framework.

| Board | Status | Description |
|-------|--------|-------------|
[**Wedge**](https://code.facebook.com/posts/681382905244727/introducing-wedge-and-fboss-the-next-steps-toward-a-disaggregated-network/) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildWedge/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildWedge/) | A 40G OS-agnostic TOR switch
[**Yosemite**](https://code.facebook.com/posts/1616052405274961/introducing-yosemite-the-first-open-source-modular-chassis-for-high-powered-microservers-) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildYosemite/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildYosemite/) | An open source modular chassis for high-powered microservers
[**Lightning**](https://code.facebook.com/posts/989638804458007/introducing-lightning-a-flexible-nvme-jbof/) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildLightning/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildLightning/) | A flexible NVMe JBOF
[**Wedge100**](https://code.facebook.com/posts/1802489260027439/wedge-100-more-open-and-versatile-than-ever/) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildWedge100/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildWedge100/) | A 32x100G TOR switch
[**Backpack LC/FC**](https://code.facebook.com/posts/864213503715814/introducing-backpack-our-second-generation-modular-open-switch/) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildGalaxy100/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildGalaxy100/) | Linecard and fabric card in a 128x100G modular open switch
[**Backpack CMM**](https://code.facebook.com/posts/864213503715814/introducing-backpack-our-second-generation-modular-open-switch/) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildCMM/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildCMM/) | Chassis management module in a 128x100G modular open switch
[**Tioga Pass**](https://code.facebook.com/posts/232534267210735/ocp-summit-2017-facebook-news-recap-/) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildFBTP/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildFBTP/) | A dual-socket compute platform
[**YosemiteV2**](https://code.facebook.com/posts/232534267210735/ocp-summit-2017-facebook-news-recap-/) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildFBY2/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildFBY2/) | A refresh of Yosemite
[**Bryce Canyon**](https://code.facebook.com/posts/1869788206569924/introducing-bryce-canyon-our-next-generation-storage-platform/) | [![Build Status](https://jenkins.osquery.io/job/openbmcHeliumBuildFBTTN/badge/icon)](https://jenkins.osquery.io/job/openbmcHeliumBuildFBTTN/) | Disk Storage platform

## Contents

This repository includes 3 set of layers:

* **OpenBMC Common Layer** - Common packages and recipes can be used in different types of BMC.
* **BMC System-on-Chip (SoC) Layer** - SoC specific drivers and tools. This layer includes the bootloader (u-boot) and the Linux kernel. Both the bootloader and Linux kernel shall include the hardware drivers specific for the SoC.
* **Board Specific Layer** - Board specific drivers, configurations, and tools. This layer defines how to configure the image. It also defines what packages to be installed for an OpenBMC image for this board. Any board specific initialization and tools are also included in this layer.

## File structure

The Yocto naming pattern is used in this repository. A "`meta-layer`" is used to name a layer or a category of layers. And `recipe-abc` is used to name a recipe. The project will exist as a meta layer itself! Within the Yocto Project's distribution call this project `meta-openbmc`.

The recipes for OpenBMC common layer are found in `common`.

The BMC SoC layer and board specific layer are grouped together based on the vendor/manufacturer name. For example, all Facebook boards specific code should be in `meta-facebook`. Likewise, `meta-aspeed` includes source code for Aspeed SoCs.

## How to build

Note: In the instruction set below, references to <platform> for some of the steps is an example only and need to be replaced with the respective platform when setting up for a different platform.

1. Set up the build environment based on the Yocto Project's [Quick Start Guide](http://www.yoctoproject.org/docs/current/yocto-project-qs/yocto-project-qs.html).

2. Clone the OpenBMC repository and other open source repositories:
 ```bash
 $ git clone -b helium https://github.com/facebook/openbmc.git
 $ cd openbmc
 $ ./sync_yocto.sh
 ```

3. Initialize a build directory for the platform to build. In the `openbmc` directory:
 ```bash
 $ source openbmc-init-build-env meta-facebook/meta-wedge
 ```
 Choose between `meta-wedge`, `meta-wedge100`, `meta-yosemite`, or any of the other platforms listed in the meta-facebook directory.
 After this step, you will be dropped into a build directory, `openbmc/build`.

4. Start the build within the build directory:
 In general to build for the platform:
 ```bash
 $ bitbake <platform>-image
 ```
 The build process automatically fetches all necessary packages and builds the complete image. The final build results are in `openbmc/build/tmp/deploy/images/<platform>`. The root password will be `0penBmc`, you may change this in the local configuration.

## Build Artifacts

* **u-boot.bin** - This is the u-boot image for the board.
* **uImage** - This the Linux kernel for the board.
* **<platform>-image-<platform>.cpio.lzma.u-boot** - This is the rootfs for the board
* **flash-<platform>** - This is the complete flash image including u-boot, kernel, and the rootfs.

## Kernel Development
By default, OpenBMC build process fetches and build Linux kernel directly from GitHub repository.
To make local kernel changes and build with the modified kernel:
  1. Clone kernel source
```
$ cd poky
$ git clone -b openbmc/helium/4.1 https://github.com/theopolis/linux.git meta-openbmc/meta-aspeed/recipes-kernel/linux/files/linux-aspeed-4.1
```
  2. Update build recipes to point to local kernel source directory
in `meta-aspeed/conf/machine/include/ast2520.inc`, add these 2 lines below the `PREFERRED_PROVIDER_virtual/kernel ?= "linux-aspeed` line

(check `meta-facebook/meta-<platfom>/conf/machine/<platfom>.conf` for the correct `astxxxx.inc` file to modify)

```
INHERIT += "externalsrc"
EXTERNALSRC_pn-linux-aspeed = "<dir>/meta-openbmc/meta-aspeed/recipes-kernel/linux/files/linux-aspeed-4.1"
```

## How can I contribute

If you have an application that can be used by different BMCs, you can contribute your application to the OpenBMC common layer.

If you are a BMC SoC vendor, you can contribute your SoC specific drivers to the BMC SoC layer.

If you are a board vendor, you can contribute your board specific configurations and tools to the Board specific layer. If the board uses a new BMC SoC that is not part of the BMC SoC layer, the SoC specific driver contribution to the BMC SoC layer is also required.