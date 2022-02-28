/*
 *  Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PRE_PROCESS_IMAGE_CONFIG_H_
#define _PRE_PROCESS_IMAGE_CONFIG_H_

/* Module headers. */
#include <common/include/edgeai_debug.h>
#include <dl_inferer/include/ti_dl_inferer.h>

/**
 * \defgroup group_edgeai_cpp_apps_pre_proc Image Pre-processing
 *
 * \brief Class providing interface for generic pre-processing logic.
 *
 * \ingroup group_edgeai_cpp_apps
 */

namespace ti::edgeai::common
{
// Default values to avoid exceptions resulting from using uninitialized values
#define TI_PREPROC_DEFAULT_WIDTH   320
#define TI_PREPROC_DEFAULT_HEIGHT  240
#define MAX_SCALE_FACTOR           4

    using namespace std;
    using namespace ti::dl;

    /**
     * \brief Configuration for the DL inferer.
     *
     * \ingroup group_edgeai_cpp_apps_post_proc
     */
    struct PreprocessImageConfig
    {
        /** Name of the model. */
        std::string         modelName{};

        /** Type of the runtime API to invoke. The valid values are:
         * - DL_INFER_RTTYPE_DLR
         * - DL_INFER_RTTYPE_TFLITE
         * - DL_INFER_RTTYPE_ONNX
         */
        std::string         rtType{};

        /** Task type.
         *  - detection
         *  - segmentation
         *  - classification
         */
        std::string         taskType{};

        /** Width of the input data. */
        int32_t             inDataWidth{TI_PREPROC_DEFAULT_WIDTH};

        /** Height of the input data. */
        int32_t             inDataHeight{TI_PREPROC_DEFAULT_HEIGHT};

        /** Out width. */
        int32_t             outDataWidth{TI_PREPROC_DEFAULT_WIDTH};

        /** Out height. */
        int32_t             outDataHeight{TI_PREPROC_DEFAULT_HEIGHT};

        /** Mean values to apply during normalization. */
        std::vector<float>  mean;

        /** Scale values to apply during normalization. */
        std::vector<float>  scale;

        /** Resize width. */
        int32_t             resizeWidth{TI_PREPROC_DEFAULT_WIDTH};

        /** Resize height. */
        int32_t             resizeHeight{TI_PREPROC_DEFAULT_HEIGHT};

        /** Layout of the data. Allowed values. */
        std::string         dataLayout{"NCHW"};

        /** Number of channels. */
        int32_t             numChans{0};

        /** Data type of Input tensor. */
        DlInferType         inputTensorType{DlInferType_Invalid};

        /** Optional debugging control configuration. */
        DebugDumpConfig     debugConfig;

        /**
         * Updates the string 's' with the gst videoscale and videobox command strings
         * based on the pre-processor configuration 'config'. The video scale string is
         * always generated but the videobox string will be conditionally generated
         * only if any of the crop (top/bottom, left/right) are non-zero.
         *
         * @param s             String with the gst commands
         */
        void getPreProcString(string   &s) const
        {
            int32_t top;
            int32_t left;
            int32_t bottom;
            int32_t right;
            int32_t t;

            /*
             * tiovxmultiscaler dose not support upscaling and downscaling with
             * scaling factor < 1/4, So use "videoscale" insted
             */
            if ((float)inDataWidth/resizeWidth > MAX_SCALE_FACTOR ||
                            (float)inDataHeight/resizeHeight > MAX_SCALE_FACTOR)
            {
                s = " ! video/x-raw, width=" +
                    to_string(max(inDataWidth/4, resizeWidth)) +
                    ", height=" +
                    to_string(max(inDataHeight/4, resizeHeight)) +
                    " ! tiovxmultiscaler";
            }
            else if (inDataWidth/resizeWidth < 1 ||
                                                  inDataHeight/resizeHeight < 1)
            {
                s = " ! video/x-raw, width=" +
                    to_string(inDataWidth) +
                    ", height=" +
                    to_string(inDataHeight) +
                    " ! videoscale";
            }
            else
            {
                s = "";
            }

            s += " ! video/x-raw, width=" +
                 to_string(resizeWidth) +
                 ", height=" +
                 to_string(resizeHeight);

            t      = resizeWidth - outDataWidth;
            left   = t/2;
            right  = t - left;

            t      = resizeHeight - outDataHeight;
            top    = t/2;
            bottom = t - top;

            if (left || right || top || bottom)
            {
                s += " ! tiovxcolorconvert out-pool-size=4"
                     " ! video/x-raw, format=RGB";

                s += " ! videobox "
                     " top="    + to_string(top) +
                     " bottom=" + to_string(bottom) +
                     " left="   + to_string(left) +
                     " right="  + to_string(right);
            }

            string channelOrder{""};
            if (dataLayout == "NCHW")
            {
                channelOrder += "0";
            }
            else if (dataLayout == "NHWC")
            {
                channelOrder += "1";
            }

            /*
             * dlpreproc takes data-type as an interger which maps to certain
             * data types, DlInferType enum in dl inferer is aligned with the
             * maping of dlpreproc
             */
            s += " ! tiovxdlpreproc"
                 " data-type="     + to_string(inputTensorType) +
                 " channel-order=" + channelOrder +
                 " mean-0="        + to_string(mean[0]) +
                 " mean-1="        + to_string(mean[0]) +
                 " mean-2="        + to_string(mean[0]) +
                 " scale-0="       + to_string(scale[0]) +
                 " scale-1="       + to_string(scale[0]) +
                 " scale-2="       + to_string(scale[0]) +
                 " tensor-format=rgb out-pool-size=4"
                 " ! application/x-tensor-tiovx";
        }

        /**
         * Helper function to dump the configuration information.
         */
        void dumpInfo() const
        {
            LOG_INFO_RAW("\n");
            LOG_INFO("PreprocessImageConfig::modelName       = %s\n", modelName.c_str());
            LOG_INFO("PreprocessImageConfig::rtType          = %s\n", rtType.c_str());
            LOG_INFO("PreprocessImageConfig::taskType        = %s\n", taskType.c_str());
            LOG_INFO("PreprocessImageConfig::dataLayout      = %s\n", dataLayout.c_str());
            LOG_INFO("PreprocessImageConfig::inDataWidth     = %d\n", inDataWidth);
            LOG_INFO("PreprocessImageConfig::inDataHeight    = %d\n", inDataHeight);
            LOG_INFO("PreprocessImageConfig::resizeWidth     = %d\n", resizeWidth);
            LOG_INFO("PreprocessImageConfig::resizeHeight    = %d\n", resizeHeight);
            LOG_INFO("PreprocessImageConfig::outDataWidth    = %d\n", outDataWidth);
            LOG_INFO("PreprocessImageConfig::outDataHeight   = %d\n", outDataHeight);
            LOG_INFO("PreprocessImageConfig::numChannels     = %d\n", numChans);
            LOG_INFO("PreprocessImageConfig::inputTensorType = Enum %d\n", inputTensorType);

            LOG_INFO("PreprocessImageConfig::mean          = [");
            for (uint32_t i = 0; i < mean.size(); i++)
            {
                LOG_INFO_RAW(" %f", mean[i]);
            }

            LOG_INFO_RAW(" ]\n");

            LOG_INFO("PreprocessImageConfig::scale         = [");
            for (uint32_t i = 0; i < scale.size(); i++)
            {
                LOG_INFO_RAW(" %f", scale[i]);
            }

            LOG_INFO_RAW(" ]\n\n");
        }
    };

} // namespace ti::edgeai::common

#endif /* _PRE_PROCESS_IMAGE_CONFIG_H_ */
