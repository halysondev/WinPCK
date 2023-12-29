
#include "base64.h"

const unsigned char BASE64_PADDING='=';  //When the input data is less than a multiple of 3, the '=' sign is filled after the output character.
#if BASE64_ZUP
const unsigned char BASE64_CODE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
#else
const unsigned char BASE64_CODE[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#endif

//Encoding function (original data address, original data byte size, encoding output address)
BEXTERN void BEXPORT base64_encode01(const void* pdata, const uint32_t data_size, void* out_pcode)
{
	const uint8_t* input = (const uint8_t*)pdata;
	const uint8_t* input_end = &input[data_size];
	uint8_t*       output = (uint8_t*)out_pcode;

	for (; input + 2 < input_end; input += 3, output += 4)
	{
		output[0] = BASE64_CODE[input[0] >> 2];
		output[1] = BASE64_CODE[((input[0] << 4) | (input[1] >> 4)) & 0x3F];
		output[2] = BASE64_CODE[((input[1] << 2) | (input[2] >> 6)) & 0x3F];
		output[3] = BASE64_CODE[input[2] & 0x3F];
	}

	uint32_t bord_width = input_end - input;
	if (bord_width == 1)
	{
		output[0] = BASE64_CODE[input[0] >> 2];
		output[1] = BASE64_CODE[(input[0] << 4) & 0x3F];
		output[2] = BASE64_PADDING;
		output[3] = BASE64_PADDING;
	}
	else if (bord_width == 2)
	{
		output[0] = BASE64_CODE[input[0] >> 2];
		output[1] = BASE64_CODE[((input[0] << 4) | (input[1] >> 4)) & 0x3F];
		output[2] = BASE64_CODE[(input[1] << 2) & 0x3F];
		output[3] = BASE64_PADDING;
	}
	output[4] = 0;
}
