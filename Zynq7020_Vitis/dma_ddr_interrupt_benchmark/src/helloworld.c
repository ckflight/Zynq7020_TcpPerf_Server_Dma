
#include <stdio.h>
#include "xparameters.h"
#include "platform.h"
#include "xil_printf.h"

#include "xaxidma.h"
#include "xscugic.h"
#include "xgpio.h"
#include "xil_cache.h"
#include "xil_exception.h"

/* ===================== SYSTEM CONSTANTS ===================== */

#undef  DMA_S2MM_INTR_ID
#define DMA_S2MM_INTR_ID            61   // IRQ_F2P[0] → GIC ID 61 on Zynq
#define XPAR_XAXIDMA_0_DEVICE_ID    0
//#define DMA_S2MM_INTR_ID XPAR_FABRIC_XAXIDMA_0_INTR

#define DDR_BASE_ADDR      0x01000000
//#define FCLK0_FREQ_HZ      142857132.0
#define FCLK0_FREQ_HZ      150000000.0

#define PRINT_DDR_CONTENT   0

const int burst_words       = 1048576; //= 1048576 ; 
const int bytes_per_word    = 8;         // 64-bit
const int data_size         = burst_words * bytes_per_word;
const int data_address_size = burst_words;
int dma_print_increment     = 1;

/* ===================== GLOBAL OBJECTS ===================== */

XAxiDma  AxiDma;
XScuGic  Intc;

XGpio GpioTrigger;
XGpio GpioStart;
XGpio GpioEnd;

volatile int S2MM_Done = 0;
volatile int DMA_Error = 0;

/* ===================== DMA ISR (XILINX STYLE) ===================== */

void S2mmIsr(void *CallbackRef)
{
    u32 IrqStatus;
    int TimeOut;
    XAxiDma *AxiDmaInst = (XAxiDma *)CallbackRef;

    XAxiDma_IntrDisable(AxiDmaInst, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

    IrqStatus = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrAckIrq(AxiDmaInst, IrqStatus, XAXIDMA_DEVICE_TO_DMA);

    if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK))
        return;

    if (IrqStatus & XAXIDMA_IRQ_ERROR_MASK) {
        xil_printf("DMA ERROR IRQ\r\n");
        DMA_Error = 1;

        XAxiDma_Reset(AxiDmaInst);
        TimeOut = 10000;
        while (TimeOut--) {
            if (XAxiDma_ResetIsDone(AxiDmaInst))
                break;
        }
        return;
    }

    if (IrqStatus & XAXIDMA_IRQ_IOC_MASK) {
        S2MM_Done = 1;
    }

    XAxiDma_IntrEnable(AxiDmaInst,
        (XAXIDMA_IRQ_IOC_MASK | XAXIDMA_IRQ_ERROR_MASK),
        XAXIDMA_DEVICE_TO_DMA);
}

/* ===================== GIC INIT (XILINX STYLE) ===================== */

int InitInterruptSystem(void)
{
    XScuGic_Config *CfgPtr;
    int Status;

    CfgPtr = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
    if (!CfgPtr) return XST_FAILURE;

    Status = XScuGic_CfgInitialize(&Intc, CfgPtr, CfgPtr->CpuBaseAddress);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    XScuGic_SetPriorityTriggerType(&Intc, DMA_S2MM_INTR_ID, 0xA0, 0x3);

    Status = XScuGic_Connect(&Intc, DMA_S2MM_INTR_ID, (Xil_ExceptionHandler)S2mmIsr, &AxiDma);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    XScuGic_Enable(&Intc, DMA_S2MM_INTR_ID);

    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
        (Xil_ExceptionHandler)XScuGic_InterruptHandler, &Intc);
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

/* ===================== DMA INIT (XILINX STYLE) ===================== */

int InitAxiDma(void)
{
    XAxiDma_Config *CfgPtr;
    int Status;

    CfgPtr = XAxiDma_LookupConfig(XPAR_XAXIDMA_0_DEVICE_ID);
    if (!CfgPtr) return XST_FAILURE;

    Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    if (XAxiDma_HasSg(&AxiDma)) return XST_FAILURE;

    XAxiDma_Reset(&AxiDma);
    while (!XAxiDma_ResetIsDone(&AxiDma));

    XAxiDma_IntrEnable(&AxiDma,
        (XAXIDMA_IRQ_IOC_MASK | XAXIDMA_IRQ_ERROR_MASK),
        XAXIDMA_DEVICE_TO_DMA);

    return XST_SUCCESS;
}

/* ===================== GPIO INIT ===================== */

void InitGpio(void)
{
    XGpio_Initialize(&GpioTrigger, XPAR_AXI_GPIO_0_BASEADDR);
    XGpio_Initialize(&GpioStart,   XPAR_AXI_GPIO_1_BASEADDR);
    XGpio_Initialize(&GpioEnd,     XPAR_AXI_GPIO_2_BASEADDR);

    XGpio_SetDataDirection(&GpioTrigger, 1, 0x00);
    XGpio_SetDataDirection(&GpioStart,   1, 0xFFFFFFFF);
    XGpio_SetDataDirection(&GpioEnd,     1, 0xFFFFFFFF);

    XGpio_DiscreteWrite(&GpioTrigger, 1, 0x00);
}

/* ===================== MAIN ===================== */

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

int VerifyDdrContent(u64 *buf, u32 word_count)
{
    u32 i;
    u32 error_count = 0;

    for (i = 0; i < word_count; i++) {
        u64 expected = (u64)i;

        if (buf[i] != expected) {
            if (error_count < 10) {
                xil_printf("DDR MISMATCH @ %u: got = 0x%08X%08X, exp = 0x%08X%08X\r\n",
                            i,
                            (u32)(buf[i] >> 32), (u32)buf[i],
                            (u32)(expected >> 32), (u32)expected);
            }
            error_count++;
        }
    }

    if (error_count == 0) {
        xil_printf("DDR VERIFY PASSED (%u x 64-bit words)\r\n", word_count);
        return XST_SUCCESS;
    } else {
        xil_printf("DDR VERIFY FAILED: %u errors\r\n", error_count);
        return XST_FAILURE;
    }
}


int main(void)
{
    init_platform();

    InitGpio();
    InitAxiDma();
    InitInterruptSystem();

    // Array poits to the ddr content it is not creating a 8MByte array.
    u8 *RxBuffer = (u8 *)DDR_BASE_ADDR;
    Xil_DCacheFlushRange((UINTPTR)RxBuffer, data_size);

    S2MM_Done = 0;
    DMA_Error = 0;

    xil_printf("Starting DMA...\r\n");

    XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)RxBuffer, data_size, XAXIDMA_DEVICE_TO_DMA);

    XGpio_DiscreteWrite(&GpioTrigger, 1, 1);
    xil_printf("GPIO start signal sent\r\n");

    while (!S2MM_Done && !DMA_Error);

    if (DMA_Error) {
        xil_printf("DMA FAILED\r\n");
        return -1;
    }

    xil_printf("DMA S2MM DONE\r\n");

    Xil_DCacheInvalidateRange((UINTPTR)RxBuffer, data_size);

    // I have changed one data in ram to see if checker works and it does.
    /* 
    volatile u32 *ptr32 = (u32 *)DDR_BASE_ADDR;
    *ptr32 = 0xDEADBEEF;

    // Make sure it really reaches DDR (not stuck in cache)
    Xil_DCacheFlushRange((UINTPTR)ptr32, 4);
    */

    VerifyDdrContent((u64 *)DDR_BASE_ADDR, burst_words);

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
            
    print_ddr_content((u32*)DDR_BASE_ADDR);

    while (1);
}
