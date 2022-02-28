.. _pub_edgeai_pi_hdr_programming:

=============================
RPi 40-pin header programming
=============================

TI J72e SK development board contain a 40 pin GPIO
header, similar to the 40 pin header in the Raspberry Pi. These GPIOs can be
controlled for digital input and output using the Python/CPP libraries provided in the
TI GPIO Library packages. The libraries have the same API as the RPi.GPIO
library for Raspberry Pi in order to provide an easy way to move applications
running on the Raspberry Pi to the TI board.

By default, the 40-pin header is not enabled on TDA4VM SK board. This can be enabled by
specifying the dtb overlay file ``k3-j721e-sk-rpi-exp-header.dtbo`` in
``/run/media/mmcblk0p1/uenv.txt`` as given below.

``name_overlays=k3-j721e-edgeai-apps.dtbo k3-j721e-sk-rpi-exp-header.dtbo``

Reboot the board after editing and saving the file.

.. _pub_edgeai_default_pin_setup:

40-pin header default configuration
===================================

The default pin configuration on the SK board is as follows. Any deviation from this
needs modifications to the Linux DTBO. The table below lists pin numbers in all three
supported modes, namely BOARD, BCM, and SOC.

BOARD: **Physical Pin Number**
BCM  : **Broadcom SOC Numbering**
SOC  : **TI SOC Naming**

.. csv-table::
    :header: "BOARD", "BCM", "SOC", "Function"

    "3","2","GPIO0_84","I2C SDA"
    "5","3","GPIO0_83","I2CSCL"
    "7","4","GPIO0_7","GPIO"
    "8","14","GPIO0_70","GPIO"
    "10","15","GPIO0_81","GPIO"
    "11","17","GPIO0_71","GPIO"
    "12","18","GPIO0_1","GPIO"
    "13","27","GPIO0_82","GPIO"
    "15","22","GPIO0_11","GPIO"
    "16","23","GPIO0_5","GPIO"
    "18","24","GPIO0_12","GPIO"
    "19","10","GPIO0_101","GPIO"
    "21","9","GPIO0_107","GPIO"
    "22","25","GPIO0_8","GPIO"
    "23","11","GPIO0_103","GPIO"
    "24","8","GPIO0_102","GPIO"
    "26","7","GPIO0_108","GPIO"
    "29","5","GPIO0_93","GPIO"
    "31","6","GPIO0_94","GPIO"
    "32","12","GPIO0_98","GPIO"
    "33","13","GPIO0_99","GPIO"
    "35","19","GPIO0_2","GPIO"
    "36","16","GPIO0_97","GPIO"
    "37","26","GPIO0_115","GPIO"
    "38","20","GPIO0_3","GPIO"
    "40","21","GPIO0_4","GPIO"

**NOTE**: Please refer to `SK-TDA4VM User's Guide <https://www.ti.com/lit/pdf/spruj21>`_ for details
on the expansion header pin names and functionality. Also refer to section 2.1.2 Power Budget Considerations
for power/voltage limits on the expansion header pins.

Repositories
============
The Python and CPP projects are hosted on Texas Instruments githib. The links to the projects
are given below. The details on installation and testing can be found in the respective project
documentation.

**Python Library**: https://github.com/TexasInstruments/ti-gpio-py.git

**CPP Library**: https://github.com/TexasInstruments/ti-gpio-cpp.git

The above repositories are cloned and installed during the initialization process upon initial boot.
The ``gpiozero`` library is also installed as a part fo the initialization.

Additional References
=====================
Please refer to the lik below for information on the 40-pin header numbering and naming conventions:

| https://pinout.xyz/#


