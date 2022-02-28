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
#include <signal.h>
#include <getopt.h>
#include <thread>

/* Module headers. */
#include <utils/app_init/include/app_init.h>
#include <utils/include/edgeai_perfstats.h>

static bool gStop = false;
static bool gDisplay = true;
static bool gLogToFile = false;
static char *gSubDirName = nullptr;
static std::thread gDispThreadId;

static void showUsage(const char *name)
{
    printf("# \n");
    printf("# %s [OPTIONAL PARAMETERS]\n", name);
    printf("# OPTIONAL PARAMETERS:\n");
    printf("#  [--no-display |-n Display report to the screen. "
           "Display is enabled by default.]\n");
    printf("#  [--log        |-l Log the reports to files.\n");
    printf("#  [--dir        |-d Sub-directory for storing the log files. "
            "The generated files will be stored under "
            "../perf_logs/<sub_dir> directory.\n"); 
    printf("#                    Note that the directory location is relative "
            "to the current directory the tool is invoked from.\n");
    printf("#  [--help       |-h]\n");
    printf("# \n");
    printf("# (C) Texas Instruments 2021\n");
    printf("# \n");
    printf("# EXAMPLE:\n");
    printf("# Turn off the report display to screen.\n");
    printf("#    %s -n \n", name);
    printf("# \n");
    printf("# Turn on the report logging to files.\n");
    printf("#    %s -l \n", name);
    printf("# \n");
    exit(0);
}

static int32_t  parse(int32_t   argc,
                      char     *argv[])
{
    int32_t longIndex;
    int32_t opt;
    static struct option long_options[] = {
        {"help",       no_argument,       0, 'h' },
        {"no-display", no_argument,       0, 'n' },
        {"log",        no_argument,       0, 'l' },
        {"dir",        required_argument, 0, 'd' },
        {0,            0,                 0,  0  }
    };

    while ((opt = getopt_long(argc, argv,"-hnld:", 
                   long_options, &longIndex )) != -1)
    {
        switch (opt)
        {
            case 'l' :
                gLogToFile = true;
                break;

            case 'n' :
                gDisplay = false;
                break;

            case 'd' :
                gSubDirName = optarg;
                break;

            case 'h' :
            default:
                showUsage(argv[0]);
                return -1;

        } // switch (opt)

    }

    return 0;

} // End of parse()

static void sigHandler(int32_t sig)
{
    gStop = true;

    /* Disable the performance report. */
    ti::utils::disableReport();
}

void displayThread()
{
    while (!gStop)
    {
        system("clear");
        appPerfStatsCpuLoadPrintAll();
        appPerfStatsHwaLoadPrintAll();
        appPerfStatsDdrStatsPrintAll();
        appPerfStatsResetAll();
        this_thread::sleep_for(chrono::milliseconds(2000));
    }
}

int main(int argc, char *argv[])
{
    int32_t status = 0;

    /* Register SIGINT handler. */
    signal(SIGINT, sigHandler);

    status = parse(argc, argv);

    if (status < 0)
    {
        return status;
    }

    /* Initialize the system. */
    status = appInit();

    if(status < 0)
    {
        perror("appInit failed");
        return status;
    }

    /* Configure the performance report. */
    ti::utils::enableReport(gLogToFile, gSubDirName);

    if (gDisplay)
    {
        gDispThreadId = std::thread([]{displayThread();});

        gDispThreadId.join();
    }

    if (gLogToFile)
    {
        ti::utils::waitForPerfThreadExit();
    }

    printf("CALLING DE-INIT.\n");

    /* De-Initialize the system. */
    status = appDeInit();

    return status;
}
