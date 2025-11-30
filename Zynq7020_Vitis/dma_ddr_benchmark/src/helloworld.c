
#include <stdio.h>
#include "xparameters.h"
#include "platform.h"
#include "xil_printf.h"

#include "xaxidma.h"
#include "xil_cache.h"
#include "xgpio.h"

#define GPIO_TRIGGER_DEVICE_ID   XPAR_AXI_GPIO_TRIGGER_DEVICE_ID  // 8-bit OUT
#define GPIO_START_DEVICE_ID     XPAR_AXI_GPIO_START_DEVICE_ID    // 32-bit IN
#define GPIO_END_DEVICE_ID       XPAR_AXI_GPIO_END_DEVICE_ID      // 32-bit IN

XAxiDma AxiDma;
XGpio GpioTrigger;   // PS -> PL (bit 0 is start trigger)
XGpio GpioStart;     // PL -> PS (start_time[31:0])
XGpio GpioEnd;       // PL -> PS (end_time[31:0])

#define FCLK0_FREQ_HZ 142857132.0

#define PRINT_DDR_CONTENT   1

// 420Mb/sec write speed 142MHz clock(150 is selected but calculated is 142 by Zynq) with 1048576 -> 1048576 * 8 = 8 MByte data
const int burst_words       = 1048576; //= 1048576 ; 
const int bytes_per_word    = 8;         // 64-bit
const int data_size         = burst_words * bytes_per_word;
const int data_address_size = burst_words;
int dma_print_increment     = 1;
u32 ticks = 0;

void init_gpios(void)
{
    int status;

    /* TRIGGER GPIO (PS → PL, 8-bit OUTPUT) */
    status = XGpio_Initialize(&GpioTrigger, XPAR_AXI_GPIO_0_BASEADDR);
    if (status != XST_SUCCESS) {
        xil_printf("ERROR: GpioTrigger init failed\r\n");
    }
    
    /* Channel 1 as output (0 = output) */
    XGpio_SetDataDirection(&GpioTrigger, 1, 0x00);
    /* Clear trigger */
    XGpio_DiscreteWrite(&GpioTrigger, 1, 0x00);

    /* START TIME GPIO (PL → PS, 32-bit INPUT) */
    status = XGpio_Initialize(&GpioStart, XPAR_AXI_GPIO_1_BASEADDR);
    if (status != XST_SUCCESS) {
        xil_printf("ERROR: GpioStart init failed\r\n");
    }
    XGpio_SetDataDirection(&GpioStart, 1, 0xFFFFFFFF);  // all bits input

    /* END TIME GPIO (PL → PS, 32-bit INPUT) */
    status = XGpio_Initialize(&GpioEnd, XPAR_AXI_GPIO_2_BASEADDR);
    if (status != XST_SUCCESS) {
        xil_printf("ERROR: GpioEnd init failed\r\n");
    }
    XGpio_SetDataDirection(&GpioEnd, 1, 0xFFFFFFFF);    // all bits input

    xil_printf("GPIOs initialized.\r\n");
}

/* -------- Simple S2MM transfer + wait -------- */
int dma_s2mm_start(u32 dst_addr, u32 length_bytes)
{
    int Status;

    // Flush destination region (not strictly needed for pure write,but harmless) 
    Xil_DCacheFlushRange(dst_addr, length_bytes);

    // Start S2MM transfer
    Status = XAxiDma_SimpleTransfer(&AxiDma, dst_addr, length_bytes, XAXIDMA_DEVICE_TO_DMA);
    if (Status != XST_SUCCESS) {
        xil_printf("XAxiDma_SimpleTransfer failed: %d\r\n", Status);
        return XST_FAILURE;
    }

    // Wait for completion
    while (XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA));

    // Read status
    u32 s2mm_status = XAxiDma_ReadReg(AxiDma.RegBase, XAXIDMA_RX_OFFSET + XAXIDMA_SR_OFFSET);
    xil_printf("S2MM_DMASR = 0x%08lx\r\n", s2mm_status);

    if (s2mm_status & XAXIDMA_ERR_ALL_MASK) {
        xil_printf("S2MM error!\r\n");
        XAxiDma_Reset(&AxiDma);
        return XST_FAILURE;
    }

    // Invalidate cache to see new data from DDR
    Xil_DCacheInvalidateRange(dst_addr, length_bytes);

    return XST_SUCCESS;
}

void init_axi_dma(){
    
    XAxiDma_Config *Cfg = XAxiDma_LookupConfig(XPAR_XAXIDMA_0_BASEADDR);
    if (!Cfg) {
        xil_printf("No DMA config found!\r\n");
        return;
    }

    if (XAxiDma_CfgInitialize(&AxiDma, Cfg) != XST_SUCCESS) {
        xil_printf("DMA init failed\r\n");
        return;
    }

    if (XAxiDma_HasSg(&AxiDma)) {
        xil_printf("DMA is in SG mode, expected simple mode!\r\n");
        return;
    }
    
}

void print_ddr_content(u32* ddr_address){

    /* DDR buffer where DMA writes */
    u32 *buf32 = ddr_address;
    
    #if PRINT_DDR_CONTENT
        xil_printf("DDR content before DMA:\r\n");
        for (int i = 0; i < data_address_size; i += dma_print_increment) {
            xil_printf("[%d] = 0x%08X\r\n", i, buf32[i]);
        }
    #endif
    
}

int main(void)
{

    init_platform();
    init_gpios();

    init_axi_dma();
    
    //print_ddr_content((u32*)0x01000000);
    
    /* ---------- Trigger PL data generator ---------- */
    // Working tested. s2mm does not start without it
    XGpio_DiscreteWrite(&GpioTrigger, 1, 0x01);   // set bit0 = 1
    xil_printf("GPIO start signal sent\r\n");

    /* ---------- Start DMA transfer ---------- */
    dma_s2mm_start(0x01000000, data_size);
    xil_printf("DMA S2MM DONE\r\n");

    //print_ddr_content((u32*)0x01000000);
    

	// Read pl counters to calculate write speed
	u32 start = XGpio_DiscreteRead(&GpioStart, 1);
	u32 end   = XGpio_DiscreteRead(&GpioEnd,   1);
	int delta = end - start;

    float delta_sec = delta / FCLK0_FREQ_HZ;
    int speed = (float)data_size / delta_sec;


	xil_printf("start = %u\r\n", start);
	xil_printf("end   = %u\r\n", end);
	xil_printf("delta = %u cycles\r\n", delta);
    xil_printf("delta_ms = %u,%u ms\r\n", (int)(delta_sec*1000.0), (((int)(delta_sec*1000000.0))%1000));
    xil_printf("speed = %ubps\r\n", speed);
    xil_printf("speed = %uMbps\r\n", speed/1000000);

    while (1) {
    }

    cleanup_platform();
    return 0;
}
