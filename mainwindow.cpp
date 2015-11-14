#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtQuick/QQuickView>
#include <QtQuick/qquickitem.h>
#include <QtQuickWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //criar os temporizadores like a Thread
    timer = new QTimer(this);

    //evento do timeout do timer - quando estourar faça o método atualizar
    connect(timer,SIGNAL(timeout()),this,SLOT(atualizarDados()));


    //Dados do QML
    /*QQuickView *view = new QQuickView();
    QWidget *container = QWidget::createWindowContainer(view, this);
    container->setMinimumSize(500, 200);
    container->setMaximumSize(500, 200);
    container->setFocusPolicy(Qt::TabFocus);
    view->setSource(QUrl("qrc:/qml/dashboard.qml"));
    ui->verticalLayout->addWidget(container);
    */

    QQuickWidget *view = new QQuickWidget();
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //connect(quickWidget, &QQuickWidget::statusChanged, this, &MainWindow::onStatusChangedWidget);
    //connect(quickWidget, &QQuickWidget::sceneGraphError, this, &MainWindow::onSceneGraphError);
    //view->setMinimumSize(500, 200);
    //view->setMaximumSize(500, 200);

    view->setSource(QUrl("qrc:/qml/dashboard.qml"));
    ui->verticalLayout->addWidget(view);

    QObject *item = view->rootObject();
    velocidade = item->findChild<QObject*>("velocidade");
    temperatura = item->findChild<QObject*>("temperatura");
    rpm = item->findChild<QObject*>("rpm");
    borboleta = item->findChild<QObject*>("borboleta");

    // Criar um agente para descoberta de dispositivos
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(discoveryAgent, SIGNAL(finished()),
               this, SLOT(finalizarBuscaDispositivos()) );

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(buscarServidor()));

    connect(ui->pushButtonConectar,SIGNAL(clicked(bool)),this,SLOT(conectarServidor()));

    //Configurações para a conexão com o servidor
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    connect( socket, SIGNAL(readyRead()),
             this, SLOT(onReadyRead()) );

    connect( socket, SIGNAL(connected()),
             this, SLOT(onBluetoothConnected()) );

    connect( socket, SIGNAL(disconnected()),
             this, SLOT(onBluetoothDisconnected()) );

    connect( socket, SIGNAL(error(QBluetoothSocket::SocketError)),
             this, SLOT(onBluetoothError(QBluetoothSocket::SocketError)) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onReadyRead()
{
    QByteArray data = socket->readAll();

    QString str(data);
    QStringList list;

    //qualquer sequência de não-caracteres serão usadas para separar o texto
    list = str.split(QRegExp("\\W+"), QString::SkipEmptyParts);

    bool ok;

    //tratando os dados enviados pela serial
    if (list.size() > 0){
        //Temperatura do motor
        if (list[0] == "41" && list[1] == "05"){
            temperaturaMotor = list[2].toInt(&ok,16) - 40;
            qDebug() << "Temperatura do motor:" << temperaturaMotor;

            if (temperatura)
                temperatura->setProperty("value", (double)temperaturaMotor/215);
        }
    }

    if (list.size() > 0){
        //Rotacao do motor
        if (list[0] == "41" && list[1] == "0C"){
            rpmMotor = ((list[2].toInt(&ok,16)*256)+ list[3].toInt(&ok,16))/4;
            qDebug() << "Rotacao do motor:" << rpmMotor;

            if (rpm)
                rpm->setProperty("value", (double)rpmMotor/1000);
        }
    }

    if (list.size() > 0){
        //Velocidade
        if (list[0] == "41" && list[1] == "0D"){
            velocidadeVeiculo = list[2].toInt(&ok,16);
            qDebug() << "Velocidade Instantanea:" << velocidadeVeiculo;

            if (velocidade)
                velocidade->setProperty("value", (double)velocidadeVeiculo);
        }
    }

    if (list.size() > 0){
        //Posicao Borboleta
        if (list[0] == "41" && list[1] == "11"){
            borboletaPosicao = (list[2].toInt(&ok,16)*100/255);
            qDebug() << "Posicao da Borboleta:" << borboletaPosicao;

            if (borboleta)
                borboleta->setProperty("value", (double)borboletaPosicao/10);
        }
    }


}

void MainWindow::onBluetoothDisconnected()
{

}

void MainWindow::onBluetoothError(QBluetoothSocket::SocketError erro)
{

}

void MainWindow::onBluetoothConnected()
{
    //inicializar o ELM327
    socket->write( QByteArray("AT E0\r") );
    socket->write( QByteArray("AT L0\r") );
    socket->write( QByteArray("AT ST 00\r") );
    socket->write( QByteArray("AT SP 0\r") );

    timer->start(2000);  //2000ms = 2s
}

void MainWindow::conectarServidor()
{

    QString address(ui->comboBox->currentText());
    QStringList lista = address.split(" ");
    if (lista.size() > 1){
        QBluetoothAddress addr(lista.at(1));
        qDebug() << lista.at(1);
        socket->connectToService(addr,
                QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB")),
                QIODevice::ReadWrite);
    }
}

void MainWindow::buscarServidor()
{
    discoveryAgent->start();
}

void MainWindow::finalizarBuscaDispositivos()
{
    QList<QBluetoothDeviceInfo> devices = discoveryAgent->discoveredDevices();

    foreach(QBluetoothDeviceInfo d, devices)
        ui->comboBox->addItem(d.name() + " " + d.address().toString());
}

void MainWindow::atualizarDados()
{
        socket->write(QByteArray("01 11\r"));
        socket->write(QByteArray("01 05\r"));
        socket->write(QByteArray("01 0C\r"));
        socket->write(QByteArray("01 0D\r"));

}
