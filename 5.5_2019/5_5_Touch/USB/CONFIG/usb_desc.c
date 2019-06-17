/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Descriptors for Joystick Mouse Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const uint8_t Joystick_DeviceDescriptor[JOYSTICK_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x10,                       /*bcdUSB */
    0x01,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize 64*/
    0x83,                       /*idVendor (0x0483)*/
    0x04,
    0x10,                       /*idProduct = 0x5710*/
    0x57,
    0x00,                       /*bcdDevice rel. 2.00*/
    0x02,
    1,                          /*Index of string descriptor describing
                                                  manufacturer */
    2,                          /*Index of string descriptor describing
                                                 product*/
    3,                          /*Index of string descriptor describing the
                                                 device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* Joystick_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t Joystick_ConfigDescriptor[JOYSTICK_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    JOYSTICK_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x01,         /*bNumInterfaces: 1 interface*/
    0x01,         /*bConfigurationValue: Configuration value*/
    0x00,         /*iConfiguration: Index of string descriptor describing
                                     the configuration*/
    0xE0,         /*bmAttributes: Self powered */
    0x20,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,         /*bLength: Interface Descriptor size*/
    USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
    0x00,         /*bInterfaceNumber: Number of Interface*/
    0x00,         /*bAlternateSetting: Alternate setting*/
    0x01,         /*bNumEndpoints*/
    0x03,         /*bInterfaceClass: HID*/
    0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0,            /*iInterface: Index of string descriptor*/
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,         /*bLength: HID Descriptor size*/
    HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x10,         /*bcdHID: HID Class Spec release number*/
    0x01,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    JOYSTICK_SIZ_REPORT_DESC,/*wItemLength: Total length of Report descriptor*/
    0x00,
    /******************** Descriptor of Joystick Mouse endpoint ********************/
    /* 27 */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

    0x81,          /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    0x40,          /*wMaxPacketSize: 4 Byte max */
    0x00,
    0x01,          /*bInterval: Polling Interval (1 ms)*/
    /* 34 */
  }
  ; /* MOUSE_ConfigDescriptor */
const uint8_t Joystick_ReportDescriptor[JOYSTICK_SIZ_REPORT_DESC] =
  {
		/*single touch*/
    0x05,          /*Usage Page(Generic Desktop)*/
    0x01,					
    0x09,          /*Usage(Mouse)*/
    0x02,						
    0xA1,          /*Collection(Logical)*/
		0x01,
		0x85,          /*REPORT_ID(Single Touch)*/
		0X01,
    0x09,          /*Usage(Pointer)*/
    0x01,
		/*10*/
    0xA1,          /*Collection(Linked)*/
    0x00,
    0x05,          /*Usage Page(Buttons)*/
    0x09,
    0x19,          /*Usage Minimum(1)*/
    0x01,
    0x29,          /*Usage Maximum(3)*/
    0x03,
    0x15,          /*Logical Minimum(0)*/
    0x00,
		/*20*/
    0x25,          /*Logical Maximum(1)*/
    0x01,
    0x95,          /*Report Count(3)*/
    0x03,
    0x75,          /*Report Size(1)*/
    0x01,
    0x81,          /*Input(Variable)*/
    0x02,
    0x95,          /*Report Count(1)*/
    0x01,
		/*30*/
    0x75,          /*Report Size(5)*/
    0x05,
    0x81,          /*Input(Constant,Array)*/
    0x03,
    0x05,          /*Usage Page(Generic Desktop)*/
    0x01,
    0x09,          /*Usage(X axis)*/
    0x30,
    0x09,          /*Usage(Y axis)*/
    0x31,
		/*40*/
    0x15,          /*Logical Minimum(0)*/
    0x00,
    0x26,          /*Logical Maximum(4095)*/
    0xff,
		0x0f,
		0x35,          /*Physical Minimum(0)*/
		0x00,    
		0x46,          /*Physical Maximum(4096)*/
		0xff,
		0x0f,
		/*50*/
    0x75,          /*Report Size(16)*/
    0x10,
    0x95,          /*Report Count(2)*/
    0x02,
    0x81,          /*Input(Variable)*/
    0x02,
    0xc0,          /*End Collection*/
    0xc0,           /*End Collection*/ 
		
		/*multipoint touch*/
		0x05,         /*USAGE_PAGE(Digitizers)*/
		0x0d,
		/*60*/
		0x09,          /*USAGE(Touch Screen)*/
		0x04,
		0xa1,          /*COLLECTION(Application)*/
		0x01,
		0x85,          /*REPORT_ID(Touch)*/
		0x02,
		0x09,          /*USAGE(Finger)*/
		0x22, 
		
    //第1点		
		0xa1,          /*COLLECTION(Logical)*/
		0x02,
		/*70*/
		0x09,          /*USAGE(Tip Switch)*/
		0x42,
		0x15,          /*LOGICAL_MINIMUM(0)*/
		0x00,
		0x25,          /*LOGICAL_MAXIMUM(1)*/
		0x01,
		0x75,          /*REPORT_SIZE(1)*/
		0x01,
		0x95,          /*REPORT_COUNT(1)*/
		0x01,
		/*80*/
		0x81,          /*INPUT(Data,Var,Abs)*/
		0x02,
		0x09,          /*USAGE(IN Range)*/
		0x32,
		0x81,          /*INPUT(Data,Var,Abs)*/
		0x02,
		0x09,          /*USAGE(Touch Valid)*/
		0x47,
		0x81,          /*INPUT(Data,Var,Abs)*/
		0x02,
		/*90*/
		0x95,          /*REPORT_COUNT(5)*/
		0x05,
		0x81,          /*INPUT(Cnst,Ary,Abs)*/
		0x03,
		0x25,          /*LOGICAL_MAXIMUM(MAX_TOUCH_POINT)*/
		MAX_TOUCH_POINT,
		0x75,          /*REPORT_SIZE(8)*/
		0x08,
		0x09,          /*USAGE(Contact Identifier)*/
		0x51,
		/*100*/
		0x95,          /*REPORT_COUNT(1)*/
		0x01,
		0X81,          /*INPUT(Data,Var,Abs)*/
		0X02,
		0x05,          /*Usage Page(Generic Desktop)*/
		0x01,
		0x26,          /*Logical Maximum(4095)*/
		0xff,
		0x0f,
		0x75,          /*REPORT_SIZE(16)*/
		0x10,
		0x55,          /*UNIT_EXPONENT(0)*/
		0x00,
		0x65,          /*UNIT(None)*/
		0x00,
		0x09,          /*USAGE(X)*/
		0x30,
		0x35,          /*PHYSICAL_MINIMUM(0)*/
		0x00,
		0x46,          /*PHYSICAL_MAXIMUM(0)*/
		0x00,
		0x00,
		0x81,          /*INPUT(Data,Var,Abs)*/
		
		0x02,
		0x09,          /*USAGE(Y)*/
		0x31,
		0x46,          /*PHYSICAL_MAXIMUM(0)*/
		0x00,
		0x00,
		0x81,          /*INPUT(Data,Var,Abs)*/
		
		0x02,
		0xc0,          /*END_COLLECTION*/
		
		
		//第2点
		0xa1,          /*COLLECTION(Logical)*/
		0x02,
		0x05,         /*USAGE_PAGE(Digitizers)*/
		0x0d,
		0x09,          /*USAGE(Tip Switch)*/
		0x42,
		0x15,          /*LOGICAL_MINIMUM(0)*/
		0x00,
		0x25,          /*LOGICAL_MAXIMUM(1)*/
		0x01,
		0x75,          /*REPORT_SIZE(1)*/
		0x01,
		0x95,          /*REPORT_COUNT(1)*/
		0x01,
		0x81,          /*INPUT(Data,Var,Abs)*/
		
		0x02,
		0x09,          /*USAGE(IN Range)*/
		0x32,
		0x81,          /*INPUT(Data,Var,Abs)*/
		
		0x02,
		0x09,          /*USAGE(Touch Valid)*/
		0x47,
		0x81,          /*INPUT(Data,Var,Abs)*/
		
		0x02,
		0x95,          /*REPORT_COUNT(5)*/
		0x05,
		0x81,          /*INPUT(Cnst,Ary,Abs)*/
		
		0x03,
		0x25,          /*LOGICAL_MAXIMUM(MAX_TOUCH_POINT)*/
		MAX_TOUCH_POINT,
		0x75,          /*REPORT_SIZE(8)*/
		0x08,
		0x09,          /*USAGE(Contact Identifier)*/
		0x51,
		0x95,          /*REPORT_COUNT(1)*/
		0x01,
		0X81,          /*INPUT(Data,Var,Abs)*/
		0X02,
		
		0x05,          /*Usage Page(Generic Desktop)*/
		0x01,
		0x26,          /*Logical Maximum(4096)*/
		0xff,
		0x0f,
		0x75,          /*REPORT_SIZE(16)*/
		0x10,
		0x55,          /*UNIT_EXPONENT(0)*/
		0x00,
		0x65,          /*UNIT(None)*/
		0x00,
		0x09,          /*USAGE(X)*/
		0x30,
		0x35,          /*PHYSICAL_MINIMUM(0)*/
		0x00,
		0x46,          /*PHYSICAL_MAXIMUM(0)*/
		0x00,
		0x00,
		0x81,          /*INPUT(Data,Var,Abs)*/
		
		0x02,
		0x09,          /*USAGE(Y)*/
		0x31,
		0x46,          /*PHYSICAL_MAXIMUM(0)*/
		0x00,
		0x00,
		0x81,          /*INPUT(Data,Var,Abs)*/
		
		0x02,
		0xc0,          /*END_COLLECTION*/
		
		/*************************/
		0x05,          /*USAGE_PAGE(Digitizers)*/
		0x0d,
		0x09,          /*USAGE(Contact count)*/
		0x54,
		0x75,          /*REPORT_SIZE(8)*/
		0x08,
		0x95,          /*REPORT_COUNT(1)*/
		0x01,
		0x15,          /*LOGICAL_MINIMUM(0)*/
		0x00,
		0x25,          /*LOGICAL_MAXIMUM(MAX_TOUCH_POINT)*/
		MAX_TOUCH_POINT,
		0x81,          /*INPUT(Data,Var,Abs)*/
		0x02,
		
		0x09,          /*USAGE(Contact count maximum)*/
		0x55,
		0xb1,          /*FEATURE(Data,Var,Abs)*/
		0x02,
		0xc0,          /*END_COLLECTION*/
		
		
		/*特性报告描述符*/
		0x05,          /*USAGE_PAGE(Digitizers)*/
		0x0d,
		0x09,          /*USAGE(Device Configuration)*/
		0x0e,
		0xa1,          /*COLLECTION(Application)*/
		0x01,
		0x85,          /*REPORT_ID(3)*/
		0x03,
		0x09,          /*USAGE(Device Settings)*/
		0x23,
		0xa1,          /*COLLECTION(Logical)*/
		0x02,
		0x09,          /*USAGE(Device Mode)*/
		0x52,
		0x09,          /*USAGE(Device Identifier)*/
		0x53,
		0x15,          /*LOGICAL_MINIMUM(0)*/
		0x00,
		0x25,          /*LOGICAL_MAXIMUM(10)*/
		0X0a,
		0x75,          /*REPORT_SIZE(8)*/
		0x08,
		0x95,          /*REPORT_COUNT(2)*/
		0x02,
		0xb1,          /*FEATURE(Data,Var,Abs)*/
		0x02,
		
		0xc0,          /*END_COLLECTION*/
		0xc0           /*END_COLLECTION*/
		/*245*/
  }
  ; /* Joystick_ReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t Joystick_StringLangID[JOYSTICK_SIZ_STRING_LANGID] =
  {
    JOYSTICK_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

const uint8_t Joystick_StringVendor[JOYSTICK_SIZ_STRING_VENDOR] =
  {
    JOYSTICK_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'D', 0, 'F', 0, 'R', 0, 'o', 0, 'b', 0, 'o', 0, 't', 0
  };

const uint8_t Joystick_StringProduct[JOYSTICK_SIZ_STRING_PRODUCT] =
{
	JOYSTICK_SIZ_STRING_PRODUCT,          /* bLength */
	USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
	'U', 0, 'S', 0, 'B', 0, ' ', 0, 'M', 0, 'u', 0, 'l', 0, 't', 0,'i', 0,//ALIENTEK
	 ' ', 0, 'T', 0, 'o', 0, 'u', 0, 'c', 0, 'h', 0
};

uint8_t Joystick_StringSerial[JOYSTICK_SIZ_STRING_SERIAL] =
  {
    JOYSTICK_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

