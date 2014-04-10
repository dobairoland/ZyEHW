/*
 * Copyright (C) 2014 Roland Dobai
 *
 * This file is part of ZyEHW.
 *
 * ZyEHW is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * ZyEHW is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with ZyEHW. If not, see <http://www.gnu.org/licenses/>.
 */

#include "dpr.h"

#include <stdlib.h>
#include <string.h>
#include "bitstream.h"
#include "xdevcfg.h"

#if 0
#define DPR_DEBUG
#endif

#if 0
#define DPR_BITSTREAM_OUT
#endif

static XDcfg_Config *devc = NULL;
static XDcfg dev;

#ifdef DPR_BITSTREAM_OUT
static inline void print_bitstream(const u32 *stream, int size)
{
        int i;

        for (i = 0; i < size; ++i)
                xil_printf("0x%X\n\r", stream[i]);
}
#endif

void dpr_reconfigure()
{
        volatile u32 reg;

        if (!devc) {
                devc = XDcfg_LookupConfig(XPAR_XDCFG_0_DEVICE_ID);

                if (XDcfg_CfgInitialize(&dev, devc, devc->BaseAddr) !=
                                XST_SUCCESS) {
                        print("DevC initialization failed\n");
                        exit(-1);
                }

                XDcfg_Unlock(&dev);

                if (XDcfg_SelfTest(&dev) != XST_SUCCESS)
                        print("DevC self-test failed\n\r");

                XDcfg_EnablePCAP(&dev);
                XDcfg_SetControlRegister(&dev, XDCFG_CTRL_PCAP_PR_MASK);
        }

        XDcfg_IntrClear(&dev, (XDCFG_IXR_DMA_DONE_MASK |
                                XDCFG_IXR_D_P_DONE_MASK));

#ifdef DPR_DEBUG
                print("Starting the DMA transfer\n\r");
#endif

        /* Download bitstream in non secure mode */
        if (XDcfg_Transfer(&dev, lut_stream, size_of_lut_stream(),
                                (void *) XDCFG_DMA_INVALID_ADDRESS, 0,
                                XDCFG_NON_SECURE_PCAP_WRITE) != XST_SUCCESS) {
                print("DMA transfer failed\n\r");
                exit(-1);
        }
#ifdef DPR_DEBUG
        else
                print("DMA transfer OK\n\r");
#endif

        for (reg = 0; (reg & XDCFG_IXR_DMA_DONE_MASK) !=
                        XDCFG_IXR_DMA_DONE_MASK;
                        reg = XDcfg_IntrGetStatus(&dev));

        for (reg = 0; (reg & XDCFG_IXR_D_P_DONE_MASK) !=
                        XDCFG_IXR_D_P_DONE_MASK;
                        reg = XDcfg_IntrGetStatus(&dev));

#ifdef DPR_DEBUG
        print("DPR complete :-)\n\r");
#endif

#ifdef DPR_BITSTREAM_OUT
        print("Bitstream:\n\r");
        print_bitstream(lut_stream, size_of_lut_stream());
#endif
}
