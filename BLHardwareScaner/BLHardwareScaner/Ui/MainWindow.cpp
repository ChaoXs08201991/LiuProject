
#include "MainWindow.h"

#include <QtGui/QPaintDevice>
#include <QtGui/QPainter>
#include <QtGui/QDesktopWidget>

#include "HardwareInforPage.h"
#include "TempManagementPage.h"
#include "..\\Src\\Log\\LLog.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    // 获取当前系统DPI, 当前系统DPI除以设计时DPI值, 则得到UI放大系数
    const float DESIGN_DPI = 96.0f; // 设计时DPI
    QPainter painter(this);
    QPaintDevice* pDevice = painter.device();
    PrintLogW(L"System DPI X: %d, Y: %d", pDevice->logicalDpiX(), pDevice->logicalDpiY());
    float ratioX = pDevice->logicalDpiX()/DESIGN_DPI;
    float ratioY = pDevice->logicalDpiY()/DESIGN_DPI;
    m_uiRatio = ratioX > ratioY ? ratioX : ratioY;
    if (m_uiRatio < 1.0f)
        m_uiRatio = 1.0f;
    PrintLogW(L"UI Ratio: %f", m_uiRatio);

  
    ui.setupUi(this);

    // 根据比例重新调整主UI大小, 并居中显示
    int width = this->geometry().width() * m_uiRatio;
    int height = this->geometry().height() * m_uiRatio;
    this->setFixedSize(width, height);
    QDesktopWidget* pDesk = QApplication::desktop();
    this->move((pDesk->width() - width) / 2, (pDesk->height() - height) / 2);


    QObject::connect(ui.toolButtonHardware, SIGNAL(clicked()), this, SLOT(HardwareInforClicked()));
    QObject::connect(ui.toolButtonTempManagement, SIGNAL(clicked()), this, SLOT(TemperatureClicked()));
    
    width = ui.stackedWidget->width() * m_uiRatio;
    height = ui.stackedWidget->height() * m_uiRatio;

    m_pHardwareInforPage = new HardwareInforPage();
    m_pHardwareInforPage->setFixedSize(width, height);
    ui.stackedWidget->addWidget(m_pHardwareInforPage);
    

    m_pTempManagementPage = new TempManagementPage();
    m_pTempManagementPage->setFixedSize(width, height);
    ui.stackedWidget->addWidget(m_pTempManagementPage);

}

MainWindow::~MainWindow()
{
    if (m_pHardwareInforPage != NULL)
    {
        delete m_pHardwareInforPage;
        m_pHardwareInforPage = NULL;
    }

    if (m_pTempManagementPage != NULL)
    {
        delete m_pTempManagementPage;
        m_pTempManagementPage = NULL;
    }
}

void MainWindow::showEvent(QShowEvent* e)
{
    ui.stackedWidget->setCurrentWidget(m_pHardwareInforPage);
}

void MainWindow::HardwareInforClicked()
{
    ui.stackedWidget->setCurrentWidget(m_pHardwareInforPage);
}

void MainWindow::TemperatureClicked()
{
    ui.stackedWidget->setCurrentWidget(m_pTempManagementPage);
}
