.. _pub_edgeai_FAQs:

====
FAQs
====

.. _pub_edgeai_vs_psdk:

EdgeAI SDK vs Processor SDK
===========================

It is important to know the differences between EdgeAI SDK and PROCESSOR SDK and
on similar lines the differences between TDA4VM-SK and J721E EVM.

While the SoC is common between TDA4VM-SK and J721E EVM, the board design and
peripherals are very different. For example both eDP and HDMI port is directly
available on TDA4VM-SK where as only eDP port is available on J721E EVM and it
requires add-on infotainment card for HDMI. Similarly there are differences
related to other available peripherals and board properties starting from
power supply. Kindly refer to the respective board user-guide to know the
differences better.

- `J721E EVM user guide <https://www.ti.com/lit/ug/spruis4b/spruis4b.pdf>`_
- `TDA4VM SK user guide <https://www.ti.com/lit/ug/spruj21a/spruj21a.pdf>`_

The SDKs are not compatible either. The EdgeAI SDK works only on
TDA4VM SK and the Processor SDK works only on J721E EVM. There are differences
in software driver availability between both the SDK. Even though EdgeAI SDK is
built on top of Processor SDK, the build configuration is different.
For example peripheral access like GPIO is enabled using Linux on
EdgeAI SDK whereas the same is made available from RTOS core in Processor SDK.

The key differences are highlighted here but not limited to below list.

.. csv-table::
    :header: "Component", "EdgeAI SDK", "Processor SDK"

    "TDA4VM-SK","Supported","Not Supported"
    "J721E-EVM","Not Supported","Supported"
    "Capture Driver","Linux Device Driver","RTOS Driver"
    "Display Driver","Linux Device Driver","RTOS Driver"
    "Video Codecs","Linux Device Driver","Linux Device Driver"
    "VPAC Driver","RTOS Driver","RTOS Driver"
    "DMPAC Driver","RTOS Driver","RTOS Driver"
    "ETHFW","Not Supported","Supported"
    "Peripherals","Linux Device Driver","RTOS Driver"

The :ref:`pub_edgeai_sdk_development_flow` steps are given mostly for users to
developing additional algorithms on C66x/C71x DSP or update existing
VPAC/DMPAC hardware accelerator drivers on RTOS or update the memory map or
add a new linux device driver and so on.

For best experience, kindly use the EdgeAI SDK only on TDA4VM-SK and
Processor SDK on J721E EVM.

.. _pub_edgeai_multiple_usb_cams:

Getting Error when trying to capture from multiple USB cameras simultaneously
=============================================================================

This is a common issue faced in the industry with many USB cameras.
You may get errors like ``Failed to allocate required memory.`` when tying to
capture simultaneously from more than one usb cameras.

The root cause for this issue is that most of the USB cameras requests for more
BW than actually required. If the use case is to capture only from 2 USB cameras
, it can be done by connecting one of them to USB type-C port since it is
internally connected to a seperate instance of USB controller. But if the use
case is to capture from more than 2 cameras, you need to modify the UVC driver
to override the BW allocation.

The root cause and work around is explained in detail in this
blog `Multiple UVC cameras on linux <https://www.thegoodpenguin.co.uk/blog/multiple-uvc-cameras-on-linux>`_

To apply the work around to our SDK, use below steps

#. Download and install Processor SDK Linux `Processor SDK Linux <https://software-dl.ti.com/jacinto7/esd/processor-sdk-linux-jacinto7/07_03_00_05/exports/docs/linux/Overview/Download_and_Install_the_SDK.html>`_
#. Get the patch to add `bandwidth_cap` parameter to `uvcvideo` kernel module `uvcvideo patch <https://www.spinics.net/lists/linux-media/msg175596.html>`_
#. Apply the patch to kernel source in Processor SDK Linux Install path. Please refer to `PSDK building kernel <https://software-dl.ti.com/jacinto7/esd/processor-sdk-linux-jacinto7/07_03_00_05/exports/docs/linux/Foundational_Components_Kernel_Users_Guide.html#overview>`_
#. Compile only `uvcvideo` module using below command

    .. code-block:: bash

        make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- tisdk_j7-evm_defconfig
        make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- ./drivers/media/usb/uvc/uvcvideo.ko
#. Copy `./drivers/media/usb/uvc/uvcvideo.ko` to sk
#. Remove `uvcvideo` module and install modified version using below commands

    .. code-block:: bash

        rmmmod uvcvideo
        insmod uvcvideo.ko
#. Set the desired BW cap as shown below

    .. code-block:: bash

        echo 1200 >  /sys/module/uvcvideo/parameters/bandwidth_cap

.. note::

    The unit of BW here is Bytes/125us, you can estimate the approximate BW
    requirment by multiplying fps with size/frame


.. _pub_psdk_rtos_faq:

PROCESSOR-SDK-RTOS-J721E Developer notes
========================================

Kindly refer to this link to know more about PROCESSOR-SDK-RTOS `Developer Notes <https://software-dl.ti.com/jacinto7/esd/processor-sdk-rtos-jacinto7/08_01_00_13/exports/docs/psdk_rtos/docs/user_guide/developer_notes.html>`_

.. _pub_psdk_linux_faq:

PROCESSOR-SDK-LINUX-J721E How to Guides
=======================================

Kindly refer to this link to know more about PROCESSOR-SDK-Linux `How to Guides <https://software-dl.ti.com/jacinto7/esd/processor-sdk-linux-jacinto7/08_01_00_07/exports/docs/linux/How_to_Guides.html>`_
