/*
 * usbc.c
 *
 *  Created on: Nov 7, 2022
 *      Author: Nelson
 */




/* Includes ------------------------------------------------------------------*/
#include "usbc.h"

uint8_t buffer_out[65];
uint8_t buffer_in[65];


static uint8_t  USBD_CUSTOM_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);

static uint8_t  USBD_CUSTOM_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

static uint8_t  *USBD_GetFSCfgDesc(uint16_t *length);

static uint8_t  USBD_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
/**
  * @}
  */

/** @defgroup USBD_HID_Private_Variables
  * @{
  */

USBD_ClassTypeDef  USBD_CUSTOM =
{
  USBD_CUSTOM_Init,
  USBD_CUSTOM_DeInit,
  NULL,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  USBD_DataIn, /*DataIn*/
  USBD_DataOut, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,
  NULL,
  USBD_GetFSCfgDesc,
  NULL,
  NULL,
};

/* USB CUSTOM device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CfgFSDesc[32]  __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  0x20,
  0x00,         /* wTotalLength: Bytes returned */
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /************** Interface Descriptor ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x00,         /*bInterfaceClass: Defined by the vendor*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Endpoint IN 1 Descriptor ********************/
  /* 07 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
  CUSTOM_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  USBD_EP_TYPE_BULK,          /*bmAttributes: Bulk endpoint*/
  CUSTOM_EPIN_SIZE, /*wMaxPacketSize: 64 Byte max */
  0x00,
  32,          /*bInterval: Polling Interval */
  /******************** Endpoint OUT 1 Descriptor ********************/
  /* 07 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
  CUSTOM_EPOUT_ADDR,     /*bEndpointAddress: Endpoint Address (OUT)*/
  USBD_EP_TYPE_BULK,          /*bmAttributes: Bulk endpoint*/
  CUSTOM_EPOUT_SIZE, /*wMaxPacketSize: 64 Byte max */
  0x00,
  32,          /*bInterval: Polling Interval */
};
/**
  * @}
  */

/** @defgroup USBD_HID_Private_Functions
  * @{
  */

/**
  * @brief  USBD_HID_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  /* Open EP IN */
  USBD_LL_OpenEP(pdev, CUSTOM_EPIN_ADDR, USBD_EP_TYPE_BULK, CUSTOM_EPIN_SIZE);
  pdev->ep_in[CUSTOM_EPIN_ADDR & 0xFU].is_used = 1U;
  /* Open EP OUT */
  USBD_LL_OpenEP(pdev, CUSTOM_EPOUT_ADDR, USBD_EP_TYPE_BULK, CUSTOM_EPOUT_SIZE);
  pdev->ep_out[CUSTOM_EPOUT_ADDR & 0xFU].is_used = 1U;

  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*)pdev->pData;
  PCD_EPTypeDef *ep = &(hpcd->OUT_ep[CUSTOM_EPOUT_ADDR & 0xFU]);
  ep->xfer_buff = buffer_out;

  pdev->pClassData = USBD_malloc(sizeof(USBD_CUSTOM_HandleTypeDef));

  if (pdev->pClassData == NULL)
  {
    return USBD_FAIL;
  }

  ((USBD_CUSTOM_HandleTypeDef *)pdev->pClassData)->state = CUSTOM_IDLE;

  return USBD_OK;
}

/**
  * @brief  USBD_HID_Init
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx)
{
  /* Close HID EPs */
  USBD_LL_CloseEP(pdev, CUSTOM_EPIN_ADDR);
  pdev->ep_in[CUSTOM_EPIN_ADDR & 0xFU].is_used = 0U;

  /* FRee allocated memory */
  if (pdev->pClassData != NULL)
  {
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return USBD_OK;
}
/**
  * @brief  USBD_HID_GetPollingInterval
  *         return polling interval from endpoint descriptor
  * @param  pdev: device instance
  * @retval polling interval
  */
uint32_t USBD_GetPollingInterval(USBD_HandleTypeDef *pdev)
{
  uint32_t polling_interval = 0U;

  /* HIGH-speed endpoints */
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Sets the data transfer polling interval for high speed transfers.
     Values between 1..16 are allowed. Values correspond to interval
     of 2 ^ (bInterval-1). This option (8 ms, corresponds to HID_HS_BINTERVAL */
    polling_interval = (((1U << (CUSTOM_FS_BINTERVAL - 1U))) / 8U);
  }
  else   /* LOW and FULL-speed endpoints */
  {
    /* Sets the data transfer polling interval for low and full
    speed transfers */
    polling_interval =  CUSTOM_FS_BINTERVAL;
  }

  return ((uint32_t)(polling_interval));
}

/**
  * @brief  USBD_HID_GetCfgFSDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_GetFSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_CfgFSDesc);
  return USBD_CfgFSDesc;
}


/**
  * @brief  USBD_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_DataIn(USBD_HandleTypeDef *pdev,
                                uint8_t epnum)
{

  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_CUSTOM_HandleTypeDef *)pdev->pClassData)->state = CUSTOM_IDLE;
  return USBD_OK;
}

/**
  * @brief  USBD_HID_DataOut
  *         handle data Out Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_DataOut(USBD_HandleTypeDef *pdev,
                                uint8_t epnum)
{
  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_CUSTOM_HandleTypeDef *)pdev->pClassData)->state = CUSTOM_IDLE;
  HAL_PCD_EP_Receive(pdev->pData, CUSTOM_EPOUT_ADDR & 0xFU, &buffer_out[64], 0);
//  HAL_PCD_EP_Transmit(hpcd, ep_addr, pBuf, len)
  return USBD_OK;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



