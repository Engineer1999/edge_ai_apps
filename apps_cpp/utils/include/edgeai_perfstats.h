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

#ifndef _TI_EDGEAI_PERFSTATS_H_
#define _TI_EDGEAI_PERFSTATS_H_

/* Standard headers. */
#include <string>
#include <filesystem>

/* Third-party headers. */
#include <yaml-cpp/yaml.h>

/* Module headers. */
#include <common/include/post_process_image.h>
#include <common/include/pre_process_image.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <utils/perf_stats/include/app_perf_stats.h>

#ifdef __cplusplus
}
#endif

/**
 * \defgroup group_edgeai_utils Utility libraries
 *
 * \brief A collection of SW components used across C++ demos.
 *
 * \ingroup group_edgeai_cpp_apps
 */

using namespace std;

namespace ti::utils
{
    using namespace ti::edgeai::common;


    void enableReport(bool state, const char *dir_name=nullptr);

    void waitForPerfThreadExit();

    void disableReport();

    void startRec();

    void endRec();

   
} 
#endif /* _TI_EDGEAI_PERFSTATS_H_ */

