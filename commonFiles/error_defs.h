/*********************************************************************
** Confirmation/Error Values
*********************************************************************/

#ifndef _ERROR_DEFS_H
#define _ERROR_DEFS_H

#define CODE_OK						0x00
#define CODE_DATA_ERROR				0x01
#define CODE_ERROR					0xEE
#define CODE_BUS_ERROR_START		0x02
#define CODE_NO_DATA				0x03
#define CODE_CHECKSUM_ERROR_KWP2000	0x04
#define CODE_CHECKSUM_ERROR_USART	0x05
#define CODE_NEGATIVE_RESPONSE		0x06
#define CODE_BUS_ERROR_STOP			0x07

#define CODE_NO_REQUEST_DATA		0x20
#define CODE_UNSUPPORTED_SID		0x21
#define CODE_UNSUPPORTED_PID		0x22
#define CODE_NONEXISTENT_PID		0x23
#define CODE_KEYBYTE_ERROR			0x24

#define CODE_NOT_IMPLEMENTED		0xFF

#endif // !_ERROR_DEFS_H