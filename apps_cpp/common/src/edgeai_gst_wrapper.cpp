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
#include <map>
#include <filesystem>

/* Module headers. */
#include <utils/include/ti_logger.h>
#include <common/include/edgeai_gst_wrapper.h>

#define TI_GST_WRAPPER_DEFAULT_IMAGE_FRAMERATE  (12)
#define GST_TIMEOUT 5000000000

using namespace ti::utils;

namespace ti::edgeai::common
{
GstPipe::GstPipe(const vector<string>          &srcCmd,
                 const string                  &sinkCmd,
                 const vector<vector<string>>  &srcElemNames,
                 const vector<string>          &sinkElemNames)
{
    int32_t status = 0;

    /* Create the source pipeline. */
    for (auto const &s : srcCmd)
    {
        auto srcPipe = createPipeline(s);

        if (srcPipe == nullptr)
        {
            LOG_ERROR("Pipeline creation for source failed.\n");
            LOG_ERROR("SRC_PIPE: %s\n", s.c_str());
            status = -1;
        }
        else
        {
            m_srcPipe.push_back(srcPipe);
        }
    }

    if (status == 0)
    {
        /* Create the sink pipeline. */
        m_sinkPipe = createPipeline(sinkCmd);

        if (m_sinkPipe == nullptr)
        {
            LOG_ERROR("Pipeline creation for sink failed.\n");
            LOG_ERROR("SINK_PIPE: %s\n", sinkCmd.c_str());
            status = -1;
        }
    }

    if (status == 0)
    {
        for (int i = 0; i < srcElemNames.size(); i++)
        {
            for (auto const &s : srcElemNames[i])
            {
                GstElement *elem;

                elem = findElementByName(m_srcPipe[i], s.c_str());

                if (elem == nullptr)
                {
                    LOG_ERROR("findElementByName(m_srcPipe, %s) failed.\n", s.c_str());
                    status = -1;
                    break;
                }

                /* Create a map entry for the new elenet created. */
                m_srcElemMap[s] = elem;
            }
            if (status != 0)
                break;
        }
    }

    if (status == 0)
    {
        for (auto const &s : sinkElemNames)
        {
            GstElement *elem;

            elem = findElementByName(m_sinkPipe, s.c_str());

            if (elem == nullptr)
            {
                LOG_ERROR("findElementByName(m_sinkPipe, %s) failed.\n", s.c_str());
                status = -1;
                break;
            }

            /* Create a map entry for the new elenet created. */
            m_sinkElemMap[s] = elem;
        }
    }

    if (status < 0)
    {
        throw runtime_error("GstPipe creation failed.\n");
    }

    LOG_DEBUG("CONSTRUCTOR\n");

    LOG_INFO("SRC_CMDS:\n");
    for (auto const &s: srcCmd)
    {
        LOG_INFO("%s\n", s.c_str());
    }
    LOG_INFO("SINK_CMD: %s\n", sinkCmd.c_str());
}

GstElement *GstPipe::createPipeline(const string &str)
{
    GstElement             *pipeline;

    pipeline = gst_parse_launch(str.c_str(), NULL);

    if (pipeline == nullptr)
    {
        LOG_ERROR("gst_parse_launch() failed.\n");
    }

    return pipeline;
}

int GstPipe::startPipeline()
{
    GstStateChangeReturn    ret;

    ret = gst_element_set_state(m_sinkPipe, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        LOG_ERROR("gst_element_set_state() failed.\n");
        return -1;
    }

    for (auto const &p : m_srcPipe)
    {
        ret = gst_element_set_state(p, GST_STATE_PLAYING);

        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            LOG_ERROR("gst_element_set_state() failed.\n");
            return -1;
        }
    }

    return 0;
}

GstElement *GstPipe::findElementByName(GstElement      *pipeline,
                                       const string    &name)
{
    GstElement *elem;

    elem = gst_bin_get_by_name(GST_BIN(pipeline), name.c_str());

    if (elem == nullptr)
    {
        LOG_ERROR("Could not find element %s in the pipeline.\n", name.c_str());
    }

    return elem;
}

int32_t GstPipe::getBuffer(const string        &name,
                           GstWrapperBuffer    &buf,
                           bool                loop)
{
    GstSample      *sample = nullptr;
    GstBuffer      *buffer = nullptr;
    GstCaps        *caps = nullptr;
    int32_t         ret;
    int32_t         status = 0;
    const auto     &it = m_srcElemMap.find(name);

    if (it == m_srcElemMap.end())
    {
        LOG_ERROR("[%s] 'elemName' lookup failed.\n", name.c_str());
        status = -1;
    }

    if (status == 0)
    {
        GstElement *elem = it->second;

        sample = gst_app_sink_try_pull_sample(GST_APP_SINK(elem), GST_TIMEOUT);

        if (sample == nullptr)
        {
            auto isEos = gst_app_sink_is_eos(GST_APP_SINK(elem));

            if (isEos)
            {
                if (loop)
                {
                    gst_element_seek_simple(elem,
                                            GST_FORMAT_TIME,
                                            GST_SEEK_FLAG_FLUSH,
                                            0);

                    sample = gst_app_sink_try_pull_sample(GST_APP_SINK(elem),
                                                          GST_TIMEOUT);

                    if (sample == nullptr)
                    {
                        LOG_ERROR("[%s] Could not get data from Gstreamer appsink.\n",
                                  name.c_str());
                        status = -1;
                    }
                }
                else
                {
                    LOG_INFO("[%s] End of Stream \n", name.c_str());
                    status = EOS;
                }
            }
            else
            {
                LOG_ERROR("[%s] Could not get data from Gstreamer appsink.\n",
                          name.c_str());
                status = -1;
            }
        }
    }

    if (status == 0)
    {
        caps = gst_sample_get_caps(sample);

        if (caps == nullptr)
        {
            LOG_ERROR("[%s] gst_sample_get_caps() failed.\n", name.c_str());
            status = -1;
        }
    }

    if (status == 0)
    {
        buffer = gst_sample_get_buffer(sample);

        if (buffer == nullptr)
        {
            LOG_ERROR("[%s] gst_sample_get_buffer() failed.\n", name.c_str());
            status = -1;
        }
    }

    if (status == 0)
    {
        /* The copy below will lead to ref counting so the buffer
         * after this point needs to be unrefd.
         */
        buffer = gst_buffer_copy(buffer);

        if (buffer == nullptr)
        {
            LOG_ERROR("[%s] gst_buffer_copy() failed.\n", name.c_str());
            status = -1;
        }
    }

    if (status == 0)
    {
        ret = gst_buffer_map(buffer, &buf.mapinfo, GST_MAP_READWRITE);

        if (ret == 0)
        {
            LOG_ERROR("[%s] gst_buffer_map() failed.\n", name.c_str());
            status = -1;
        }
    }

    if (status == 0)
    {
        GstStructure   *strc = gst_caps_get_structure(caps, 0);

        if (strc == nullptr)
        {
            LOG_ERROR("[%s] gst_caps_get_structure() failed.\n", name.c_str());
            status = -1;
        }
        else
        {
            gst_structure_get_int(strc, "width", &buf.width);
            gst_structure_get_int(strc, "height", &buf.height);
            sample = gst_sample_make_writable(sample);
            gst_sample_set_buffer(sample,buffer);
            buf.sample = sample;
            buf.gbuf   = buffer;
            buf.addr   = buf.mapinfo.data;
        }
    }
    else
    {
        /* Clean up. */
        if (buffer != nullptr)
        {
            gst_buffer_unref(buffer);
        }

        if (sample != nullptr)
        {
            gst_sample_unref(sample);
        }
    }

    return status;
}

int32_t GstPipe::putBuffer(const string        &name,
                           GstWrapperBuffer    &buff)
{
    int32_t     status = 0;
    const auto &it = m_sinkElemMap.find(name);

    if (it == m_sinkElemMap.end())
    {
        LOG_ERROR("'name' lookup failed.\n");
        status = -1;
    }
    else
    {
        GstElement *elem = it->second;
        gst_app_src_push_sample(GST_APP_SRC(elem), buff.sample);
    }

    return status;
}

int32_t GstPipe::sendEOS(const string        &name)
{
    int32_t     status = 0;
    const auto &it = m_sinkElemMap.find(name);

    if (it == m_sinkElemMap.end())
    {
        LOG_ERROR("'name' lookup failed.\n");
        status = -1;
    }
    else
    {
        GstElement *elem = it->second;
        gst_app_src_end_of_stream(GST_APP_SRC(elem));
    }

    return status;
}

int32_t GstPipe::allocBuffer(GstWrapperBuffer  &buf,
                             uint32_t           width,
                             uint32_t           height,
                             uint32_t           bpp)
{
    GstSample  *sample = nullptr;
    GstBuffer  *buffer = nullptr;
    GstCaps    *caps = nullptr;
    int32_t     ret;
    int32_t     status = 0;

    freeBuffer(buf);
    
    /* TODO: When optimizing performance, this should be allocated from the
     * GstPool passed by appsrc's peer element so that kmssink can allocate
     * physically contiguous buffer so as to avoid doig a buffer copy
     */
    buffer = gst_buffer_new_allocate(NULL, width * height * bpp, NULL);

    if (buffer == nullptr)
    {
        LOG_ERROR("Failed to allocate a new buffer\n");
        status = -1;
    }

    if (status == 0)
    {
        ret = gst_buffer_map(buffer, &buf.mapinfo, GST_MAP_WRITE);

        if ((ret == 0) || (buf.mapinfo.data == NULL))
        {
            LOG_ERROR("gst_buffer_map() failed.\n");
            status = -1;
        }
    }

    if (status == 0)
    {
        caps = gst_caps_new_simple("video/x-raw",
                                   "width", G_TYPE_INT, width,
                                   "height", G_TYPE_INT, height,
                                   "format", G_TYPE_STRING, "RGB",
                                   "framerate", GST_TYPE_FRACTION, 0, 1,
                                   NULL);
        if (caps == nullptr)
        {
            LOG_ERROR("Cannot create caps from gst_caps_new_simple\n");
            status = -1;
        }
    }

    if (status == 0)
    {
        sample = gst_sample_new(buffer, caps, NULL, NULL);

        if (sample == nullptr)
        {
            LOG_ERROR("gst_sample_new() failed.\n");
            status = -1;
        }
    }

    if (status == 0)
    {
        buf.width  = width;
        buf.height = height;
        buf.sample = sample;
        buf.gbuf   = buffer;
        buf.addr   = buf.mapinfo.data;
    }
    else
    {
        /* Clean up. */
        if (buffer != nullptr)
        {
            gst_buffer_unref(buffer);
        }

        if (sample != nullptr)
        {
            gst_sample_unref(sample);
        }
    }

    /* This should be released irrespective of the overall status. */
    if (caps != nullptr)
    {
        gst_caps_unref(caps);
    }

    return status;
}

void GstPipe::freeBuffer(GstWrapperBuffer  &buf)
{
    buf.reset();
}

void GstWrapperConfig::dumpInfo() const
{
    LOG_INFO("\n");
    LOG_INFO("GstWrapperConfig::input         = %s\n", input.c_str());
    LOG_INFO("GstWrapperConfig::output        = %s\n", output.c_str());
    LOG_INFO("GstWrapperConfig::device        = %s\n", device.c_str());
    LOG_INFO("GstWrapperConfig::frameRate     = %d\n", frameRate);
    LOG_INFO("GstWrapperConfig::connectorId   = %d\n", connectorId);
    LOG_INFO("GstWrapperConfig::startIndex    = %d\n", startIndex);
    LOG_INFO("\n");
}

GstPipe::~GstPipe()
{
    LOG_DEBUG("DESTRUCTOR\n");

    //Wait for EOS from sink pipeline
    auto bus = gst_pipeline_get_bus(GST_PIPELINE(m_sinkPipe));

    GstMessage *msg = gst_bus_poll(bus,
                                  (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR),
                                  GST_CLOCK_TIME_NONE);

    if (msg) {
        gst_message_unref (msg);
    }

    gst_object_unref(bus);

    //Set the pipelines to NULL and unref
    for (auto const &p : m_srcPipe)
    {
        gst_element_set_state(p, GST_STATE_NULL);
        gst_object_unref(p);
    }

    gst_element_set_state(m_sinkPipe, GST_STATE_NULL);
    gst_object_unref(m_sinkPipe);

    //Unref the appsink and appsrc elements
    for (auto &m : m_sinkElemMap)
    {
        auto const &sink = m.second;
        gst_object_unref(sink);
    }

    for (auto &m : m_srcElemMap)
    {
        auto const &src = m.second;
        gst_object_unref(src);
    }
}

} // namespace ti::edgeai::common

