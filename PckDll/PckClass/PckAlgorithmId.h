#pragma once

/*
	PCK version judgment, the code comes from BeySoft¡¯s PckLib
*/
#pragma warning ( disable : 4309 )

class CPckAlgorithmId
{
public:
	CPckAlgorithmId(unsigned __int32 id);
	~CPckAlgorithmId();

	unsigned __int32 GetPckGuardByte0() { return PckGuardByte0; }
	unsigned __int32 GetPckGuardByte1() { return PckGuardByte1; }
	unsigned __int32 GetPckMaskDword() { return PckMaskDword; }
	unsigned __int32 GetPckCheckMask() { return PckCheckMask; }

private:
	unsigned __int32  PckGuardByte0, PckGuardByte1, PckMaskDword, PckCheckMask;

	//void SetAlgorithmId(unsigned __int32 id);
	void SetAlgorithmId(unsigned __int32 id, __int32 CustomPckGuardByte0 = 0, __int32 CustomPckGuardByte1 = 0, __int32 CustomPckMaskDword = 0, __int32 CustomPckCheckMask = 0);

};

