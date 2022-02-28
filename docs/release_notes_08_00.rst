==============================
Release notes version 08.00.01
==============================

.. _pub_edgeai_new_in_this_release_08_00:

New in this Release
===================

 - First release of edge_ai_apps integrated with edgeai-gst-plugins
 - Python and C++ apps updates with color-convert, multi-scaler and dl-pre-processing offload
 - Improved support for USB cameras, video streams and images
 - Support for CSI camera with integrated ISP (OV5640)
 - RTSP video stream support
 - Single input, multiple inference support
 - Multi input, multiple inference support

.. _pub_fixed_in_this_release_08_00:

Fixed in this Release
=====================

 - ADASVISION-4607 - Running demo with CSI camera does not run for more than 4-8 frames
 - ADASVISION-4720 - Output video encode framerate incorrect it is hardcoded to 30fps
 - ADASVISION-4721 - Docker session restart required sometimes when demos exit abruptly with Ctrl+C
 - ADASVISION-4722 - Demo with video input does not loop continuously like image inputs
 - ADASVISION-4783 - Functional issues seen when more than 2 USB cameras are used
 - ADASVISION-4796 - Frame drop observed with video input
 - ADASVISION-4805 - Default etcher image size not sufficient for docker build and all model download
 - ADASVISION-4806 - Python based Multi-input, Multi-output demo stops after video stops
 - ADASVISION-4807 - Model name font size does not scale
 - ADASVISION-4840 - Docker setup behind proxy server failing
 - ADASVISION-4870 - Start NTP server to sync clock on board
 - ADASVISION-4889 - All test cases are failing with VX_ZONE_ERROR
 - ADASVISION-4890 - Documentation feedback from Field Team and 3P
 - ADASVISION-4907 - AppInit called multiple times in the C++ usecase
 - ADASVISION-4908 - Post processing of buffers happening in-place
 - ADASVISION-4909 - Ctrl-C not working with applications with Custom GST offload plugins
 - ADASVISION-4922 - ONNX RT Object detection models not working in 08 RC
 - ADASVISION-4944 - Getting VX_ZONE_ERRORS during app de-init
 - ADASVISION-4945 - Image input not working with tiovx plugins
 - ADASVISION-4946 - Multi Input, Multi Inference causing memory leak
 - ADASVISION-4949 - Error from init_script.sh upon login
 - ADASVISION-4969 - C++ Application hangs when Ctrl+C entered  for video, Camera input
 - ADASVISION-4970 - Image input is not working on both Python and C++ apps
 - ADASVISION-4972 - VX_ZONE_ERROR observed in TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416 model for video input
 - ADASVISION-4973 - Python Application hangs when Ctrl+C entered for some OD model with Video/RTSP/Camera input
 - ADASVISION-4974 - Python Application hangs when Ctrl+C entered for some segmentation models model with video input
 - ADASVISION-4977 - C++ apps output for multi-instance stutter issue

  .. _pub_edgeai_known_issues_08_00:

Known Issues
============

 - ADASVISION-4981 - GStreamer-CRITICAL error observed in logs, however demos run fine
 - ADASVISION-4979 - Demo apps hang if RTSP server is down
 - ADASVISION-4960 - Edge AI C++ apps do not run when building in debug mode
 - ADASVISION-4959 - Edge AI Demos do not run more than 4-5 hours
 - ADASVISION-4953 - Video looping does not work with v4l2 HW decoder plugin
 - ADASVISION-4943 - Edge AI: RPI camera cannot capture data correctly for 10 bit format
 - ADASVISION-4809 - Python and C++ apps cannot measure pre and post processing time accurately
 - ADASVISION-4794 - Application takes 10-15 seconds to come up in some cases

.. note::
    Please also refer to `edgeai-gst-plugins issues <https://github.com/TexasInstruments/edgeai-gst-plugins/issues>`_
    for more details on bugs filed on custom GStreamer plugins

.. _pub_edgeai_software_components_08_00:

Software components
===================

List of software components used in this version

+------------------------------+---------------------+
| Componenet                   | Version             |
+==============================+=====================+
| PSDKLA (filesystem, uboot)   | 08.00.01            |
+------------------------------+---------------------+
| PSDKRA (firmware)            | 08.00.01            |
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
| TI Model Zoo                 | 8.0.0               |
+------------------------------+---------------------+
