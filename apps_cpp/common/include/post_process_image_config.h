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

#ifndef _POST_PROCESS_IMAGE_CONFIG_H_
#define _POST_PROCESS_IMAGE_CONFIG_H_

/* Standard headers. */
#include <map>

/* Module headers. */
#include <common/include/edgeai_debug.h>
#include <dl_inferer/include/ti_dl_inferer.h>

/**
 * \defgroup group_edgeai_cpp_apps_post_proc Image Post-processing
 *
 * \brief Class providing interface for generic post-processing logic.
 *
 * \ingroup group_edgeai_cpp_apps
 */

namespace ti::edgeai::common
{
// Default values to avoid exceptions resulting from using uninitialized values
#define TI_POSTPROC_DEFAULT_WIDTH   1280
#define TI_POSTPROC_DEFAULT_HEIGHT  720
#define TI_DEFAULT_DISP_WIDTH       1920
#define TI_DEFAULT_DISP_HEIGHT      1080

    using namespace std;
    using namespace ti::dl;

    /**
     * \brief Configuration for the DL inferer.
     *
     * \ingroup group_edgeai_cpp_apps_post_proc
     */
    struct PostprocessImageConfig
    {
        /** Name of the model. */
        std::string                 modelName{};

        /** Type of the runtime API to invoke. The valid values are:
         * - DL_INFER_RTTYPE_DLR
         * - DL_INFER_RTTYPE_TFLITE
         * - DL_INFER_RTTYPE_ONNX
         */
        std::string                 rtType{};

        /** Task type.
         *  - detection
         *  - segmentation
         *  - classification
         */
        std::string                 taskType{};

        /** Layout of the data. Allowed values. */
        std::string                 dataLayout{"NCHW"};

        /** Optional offset to be applied when detecting the output
         * class. This is applicable for image classification and
         * detection cases only.
         * Classification - a single scalar value
         * Detection      - a map
         */
        std::map<int32_t,int32_t>   labelOffsetMap{{0,0}};

        /** Order of results for detection use case
         * default is assumed to be [0 1 2 3 4 5] which means
         * [x1y1 x2y2 label score]
         */
        std::vector<int32_t>        formatter{0, 1, 2, 3, 4, 5};

        /** Ignore given index*/
        int32_t                     ignoreIndex{-1};

        /** If detections are normalized to 0-1 */
        bool                        normDetect{false};

        /** Order of tensors for detection results */
        std::vector<int32_t>        resultIndices{0, 1, 2};

        /** Multiplicative factor to be applied to Y co-ordinates. This is used
         * for visualization of the bounding boxes for object detection post-
         * processing only.
         */
        float                       vizThreshold{0.50f};

        /** Alpha value for blending. This is used for semantic segmentation
         *  post-processing only.
         */
        float                       alpha{0.5f};

        /** Number of classification results to pick from the top of the model output. */
        int32_t                     topN{5};

        /** Width of the output to display after adding tile. */
        int32_t                     dispWidth{TI_DEFAULT_DISP_WIDTH};

        /** Height of the output to display after adding tile. */
        int32_t                     dispHeight{TI_DEFAULT_DISP_HEIGHT};

        /** Width of the input data. */
        int32_t                     inDataWidth{TI_POSTPROC_DEFAULT_WIDTH};

        /** Height of the input data. */
        int32_t                     inDataHeight{TI_POSTPROC_DEFAULT_HEIGHT};

        /** Width of the output data. */
        int32_t                     outDataWidth{TI_POSTPROC_DEFAULT_WIDTH};

        /** Height of the output data. */
        int32_t                     outDataHeight{TI_POSTPROC_DEFAULT_HEIGHT};

        /** An array of strings for object class names. */
        const std::string          *classnames{nullptr};

        /** Optional debugging control configuration. */
        DebugDumpConfig             debugConfig;

        /**
         * Updates the string 's' with any post processing to be done
         * in the gstreamer sink pipeline, for a particular subflow.
         * 
         * @param s             String with the gst commands
         */
        void getPostProcString(string   &s) const
        {
            s = " ! tiovxcolorconvert ! video/x-raw,format=NV12";
        }

        /**
         * Helper function to dump the configuration information.
         */
        void dumpInfo() const
        {
            LOG_INFO_RAW("\n");
            LOG_INFO("PostprocessImageConfig::modelName      = %s\n", modelName.c_str());
            LOG_INFO("PostprocessImageConfig::rtType         = %s\n", rtType.c_str());
            LOG_INFO("PostprocessImageConfig::taskType       = %s\n", taskType.c_str());
            LOG_INFO("PostprocessImageConfig::dataLayout     = %s\n", dataLayout.c_str());
            LOG_INFO("PostprocessImageConfig::inDataWidth    = %d\n", inDataWidth);
            LOG_INFO("PostprocessImageConfig::inDataHeight   = %d\n", inDataHeight);
            LOG_INFO("PostprocessImageConfig::outDataWidth   = %d\n", outDataWidth);
            LOG_INFO("PostprocessImageConfig::outDataHeight  = %d\n", outDataHeight);
            LOG_INFO("PostprocessImageConfig::dispWidth      = %d\n", dispWidth);
            LOG_INFO("PostprocessImageConfig::dispHeight     = %d\n", dispHeight);
            LOG_INFO("PostprocessImageConfig::vizThreshold   = %f\n", vizThreshold);
            LOG_INFO("PostprocessImageConfig::alpha          = %f\n", alpha);
            LOG_INFO("PostprocessImageConfig::normDetect     = %d\n", normDetect);
            LOG_INFO("PostprocessImageConfig::labelOffsetMap = [ ");

            for (const auto& [key, value] : labelOffsetMap)
            {
                LOG_INFO_RAW("(%d, %d) ", key, value);
            }

            LOG_INFO_RAW("]\n\n");

            LOG_INFO("PostprocessImageConfig::formatter = [ ");

            for (uint32_t i = 0; i < formatter.size(); i++)
            {
                LOG_INFO_RAW(" %d", formatter[i]);
            }

            LOG_INFO_RAW("]\n\n");

            LOG_INFO("PostprocessImageConfig::resultIndices = [ ");

            for (uint32_t i = 0; i < resultIndices.size(); i++)
            {
                LOG_INFO_RAW(" %d", resultIndices[i]);
            }

            LOG_INFO_RAW("]\n\n");

            debugConfig.dumpInfo("PostprocessImageConfig::");
        }

    };
} // namespace ti::edgeai::common

#endif /* _POST_PROCESS_IMAGE_CONFIG_H_ */
