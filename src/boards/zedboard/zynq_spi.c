/* This file contains functions for handling SPI 1 Controller on Zedboard */


#include <zynq_spi.h>
#include <io.h>
#include <board.h>

/* buffer for storing incoming data from RX FIFO buffer in SPI controller */
uint8_t RX_BUFFER[RX_BUFF_SIZE*2];
uint8_t RX_BUFFER_HEAD, RX_BUFFER_TAIL;


/* little function for delay */
static inline void wait()
{
    int32_t count = 100000;
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
    : "=r"(count): [count]"0"(count) : "cc");
}

//==============================================================================
/* Function for reset of the SPI controller
 * The controller has two reset domains: the APB interface and the controller itself.
 * The reset for the two domain must be used together.
 * It's recommended to restart controller before using it.
 * Do not call this function while SPI controller is in use.
 */
void SPI_1_Reset(void){

      write32( (void *)SLCR_UNLOCK, SLCR_UNLOCK_KEY);

      volatile uint32_t *rst_register = (volatile uint32_t *)SPI_RST_CTRL;

      // reset controller by writing one's to certain bits
      *rst_register |= SPI1_REF_RST_YES | SPI1_CPU1X_RST_YES;

      // do mini delay
      wait();

      // write no reset
      *rst_register = SPI1_REF_RST_NO | SPI1_CPU1X_RST_NO;

      write32( (void *)SLCR_LOCK, SLCR_LOCK_KEY);


      return;
}


//==============================================================================
/* Function enables clock for SPI controller */
void SPI_1_ClockEnable(void){

      write32( (void *)SLCR_UNLOCK, SLCR_UNLOCK_KEY);

      volatile uint32_t *spi_clk_reg = (volatile uint32_t *) SPI_CLK_CTRL;
      volatile uint32_t *aper = (volatile uint32_t *) APER_CLK_CTRL;

      // enable AMBA peripheral clock for SPI1
      *aper |= (uint32_t)SPI1_CPU_1XCLKACT;

      // reset register to default value
      *spi_clk_reg = (uint32_t)SPI_CLK_RESET_VAL;

      // select PLL source I/O PLL
      *spi_clk_reg = (uint32_t)SPI_SRCSEL_IO;

      // divide I/O PLL clock by 10
      *spi_clk_reg |= (uint32_t)SPI_CLK_DIV_40;

      // enable the SPI 1 reference clock
      *spi_clk_reg |= (uint32_t)SPI_CLKACT1_EN;

      write32( (void *)SLCR_LOCK, SLCR_LOCK_KEY);


      return;
}


//==============================================================================
/* Function for SPI1 signal routing
 * Signals from SPI Controller can be routed to many different pins. There are
 * two main pin groups - the MIO and the EMIO pins. This function routes the
 * SPI1 signals to the MIO interface. Signals are routed to the MIO10-15 so that
 * user can communicate with exernal slave device via JE1 connector on the board.
 *
 * MIO 10 => JE2 (MOSI)
 * MIO 11 => JE3 (MISO)
 * MIO 12 => JE4 (CLK)
 * MIO 13 => JE1 (SS0)
 */
void SPI_1_SignalRoute(void){

      write32( (void *)SLCR_UNLOCK, SLCR_UNLOCK_KEY);

      volatile uint32_t *mosi = (volatile uint32_t *) MIO_10;
      volatile uint32_t *miso = (volatile uint32_t *) MIO_11;
      volatile uint32_t *clk = (volatile uint32_t *) MIO_12;
      volatile uint32_t *ss0 = (volatile uint32_t *) MIO_13;
      volatile uint32_t *ss1 = (volatile uint32_t *) MIO_14;
      volatile uint32_t *ss2 = (volatile uint32_t *) MIO_15;

      /* MOSI signal routing */
      *mosi = (uint32_t)MIO_RESET_VAL;
      *mosi = (uint32_t)(MIO10_L3_SEL_SPI1 | MIO_TRI_DIS  | MIO_IO_TYPE_CMOS33);
      *mosi |= (uint32_t)(MIO_SPEED_SLOW | MIO_PULLUP_DIS | MIO_HSTL_REC_DIS);

      /* MISO signal routing */
      *miso = (uint32_t)MIO_RESET_VAL;
      *miso = (uint32_t)(MIO11_L3_SEL_SPI1 | MIO_TRI_EN | MIO_IO_TYPE_CMOS33);
      *miso |= (uint32_t)(MIO_SPEED_SLOW | MIO_PULLUP_DIS | MIO_HSTL_REC_DIS);

      /* SPI CLK signal routing */
      *clk = (uint32_t)(MIO_RESET_VAL);
      *clk = (uint32_t) (MIO12_L3_SEL_SPI1 | MIO_TRI_DIS  | MIO_IO_TYPE_CMOS33);
      *clk |= (uint32_t)(MIO_SPEED_SLOW | MIO_PULLUP_DIS | MIO_HSTL_REC_DIS);

      /* Slave select 0 signal routing */
      *ss0 = (uint32_t)MIO_RESET_VAL;
      *ss0 = (uint32_t)(MIO13_L3_SEL_SPI1 | MIO_TRI_DIS | MIO_IO_TYPE_CMOS33);
      *ss0 |= (uint32_t)(MIO_SPEED_SLOW | MIO_PULLUP_EN | MIO_HSTL_REC_DIS);

      write32( (void *)SLCR_LOCK, SLCR_LOCK_KEY);


      return;

}


//==============================================================================
/* Function for SPI1 controller configuration
 * Configures SPI1 in master mode
 */
void SPI_1_Config(void){

      // SPI_Zynq * SPI_InitStruct = Ptr_SPI[1];
      SPI_Zynq * SPI_InitStruct = (SPI_Zynq *) SPI1_BASE_ADDR;

      // first reset control register
      SPI_InitStruct->cr_register = (uint32_t)CR_RESET;

      // de-assert all chip selects (for now)
      SPI_InitStruct->cr_register = (uint32_t)CR_CS_NS;

      // no external 3-to-8 chip select decoder
      SPI_InitStruct->cr_register |= (uint32_t)CR_PERI_SEL_1TO3;

      // set baudrate to 12.5Mbps (we assume 50MHz SPI ref clk)
      SPI_InitStruct->cr_register |= (uint32_t)CR_BAUD_RATE_DIV_4;

      // set clock phase and polarity
      SPI_InitStruct->cr_register |= (uint32_t)(CR_CLK_POL_HIGH | CR_CLK_PHASE_INACTIVE);

      // configure master mode
      SPI_InitStruct->cr_register |= (uint32_t)CR_MASTER_MODE;

      // look for bus collisions
      SPI_InitStruct->cr_register |= (uint32_t)CR_MODE_FAIL_EN;

      // do not initiate a transmission
      SPI_InitStruct->cr_register |= (uint32_t)CR_MAN_START_AUTO;


      // SPI_InitStruct->dr_register = (uint32_t)DR_AFTER_MASK;

      // manual chip select
      // SPI_InitStruct->cr_register |= (uint32_t)CR_MAN_CS;

      // initiate RX FIFO BUFFER
      RX_BUFFER_HEAD = 0;
      RX_BUFFER_TAIL = 0;


      // set RX and TX FIFO treshold values
      // RX FIFO treshold and TX FIFO treshold set to one byte
      SPI_InitStruct->rx_thres_reg = (uint32_t)RX_THRES_VAL;
      SPI_InitStruct->txwr_register = (uint32_t)TX_THRES_VAL;

      return;
}

//==============================================================================
/* Enable the SPI 1 controller to communicate with slave 0. */
void SPI_1_Enable(void){

      // SPI_Zynq * SPI_Struct = Ptr_SPI[1];
      SPI_Zynq * SPI_Struct = (SPI_Zynq *) SPI1_BASE_ADDR;

      // enable manual SS
      SPI_Struct->cr_register |= (uint32_t)(CR_MAN_CS);

      /* enable the interrupts enable RX FIFO full RX FIFO overflow, TX FIFO
        empty and fault conditions 0x27 */
      SPI_Struct->ien_register =  (uint32_t)IER_RXFULL_EN;

      // enable the controller
      SPI_Struct->er_register = (uint32_t)ER_SPI_ENABLE;

      return;

}

//==============================================================================
/* Disable the SPI 1 Controller - terminate communication with slave. */
void SPI_1_Disable(void){

      // SPI_Zynq * SPI_Struct = Ptr_SPI[1];
      SPI_Zynq * SPI_Struct = (SPI_Zynq *) SPI1_BASE_ADDR;

      // de-assert slave select
      SPI_Struct->cr_register |= (uint32_t)CR_CS_NS;

      // disable the controller
      SPI_Struct->er_register = (uint32_t)ER_SPI_DISABLE;

      // disable interrupts
      SPI_Struct->idis_register = (uint32_t)(IDR_RXOVR_DIS | IDR_MODF_DIS | IDR_TXOVR_DIS |
                                  IDR_RXFULL_DIS | IDR_TXFULL_DIS | IDR_RX_NEMPTY_DIS);

      return;

}
//==============================================================================
/* Interrupt service routine for SPI 1 interrupts.
 * This function reads from the RX FIFO. */
void SPI_1_irq_handler(uint32_t interrupt){

      SPI_Zynq * SPI_Struct = (SPI_Zynq *) SPI1_BASE_ADDR;
      uint32_t irq_status;
      char rx_char, rx_head;
      uint32_t read;

      // printk("u interruptu\n");
      // disable all interrupts except TX FIFO FULL and RX FIFO NOT EMPTY
      SPI_Struct->idis_register = (uint32_t)(IDR_RXOVR_DIS | IDR_MODF_DIS | IDR_RXFULL_DIS);

      // determine the source of the interrupt - read from status register
      irq_status = (SPI_Struct->sr_register);

      /* clear the interrupts : write 1s to the interrupt status register */

      /* clear TX FIFO FULL interrupt */
      if ( irq_status & SR_TX_FULL )  SPI_Struct->sr_register |=  (uint32_t)SR_TX_FULL;
      /* clear RX overflow interrupt */
      if ( irq_status & SR_RXOVR_YES ) SPI_Struct->sr_register |=  (uint32_t)SR_RXOVR_YES;
      /* clear ModeFail interrupt */
      if ( irq_status & SR_MODEF_YES ) SPI_Struct->sr_register |=  (uint32_t)SR_MODEF_YES;
      /* clear TX FIFO underflow interrupt*/
      if ( irq_status & SR_TX_UF ) SPI_Struct->sr_register |=  (uint32_t)SR_TX_UF;

      // RX event
      // empty the RX FIFO (check for RX FIFO full and RX FIFO not empty)
      if((SPI_Struct->sr_register & SR_RX_NEMPTY)){
        uint8_t entries = 0;

        while (entries < RX_THRES_VAL){
          read = SPI_Struct->rxd_register;

          entries++;

          // check for buffer overrun
          rx_head = RX_BUFFER_HEAD + 1;
          if (rx_head == RX_BUFF_SIZE) rx_head = 0;

          if (rx_head != RX_BUFFER_TAIL){
                RX_BUFFER[RX_BUFFER_HEAD] = (uint8_t)read;
                RX_BUFFER_HEAD = rx_head; // update head
          }
        }
      }

      // enable interrupts
      SPI_Struct->ien_register = (uint32_t)(IER_RXOVR_EN | IER_MODF_EN |
                                 IER_RX_NEMPTY_EN | IER_RXFULL_EN);
      return;
}

//==============================================================================
/* Send data in master mode - manual slave select and auto start.
 * Software controls the slave select, but the controller hardware autimatically
 * starts to serialize data when there is data in the TxFIFO. This mode is
 * recommended for general use. */
uint8_t SPI1_SendData(uint8_t data){

      SPI_Zynq * SPI_Struct = (SPI_Zynq *) SPI1_BASE_ADDR;
      uint8_t byte_counter = 0;

      // assert slave select 0
      uint32_t reg = SPI_Struct->cr_register;
      uint32_t cs_mask = 0xFFFFC3FF;
      reg &= cs_mask;

      SPI_Struct->cr_register = (uint32_t)reg | CR_CS_0;

      // send data
      SPI_Struct->txd_register =(uint32_t) data;


      byte_counter++;

      return byte_counter;

}


//==============================================================================
/* Function reads data from RX BUFFER */
uint8_t SPI1_ReadData(uint8_t *data){


    SPI_Zynq * SPI_Struct = (SPI_Zynq *) SPI1_BASE_ADDR;
    uint8_t ret = 0;
    uint8_t rx_char;

    // wait();

    // disable spi interrupt
    SPI_Struct->idis_register = (uint32_t)(IDR_RXOVR_DIS | IDR_MODF_DIS | IDR_TXOVR_DIS |
                                IDR_RX_NEMPTY_DIS | IDR_RXFULL_DIS);

    // fetch data from RX buffer
    if (RX_BUFFER_HEAD != RX_BUFFER_TAIL){

        *data = RX_BUFFER[RX_BUFFER_TAIL];
        RX_BUFFER_TAIL++;
        if (RX_BUFFER_TAIL == RX_BUFF_SIZE) RX_BUFFER_TAIL = 0;
        ret = 1;

    }

    // de-assert all chip selects
    SPI_Struct->cr_register |= (uint32_t)CR_CS_NS;

    // enable interrupt
    SPI_Struct->ien_register = (uint32_t)(IER_RXOVR_EN | IER_MODF_EN |
                               IER_RX_NEMPTY_EN | IER_RXFULL_EN);

    return ret;

}

//==============================================================================
void SPI1_assert_slave(uint8_t slave_nmr){

  SPI_Zynq * SPI_Struct = (SPI_Zynq *) SPI1_BASE_ADDR;

  uint32_t reg = SPI_Struct->cr_register;
  uint32_t cs_mask = 0xFFFFC3FF;
  reg &= cs_mask;

  if (slave_nmr == 0){
      SPI_Struct->cr_register = (uint32_t)reg | CR_CS_0;
  }

  return;
}

void SPI1_deassert_slave(void){

  SPI_Zynq * SPI_Struct = (SPI_Zynq *) SPI1_BASE_ADDR;
  SPI_Struct->cr_register |= (uint32_t)CR_CS_NS;

  return;

}