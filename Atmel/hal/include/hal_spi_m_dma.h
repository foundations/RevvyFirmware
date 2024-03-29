/**
 * \file
 *
 * \brief SPI DMA related functionality declaration.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef _HAL_SPI_M_DMA_H_INCLUDED
#define _HAL_SPI_M_DMA_H_INCLUDED

#include <hal_io.h>
#include <hpl_spi_m_dma.h>

/**
 * \addtogroup doc_driver_hal_spi_master_dma
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration of spi_descriptor. */
struct spi_m_dma_descriptor;

/** The callback types */
enum spi_m_dma_cb_type {
	/** Callback type for DMA transfer buffer done */
	SPI_M_DMA_CB_TX_DONE,
	/** Callback type for DMA receive buffer done */
	SPI_M_DMA_CB_RX_DONE,
	/** Callback type for DMA errors */
	SPI_M_DMA_CB_ERROR,
	SPI_M_DMA_CB_N
};

/**
 * \brief SPI Master DMA callback type
 */
typedef void (*spi_m_dma_cb_t)(struct _dma_resource *resource);

/** \brief SPI HAL driver struct for DMA access
 */
struct spi_m_dma_descriptor {
	struct _spi_m_dma_hpl_interface *func;
	/** Pointer to SPI device instance */
	struct _spi_m_dma_dev dev;
	/** I/O read/write */
	struct io_descriptor io;
	/** DMA resource */
	struct _dma_resource *resource;
};

/** \brief Set the SPI HAL instance function pointer for HPL APIs.
 *
 *  Set SPI HAL instance function pointer for HPL APIs.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] func Pointer to the HPL api structure.
 *
 */
void spi_m_dma_set_func_ptr(struct spi_m_dma_descriptor *spi, void *const func);

/** \brief Initialize the SPI HAL instance and hardware for DMA mode
 *
 *  Initialize SPI HAL with dma mode.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] hw Pointer to the hardware base.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_INVALID_DATA Error, initialized.
 */
int32_t spi_m_dma_init(struct spi_m_dma_descriptor *spi, void *const hw);

/** \brief Deinitialize the SPI HAL instance
 *
 *  Abort transfer, disable and reset SPI, de-init software.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
void spi_m_dma_deinit(struct spi_m_dma_descriptor *spi);

/** \brief Enable SPI
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
void spi_m_dma_enable(struct spi_m_dma_descriptor *spi);

/** \brief Disable SPI
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
void spi_m_dma_disable(struct spi_m_dma_descriptor *spi);

/** \brief Set SPI baudrate
 *
 *  Works if SPI is initialized as master.
 *  In the function a sanity check is used to confirm it's called in the correct mode.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] baud_val The target baudrate value
 *                  (See "baudrate calculation" for calculating the value).
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy.
 *
 *  note: This api should be used to write the baudrate register with the given baud_val
 *  paramter, the user has to calculate the baud_val for required baud rate and pass it as
 *  argument(baud_val) to this api
 */
int32_t spi_m_dma_set_baudrate(struct spi_m_dma_descriptor *spi, const uint32_t baud_val);

/** \brief Set SPI mode
 *
 *  Set SPI transfer mode (\ref spi_transfer_mode),
 * which controls clock polarity and clock phase:
 *  - Mode 0: leading edge is rising edge, data sample on leading edge.
 *  - Mode 1: leading edge is rising edge, data sample on trailing edge.
 *  - Mode 2: leading edge is falling edge, data sample on leading edge.
 *  - Mode 3: leading edge is falling edge, data sample on trailing edge.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] mode The mode (\ref spi_transfer_mode).
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy, CS activated.
 */
int32_t spi_m_dma_set_mode(struct spi_m_dma_descriptor *spi, const enum spi_transfer_mode mode);

/** \brief Set SPI transfer data order
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] dord The data order: send LSB/MSB first.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy, CS activated.
 *  \retval ERR_INVALID The data order is not supported.
 */
int32_t spi_m_dma_set_data_order(struct spi_m_dma_descriptor *spi, const enum spi_data_order dord);

/** \brief Perform the SPI data transfer (TX and RX) with the DMA
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] txbuf Pointer to the transfer information.
 *  \param[out] rxbuf Pointer to the receiver information.
 *  \param[in] length SPI transfer data length.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy.
 */
int32_t spi_m_dma_transfer(struct spi_m_dma_descriptor *spi, uint8_t const *txbuf, uint8_t *const rxbuf,
                           const uint16_t length);

/** \brief Register a function as an SPI transfer completion callback
 *
 *  Register a callback function specified by its \c type.
 *  - SPI_CB_COMPLETE: set the function that will be called on the SPI transfer
 *    completion including deactivating the CS.
 *  - SPI_CB_XFER: set the function that will be called on the SPI buffer transfer
 *    completion.
 *  Register a NULL function to not use the callback.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] type Callback type (\ref spi_m_dma_cb_type).
 *  \param[in] func Pointer to callback function.
 */
void spi_m_dma_register_callback(struct spi_m_dma_descriptor *spi, const enum spi_m_dma_cb_type type,
                                 spi_m_dma_cb_t func);

/**
 * \brief Return I/O descriptor for this SPI instance
 *
 * This function will return an I/O instance for this SPI driver instance
 *
 * \param[in] spi An SPI master descriptor, which is used to communicate through
 *                SPI
 * \param[in, out] io A pointer to an I/O descriptor pointer type
 *
 * \retval ERR_NONE
 */
int32_t spi_m_dma_get_io_descriptor(struct spi_m_dma_descriptor *const spi, struct io_descriptor **io);

/** \brief Retrieve the current driver version
 *
 *  \return Current driver version.
 */
uint32_t spi_m_dma_get_version(void);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* ifndef _HAL_SPI_M_DMA_H_INCLUDED */
