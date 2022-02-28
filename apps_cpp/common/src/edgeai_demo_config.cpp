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
/* Standard headers. */
#include <iostream>
#include <filesystem>

/* Module headers. */
#include <utils/include/ti_stl_helpers.h>
#include <common/include/edgeai_utils.h>
#include <common/include/edgeai_classnames.h>
#include <common/include/edgeai_demo_config.h>

namespace ti::edgeai::common
{
using namespace std;
using namespace cv;

using namespace ti::edgeai::common;

static char gFilePath[2048];

static map<string, string> gImageFormatMap =
{
    {".jpg", "jpeg"},
    {".png", "png"}
};

static map<string, string> gImageDecMap =
{
    {".jpg", " ! jpegdec "},
    {".png", " ! pngdec "}
};

static map<string, string> gVideoInExtMap =
{
    {".mp4", " ! qtdemux "},
    {".mov", " ! qtdemux "},
    {".avi", " ! avidemux "}
};

static map<string, string> gVideoDecMap =
{
    {"h264", " ! h264parse ! v4l2h264dec"
             " ! video/x-raw, format=NV12 "},
    {"h265", " ! h265parse ! v4l2h265dec"
             " ! video/x-raw, format=NV12 "},
    {"auto", " ! decodebin "
             " ! tiovxcolorconvert"
             " ! video/x-raw, format=NV12"}
};

static map<string, string> gImageEncMap =
{
    {".jpg", " jpegenc ! "}
};

static map<string, string> gVideoEncMap =
{
    {".mov", " v4l2h264enc bitrate=10000000 ! h264parse ! qtmux ! "},
    {".mp4", " v4l2h264enc bitrate=10000000 ! h264parse ! mp4mux ! "},
    {".mkv", " v4l2h264enc bitrate=10000000 ! h264parse ! matroskamux ! "}
};

int32_t InputInfo::m_numInstances = 0;

InputInfo::InputInfo(const YAML::Node &node)
{
    m_instId = m_numInstances++;

    m_source    = node["source"].as<string>();
    m_width     = node["width"].as<int32_t>();
    m_height    = node["height"].as<int32_t>();
    m_framerate = node["framerate"].as<int32_t>();

    if (node["index"])
    {
        m_index = node["index"].as<int32_t>();
    }

    if (node["format"])
    {
        m_format = node["format"].as<string>();
    }

    if (node["drop"])
    {
        m_drop = node["drop"].as<bool>();
    }

    if (node["loop"])
    {
        m_loop = node["loop"].as<bool>();
    }

    if (node["pattern"])
    {
        m_pattern = node["pattern"].as<string>();
    }

    if (node["subdev-id"])
    {
        m_subdev_id = node["subdev-id"].as<int32_t>();
    }

    LOG_DEBUG("CONSTRUCTOR\n");
}

void InputInfo::dumpInfo(const char *prefix) const
{
    LOG_INFO("%sInputInfo::source        = %s\n", prefix, m_source.c_str());
    LOG_INFO("%sInputInfo::width         = %d\n", prefix, m_width);
    LOG_INFO("%sInputInfo::height        = %d\n", prefix, m_height);
    LOG_INFO("%sInputInfo::framerate     = %d\n", prefix, m_framerate);
    LOG_INFO("%sInputInfo::index         = %d\n", prefix, m_index);
    LOG_INFO("%sInputInfo::srcCmd        =\n", prefix);
    LOG_INFO("\t\t%s\n", m_srcCmd.c_str());

    LOG_INFO("%sInputInfo::srcElemNames  =\n", prefix);
    for (auto const &s: m_srcElemNames)
    {
        LOG_INFO("\t\t%s\n", s.c_str());
    }
    LOG_INFO_RAW("\n");
}

int32_t InputInfo::addGstPipeline(const vector<string>         &preProcStr,
                                  const vector<string>         &srcElemNames,
                                  const vector<MosaicInfo*>    &mosaic)
{
    string  srcExt;
    string  whStr;
    int32_t status = 0;
    int32_t indexEnd = -1;

    srcExt = filesystem::path(m_source).extension();
    whStr  = "width=" + to_string(m_width) + ", height=" + to_string(m_height);

    if (filesystem::exists(m_source))
    {
        if (gVideoInExtMap.find(srcExt) != gVideoInExtMap.end())
        {
            m_srcType = "video";
        }
        else if (gImageDecMap.find(srcExt) != gImageDecMap.end())
        {
            m_srcType = "image";
            indexEnd = 0;
        }
        else
        {
            m_srcType = "camera";
        }
    }
    else if (m_source.rfind('%') != string::npos)
    {
        /* Form the file name, replacing with the index. */
        sprintf(gFilePath, m_source.c_str(), m_index);

        /* Check if the file exists. */
        if (filesystem::exists(gFilePath) &&
            (gImageDecMap.find(srcExt) != gImageDecMap.end()))
        {
            m_srcType = "image";
        }
        else
        {
            LOG_ERROR("Invalid source format.\n");
            status = -1;
        }
    }
    else if (m_source.rfind("rtsp") == 0)
    {
        m_srcType = "rtsp";
    }
    else if (m_source == "videotestsrc")
    {
        m_srcType = "videotestsrc";
    }
    else
    {
        LOG_ERROR("Invalid source.\n");
        status = -1;
    }

    if (status == 0)
    {
        string          srcStr;

        if (m_srcType == "camera")
        {
            if (m_format == "jpeg")
            {
                srcStr = " v4l2src device=" +
                          m_source + " io-mode=2"
                          " ! image/jpeg, " +
                          whStr +
                          " ! jpegdec"
                          " ! tiovxcolorconvert"
                          " ! video/x-raw, format=NV12";
            }
            else if (m_format == "NV12")
            {
                srcStr = " v4l2src device=" +
                          m_source + " io-mode=2"
                          " ! video/x-raw, " +
                          whStr +
                          ", format=NV12 ";
            }
            else if (m_format == "raw")
            {
                //TODO - Take sensor name and subdev as params
                srcStr = " v4l2src device=" +
                          m_source + " io-mode=5"
                          " ! video/x-bayer, " +
                          whStr +
                          ", format=rggb !"
                          " tiovxisp device=/dev/v4l-subdev" +
                          to_string(m_subdev_id) +
                          " dcc-isp-file=/opt/imaging/imx219/dcc_viss.bin"
                          " dcc-2a-file=/opt/imaging/imx219/dcc_2a.bin"
                          " format-msb=7 ! video/x-raw, format=NV12 ";
            }
            else
            {
                srcStr = " v4l2src device=" +
                          m_source + " io-mode=2"
                          " ! video/x-raw, " +
                          whStr +
                          " ! tiovxcolorconvert"
                          " ! video/x-raw, format=NV12"
                          " ";
            }
        }
        else if (m_srcType == "image")
        {
            srcStr = " multifilesrc location=" +
                     m_source +
                     " loop=" +
                     to_string(m_loop)  +
                     " index=" + to_string(m_index) +
                     " stop-index=" + to_string(indexEnd) +
                     " caps=image/" + gImageFormatMap[srcExt] +
                     ",framerate=" + to_string(m_framerate) +
                     "/1" + gImageDecMap[srcExt] +
                     " ! videoscale ! video/x-raw, " +
                     whStr +
                     " ! tiovxcolorconvert"
                     " ! video/x-raw, format=NV12";
        }
        else if (m_srcType == "video")
        {
            if (gVideoDecMap.find(m_format) == gVideoDecMap.end())
            {
                m_format = "auto";
            }
            srcStr = " filesrc location=" +
                    m_source + " " +
                    gVideoInExtMap[srcExt] +
                    gVideoDecMap[m_format];
        }
        else if (m_srcType == "rtsp")
        {
            srcStr = " rtspsrc location=" +
                     m_source +
                     " latency=0 buffer-mode=auto ! rtph264depay"
                     " ! h264parse ! v4l2h264dec ! video/x-raw, format=NV12";
        }
        else if (m_srcType == "videotestsrc")
        {
            if (m_format == "NV12"){
                srcStr = " videotestsrc pattern=" +
                        m_pattern +
                        " ! video/x-raw, width=" +
                        to_string(m_width) +
                        ", height=" +
                        to_string(m_height) +
                        ", format=" +
                        m_format;
            }
            else{
                srcStr = " videotestsrc pattern=" +
                        m_pattern +
                        " ! video/x-raw, width=" +
                        to_string(m_width) +
                        ", height=" +
                        to_string(m_height) +
                        ", format=" +
                        m_format + 
                        " ! tiovxcolorconvert"
                        " ! video/x-raw, format=NV12";
            }
        }

        /* NOTE: The assumption of the srcElemNames vector layout is as follows:
         * - srcElemNames[0]: direct input from the sensor (ex:- camera)
         * - srcElemNames[1]: pre-processed input
         */
        const string &buffCntStr = m_drop ?
                                   " ! appsink drop=true max-buffers="
                                   + to_string(m_appSinkBuffDepth) + " name=":
                                   " ! appsink max-buffers="
                                   + to_string(m_appSinkBuffDepth) + " name=";

        const string &splitStr = "input" + to_string(m_instId) + "_split";
        if (preProcStr.size() > 1)
        {
            auto const &srcBase = " ! tee name="
                                + splitStr;
                                
            m_srcCmd = srcStr + srcBase;
        }
        else
        {
            m_srcCmd = srcStr;
        }

        for (auto i = 0; i < preProcStr.size(); i++)
        {
            auto j = i * 2;

            const auto    mosaicInfo = mosaic[i];
            const string &mosaicStr = " ! video/x-raw,"
                                      " width="
                                      + to_string(mosaicInfo->m_width)
                                      + ", height="
                                      + to_string(mosaicInfo->m_height);

            /* tiovxmultiscaler for each model */
            const string &splitName = + "multiscaler_split_"
                                     + to_string(m_instId)
                                     + to_string(i);

            string scaleStr = " ! tiovxmultiscaler name="
                               + splitName;

            if (preProcStr.size() > 1)
            {
                scaleStr = " " + splitStr + ". ! queue" + scaleStr;
            }
                                     
            /* Direct path from the sensor input. */
            auto const &sensorPath = " " + splitName
                                     + ". ! queue"
                                     + mosaicStr
                                     + " ! tiovxcolorconvert out-pool-size=4"
                                       " target=1"
                                       " ! video/x-raw, format=RGB"
                                     + buffCntStr
                                     + srcElemNames[j];

            /* Pre-processing of the sensor input. 'preProcStr' determines what
             * pre-processing is done.
             */
            auto const &preProcPath = " " + splitName
                                      + ". ! queue"
                                      + preProcStr[i]
                                      + buffCntStr
                                      + srcElemNames[j+1];

            /* Final concatenated source command string. */
            m_srcCmd += scaleStr + sensorPath + preProcPath;
        }

        m_srcElemNames.insert(m_srcElemNames.end(),
                              srcElemNames.begin(),
                              srcElemNames.end());
    }

    return status;
}

int32_t InputInfo::getSrcCmdString(vector<string>          &srcCmd,
                                   vector<vector<string>>  &srcElemNames) const
{
    /* Append the command string. */
    srcCmd.push_back(m_srcCmd);

    /* Insert the source element names. */
    srcElemNames.push_back(m_srcElemNames);

    return 0;
}

InputInfo::~InputInfo()
{
    LOG_DEBUG("DESTRUCTOR\n");
}

int32_t OutputInfo::m_numInstances = 0;

OutputInfo::OutputInfo(const YAML::Node    &node,
                       const string        &title):
    m_title(title)
{
    m_instId = m_numInstances++;

    m_bkgndElemName = "background" + to_string(m_instId); 
    m_mosaicElemName = "mosaic" + to_string(m_instId);

    m_sink   = node["sink"].as<string>();
    m_width  = node["width"].as<int32_t>();
    m_height = node["height"].as<int32_t>();

    if (node["connector"])
    {
        m_connector = node["connector"].as<int32_t>();
    }

    LOG_DEBUG("CONSTRUCTOR\n");
}


int32_t OutputInfo::appendGstPipeline(const string         &postProcStr,
                                      const string         &sinkElemName,
                                      const int32_t        &disp_id)
{
    int32_t status = 0;
    string cmd;
    cmd = "appsrc format=GST_FORMAT_TIME is-live=true block=true do-timestamp=true name="
        + sinkElemName
        + postProcStr
        + " ! queue ! " + m_mosaicElemName + ".sink_" + to_string(disp_id)
        + "\n";
    m_postProcCmds += cmd;

    m_sinkElemNames.push_back(sinkElemName);
    return status;
}

int32_t OutputInfo::getSinkCmdString(string            &sinkCmd,
                                     vector<string>    &sinkElemNames) const

{
    string  sinkType;
    string  sinkExt;
    string  sinkStr;
    int32_t status = 0;

    sinkExt = filesystem::path(m_sink).extension();

    if (gVideoEncMap.find(sinkExt) != gVideoEncMap.end())
    {
        sinkType = "video";
    }
    else if (gImageEncMap.find(sinkExt) != gImageEncMap.end())
    {
        sinkType = "image";
    }
    else if (m_sink == "fakesink")
    {
        sinkType = "fakesink";
    }
    else
    {
        sinkType = "display";
    }

    if (sinkType == "display")
    {
        sinkStr = "kmssink sync=false driver-name=tidss";

        if (m_connector)
        {
            sinkStr += " connector-id=" + to_string(m_connector);
        }
    }
    else if (sinkType == "image")
    {
        sinkStr = gImageEncMap[sinkExt] +
                  " multifilesink location=" +
                  m_sink;
    }
    else if (sinkType == "video")
    {
        sinkStr = gVideoEncMap[sinkExt] +
                  " filesink location=" +
                  m_sink;
    }
    else if (sinkType == "fakesink")
    {
        sinkStr = "fakesink";
    }

    /* Add post proc strings */
    sinkCmd += m_postProcCmds;

    /* Background image string */
    string cmd = "appsrc format=GST_FORMAT_TIME block=true num-buffers=1 name="
        + m_bkgndElemName
        + " ! tiovxcolorconvert ! video/x-raw,format=NV12 ! queue ! "
        + m_mosaicElemName + ".background\n";
    sinkCmd += cmd;

    /* Mosaic string */
    sinkCmd += "tiovxmosaic name=" + m_mosaicElemName + "\n";
    for(int32_t id=0; id<m_mosaicCnt; id++)
    {
        auto const &mosaicInfo = m_instIdMosaicMap.at(id);
        string mosaicSinkPad = "  sink_" + to_string(id);
        sinkCmd += mosaicSinkPad + "::startx=" + to_string(mosaicInfo->m_posX);
        sinkCmd += mosaicSinkPad + "::starty=" + to_string(mosaicInfo->m_posY);
        sinkCmd += mosaicSinkPad + "::width="  + to_string(mosaicInfo->m_width);
        sinkCmd += mosaicSinkPad + "::height=" + to_string(mosaicInfo->m_height);
        sinkCmd += "\n";
    }

    /* Finally add the base string. */
    sinkCmd += " ! video/x-raw, format=NV12, width="  + to_string(m_width)
                                        + ", height=" + to_string(m_height)
                                        + " ! " + sinkStr + "\n";

    /* Insert the sink element name into the vector. */
    sinkElemNames.insert(sinkElemNames.end(),
                         m_sinkElemNames.begin(),
                         m_sinkElemNames.end());
    sinkElemNames.push_back(m_bkgndElemName);

    return 0;
}

void OutputInfo::dumpInfo(const char *prefix) const
{
    LOG_INFO("%sOutputInfo::sink         = %s\n", prefix, m_sink.c_str());
    LOG_INFO("%sOutputInfo::width        = %d\n", prefix, m_width);
    LOG_INFO("%sOutputInfo::height       = %d\n", prefix, m_height);
    LOG_INFO("%sOutputInfo::connector    = %d\n", prefix, m_connector);

    LOG_INFO_RAW("\n");
}

int32_t OutputInfo::allocOutBuff(GstPipe   *gstPipe,
                                 int32_t   numChans)
{
    int32_t status = 0;

    m_gstPipe = gstPipe;

    /* Allocate a RGB buffer for display */
    status = m_gstPipe->allocBuffer(m_outBuff,
                                    m_width,
                                    m_height,
                                    numChans);
    if (status < 0)
    {
        LOG_ERROR("allocBuffer() failed.\n");
    }

    if (status == 0)
    {
        m_titleFrame = Mat(m_height, m_width, CV_8UC3, m_outBuff.getAddr());

        m_titleFrame.setTo(Scalar(0,0,0));

        putText(m_titleFrame,
                "Texas Instruments - Edge Analytics",
                Point(40, 40),
                FONT_HERSHEY_SIMPLEX,
                1.2,
                Scalar(0, 0, 255),
                2);

        putText(m_titleFrame,
                m_title.c_str(),
                Point(40, 80),
                FONT_HERSHEY_SIMPLEX,
                1,
                Scalar(255, 0, 0),
                2);

        for(int32_t id=0; id<m_mosaicCnt; id++)
        {
            auto const &mosaicInfo = m_instIdMosaicMap.at(id);
            const string title = m_titleMap.at(id);
            int   rowSize = (40 * mosaicInfo->m_width)/TI_POSTPROC_DEFAULT_WIDTH;
            float txtSize = 
                static_cast<float>(mosaicInfo->m_width)/TI_POSTPROC_DEFAULT_WIDTH;

            putText(m_titleFrame,
                    title.c_str(),
                    Point(mosaicInfo->m_posX + 5, mosaicInfo->m_posY - rowSize/4),
                    FONT_HERSHEY_SIMPLEX,
                    txtSize,
                    Scalar(255, 255, 255),
                    2);
        }
    }

    return status;
}

int32_t OutputInfo::registerDispParams(const MosaicInfo  *mosaicInfo, 
                                       const string      &modelTitle)
{
    int32_t id = m_mosaicCnt++;

    /* Store the mosaic information in the map. */
    m_instIdMosaicMap[id] = mosaicInfo;

    m_titleMap.push_back(modelTitle);

    return id;
}


OutputInfo::~OutputInfo()
{
    LOG_DEBUG("DESTRUCTOR\n");

    /* Free the buffer. */
    m_gstPipe->freeBuffer(m_outBuff);
}

MosaicInfo::MosaicInfo(const YAML::Node &node)
{
    m_width  = node["width"].as<int32_t>();
    m_height = node["height"].as<int32_t>();
    m_posX   = node["pos_x"].as<int32_t>();
    m_posY   = node["pos_y"].as<int32_t>();

    LOG_DEBUG("CONSTRUCTOR\n");
}

void MosaicInfo::dumpInfo(const char *prefix) const
{
    LOG_INFO("%sMosaicInfo::width        = %d\n", prefix, m_width);
    LOG_INFO("%sMosaicInfo::height       = %d\n", prefix, m_height);
    LOG_INFO("%sMosaicInfo::pos_x        = %d\n", prefix, m_posX);
    LOG_INFO("%sMosaicInfo::pos_y        = %d\n", prefix, m_posY);
    LOG_INFO_RAW("\n");
}

MosaicInfo::~MosaicInfo()
{
    LOG_DEBUG("DESTRUCTOR\n");
}

ModelInfo::ModelInfo(const YAML::Node &node)
{
    m_modelPath  = node["model_path"].as<string>();

    if (node["labels_path"])
    {
        m_labelsPath = node["labels_path"].as<string>();
    }

    if (node["alpha"])
    {
        m_alpha = node["alpha"].as<float>();
    }

    if (node["viz_threshold"])
    {
        m_vizThreshold = node["viz_threshold"].as<float>();
    }

    if (node["topN"])
    {
        m_topN = node["topN"].as<int32_t>();
    }

    LOG_DEBUG("CONSTRUCTOR\n");
}

int32_t ModelInfo::initialize()
{
    YAML::Node          yaml;
    InfererConfig       infConfig;
    int32_t             status = 0;
    const string       &configFile = m_modelPath + "/param.yaml";

    // Check if the specified configuration file exists
    if (!std::filesystem::exists(configFile))
    {
        LOG_ERROR("The file [%s] does not exist.\n",
                  configFile.c_str());
        status = -1;
    }

    if (status == 0)
    {
        yaml = YAML::LoadFile(configFile.c_str());

        // Populate infConfig from yaml
        status = getInfererConfig(yaml, m_modelPath, infConfig);

        if (status < 0)
        {
            LOG_ERROR("getInfererConfig() failed.\n");
        }
    }

    if (status == 0)
    {
        m_infererObj = DLInferer::makeInferer(infConfig);

        if (m_infererObj == nullptr)
        {
            LOG_ERROR("DLInferer::makeInferer() failed.\n");
            status = -1;
        }
    }

    // Populate pre-process config from yaml
    if (status == 0)
    {
        status = getPreprocessImageConfig(yaml, m_preProcCfg);

        if (status < 0)
        {
            LOG_ERROR("getPreprocessImageConfig() failed.\n");
        }
    }

    // Populate post-process config from yaml
    if (status == 0)
    {
        status = getPostprocessImageConfig(yaml, m_postProcCfg);

        if (status < 0)
        {
            LOG_ERROR("getPostprocessImageConfig() failed.\n");
        }
    }

    // Populate post-process config from yaml
    if (status == 0)
    {
        const VecDlTensor  *dlInfOutputs;
        const VecDlTensor  *dlInfInputs;
        const DlTensor     *ifInfo;

        /* Query the output information for setting up the output buffers. */
        dlInfOutputs = m_infererObj->getOutputInfo();

        /* Query the input information for setting the tensor type in pre process. */
        dlInfInputs = m_infererObj->getInputInfo();
        ifInfo = &dlInfInputs->at(0);
        m_preProcCfg.inputTensorType = ifInfo->type;

        /* Set input data width and height based on the infererence engine
         * information. This is only used for semantic segmentation models
         * which have 4 dimensions. The logic is extended to any models that
         * have atleast three dimensions which has the following
         * - Num channels (C)
         * - Height (H)
         * - Width (W)
         *
         * The semantic segmentation model output will have one extra dimension
         * which leads to NCHW dimensions in the output.
         * - Batch (N)
         *
         * For all other cases, the default values (set in the post-process
         * obhect are used.
         */
        ifInfo = &dlInfOutputs->at(0);

        if (m_postProcCfg.taskType == "segmentation")
        {
            /* Either NCHW or CHW. Width is the last dimention and the height 
             * is the previous to last.
             */
            m_postProcCfg.inDataWidth  = ifInfo->shape[ifInfo->dim - 1];
            m_postProcCfg.inDataHeight = ifInfo->shape[ifInfo->dim - 2];
            m_postProcCfg.classnames   = nullptr;
            m_postProcCfg.alpha        = m_alpha;
        }
        else
        {
            // Query the output data dimension ofrom the pre-process module.
            m_postProcCfg.inDataWidth  = m_preProcCfg.outDataWidth;
            m_postProcCfg.inDataHeight = m_preProcCfg.outDataHeight;

            if (m_postProcCfg.taskType == "classification")
            {
                m_postProcCfg.classnames = gClassNameMap["imagenet"];
                m_postProcCfg.topN       = m_topN;
            }
            else
            {
                m_postProcCfg.classnames   = gClassNameMap["coco"];
                m_postProcCfg.vizThreshold = m_vizThreshold;
            }
        }

        string modelName = m_modelPath;

        if (modelName.back() == '/')
        {
            modelName.pop_back();
        }

        modelName = std::filesystem::path(modelName).filename();

        m_preProcCfg.modelName  = modelName;
        m_postProcCfg.modelName = modelName;
    }

    return status;
}

int32_t ModelInfo::createPreprocCntxt(const InputInfo          &inputInfo,
                                      const DebugDumpConfig    &debugConfig,
                                      string                   &preProcStr,
                                      PreprocessImage         *&preProcObj)
{
    PreprocessImageConfig   preProcCfg(m_preProcCfg);
    int32_t                 status = 0;

    preProcCfg.debugConfig  = debugConfig;
    preProcCfg.inDataWidth  = inputInfo.m_width;
    preProcCfg.inDataHeight = inputInfo.m_height;

    preProcCfg.getPreProcString(preProcStr);

    /* Instantiate pre-processing object. */
    preProcObj = PreprocessImage::makePreprocessImageObj(preProcCfg);

    if (preProcObj == nullptr)
    {
        LOG_ERROR("PreprocessImage::makePreprocessImageObj() failed.\n");
        status = -1;
    }

    return status;
}

int32_t ModelInfo::createPostprocCntxt(const OutputInfo        &outputInfo,
                                       const DebugDumpConfig   &debugConfig,
                                       const MosaicInfo        &mosaicInfo,
                                       string                  &postProcStr,
                                       PostprocessImage       *&postProcObj)
{
    PostprocessImageConfig  postProcCfg(m_postProcCfg);
    int32_t                 status = 0;

    postProcCfg.debugConfig   = debugConfig;
    postProcCfg.outDataWidth  = mosaicInfo.m_width;
    postProcCfg.outDataHeight = mosaicInfo.m_height;
    postProcCfg.dispWidth     = outputInfo.m_width;
    postProcCfg.dispHeight    = outputInfo.m_height;

    postProcCfg.getPostProcString(postProcStr);

    postProcObj = PostprocessImage::makePostprocessImageObj(postProcCfg);

    if (postProcObj == nullptr)
    {
        LOG_ERROR("PostprocessImage::makePostprocessImageObj() failed.\n");
        status = -1;
    }

    return status;
}

void ModelInfo::dumpInfo(const char *prefix) const
{
    LOG_INFO("%sModelInfo::modelPath     = %s\n", prefix, m_modelPath.c_str());
    LOG_INFO("%sModelInfo::labelsPath    = %s\n", prefix, m_labelsPath.c_str());
    LOG_INFO("%sModelInfo::vizThreshold  = %f\n", prefix, m_vizThreshold);
    LOG_INFO("%sModelInfo::alpha         = %f\n", prefix, m_alpha);
    LOG_INFO("%sModelInfo::topN          = %d\n", prefix, m_topN);
    LOG_INFO_RAW("\n");
}

ModelInfo::~ModelInfo()
{
    LOG_DEBUG("DESTRUCTOR\n");
    delete m_infererObj;
}

SubFlowInfo::SubFlowInfo(InferencePipe     *inferPipe,
                         PreprocessImage   *preProcObj,
                         PostprocessImage  *postProcObj,
                         MosaicInfo        *mosaicInfo):
    m_preProcObj(preProcObj),
    m_inferPipe(inferPipe),
    m_postProcObj(postProcObj),
    m_mosaicInfo(mosaicInfo)
{
    auto    out = m_mosaicInfo->m_outputInfo;

    m_dispId = out->registerDispParams(m_mosaicInfo,m_postProcObj->m_title);

    LOG_DEBUG("CONSTRUCTOR\n");
}

int32_t SubFlowInfo::start(GstPipe   *gstPipe, int32_t  numChans)
{
    auto    out = m_mosaicInfo->m_outputInfo;
    int32_t status;

    status = out->allocOutBuff(gstPipe, numChans);

    if (status  < 0)
    {
        LOG_ERROR("allocOutBuff() failed.\n");
    }
    else
    {
        m_inferPipe->start(gstPipe);
    }

    return status;
}


SubFlowInfo::~SubFlowInfo()
{
    LOG_DEBUG("DESTRUCTOR\n");

    delete m_preProcObj;
    delete m_inferPipe;
    delete m_postProcObj;
}

int32_t FlowInfo::m_numInstances = 0;

FlowInfo::FlowInfo(const YAML::Node &node)
{
    m_inputId = node["input"].as<string>();

    m_instId = m_numInstances++;

    for (auto const &m : node["models"])
    {
        m_modelIds.insert(m.as<string>());
    }

    for (auto const &o : node["outputs"])
    {
        m_outputIds.push_back(o.as<string>());
    }

    const YAML::Node   &dispNode = node["mosaic"];
    for (auto it = dispNode.begin(); it != dispNode.end(); ++it)
    {
        m_mosaicVec.push_back(new MosaicInfo(it->second));
    }

    if (node["debug"])
    {
        const YAML::Node &debugNode = node["debug"];

        m_debugEnableMask = debugNode["enable_mask"].as<uint32_t>();

        if (m_debugEnableMask > EDGEAI_ENABLE_DATA_DUMP_MASK)
        {
            LOG_ERROR("Invalid debug enable mask specified. "
                      "Disabling debug logging.\n");

            m_debugEnableMask = 0;
        }

        if (m_debugEnableMask)
        {
            if (debugNode["out_dir"])
            {
                m_debugConfig.dir =
                    debugNode["out_dir"].as<string>();
            }

            if (debugNode["dir_suffix"])
            {
                m_debugConfig.suffix =
                    debugNode["dir_suffix"].as<string>();
            }

            if (debugNode["start_frame"])
            {
                m_debugConfig.startFrame =
                    debugNode["start_frame"].as<uint32_t>();
            }

            if (debugNode["end_frame"])
            {
                m_debugConfig.endFrame =
                    debugNode["end_frame"].as<uint32_t>();
            }
        }
    }

    LOG_DEBUG("CONSTRUCTOR\n");
}

int32_t FlowInfo::initialize(map<string, ModelInfo*>   &modelMap,
                             map<string, InputInfo*>   &inputMap,
                             map<string, OutputInfo*>  &outputMap)
{
    auto           &inputInfo = inputMap[m_inputId];
    vector<string>  preProcStrVec;;
    vector<string>  flowSrcElemNames;
    int32_t         cnt = 0;
    string          flowStr = "flow" + to_string(m_instId);
    int32_t         status = 0;

    /* Set up the flows. */
    for (auto m : m_modelIds)
    {
        ModelInfo          *model = modelMap[m];
        auto const         &outId = m_outputIds[cnt];
        auto               &mosaicInfo = m_mosaicVec[cnt];
        auto               &outputInfo = outputMap[outId];
        PreprocessImage    *preProcObj;
        PostprocessImage   *postProcObj;
        InferencePipe      *inferPipe;
        SubFlowInfo        *subFlow;
        InferencePipeConfig ipCfg;
        vector<string>      srcElemNames;
        string              sinkElemName;
        string              preProcStr;
        string              postProcStr;
        string              inputName;

        string modelName = model->m_modelPath;

        if (modelName.back() == '/')
        {
            modelName.pop_back();
        }

        modelName = std::filesystem::path(modelName).filename();

        /* Create the instanceId to Mosaic info mapping. */
        mosaicInfo->m_outputInfo = outputInfo;

        /* Set the degug/test control parameters. */
        DebugDumpConfig debugConfig(m_debugConfig);

        debugConfig.dir += "/" + modelName + "/cpp/" + debugConfig.suffix;
        debugConfig.file = flowStr + "_pre";

        debugConfig.startFrameIndex = inputInfo->m_index;

        if (m_debugEnableMask & EDGEAI_ENABLE_PREPROC_DUMP)
        {
            debugConfig.enable = true;
        }
        else
        {
            debugConfig.enable = false;
        }

        /* Create pre-process context. */
        status = model->createPreprocCntxt(*inputInfo,
                                           debugConfig,
                                           preProcStr,
                                           preProcObj);

        if (status < 0)
        {
            LOG_ERROR("createPreprocCntxt() failed.\n");
            break;
        }

        debugConfig.file = flowStr + "_post";

        if (m_debugEnableMask & EDGEAI_ENABLE_POSTPROC_DUMP)
        {
            debugConfig.enable = true;
        }
        else
        {
            debugConfig.enable = false;
        }

        /* Create post-process context. */
        status = model->createPostprocCntxt(*outputInfo,
                                            debugConfig,
                                            *mosaicInfo,
                                            postProcStr,
                                            postProcObj);

        if (status < 0)
        {
            LOG_ERROR("createPostprocCntxt() failed.\n");
            break;
        }

        /* Store the contexts. */
        preProcStrVec.push_back(preProcStr);

        /* Construct source command strings. */
        auto const &srcStr1 = flowStr + "_sensor" + to_string(cnt);
        auto const &srcStr2 = flowStr + "_pre_proc" + to_string(cnt);
        srcElemNames.push_back(srcStr1);
        srcElemNames.push_back(srcStr2);
        sinkElemName = flowStr + "_post_proc" + to_string(cnt);

        /* Append the source element names into the global vector. */
        flowSrcElemNames.insert(flowSrcElemNames.end(),
                                srcElemNames.begin(),
                                srcElemNames.end());

        /* Store the frame rate and channel information for allocating the
         * output buffer later.
         */
        m_numChans  = model->m_preProcCfg.numChans;

        debugConfig.file = flowStr + "_infer";

        if (m_debugEnableMask & EDGEAI_ENABLE_INFERENCE_DUMP)
        {
            debugConfig.enable = true;
        }
        else
        {
            debugConfig.enable = false;
        }

        ipCfg.modelBasePath = model->m_modelPath;
        ipCfg.inDataWidth   = inputInfo->m_width;
        ipCfg.inDataHeight  = inputInfo->m_height;
        ipCfg.loop          = inputInfo->m_loop;
        ipCfg.dispWidth     = outputInfo->m_width;
        ipCfg.dispHeight    = outputInfo->m_height;
        ipCfg.frameRate     = inputInfo->m_framerate;
        ipCfg.numChans      = model->m_preProcCfg.numChans;
        ipCfg.debugConfig   = debugConfig;

        inferPipe = new InferencePipe(ipCfg,
                                      model->m_infererObj,
                                      preProcObj,
                                      postProcObj,
                                      srcElemNames,
                                      sinkElemName);

        subFlow = new SubFlowInfo(inferPipe,
                                  preProcObj,
                                  postProcObj,
                                  mosaicInfo);

        m_subFlowVec.push_back(subFlow);

        /**
         * Incrementally create the GST sink command string for this output
        */
        status = outputInfo->appendGstPipeline(postProcStr,
                                      sinkElemName,
                                      subFlow->m_dispId);

        /* Register a statistics entry for this instance. */
        const string &taskType = postProcObj->getTaskType();
        int32_t instId = inferPipe->getInstId();
        Statistics::addEntry(instId,
                             inputInfo->m_source,
                             taskType,
                             model->m_modelPath);

        /* Increment the model count. */
        cnt++;
    }

    if (status == 0)
    {
        /* Create the GST source command string. */
        status = inputInfo->addGstPipeline(preProcStrVec,
                                           flowSrcElemNames,
                                           m_mosaicVec);
        if (status < 0)
        {
            LOG_ERROR("addGstPipeline() failed.\n");
        }
    }

    return status;
}

int32_t FlowInfo::start(GstPipe   *gstPipe)
{
    int32_t status;

    for (auto const &s: m_subFlowVec)
    {
        status = s->start(gstPipe, m_numChans);

        if (status < 0)
        {
            break;
        }
    }

    return status;
}

void FlowInfo::waitForExit()
{
    for (auto const &s: m_subFlowVec)
    {
        s->m_inferPipe->waitForExit();
    }
}

void FlowInfo::sendExitSignal()
{
    for (auto const &s: m_subFlowVec)
    {
        s->m_inferPipe->sendExitSignal();
    }
}

void FlowInfo::dumpInfo(const char *prefix) const
{
    LOG_INFO("%sFlowInfo::input          = %s\n", prefix, m_inputId.c_str());

    LOG_INFO("%sFlowInfo::modelIds       = [ ", prefix);

    for (auto const &m : m_modelIds)
    {
        LOG_INFO_RAW("%s ", m.c_str());
    }

    LOG_INFO_RAW("]\n");

    LOG_INFO("%sFlowInfo::outputIds      = [ ", prefix);

    for (auto const &o : m_outputIds)
    {
        LOG_INFO_RAW("%s ", o.c_str());
    }

    LOG_INFO_RAW("]\n");

    LOG_INFO("%sFlowInfo::mosaic        =\n", prefix);
    for (auto const &d: m_mosaicVec)
    {
        d->dumpInfo("\t\t");
    }
}

FlowInfo::~FlowInfo()
{
    LOG_DEBUG("DESTRUCTOR\n");
    DeleteVec(m_mosaicVec);
    DeleteVec(m_subFlowVec);
}

int32_t DemoConfig::parse(const YAML::Node &yaml)
{
    int32_t status;

    auto const &title = yaml["title"].as<string>();

    /* Parse input information. */
    for (auto &n : yaml["inputs"])
    {
        const string &s = n.first.as<string>();
        m_inputMap[s] = new InputInfo(n.second);
    }

    for (auto &n : yaml["models"])
    {
        const string &s = n.first.as<string>();
        m_modelMap[s] = new ModelInfo(n.second);
    }

    for (auto &n : yaml["outputs"])
    {
        const string &s = n.first.as<string>();
        m_outputMap[s] = new OutputInfo(n.second, title);
    }

    /* Parse flow information. */
    status = parseFlowInfo(yaml["flows"]);

    return status;
}

DemoConfig::DemoConfig()
{
    LOG_DEBUG("CONSTRUCTOR\n");
}

void DemoConfig::dumpInfo() const
{
    LOG_INFO_RAW("\n");
    
    LOG_INFO("DemoConfig::Inputs:\n");
    for (auto &[name, obj]: m_inputMap)
    {
        obj->dumpInfo("\t");
    }
    
    LOG_INFO("DemoConfig::Models:\n");
    for (auto &[name, obj]: m_modelMap)
    {
        obj->dumpInfo("\t");
    }
    
    LOG_INFO("DemoConfig::Outputs:\n");
    for (auto &[name, obj]: m_outputMap)
    {
        obj->dumpInfo("\t");
    }
    
    LOG_INFO("DemoConfig::Flows:\n");
    for (auto &[name, obj]: m_flowMap)
    {
        obj->dumpInfo("\t");
    }

    LOG_INFO_RAW("\n");
}

int32_t DemoConfig::parseFlowInfo(const YAML::Node &flows)
{
    int32_t status = 0;

    /* Create a set of inputs for catching duplicate usage across flows. */
    map<string, string> inputMap;

    for (auto &flow : flows)
    {
        const string   &s = flow.first.as<string>();
        auto const     &f = new FlowInfo(flow.second);
        int32_t         modelCnt = 0;
        int32_t         outputCnt = 0;

        /* Validate the input. */
        if (m_inputMap.find(f->m_inputId) == m_inputMap.end())
        {
            LOG_ERROR("[Flow: %s] Invalid input [%s] specified.\n",
                      s.c_str(), f->m_inputId.c_str());
            status = -1;
            break;
        }

        /* Check if the input is in use by other flows. */
        if (inputMap.find(f->m_inputId) != inputMap.end())
        {
            auto const &id = inputMap[f->m_inputId];

            LOG_ERROR("[Flow: %s] Invalid input [%s] specified. "
                      "Input already in use by flow [%s].\n",
                      s.c_str(), f->m_inputId.c_str(),
                      id.c_str());
            status = -1;
            break;
        }

        /* Save the (inout, flow) information. */
        inputMap[f->m_inputId] = s;

        /* Validate the models. */
        for (auto m : f->m_modelIds)
        {
            if (m_modelMap.find(m) == m_modelMap.end())
            {
                LOG_ERROR("[Flow: %s] Invalid ModelId [%s] specified.\n",
                          s.c_str(), m.c_str());
                status = -1;
                break;
            }

            modelCnt++;
        }

        /* Validate the outputs. */
        for (auto o : f->m_outputIds)
        {
            if (m_outputMap.find(o) == m_outputMap.end())
            {
                LOG_ERROR("[Flow: %s] Invalid OutputId [%s] specified.\n",
                          s.c_str(), o.c_str());
                status = -1;
                break;
            }

            outputCnt++;
        }

        if (modelCnt != outputCnt)
        {
            LOG_ERROR("[Flow: %s] Model Count [%d] does not "
                      "match Output Count [%d].\n",
                      s.c_str(), modelCnt, outputCnt);
            status = -1;
            break;
        }

        if (f->m_outputIds.size() != f->m_mosaicVec.size())
        {
            LOG_ERROR("[Flow: %s] Output Count [%d] does not "
                      "match Mosaic Count [%d].\n",
                      s.c_str(), f->m_outputIds.size(), f->m_mosaicVec.size());
            status = -1;
            break;
        }

        /* Create a set of the output Ids. This will eliminate duplicate
         * entities.
         */
        set<string> outputIdSet;
        outputIdSet.insert(f->m_outputIds.begin(), f->m_outputIds.end());

        /* Validate the mosaic properties against the output height and
         * width specifications.
         */
        for (auto const &o : outputIdSet)
        {
            auto const &out = m_outputMap[o];
            int32_t totalWidth = 0;
            int32_t totalHeight = 0;

            if (out->m_width <= 0)
            {
                LOG_ERROR("[Flow: %s] Invalid Output width [%d].\n",
                          s.c_str(), out->m_width);
                status = -1;
                break;
            }

            if (out->m_height <= 0)
            {
                LOG_ERROR("[Flow: %s] Invalid Output height [%d].\n",
                          s.c_str(), out->m_height);
                status = -1;
                break;
            }

            for (auto const &m : f->m_mosaicVec)
            {
                if (m->m_width <= 0)
                {
                    LOG_ERROR("[Flow: %s] Invalid mosaic width [%d].\n",
                              f->m_instId, m->m_width);
                    status = -1;
                    break;
                }

                if (m->m_height <= 0)
                {
                    LOG_ERROR("[Flow: %s] Invalid mosaic height [%d].\n",
                              s.c_str(), m->m_height);
                    status = -1;
                    break;
                }

                if ((m->m_width + m->m_posX) > out->m_width)
                {
                    LOG_ERROR("[Flow: %s] Mosaic (posX + width) [%d] exceeds "
                              "Output width [%d] .\n",
                              s.c_str(), (m->m_width + m->m_posX), out->m_width);
                    status = -1;
                    break;
                }

                if ((m->m_height + m->m_posY) > out->m_height)
                {
                    LOG_ERROR("[Flow: %s] Mosaic (posY + height) [%d] exceeds "
                              "Output height [%d] .\n",
                              s.c_str(), (m->m_height + m->m_posY),
                              out->m_height);
                    status = -1;
                    break;
                }

            }

            if (status < 0)
            {
                break;
            }

        }

        m_flowMap[s] = f;
    }

    return status;
}

DemoConfig::~DemoConfig()
{
    LOG_DEBUG("DESTRUCTOR\n");
    DeleteMap(m_inputMap);
    DeleteMap(m_modelMap);
    DeleteMap(m_outputMap);
    DeleteMap(m_flowMap);
}

} // namespace ti::edgeai::common

