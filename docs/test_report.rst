===========
Test Report
===========

Here is the summary of the sanity tests we ran with both Python and C++ demos.
Test cases vary with different inputs, outputs, runtime, models, python/c++ apps.

1. Inputs:

    * Camera (Logitech C270, 1280x720, JPEG)
    * Camera (Omnivision OV5640, 1280x720, YUV)
    * Camera (Rpi v2 Sony IMX219, 1920x1080, RAW)
    * Image files (30 images under edge_ai_apps/data/images)
    * Video file (10s video 1 file under edge_ai_apps/data/videos)
    * RSTP Video Server

2. Outputs:

    * Display (eDP or HDMI)
    * File write to SD card

3. Inference Type:

    * Image classification
    * Object detection
    * Semantic segmentation

4. Runtime/models:

    * DLR
    * TFLite
    * ONNX

5. Applications:

    * Python
    * C++

6. Platform:

   * Host OS
   * Docker


Demo Apps test report
=====================

Single Input Single Output
--------------------------

.. csv-table::
        :header: "Category", "# test case", "Pass", "Fail"

        Host OS - Python,99,99,0
        Host OS - C++,99,99,0

.. csv-table::
        :header: "S.No", "Models", "Input", "Output", "Host OS-C++", "Host OS-Python", "Comments"

        1,TVM-CL-3410-gluoncv-mxnet-mobv2,Image,Display,Pass,Pass,
        2,TVM-CL-3410-gluoncv-mxnet-mobv2,Image,Video-Filewrite,Pass,Pass,
        3,TVM-CL-3410-gluoncv-mxnet-mobv2,Image,Image-Filewrite,Pass,Pass,
        4,TVM-CL-3410-gluoncv-mxnet-mobv2,Video,Display,Pass,Pass,
        5,TVM-CL-3410-gluoncv-mxnet-mobv2,Video,Video-Filewrite,Pass,Pass,
        6,TVM-CL-3410-gluoncv-mxnet-mobv2,USB Camera,Display,Pass,Pass,
        7,TVM-CL-3410-gluoncv-mxnet-mobv2,USB Camera,Video-Filewrite,Pass,Pass,
        8,TVM-CL-3410-gluoncv-mxnet-mobv2,CSI Camera,Display,Pass,Pass,
        9,TVM-CL-3410-gluoncv-mxnet-mobv2,CSI Camera,Video-Filewrite,Pass,Pass,
        10,TVM-CL-3410-gluoncv-mxnet-mobv2,RTSP - Video,Display,Pass,Pass,
        11,TVM-CL-3410-gluoncv-mxnet-mobv2,RTSP - Video,Video-Filewrite,Pass,Pass,
        12,TFL-CL-0000-mobileNetV1-mlperf,Image,Display,Pass,Pass,
        13,TFL-CL-0000-mobileNetV1-mlperf,Image,Video-Filewrite,Pass,Pass,
        14,TFL-CL-0000-mobileNetV1-mlperf,Image,Image-Filewrite,Pass,Pass,
        15,TFL-CL-0000-mobileNetV1-mlperf,Video,Display,Pass,Pass,
        16,TFL-CL-0000-mobileNetV1-mlperf,Video,Video-Filewrite,Pass,Pass,
        17,TFL-CL-0000-mobileNetV1-mlperf,USB Camera,Display,Pass,Pass,
        18,TFL-CL-0000-mobileNetV1-mlperf,USB Camera,Video-Filewrite,Pass,Pass,
        19,TFL-CL-0000-mobileNetV1-mlperf,CSI Camera,Display,Pass,Pass,
        20,TFL-CL-0000-mobileNetV1-mlperf,CSI Camera,Video-Filewrite,Pass,Pass,
        21,TFL-CL-0000-mobileNetV1-mlperf,RTSP - Video,Display,Pass,Pass,
        22,TFL-CL-0000-mobileNetV1-mlperf,RTSP - Video,Video-Filewrite,Pass,Pass,
        23,ONR-CL-6360-regNetx-200mf,Image,Display,Pass,Pass,
        24,ONR-CL-6360-regNetx-200mf,Image,Video-Filewrite,Pass,Pass,
        25,ONR-CL-6360-regNetx-200mf,Image,Image-Filewrite,Pass,Pass,
        26,ONR-CL-6360-regNetx-200mf,Video,Display,Pass,Pass,
        27,ONR-CL-6360-regNetx-200mf,Video,Video-Filewrite,Pass,Pass,
        28,ONR-CL-6360-regNetx-200mf,USB Camera,Display,Pass,Pass,
        29,ONR-CL-6360-regNetx-200mf,USB Camera,Video-Filewrite,Pass,Pass,
        30,ONR-CL-6360-regNetx-200mf,CSI Camera,Display,Pass,Pass,
        31,ONR-CL-6360-regNetx-200mf,CSI Camera,Video-Filewrite,Pass,Pass,
        32,ONR-CL-6360-regNetx-200mf,RTSP - Video,Display,Pass,Pass,
        33,ONR-CL-6360-regNetx-200mf,RTSP - Video,Video-Filewrite,Pass,Pass,
        34,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,Image,Display,Pass,Pass,
        35,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,Image,Video-Filewrite,Pass,Pass,
        36,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,Image,Image-Filewrite,Pass,Pass,
        37,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,Video,Display,Pass,Pass,
        38,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,Video,Video-Filewrite,Pass,Pass,
        39,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,USB Camera,Display,Pass,Pass,
        40,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,USB Camera,Video-Filewrite,Pass,Pass,
        41,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,CSI Camera,Display,Pass,Pass,
        42,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,CSI Camera,Video-Filewrite,Pass,Pass,
        43,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,RTSP - Video,Display,Pass,Pass,
        44,TVM-OD-5020-yolov3-mobv1-gluon-mxnet-416x416,RTSP - Video,Video-Filewrite,Pass,Pass,
        45,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,Image,Display,Pass,Pass,
        46,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,Image,Video-Filewrite,Pass,Pass,
        47,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,Image,Image-Filewrite,Pass,Pass,
        48,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,Video,Display,Pass,Pass,
        49,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,Video,Video-Filewrite,Pass,Pass,
        50,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,USB Camera,Display,Pass,Pass,
        51,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,USB Camera,Video-Filewrite,Pass,Pass,
        52,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,CSI Camera,Display,Pass,Pass,
        53,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,CSI Camera,Video-Filewrite,Pass,Pass,
        54,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,RTSP - Video,Display,Pass,Pass,
        55,TFL-OD-2020-ssdLite-mobDet-DSP-coco-320x320,RTSP - Video,Video-Filewrite,Pass,Pass,
        56,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,Image,Display,Pass,Pass,
        57,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,Image,Video-Filewrite,Pass,Pass,
        58,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,Image,Image-Filewrite,Pass,Pass,
        59,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,Video,Display,Pass,Pass,
        60,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,Video,Video-Filewrite,Pass,Pass,
        61,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,USB Camera,Display,Pass,Pass,
        62,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,USB Camera,Video-Filewrite,Pass,Pass,
        63,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,CSI Camera,Display,Pass,Pass,
        64,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,CSI Camera,Video-Filewrite,Pass,Pass,
        65,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,RTSP - Video,Display,Pass,Pass,
        66,ONR-OD-8050-ssd-lite-regNetX-800mf-fpn-bgr-coco-512x512,RTSP - Video,Video-Filewrite,Pass,Pass,
        67,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,Image,Display,Pass,Pass,
        68,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,Image,Video-Filewrite,Pass,Pass,
        69,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,Image,Image-Filewrite,Pass,Pass,
        70,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,Video,Display,Pass,Pass,
        71,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,Video,Video-Filewrite,Pass,Pass,
        72,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,USB Camera,Display,Pass,Pass,
        73,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,USB Camera,Video-Filewrite,Pass,Pass,
        74,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,CSI Camera,Display,Pass,Pass,
        75,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,CSI Camera,Video-Filewrite,Pass,Pass,
        76,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,RTSP - Video,Display,Pass,Pass,
        77,TVM-SS-5720-deeplabv3lite-regnetx800mf-cocoseg21-512x512,RTSP - Video,Video-Filewrite,Pass,Pass,
        78,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,Image,Display,Pass,Pass,
        79,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,Image,Video-Filewrite,Pass,Pass,
        80,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,Image,Image-Filewrite,Pass,Pass,
        81,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,Video,Display,Pass,Pass,
        82,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,Video,Video-Filewrite,Pass,Pass,
        83,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,USB Camera,Display,Pass,Pass,
        84,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,USB Camera,Video-Filewrite,Pass,Pass,
        85,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,CSI Camera,Display,Pass,Pass,
        86,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,CSI Camera,Video-Filewrite,Pass,Pass,
        87,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,RTSP - Video,Display,Pass,Pass,
        88,TFL-SS-2580-deeplabv3_mobv2-ade20k32-mlperf-512x512,RTSP - Video,Video-Filewrite,Pass,Pass,
        89,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,Image,Display,Pass,Pass,
        90,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,Image,Video-Filewrite,Pass,Pass,
        91,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,Image,Image-Filewrite,Pass,Pass,
        92,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,Video,Display,Pass,Pass,
        93,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,Video,Video-Filewrite,Pass,Pass,
        94,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,USB Camera,Display,Pass,Pass,
        95,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,USB Camera,Video-Filewrite,Pass,Pass,
        96,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,CSI Camera,Display,Pass,Pass,
        97,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,CSI Camera,Video-Filewrite,Pass,Pass,
        98,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,RTSP - Video,Display,Pass,Pass,
        99,ONR-SS-8610-deeplabv3lite-mobv2-ade20k32-512x512,RTSP - Video,Video-Filewrite,Pass,Pass,


Single Input Multi Output
-------------------------
.. csv-table::
        :header: "Category", "# test case", "Pass", "Fail"

        Host OS - Python,15,15,0
        docker - Python,15,15,0
        Host OS - C++,15,15,0
        Docker - C++,15,15,0
.. csv-table::
        :header: "S.No", "Models", "Input", "Output", "Host OS-C++", "Host OS-Python", "Comments"

        1,"2 Models (TFL-CL, ONR-SS)",%04d.jpg,Display,Pass,Pass,
        2,"3-Models (TVM-CL, TFL-OD, ONR-SS)",%04d.jpg,Display,Pass,Pass,
        3,"4-Models (TVM-SS, TFL-OD, ONR-SS, ONR-CL)",%04d.jpg,Display,Pass,Pass,
        4,"2 Models (TFL-CL, ONR-SS)",video_0000.mp4,Display,Pass,Pass,
        5,"3-Models (TVM-CL, TFL-OD, ONR-SS)",video_0000.mp4,Display,Pass,Pass,
        6,"4-Models (TVM-SS, TFL-OD, ONR-SS, ONR-CL)",video_0000.mp4,Display,Pass,Pass,
        7,"2 Models (TFL-CL, ONR-SS)",USB_camera,Display,Pass,Pass,
        8,"3-Models (TVM-CL, TFL-OD, ONR-SS)",USB_camera,Display,Pass,Pass,
        9,"4-Models (TVM-SS, TFL-OD, ONR-SS, ONR-CL)",USB_camera,Display,Pass,Pass,
        10,"2 Models (TFL-CL, ONR-SS)",CSI_camera,Display,Pass,Pass,
        11,"3-Models (TVM-CL, TFL-OD, ONR-SS)",CSI_camera,Display,Pass,Pass,
        12,"4-Models (TVM-SS, TFL-OD, ONR-SS, ONR-CL)",CSI_camera,Display,Pass,Pass,
        13,"2 Models (TFL-CL, ONR-SS)",rtsp,Display,Pass,Pass,
        14,"3-Models (TVM-CL, TFL-OD, ONR-SS)",rtsp,Display,Pass,Pass,
        15,"4-Models (TVM-SS, TFL-OD, ONR-SS, ONR-CL)",rtsp,Display,Pass,Pass,

Multi Input Multi Output
------------------------
.. csv-table::
        :header: "Category", "# test case", "Pass", "Fail"

        Host OS - Python,8,8,0
        docker - Python,8,8,0
        Host OS - C++,8,8,0
        Docker - C++,8,8,0
.. csv-table::
        :header: "S.No", "Models", "Input", "Output", "Host OS-C++", "Host OS-Python","Comments"

        1,"2 Models (TVM-CL, TFL-OD)","%04d.jpg,video_0000.mp4",Display,Pass,Pass,
        2,"2 Models (TVM-OD, ONR-SS)","%04d.jpg,rtsp",Video-Filewrite,Pass,Pass,
        3,"2 Models (ONR-CL, TVM-SS)","%04d.jpg,USB_camera",Display,Pass,Pass,
        4,"3-Models (TVM-CL, TFL-OD, ONR-SS)","%04d.jpg,CSI_camera,rtsp",Video-Filewrite,Pass,Pass,
        5,"3-Models (TVM-CL, TFL-OD, ONR-SS)","video_0000.mp4,rtsp,%04d.jpg",Display,Pass,Pass,
        6,"3-Models (TFL-CL, ONR-CL, TVM-SS)","video_0000.mp4,USB_camera,CSI_camera",Video-Filewrite,Pass,Pass,
        7,"4-Models (TVM-CL, TFL-SS, ONR-OD, TFL-CL)","USB_camera,CSI_camera",Display,Pass,Pass,
        8,"4-Models (TVM-SS, TFL-SS, ONR-SS, ONR-OD)","USB_camera,video_0000.mp4",Video-Filewrite,Pass,Pass,

.. note::
    * Video file from RTSP server used for RTSP input test
    * Please refer to the :ref:`pub_edgeai_known_issues` section for more details
