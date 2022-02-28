==============================
Release notes version 00.04.02
==============================

.. _pub_edgeai_new_in_this_release_00_04:

New in this Release
===================

 - First release of Edge AI SDK on TDA4VM device.
 - Python based Deep-learning demos run on host Linux for quick evaluation
 - C++ based Deep-learning demos run in Docker container with Ubuntu 20.04.LTS
 - Deep-learning applications such as image classification,
   object detection and semantic segmentation provided in both C++ and Python
 - Each application is validated with **TFLite** runtime and **NeoAI-DLR**
   runtime offloaded to deep-learning accelerators.
 - Demonstration of GStreamer to interact with USB cameras and eDP/HDMI display
 - Use of OpenCV to perform pre and post processing on A72.
 - All demos are validated using models in TI Model Zoo (2.0) for TFLite and
   TVM based Neo-AI-DLR.

.. _pub_fixed_in_this_release_00_04:

Fixed in this Release
=====================

 - NA

.. _pub_edgeai_known_issues_00_04:

Known Issues
============

 - ADASVISION-4719 - C++ apps exits throwing an error for video inputs
 - ADASVISION-4720 - Output video encode framerate incorrect it is hardcoded to 30fps
 - ADASVISION-4721 - Docker session restart required sometimes when demos exit abruptly with Ctrl+C
 - ADASVISION-4722 - Demo with video input does not loop continuously like image inputs
 - ADASVISION-4723 - Python demos cannot be run in docker session
 - ADASVISION-4724 - C++ demos cannot be built or run in the host OS
 - ADASVISION-4725 - Building OpenCV in docker container on target fails throwing out of memory error

.. _pub_edgeai_software_components_00_04:

Software components
===================

List of software components used in this version

+------------------------------+---------------------+
| Componenet                   | Version             |
+==============================+=====================+
| PSDKLA (filesystem, uboot)   | 07.03.00.05         |
+------------------------------+---------------------+
| PSDKRA (firmware)            | 07.03.00.07         |
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
| PyYAML                       | 5.4.1               |
+------------------------------+---------------------+
| TI Model Zoo                 | 2.0                 |
+------------------------------+---------------------+
