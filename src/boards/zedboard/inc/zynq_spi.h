#ifndef _ZYNQ_SPI_H_
#define _ZYNQ_SPI_H_

#include "types.h"

#define SPI_1_INTERRUPT       (81)

#define SPI_NUM               2

/* RX FIFO has capacity of 128 bytes */
#define RX_BUFF_SIZE          128

//================================================================================
/* AMBA Peripheral Clock Control Register */
#define APER_CLK_CTRL         0xF800012C

#define SPI1_CPU_1XCLKACT     (1 << 15)   // enable clock for SPI 1
//=================================================================================
/* SPI Software Reset Control Register Base Address */
#define SPI_RST_CTRL          0xF800021C
#define SPI_RST_CTRL_RESET    0x00000000

/* SPI_REF - reference software reset:
 * On assertion of this reset, the reference clock portion of
 * the SPIx subsystem will be reset. */

/* SPI_CPU1X - AMBA software reset:
 * On assertion of this reset, the AMBA clock portion of
 * the SPIx subsystem will be reset */

#define SPI1_REF_RST_NO       (0 << 3)    // no reset
#define SPI1_REF_RST_YES      (1 << 3)    // reset
#define SPI1_CPU1X_RST_NO     (0 << 1)    // no reset
#define SPI1_CPU1X_RST_YES    (1 << 1)    // reset

#define SPI0_REF_RST_NO       (0 << 2)    // no reset
#define SPI0_REF_RST_YES      (1 << 2)    // reset
#define SPI0_CPU1X_RST_NO     0x0         // no reset
#define SPI0_CPU1X_RST_YES    0x1         // reset

//=================================================================================
//=================================================================================
/* SPI Clock Control Register */
#define SPI_CLK_CTRL          0xF8000158
#define SPI_CLK_RESET_VAL     0x00003F03

#define SPI_CLKACT0_EN        0x1         // SPI 0 reference clock enabled
#define SPI_CLKACT0_DIS       0x0         // SPI 0 reference clock disabled
#define SPI_CLKACT1_EN        (1 << 1)    // SPI 1 reference clock enabled
#define SPI_CLKACT1_DIS       (0 << 1)    // SPI 1 reference clock disabled

/* Select the source used to generate the clock */
#define SPI_SRCSEL_IO         (00 << 4)   // I/O PLL
#define SPI_SRCSEL_ARM        (10 << 4)   // ARM PLL
#define SPI_SRCSEL_DDR        (11 << 4)   // DDR PLL

/* Provide the divisor used to devide the source clock to
   generate the required generated clock frequency. */
#define SPI_CLK_DIV_10        (0x0A << 8)  // divisor = 10
#define SPI_CLK_DIV_20        (0x14 << 8)  // divisor = 20
#define SPI_CLK_DIV_40        (0x28 << 8)  // divisor = 40


//=================================================================================
//=================================================================================
/* MIO Registers addresses - for SPI singal routing
 * For SPI 1 MIO pins listed below are used
 * These are apsolute addresses */

#define MIO_10      0xF8000728
#define MIO_11      0xF800072C
#define MIO_12      0xF8000730
#define MIO_13      0xF8000734
#define MIO_14      0xF8000738
#define MIO_15      0xF800073C

#define MIO_RESET_VAL 0x00001601

/* ------ MIO 10 register (same applies to the MIO 11/12/13/14/15 - in case of using SPI1) -------- */
/* Tri-state enable, active high */
#define MIO_TRI_DIS        (0x0)       // disable
#define MIO_TRI_EN         (0x1)       // enable

/* L3_SEL is specific to every MIO pin*/
#define MIO10_L3_SEL_SPI1   (101 << 5)  // select SPI 1 MOSI signal on MIO 10

/* Select IO Buffer edge rate, applicable when IO_Type is LVCMOS18/25/33 */
#define MIO_SPEED_SLOW      (0 << 8)    // slow CMOS edge
#define MIO_SPEED_FAST      (1 << 8)    // fast CMOS edge

/* Select the IO Buffer type */
#define MIO_IO_TYPE_CMOS18  (001 << 9)  // LVCMOS18
#define MIO_IO_TYPE_CMOS25  (010 << 9)  // LVCMOS25
#define MIO_IO_TYPE_CMOS33  (011 << 9)  // LVCMOS33
#define MIO_IO_TYPE_HSTL    (100 << 9)  // HSTL

/* Pull up resistor on IO Buffer pin */
#define MIO_PULLUP_EN       (1 << 12)   // enable pull up
#define MIO_PULLUP_DIS      (0 << 12)   // disable pull up

/* Use HSTL input buffer to save power when it is an output-only (IO_Type must be HSTL)*/
#define MIO_HSTL_REV_EN     (0 << 13)   // enable
#define MIO_HSTL_REC_DIS    (1 << 13)   // disable

/* L3_SEL for MIO 11/12/13/14/15 pins */

#define MIO11_L3_SEL_SPI1    (101 << 5)  // select SPI 1 MISO signal on MIO 11
#define MIO12_L3_SEL_SPI1    (101 << 5)  // select SPI 1 CLK signal on MIO 12
#define MIO13_L3_SEL_SPI1    (101 << 5)  // select SPI 1 SS0 signal on MIO 13
#define MIO14_L3_SEL_SPI1    (101 << 5)  // select SPI 1 SS1 signal on MIO 14
#define MIO15_L3_SEL_SPI1    (101 << 5)  // select SPI 1 SS2 signal on MIO 15

//=================================================================================
//=================================================================================
/* There are two independent SPI interface controllers - SPI0 and SPI1 */

#define NUM_SPI         2
/* Base addresses of each controller */
#define SPI0_BASE_ADDR  0xE0006000
#define SPI1_BASE_ADDR  0xE0007000

/* Each of the controllers has registers listed below
 * these are realtive addresses */

#define CR_OFFSET       0x00000000  // SPI configuration
#define SR_OFFSET       0x00000004  // SPI interrupt status
#define IER_OFFSET      0x00000008  // Interrupt enable
#define IDR_OFFSET      0x0000000C  // Interupt disable
#define IMR_OFFSET      0x00000010  // Interrupt mask
#define ER_OFFSET       0x00000014  // SPI controller enable
#define DR_OFFSET       0x00000018  // Delay control
#define TXD_OFFSET      0x0000001C  // Transmit data
#define RXD_OFFSET      0x00000020  // Receive data
#define SICR_OFFSET     0x00000024  // Slave idle count
#define TXWR_OFFSET     0x00000028  // TX_FIFO treshold
#define RX_THRES        0x0000002C  // RX_FIFO treshold
#define MOD_ID          0x000000FC  // Module ID

//===================================================================================
//===================================================================================
/* SPI Configuration register */

#define CR_RESET                    0x00020000  // reset value
#define CR_MASTER_MODE              0x1         //operate in master mode
#define CR_SLAVE_MODE               0x0         // operate in slave mode

#define CR_CLK_POL_LOW              (0 << 1)    // SPI clock is quiescent high
#define CR_CLK_POL_HIGH             (1 << 1)    // SPI clock is quiescent low

#define CR_CLK_PHASE_ACTIVE         (0 << 2)    // SPI clock is active outside the world
#define CR_CLK_PHASE_INACTIVE       (1 << 2)    // SPI clock is inactive outside the world

/* Master mode baud rate divisor controls the amount of the
 * spi_clf is divided inside the SPI block */
#define CR_BAUD_RATE_DIV_0          (000 << 3)  // division not supported
#define CR_BAUD_RATE_DIV_4          (001 << 3)  // divide by 4
#define CR_BAUD_RATE_DIV_8          (010 << 3)  // divide by 8
#define CR_BAUD_RATE_DIV_16         (011 << 3)  // divide by 16
#define CR_BAUD_RATE_DIV_32         (100 << 3)  // divide by 32
#define CR_BAUD_RATE_DIV_64         (101 << 3)  // divide by 64
#define CR_BAUD_RATE_DIV_128        (110 << 3)  // divide by 128
#define CR_BAUD_RATE_DIV_256        (111 << 3)  // divide by 256

#define CR_REF_CLK_NS               (1 << 8)    // master ref clock not supported
#define CR_REF_CLK_SPI              (0 << 8)    // use SPI reference clock

/* Peripheral select decode */
#define CR_PERI_SEL_1TO3            (0 << 9)    // only 1 to 3 selects
#define CR_PERI_SEL_3TO8            (1 << 9)    // allow external 3 to 8 decode

/* Peripheral chip select lines */
#define CR_CS_0                     (0000 << 10)  // slave 0 selected
#define CR_CS_1                     (0001 << 10)  // slave 1 selected
#define CR_CS_2                     (0011 << 10)  // slave 2 selected
#define CR_CS_R                     (0111 << 10)  // reserved
#define CR_CS_NS                    (1111 << 10)  // no slave selected

#define CR_MAN_CS                   (1 << 14)     // manual CS
#define CR_MAN_AUTO                 (0 << 14)     // auto CS

#define CR_MAN_START_EN             (1 << 15)     // manual start enbale
#define CR_MAN_START_AUTO           (0 << 15)     // auto mode

#define CR_MODE_FAIL_EN             (1 << 17)     // ModeFail generation enable
#define CR_MODE_FAIL_DIS            (0 << 17)     // ModeFail generation disable

//===================================================================================
//===================================================================================
/* SPI Interrupt Status Register */
/* This register is set when the described event occurs and the interrupt is enabled
 * in the mask register. When any of these bits are set the interrupt output is
 * asserted high, but may be masked by Intrpt_Mask_reg0 before generating the
 * IRQ interrupt. */

#define SR_RXOVR_YES        1          // RX overflow occured -> write 1 to clear
#define SR_RXOVR_NO         0          // no RX overflov occured

#define SR_MODEF_YES        (1 << 1)   // ModeFail interrupt occured -> write 1 to clear
#define SR_MODEF_NO         (0 << 1)   // no ModeFail interrupt

#define SR_TX_OVRFLW_NO     (0 << 2)   // TX FIFO has less than TRESHOLD entries
#define SR_TX_OVRFLW_YES    (1 << 2)   // TX FIFO has more than or equal to TRESHOLD entries

#define SR_TX_FULL          (1 << 3)   // TX FIFO is full
#define SR_TX_NFULL         (0 << 3)   // TX FIFO is not full

#define SR_RX_NEMPTY        (1 << 4)   // RX FIFO has more than or equal to TRESHOLD entries
#define SR_RX_EMPTY         (0 << 4)   // RX FIFO has less than RX TRESHOLD entries

#define SR_RX_FULL          (1 << 5)   // RX FIFO is full
#define SR_RX_NFULL         (0 << 5)   // RX FIFO is not full

#define SR_TX_UF            (1 << 6)   // TX FIFO underflow detected -> write 1 to clar
#define SR_TX_NUF           (0 << 6)   // no TX FIFO underflow

//===================================================================================
//===================================================================================
/* SPI Interrupt Enable Register */
/* Writing zeros to these bits make no effect */

#define IER_RESET_VAL       0x00000000

#define IER_RXOVR_EN        1             // RX overflow interrupt enable
#define IER_MODF_EN         (1 << 1)      // ModeFail interrupt enable
#define IER_TXOVR_EN        (1 << 2)      // TX FIFO not full interrupt enable
#define IER_TXFULL_EN       (1 << 3)      // TX FIFO full interrupt enable
#define IER_RX_NEMPTY_EN    (1 << 4)      // RX FIFO not empty interrupt enable
#define IER_RXFULL_EN       (1 << 5)      // RX FIFO full interrupt enable
#define IER_TXUF_EN         (1 << 6)      // TX FIFO underflow interrupt enable

//===================================================================================
//===================================================================================
/* SPI Inerrupt Disable Register */
/* Writing zeros to these bits make no effect */

#define IDR_RESET_VAL       0x00000000

#define IDR_RXOVR_DIS        1             // RX overflow interrupt disable
#define IDR_MODF_DIS         (1 << 1)      // ModeFail interrupt disable
#define IDR_TXOVR_DIS        (1 << 2)      // TX FIFO not full interrupt disable
#define IDR_TXFULL_DIS       (1 << 3)      // TX FIFO full interrupt disable
#define IDR_RX_NEMPTY_DIS    (1 << 4)      // RX FIFO not empty interrupt disable
#define IDR_RXFULL_DIS       (1 << 5)      // RX FIFO full interrupt disable
#define IDR_TXUF_DIS         (1 << 6)      // TX FIFO underflow interrupt disable

//===================================================================================
//===================================================================================
/* SPI Inerrupt Mask Register */

#define IMR_RESET_VAL       0x00000000

#define IMR_RXOVR_DIS        1             // RX overflow interrupt disable
#define IMR_RXOVR_EN         0             // RX overflow interrupt enable

#define IMR_MODF_DIS         (1 << 1)      // ModeFail interrupt disable
#define IMR_MODF_EN          (0 << 1)      // ModeFail interrupt enable

#define IMR_TXOVR_DIS        (1 << 2)      // TX FIFO not full interrupt disable
#define IMR_TXOVR_EN         (0 << 2)      // TX FIFO not full interrupt enable

#define IMR_TXFULL_DIS       (1 << 3)      // TX FIFO full interrupt disable
#define IMR_TXFULL_EN        (0 << 3)      // TX FIFO full interrupt enable

#define IMR_RX_NEMPTY_DIS    (1 << 4)      // RX FIFO not empty interrupt disable
#define IMR_RX_NEMPTY_EN     (0 << 4)      // RX FIFO not empty interrupt emable

#define IMR_RXFULL_DIS       (1 << 5)      // RX FIFO full interrupt disable
#define IMR_RXFULL_EN        (0 << 5)      // RX FIFO full interrupt enable

#define IMR_TXUF_DIS         (1 << 6)      // TX FIFO underflow interrupt disable
#define IMR_TXUF_EN          (0 << 6)      // TX FIFO underflow interrupt enable

//===================================================================================
//===================================================================================
/* SPI Controller Enable Register */
/* Bit 0 is only used in this register, other bits are reserved */
#define ER_SPI_ENABLE       1       // enable SPI controller
#define ER_SPI_DISABLE      0       // disable SPI cnotrolerr

//===================================================================================
//===================================================================================
/*  SPI Delay Control Register */

/* delay in SPI REFERENCE CLOCK between last bit of
 * current word and the first bit of the next word */
#define DR_AFTER_MASK       (0b11 << 8)

/* delay in SPI REFERENCE CLOCK for the length that the master mode chip
 * select outputs are de-asserted between words when cpha = 0 */
#define DR_D_NSS            (0x3 << 24)
//===================================================================================
//===================================================================================
// define master/slave mode
#define SPI_MASTER_MODE     1
#define SPI_SLAVE_MODE      0

//==================================================================================
//==================================================================================
/* Treshold values for RX and TX FIFO buffers in the controller */
#define RX_THRES_VAL        0x06
#define TX_THRES_VAL        0x01


/* Zynq SPI register structure - all registers are 32 bit wide */
typedef struct{

      /* SPI Configuration Register */
      volatile uint32_t cr_register;
      /* SPI Interrupt Status register */
      volatile uint32_t sr_register;
      /* Interupt enable register */
      volatile uint32_t ien_register;
      /* Interrupt disable register */
      volatile uint32_t idis_register;
      /* Interrupt Mask Register */
      volatile uint32_t imask_register;
      /* SPI Controller Enable Register */
      volatile uint32_t er_register;
      /* Delay Control Register */
      volatile uint32_t dr_register;
      /* Write data to TX_FIFO, valid data bits are [7:0] */
      volatile uint32_t txd_register;
      /* Read data from RX_FIFO, valid data bits are [7:0] */
      volatile uint32_t rxd_register;
      /* Slave Idle Count Register */
      volatile uint32_t sicr_register;
      /* Defines the level at which the TX FIFO not full
         interrupt is generated. (TX FIFO TRESHOLD) */
      volatile uint32_t txwr_register;
      /* Defines the level at which the RX FIFO not empty
         interrupt is generated. (RX FIFO TRESHOLD)*/
      volatile uint32_t rx_thres_reg;
} SPI_Zynq;


//================================================================================
//================================================================================
/* Function declarations */
void SPI_1_Reset(void);

void SPI_1_ClockEnable(void);

void SPI_1_SignalRoute(void);

void SPI_1_Config(void);

void SPI_1_Enable(void);

void SPI_1_Disable(void);

void SPI_1_irq_handler(uint32_t interrupt);

uint8_t SPI1_ReadData(uint8_t *data);

uint8_t SPI1_SendData(uint8_t data);

void SPI1_assert_slave(uint8_t slave_nmr);

void SPI1_deassert_slave(void);



#endif