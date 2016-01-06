

#ifndef _LWINRING0_H_
#define _LWINRING0_H_

#include <string>
using std::wstring;

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

namespace LWinRing0
{
    /// @brief ��ʼ��WinRing0
    /// @param[in] filePath DLL�ļ�·��
    /// @return �ɹ�����true, ʧ�ܷ���false
    bool InitializeWinRing0(IN const wstring& filePath);

    /// @brief �ͷ�WinRing0
    void DeinitializeWinRing0();

    /// @brief CPUIDָ��
    /// @param[in] index CPUIDָ������
    /// @param[out] eax
    /// @param[out] ebx
    /// @param[out] ecx
    /// @param[out] edx
    /// @return �ɹ�����1, ʧ�ܷ���0
    int Cpuid(
        IN unsigned long index, 
        OUT unsigned long* eax, 
        OUT unsigned long* ebx, 
        OUT unsigned long* ecx, 
        OUT unsigned long* edx);

    /// @brief 
    /// @param[in] index
    /// @param[out] eax
    /// @param[out] edx
    /// @return �ɹ�����1, ʧ�ܷ���0
    int Rdmsr(IN unsigned long index, OUT unsigned long* eax, OUT unsigned long* edx);
};

#endif