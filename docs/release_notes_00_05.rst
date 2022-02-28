==============================
Release notes version 00.05.00
==============================

.. _pub_edgeai_new_in_this_release_00_05:

New in this Release
===================

 - Python and C++ demos supported on both docker and host
 - ONNX Runtime support
 - RTSP video stream support
 - Single input, multiple inference support
 - Multi input, multiple inference support


.. _pub_fixed_in_this_release_00_05:

Fixed in this Release
=====================

 - ADASVISION-4719 - C++ apps exits throwing an error for video inputs
 - ADASVISION-4723 - Python demos cannot be run in docker session
 - ADASVISION-4724 - C++ demos cannot be built or run in the host OS
 - ADASVISION-4780 - Demo apps hang while running them back to back
 - ADASVISION-4784 - Docker run command not spawning to /opt/edge_ai_apps
 - ADASVISION-4785 - C++ application crashes for TVM based mxnet and video input
 - ADASVISION-4790 - ONNX semantic segmentation model output corrupted
 - ADASVISION-4791 - Few regNetx ONNX & TVM classification models are not running in Python demo
 - ADASVISION-4792 - Python demos performance poorer than C++ demos on 0.5 RC
 - ADASVISION-4793 - Performance of demos in 0.5RC poorer than 0.4.2
 - ADASVISION-4798 - Font size of Top5 class is large for multichannel display

  .. _pub_edgeai_known_issues_00_05:

Known Issues
============

 - ADASVISION-4720 - Output video encode framerate incorrect it is hardcoded to 30fps
 - ADASVISION-4721 - Docker session restart required sometimes when demos exit abruptly with Ctrl+C
 - ADASVISION-4722 - Demo with video input does not loop continuously like image inputs
 - ADASVISION-4781 - install_dl_dependencies.sh is not working on Host OS
 - ADASVISION-4783 - Performance issues observed for more than 2 USB cameras
 - ADASVISION-4794 - Application tasks 10-15 seconds to come up in some cases
 - ADASVISION-4795 - Some ONNX classification models occasionally do not run
 - ADASVISION-4796 - Frame drop observed with video input
 - ADASVISION-4805 - Default etcher image size not sufficient for docker build and all model download
 - ADASVISION-4806 - Python based Multi-input, Multi-output demo stops after video stops
 - ADASVISION-4807 - Model name font size does not scale
 - ADASVISION-4809 - Python and C++ apps cannot measure pre and post processing time accurately

.. _pub_edgeai_software_components_00_05:

Software components
===================

List of software components used in this version

+------------------------------+---------------------+
| Componenet                   | Version             |
+==============================+=====================+
| PSDKLA (filesystem, uboot)   | 07.03.01            |
+------------------------------+---------------------+
| PSDKRA (firmware)            | 07.03.01            |
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
| TI Model Zoo                 | 2.0                 |
+------------------------------+---------------------+
