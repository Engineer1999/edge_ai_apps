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
#include <algorithm>
#include <filesystem>
#include <thread>
#include <unistd.h>
#include <ncurses.h>

/* Module headers. */
#include <common/include/edgeai_utils.h>

namespace ti::edgeai::common
{
// Please keep the following array and map consistent in terms of the
// number anf names of the elements
const string gStatKeys[] = {"dl-inference"};
const string gMetricKeys[] = {"total time", "framerate"};

/**
 * Hold the processing time of different operations
 */
struct ProcTime
{
    float       average{0.0f};
    uint64_t    samples{0};
};

struct Metrics
{
    float       value{0.0f};
    string      unit{"ms"};
    uint64_t    samples{0};
};

using MapProcTime = map<string, ProcTime>;
using MapMetrics  = map<string, Metrics>;

struct StatEntry
{
    /** Name of the input source. */
    string      m_inputName;

    /** Type of the model.
     * - classification
     * - detection
     * = segmentation
     */
    string      m_modelType;

    /** Name of the model. */
    string      m_modelName;

    /** Processing time details. */
    MapProcTime m_proc{};

    /** Metrics context. */
    MapMetrics  m_metrics{};
};

/* Initialize the status. */
MapStatEntry Statistics::m_stats{};
bool Statistics::m_printCurses = false;
bool Statistics::m_printStdout = !Statistics::m_printCurses;
thread Statistics::m_reportingThread;

int32_t Statistics::addEntry(uint32_t       key,
                             const string  &inputName,
                             const string  &modelType,
                             const string  &modelPath)
{
    int32_t status = 0;

    /* Check if an entry for this key already exists. */
    if (m_stats.find(key) != m_stats.end())
    {
        LOG_ERROR("An entry for the key [%d] already exists.\n");
        status = -1;
    }

    if (status == 0)
    {
        string      fName = modelPath;
        StatEntry   s;

        /* Delete the trailing '/' if present. This will lead to an empty
         * string in the call to filename() below, if not deleted.
         */
        if (fName.back() == '/')
        {
            fName.pop_back();
        }

        s.m_modelName = "Model Name:   " + string(filesystem::path(fName).filename());
        s.m_modelType = "Model Type:   " + modelType;
        s.m_inputName = "Input Source: " + inputName;

        /* Initialize the maps. */
        for (auto const &tag : gStatKeys)
        {
            s.m_proc[tag] = ProcTime();
        }

        for (auto const &tag : gMetricKeys)
        {
            s.m_metrics[tag] = Metrics();
        }

        m_stats[key] = s;
    }

    return status;
}

int32_t Statistics::reportProcTime(uint32_t         key,
                                   const string    &tag,
                                   float            value)
{
    StatEntry  *e;
    ProcTime   *p;
    int32_t     status = 0;

    if (m_stats.find(key) == m_stats.end())
    {
        LOG_ERROR("Key [%d] not found.\n", key);
        status = -1;
    }

    if (status == 0)
    {
        e = &m_stats[key];
        p = &e->m_proc[tag];

        p->average = (p->average * p->samples + value)/(p->samples + 1);
        p->samples++;

        if (m_printStdout)
        {
            printf("[UTILS] [%s] Time for '%s': %5.2f ms (avg %5.2f ms)\n",
                    e->m_modelName.c_str(),
                    tag.c_str(), value, p->average);
        }
    }

    return status;
}

int32_t Statistics::reportMetric(uint32_t       key,
                                 const string  &tag,
                                 const string  &unit,
                                 float          value)
{
    StatEntry  *e;
    Metrics    *m;
    int32_t     status = 0;

    if (m_stats.find(key) == m_stats.end())
    {
        LOG_ERROR("Key [%d] not found.\n", key);
        status = -1;
    }

    if (status == 0)
    {
        e = &m_stats[key];
        m = &e->m_metrics[tag];

        m->value = (m->value * m->samples + value)/(m->samples + 1);
        m->unit  = unit;

        m->samples++;

        if (m_printStdout)
        {
            printf("[UTILS] [%s] Metric '%s': %5.2f %s\n",
                    e->m_modelName.c_str(),
                    tag.c_str(), m->value, m->unit.c_str());
        }
    }

    return status;
}

static inline void drawDataRow(int32_t     &row,
                               const char  *title,
                               float        data1, 
                               const char  *data1Unit,
                               int32_t      data2,
                               int32_t      lastPos)
{
    mvprintw(row, 1, "| %-29s:", title);
    attron(A_BOLD);
    mvprintw(row, 34, "%8.2f %s", data1, data1Unit);
    attroff(A_BOLD);
    mvprintw(row, 46, " from %5d samples", data2);
    mvprintw(row, lastPos, "|");
    row++;
}

void Statistics::reportingLoop(const string &demoName)
{
    int32_t     rows;
    int32_t     startRow;
    int32_t     coloumns;
    int32_t     len;
    int32_t     maxLen = 65;
    uint64_t    samples;
    auto       &statsDb = m_stats;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    getmaxyx(stdscr, rows, coloumns);
    //resize_term(rows, coloumns);

    /* Compute the length of a data row. It is of the form
     * | .............. |
     *  ^              ^
     * which means that we need a space at the either end of the string
     * and hence +2 below.
     */
    len = 0;
    for (auto i = 0; i < statsDb.size(); i++)
    {
        auto const s = &statsDb[i];
        int32_t ml = s->m_modelName.length() + 2;
        int32_t nl = s->m_inputName.length() + 2;

        len = std::max({len, ml, nl});
    }

    /* Set the length to fit the data nicely. */
    len = std::max(len, maxLen);

    const string   &border = '+' + string(len, '-') + '+';
    const string   &fmt = "| %-" + to_string(len-1) + "s|";

    startRow = 1;

    while (m_printCurses)
    {
        clear();
        int row = 1;

        mvprintw(row++, 1, border.c_str());
        mvprintw(row++, 1, fmt.c_str(), demoName.c_str());
        mvprintw(row++, 1, border.c_str());

        for (auto i = 0; i < statsDb.size(); i++)
        {
            auto const s = &statsDb[i];

            mvprintw(row++, 1, border.c_str());
            mvprintw(row++, 1, fmt.c_str(), s->m_inputName.c_str());
            mvprintw(row++, 1, fmt.c_str(), s->m_modelName.c_str());
            mvprintw(row++, 1, fmt.c_str(), s->m_modelType.c_str());
            mvprintw(row++, 1, border.c_str());

            for (auto &key : gStatKeys)
            {
                auto const *p = &s->m_proc[key];
                float avg = p->average;

                samples = p->samples;
                drawDataRow(row, key.c_str(), avg, "ms", samples, len+2);
            }

            for (auto &key : gMetricKeys)
            {
                auto const *m = &s->m_metrics[key];

                drawDataRow(row, key.c_str(), m->value,
                            m->unit.c_str(), m->samples, len+2);
            }

            mvprintw(row++, 1, border.c_str());
        }

        refresh();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    echo();
    nocbreak();
    endwin();
}

void Statistics::enableCursesReport(bool            state,
                                    bool            verbose,
                                    const string   &demoName)
{
    m_printCurses = state;
    m_printStdout = !state && verbose;

    if (state)
    {
        m_reportingThread = std::thread(reportingLoop, demoName);
    }
}

void Statistics::disableCursesReport()
{
    m_printCurses = false;
    if (m_reportingThread.joinable())
    {
        m_reportingThread.join();
    }
}

int32_t getInfererConfig(const YAML::Node  &appConfig,
                         const string      &modelBasePath,
                         InfererConfig     &infConfig)
{
    const YAML::Node   &n = appConfig["session"];
    int32_t             status = 0;

    // Validate the parsed yaml configuration and create the configuration 
    // for the inference object creation.
    if (!n)
    {
        LOG_ERROR("Inference configuration parameters  missing.\n");
        status = -1;
    }
    else if (!n["model_path"])
    {
        LOG_ERROR("Please specifiy a valid model path.\n");
        status = -1;
    }
    else if (!n["artifacts_folder"])
    {
        LOG_ERROR("Artifacts directory path missing.\n");
        status = -1;
    }
    else if (!n["session_name"])
    {
        LOG_ERROR("Please specifiy a valid run-time API type.\n");
        status = -1;
    }
    else
    {
        // Initialize the inference configuration parameter object
        infConfig.rtType = n["session_name"].as<string>();

        if (infConfig.rtType == "tvmdlr")
        {
            infConfig.modelFile = modelBasePath + "/model";
        }
        else if ((infConfig.rtType == "tflitert") ||
                 (infConfig.rtType == "onnxrt"))
        {
            const string &modSrc = n["model_path"].as<string>();
            infConfig.modelFile = modelBasePath + "/" + modSrc;
        }

        const string &artSrc = n["artifacts_folder"].as<string>();
        infConfig.artifactsPath = modelBasePath + "/artifacts";

        if (n["device_type"])
        {
            infConfig.devType = n["device_type"].as<string>();
        }
        else
        {
            infConfig.devType = "CPU";
        }

        if (n["device_id"])
        {
            infConfig.devId = n["device_id"].as<int32_t>();
        }
        else
        {
            infConfig.devId = 0;
        }
    }

    return status;
}

int32_t getPostprocessImageConfig(const YAML::Node         &yaml,
                                  PostprocessImageConfig   &config)
{
    const YAML::Node   &session = yaml["session"];
    const YAML::Node   &taskType = yaml["task_type"];
    const YAML::Node   &postProc = yaml["postprocess"];
    int32_t             status = 0;

    // Validate the parsed yaml configuration and create the configuration 
    // for the inference object creation.
    if (!session)
    {
        LOG_ERROR("Inference configuration parameters  missing.\n");
        status = -1;
    }
    else if (!postProc)
    {
        LOG_WARN("Postprocess configuration parameters missing.\n");
        status = -1;
    }
    else if (!taskType)
    {
        LOG_WARN("Tasktype configuration parameters missing.\n");
        status = -1;
    }

    if (status == 0)
    {
        config.rtType   = session["session_name"].as<string>();
        config.taskType = taskType.as<std::string>();

        // Read the data layout
        if (postProc["data_layout"])
        {
            config.dataLayout = postProc["data_layout"].as<std::string>();
        }

        if (postProc["formatter"] && postProc["formatter"]["src_indices"])
        {
            const YAML::Node &formatterNode = postProc["formatter"]["src_indices"];

            /* default is assumed to be [0 1 2 3 4 5] which means
             * [x1y1 x2y2 label score].
             *
             * CASE 1: Only 2 values are specified. These are assumed to
             *         be "label" and "score". Keep [0..3] same as the default
             *         values but overwrite [4,5] with these two values.
             *
             * CASE 2: Only 4 values are specified. These are assumed to
             *         be "x1y1" and "x2y2". Keep [4,5] same as the default
             *         values but overwrite [0..3] with these four values.
             *
             * CASE 3: All 6 values are specified. Overwrite the defaults.
             *
             */
            if (formatterNode.size() == 2)
            {
                config.formatter[4] = formatterNode[0].as<int32_t>();
                config.formatter[5] = formatterNode[1].as<int32_t>();
            }
            else if ((formatterNode.size() == 6) ||
                     (formatterNode.size() == 4))
            {
                for (uint8_t i = 0; i < formatterNode.size(); i++)
                {
                    config.formatter[i] = formatterNode[i].as<int32_t>();
                }
            }
            else
            {
                LOG_ERROR("formatter specification incorrect.\n");
                status = -1;
            }
        }

        if (postProc["ignore_index"])
        {
            config.ignoreIndex = postProc["ignore_index"].as<int32_t>();
        }

        if (postProc["normalized_detections"])
        {
            config.normDetect = postProc["normalized_detections"].as<bool>();
        }

        if (postProc["shuffle_indices"])
        {
            const YAML::Node indicesNode = postProc["shuffle_indices"];

            for (uint8_t i = 0; i < indicesNode.size(); i++)
            {
                config.resultIndices[i] = indicesNode[i].as<int32_t>();
            }
        }

        const YAML::Node   &metric = yaml["metric"];

        if (metric && metric["label_offset_pred"])
        {
            // Read the width and height values
            const YAML::Node &offset = metric["label_offset_pred"];

            if (offset.Type() == YAML::NodeType::Scalar)
            {
                /* Use "0" key to store the value. */
                config.labelOffsetMap[0] = offset.as<int32_t>();
            }
            else if (offset.Type() == YAML::NodeType::Map)
            {
                for (const auto& it : offset)
                {
                    if (it.second.Type() == YAML::NodeType::Scalar)
                    {
                        config.labelOffsetMap[it.first.as<int32_t>()] =
                            it.second.as<int32_t>();
                    }
                }
            }
            else
            {
                LOG_ERROR("label_offset_pred specification incorrect.\n");
                status = -1;
            }
        }
    }

    return status;
}

int32_t getPreprocessImageConfig(const YAML::Node      &yaml,
                                 PreprocessImageConfig &config)
{
    const YAML::Node   &session = yaml["session"];
    const YAML::Node   &taskType = yaml["task_type"];
    const YAML::Node   &preProc = yaml["preprocess"];
    int32_t             status = 0;

    // Validate the parsed yaml configuration and create the configuration 
    // for the inference object creation.
    if (!preProc)
    {
        LOG_ERROR("Preprocess configuration parameters missing.\n");
        status = -1;
    }
    else if (!preProc["mean"])
    {
        LOG_ERROR("Mean value specification missing.\n");
        status = -1;
    }
    else if (!preProc["scale"])
    {
        LOG_ERROR("Scale value specification missing.\n");
        status = -1;
    }
    else if (!preProc["data_layout"])
    {
        LOG_ERROR("Data layout specification missing.\n");
        status = -1;
    }
    else if (!preProc["resize"])
    {
        LOG_ERROR("Resize specification missing.\n");
        status = -1;
    }
    else if (!preProc["crop"])
    {
        LOG_ERROR("Crop specification missing.\n");
        status = -1;
    }
    
    // Check if crop information exists
    if (status == 0)
    {
        config.rtType   = session["session_name"].as<string>();
        config.taskType = taskType.as<std::string>();

        // Read the width and height values
        const YAML::Node &cropNode = preProc["crop"];

        if (cropNode.Type() == YAML::NodeType::Sequence)
        {
            config.outDataHeight = cropNode[0].as<int32_t>();
            config.outDataWidth  = cropNode[1].as<int32_t>();
        }
        else if (cropNode.Type() == YAML::NodeType::Scalar)
        {
            config.outDataHeight = cropNode.as<int32_t>();
            config.outDataWidth  = config.outDataHeight;
        }

        // Read the data layout
        config.dataLayout = preProc["data_layout"].as<std::string>();

        // Read the mean values
        const YAML::Node &meanNode = preProc["mean"];
        for (uint32_t i = 0; i < meanNode.size(); i++)
        {
            config.mean.push_back(meanNode[i].as<float>());
        }

        // Read the scale values
        const YAML::Node &scaleNode = preProc["scale"];
        for (uint32_t i = 0; i < scaleNode.size(); i++)
        {
            config.scale.push_back(scaleNode[i].as<float>());
        }

        // Read the width and height values
        const YAML::Node &resizeNode = preProc["resize"];

        if (resizeNode.Type() == YAML::NodeType::Sequence)
        {
            config.resizeHeight = resizeNode[0].as<int32_t>();
            config.resizeWidth  = resizeNode[1].as<int32_t>();
        }
        else if (resizeNode.Type() == YAML::NodeType::Scalar)
        {
            int32_t resize = resizeNode.as<int32_t>();
            int32_t minVal = std::min(config.inDataHeight, config.inDataWidth);

            /* tiovxmultiscaler dosen't support odd resolutions */
            config.resizeHeight = (((config.inDataHeight * resize)/minVal) >> 1) << 1;
            config.resizeWidth  = (((config.inDataWidth * resize)/minVal) >> 1) << 1;
        }

        if (config.mean.size() != config.scale.size())
        {
            LOG_ERROR("The sizes of mean and scale vectors do not match.\n");
            status = -1;
        }
    }

    config.numChans = config.mean.size();

    return status;
}

} // namespace ti::edgeai::common

