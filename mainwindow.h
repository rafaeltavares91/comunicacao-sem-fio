#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/qbluetoothdeviceinfo.h>
#include <QtBluetooth/qbluetoothsocket.h>
#include <QtBluetooth/qbluetoothservicediscoveryagent.h>
#include <QTimer>

namespace Ui {
class MainWindow;
}

//UUID que será usado na aplicação.
static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void finalizarBuscaDispositivos(); //apos finalizar a busca por dispositivos bluetooth, imprimi no combobox
    void buscarServidor();
    void conectarServidor();
    void atualizarDados();
    void onReadyRead();  //quando tiver dados para serem lidos no sockets
    void onBluetoothConnected(); //quando o servidor se conectar ao socket
    void onBluetoothDisconnected();
    void onBluetoothError(QBluetoothSocket::SocketError erro);

private:
    Ui::MainWindow *ui;

    QObject *velocidade;
    QObject *temperatura;
    QObject *rpm;
    QObject *borboleta;

    QTimer *timer;

    int temperaturaMotor;
    int rpmMotor;
    int velocidadeVeiculo;
    int borboletaPosicao;

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothServiceInfo servico;
    QBluetoothSocket *socket;
    QBluetoothServiceDiscoveryAgent *discoveryServiceAgent;

};

#endif // MAINWINDOW_H
