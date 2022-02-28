==============================
Release notes version 08.01.00
==============================

.. _pub_edgeai_new_in_this_release:

New in this Release
===================
 - Support for Sony's IMX219 raw sensor with ISP offload using GStreamer element
 - Python and C++ apps integrated with newer GStreamer plugins,
   - `tiovxmosaic <https://github.com/TexasInstruments/edgeai-gst-plugins/wiki/tiovxmosaic>`_
   - `tiovxisp <https://github.com/TexasInstruments/edgeai-gst-plugins/wiki/tiovxisp>`_
 - Newer GST plugins available part of edgeai-gst-plugins repo
   - `tiovxcolorblend <https://github.com/TexasInstruments/edgeai-gst-plugins/wiki/tiovxdlcolorblend>`_
   - `tiovxldc <https://github.com/TexasInstruments/edgeai-gst-plugins/wiki/tiovxldc>`_
   - `tiovxmux <https://github.com/TexasInstruments/edgeai-gst-plugins/wiki/tiovxmux>`_
   - `tiovxdemux <https://github.com/TexasInstruments/edgeai-gst-plugins/wiki/tiovxdemux>`_
 - Support for hardware accelerated H.264 encoder
 - Support for hardware accelerated H.265 decoder
 - New easy to use ti-gpio python and C++ libraries
 - Multiple bug fixes to improve stability.

.. _pub_fixed_in_this_release:

Fixed in this Release
=====================

 - ADASVISION-5115 - Edge AI Apps with RPi Camera as source hangs after running for around 45 min
 - ADASVISION-5103 - Random hang at vxWaitGraph for MCU2_0 based OpenVx nodes
 - ADASVISION-5102 - v4l2h265 decoder not working when followed by tee and tiovxmultiscalar
 - ADASVISION-5097 - Video file output is corrupt / not playable in docker
 - ADASVISION-5079 - Documentation errors for OV5640 CSI camera
 - ADASVISION-5059 - Edge AI SDK setup script fails to build edgeai-gst-plugins
 - ADASVISION-5048 - appDeInit not called when tiovx element is present in output pipeline (C++ apps only)
 - ADASVISION-5042 - setup_script fails to install dependency
 - ADASVISION-4991 - Decoder + kmssink elements generates pink and green frames blinking
 - ADASVISION-4981 - GStreamer-CRITICAL error observed in logs, however demos run fine
 - ADASVISION-4979 - Demo apps hang if RTSP server is down
 - ADASVISION-4960 - Edge AI C++ apps do not run when building in debug mode
 - ADASVISION-4959 - Edge AI Demos do not run more than 4-5 hours
 - ADASVISION-4872 - VX_ZONE-ERROR when application runs for more than 250 times

  .. _pub_edgeai_known_issues:

Known Issues
============

 - ADASVISION-5149 - Hostname incorrect in SK shows j7-evm instead of tda4vm-sk
 - ADASVISION-5117 - USB 3.0 device is recognized as USB 2.1 sometimes
 - ADASVISION-5113 - GST Pipeline error: Error pulling tensor from GST Pipeline
 - ADASVISION-5070 - Edge AI demo output not resizing properly on 4K monitor
 - ADASVISION-4997 - Multi-input multi-inference demo freezes after long duration run
 - ADASVISION-4953 - Video looping does not work with v4l2 HW decoder plugin
 - ADASVISION-4943 - IMX219 based Rpi camera cannot capture data correctly for 10 bit format
 - ADASVISION-4809 - Python and C++ apps cannot measure pre and post processing time accurately
 - ADASVISION-4794 - Application takes 10-15 seconds to come up in some cases

.. note::
    Please also refer to `edgeai-gst-plugins issues <https://github.com/TexasInstruments/edgeai-gst-plugins/issues>`_
    for more details on bugs filed on custom GStreamer plugins

.. _pub_edgeai_software_components:

Software components
===================

List of software components used in this version

+------------------------------+---------------------+
| Componenet                   | Version             |
+==============================+=====================+
| PSDKLA (filesystem, uboot)   | 08.01.00.07         |
+------------------------------+---------------------+
| PSDKRA (firmware)            | 08.01.00.13         |
+------------------------------+---------------------+
| Docker                       | 19.03.8-ce          |
+------------------------------+---------------------+
| Ubuntu (Docker)              | 20.04.LTS           |
+------------------------------+---------------------+
| Python                       | 3.8                 |
+------------------------------+---------------------+
| OpenCV                       | 4.5.1               |
+------------------------------+---------------------+
| GStreamer                    | 1.16.2              |
+------------------------------+---------------------+
| Cmake                        | 3.16.3              |
+------------------------------+---------------------+
| Ninja                        | 1.10.0              |
+------------------------------+---------------------+
| Meson                        | 0.57.2              |
+------------------------------+---------------------+
| Jupyterlab                   | 3.0.14              |
+------------------------------+---------------------+
| NeoAI - DLR                  | 1.8.0               |
+------------------------------+---------------------+
| Tensorflow                   | 2.4                 |
+------------------------------+---------------------+
| TFLite-runtime               | 2.5.0               |
+------------------------------+---------------------+
| ONNX-runtime                 | 1.7.0               |
+------------------------------+---------------------+
| PyYAML                       | 5.4.1               |
+------------------------------+---------------------+
| TI Model Zoo                 | 8.1.0               |
+------------------------------+---------------------+
| edgeai-gst-plugins           | 0.5.3               |
+------------------------------+---------------------+
| edgeai-tiovx-modules         | 0.2.0               |
+------------------------------+---------------------+