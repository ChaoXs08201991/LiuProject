

#include "TempManagementPage.h"

#include <QtGui/QMessageBox>

#include "..\\Src\\HardwareInfor.h"
#include "..\\Src\\TemperatureProbe.h"
#include "..\\Src\\PerformanceCounter.h"
#include "..\\Src\\Log\\LLog.h"



/// @brief 温度仓库类
class TempHouse
{
public:
    TempHouse()
    {

    }
    ~TempHouse()
    {

    }

    const CpuTempInfor& GetCpuTemp() const
    {
        return s_cpuTempInfor;
    }

    const GpuTempInfor& GetGpuTemp() const
    {
        return s_gpuTempInfor;
    }

    const DiskTempInforArray& GetDiskTemp() const
    {
        return s_diskTempInforArray;
    }

private:

    void SetCpuTemp(IN const CpuTempInfor& cpuTempInfor)
    {
        s_cpuTempInfor = cpuTempInfor;
    }

    void SetGpuTemp(IN const GpuTempInfor& gpuTempInfor)
    {
        s_gpuTempInfor = gpuTempInfor;
    }

    void SetDiskTemp(IN const DiskTempInforArray& diskTempInforArray)
    {
        s_diskTempInforArray = diskTempInforArray;
    }

private:
    static CpuTempInfor s_cpuTempInfor; ///< CPU温度信息
    static GpuTempInfor s_gpuTempInfor; ///< GPU温度信息
    static DiskTempInforArray s_diskTempInforArray; ///< 磁盘温度信息

    friend class ScanTempThread;
};

CpuTempInfor TempHouse::s_cpuTempInfor = {0};
GpuTempInfor TempHouse::s_gpuTempInfor = {0};
DiskTempInforArray TempHouse::s_diskTempInforArray = {0};

/// @brief 性能仓库类
class PerformanceHouse
{
public:
    PerformanceHouse()
    {

    }

    ~PerformanceHouse()
    {

    }

    const MemoryPerformance& GetMemoryPerformance() const
    {
        return s_memoryPerformance;
    }

    const ProcessorPerformance& GetProcessorPerformance() const
    {
        return s_processorPerformance;
    }

private:

    void SetMemoryPerformance(IN MemoryPerformance& memoryPerf)
    {
        s_memoryPerformance = memoryPerf;
    }

    void SetProcessorPerformance(IN ProcessorPerformance& processorPerf)
    {
        s_processorPerformance = processorPerf;
    }

private:
    static MemoryPerformance s_memoryPerformance; ///< 内存性能
    static ProcessorPerformance s_processorPerformance; ///< 处理器性能

    friend class ScanPerformanceThread;
};

MemoryPerformance PerformanceHouse::s_memoryPerformance = {0};
ProcessorPerformance PerformanceHouse::s_processorPerformance = {0};

ScanTempThread::ScanTempThread()
{
    m_bStopThread = false;
}

ScanTempThread::~ScanTempThread()
{

}

void ScanTempThread::Stop()
{
    m_bStopThread = true;
}

void ScanTempThread::run()
{
    m_bStopThread = false;

    bool bRet = false;
    TemperatureProbe tempProbe;

    CpuTempInfor cpuTempInfor = {0};
    GpuTempInfor gpuTempInfor = {0};
    DiskTempInforArray diskTempInforArray = {0};

    TempHouse tempHouse;

    int refreshCount = -1;
    while (!m_bStopThread)
    {
        refreshCount++;

        bRet = tempProbe.GetCpuTemp(cpuTempInfor);
        if (!bRet)
        {
            PrintLogW(L"Warning: TemperatureProbe::GetCpuTemp Error");
        }
        bRet = tempProbe.GetGpuTemp(gpuTempInfor);
        if (!bRet)
        {
            PrintLogW(L"Warning: TemperatureProbe::GetGpuTemp Error");
        }
        tempProbe.GetDiskTemp(diskTempInforArray);
       
        tempHouse.SetCpuTemp(cpuTempInfor);
        tempHouse.SetGpuTemp(gpuTempInfor);
        tempHouse.SetDiskTemp(diskTempInforArray);

        this->msleep(500);

        // 每刷新10次写一次LOG
        if (refreshCount%10 != 0)
            continue;

        PrintLogW(L"Cpu Cores(Temperature): %u", cpuTempInfor.CoreNum);
        for (unsigned int i = 0; i < cpuTempInfor.CoreNum; i++)
        {
            PrintLogW(L"Cpu Cores: %u, Temp: %u", i, cpuTempInfor.CoreTemp[i]);
        }
        PrintLogW(L"Gpu Temp: %u", gpuTempInfor.Temp[0]);
        PrintLogW(L"Disk Count(Temperature): %u", diskTempInforArray.Count);
        for (unsigned int i = 0; i < diskTempInforArray.Count; i++)
        {
            PrintLogW(L"Disk:%s, Temp: %u", diskTempInforArray.DiskDriveID[i].c_str(), diskTempInforArray.Temp[i]);
        }

        PrintLogW(L"");
        
    }


}

ScanPerformanceThread::ScanPerformanceThread()
{
    m_bStopThread = false;
}

ScanPerformanceThread::~ScanPerformanceThread()
{

}

void ScanPerformanceThread::Stop()
{
    m_bStopThread = true;
}

void ScanPerformanceThread::run()
{
    m_bStopThread = false;

    bool bRet = false;
    PerformanceCounter perfCounter;
    MemoryPerformance memoryPerf = {0};
    ProcessorPerformance processorPerf = {0};

    PerformanceHouse perfHouse;

    int refreshCount = -1;
    while (!m_bStopThread)
    {
        refreshCount++;
       
        bRet = perfCounter.GetMemoryPerformance(memoryPerf);
        if (!bRet)
        {
            PrintLogW(L"Warning: PerformanceCounter::GetMemoryPerformance Error");
        }
        perfCounter.GetProcessorPerformance(processorPerf);
        if (!bRet)
        {
            PrintLogW(L"Warning: PerformanceCounter::GetProcessorPerformance Error");
        }

        perfHouse.SetMemoryPerformance(memoryPerf);
        perfHouse.SetProcessorPerformance(processorPerf);
        

        this->msleep(500);

        // 每刷新10次写一次LOG
        if (refreshCount%10 != 0)
            continue;

        PrintLogW(L"Cpu Usage: %uP", processorPerf.LoadPercentage);
        PrintLogW(L"Memory Total Size: %u", memoryPerf.TotalSize);
        PrintLogW(L"Memory Available Size: %u", memoryPerf.AvailableSize);

        PrintLogW(L"");
    }
}

TempManagementPage::TempManagementPage(IN QWidget *parent, IN Qt::WFlags flags)
    : QWidget(parent, flags)
{
    ui.setupUi(this);

    m_pUiRefreshTimer = new QTimer();
    m_pUiRefreshTimer->setInterval(500);
    QObject::connect(m_pUiRefreshTimer, SIGNAL(timeout()), this, SLOT(UiRefreshTimerTimeout()));

    ui.progressBarCpuUsage->setValue(0);
    ui.progressBarMemUsage->setValue(0);
    ui.progressBarCpuTemp->setValue(0);
    ui.progressBarGpuTemp->setValue(0);
    ui.progressBarDiskTemp->setValue(0);

    m_bExternalVideoCardExist = false;
    const VideoCardInforArray& videoCardInfor = HardwareInfor::GetInstance().GetVideoCardInfor();
    for (unsigned int i = 0; i < videoCardInfor.Count; i++)
    {
        if (videoCardInfor.Type[i] == VIDEO_CARD_EXTERNAL)
        {
            m_bExternalVideoCardExist = true;
            break;
        }
    }


}

TempManagementPage::~TempManagementPage()
{
    if (m_pUiRefreshTimer != NULL)
    {
        delete m_pUiRefreshTimer;
        m_pUiRefreshTimer = NULL;
    }
}

void TempManagementPage::showEvent(QShowEvent* e)
{
    m_pUiRefreshTimer->start();
    m_scanPerformanceThred.start();
    m_scanTempThread.start();
}

void TempManagementPage::hideEvent(QHideEvent* e)
{
    m_pUiRefreshTimer->stop();

    m_scanTempThread.Stop();
    m_scanPerformanceThred.Stop();
}

void TempManagementPage::UiRefreshTimerTimeout()
{
    this->RefreshUi();
}

void TempManagementPage::RefreshUi()
{
    TempHouse tempHouse;

    CpuTempInfor cpuTempInfor = tempHouse.GetCpuTemp();
    unsigned int cpuTemp = 0;
    for (unsigned int coreIndex = 0; coreIndex < cpuTempInfor.CoreNum; coreIndex++)
        cpuTemp += cpuTempInfor.CoreTemp[coreIndex];

    if (cpuTempInfor.CoreNum != 0)
        cpuTemp = cpuTemp/cpuTempInfor.CoreNum;

    ui.progressBarCpuTemp->setValue(cpuTemp);

    unsigned int gpuTemp = 0;
    if (m_bExternalVideoCardExist)
    {
        GpuTempInfor gpuTempInfor = tempHouse.GetGpuTemp();
        gpuTemp = gpuTempInfor.Temp[0];
    }
    else
    {
        gpuTemp = cpuTemp;
    }
    
    ui.progressBarGpuTemp->setValue(gpuTemp);

    DiskTempInforArray diskTempInforArray = tempHouse.GetDiskTemp();
    for (unsigned int i = 0; i < diskTempInforArray.Count; i++)
    {
        if (diskTempInforArray.DiskDriveID[i].find(L"0") != wstring::npos)
        {
            ui.progressBarDiskTemp->setValue(diskTempInforArray.Temp[i]);
            break;
        }
    }

    PerformanceHouse perfHouse;
    MemoryPerformance memoryPerf = perfHouse.GetMemoryPerformance();
    ProcessorPerformance processorPerf = perfHouse.GetProcessorPerformance();

    ui.progressBarCpuUsage->setValue(processorPerf.LoadPercentage);
    ui.progressBarMemUsage->setValue(memoryPerf.LoadPercentage);
        
}

