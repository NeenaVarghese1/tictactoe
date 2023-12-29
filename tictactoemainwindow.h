#ifndef TICTACTOEMAINWINDOW_H
#define TICTACTOEMAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include<QDialog>
#include<QDebug>
#include<QLineEdit>
#include<QLabel>


#include "tictactoeserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TicTacToeMainWindow; }
QT_END_NAMESPACE

const int DEFAULT_NO_ROWS = 3;
const int DEFAULT_NO_COLS=  3;

class Cell
{
public:
     QFont m_CellTextFont;
    QRect           m_CellRect;
    QString         m_strText;
    Cell()
    {
        //Just initialize some character to display
        m_strText="i";

    }
};

enum class PlayerType {
    PlayerX,
    PlayerO
};

class HelloWorldServer;

class TicTacToeMainWindow : public QMainWindow
{
    Q_OBJECT
    QTcpSocket *m_pClientSocket;
    HelloWorldServer* m_pBoxServer;
public:
    int m_width;
    int m_Height;
    int m_NoOfRows;
    int m_NoOfCols;
    int m_XOffset; //Offset from which drawing start
    int m_YOffset;
    Cell            m_CellArray[DEFAULT_NO_ROWS][DEFAULT_NO_COLS];
    QString         m_Message;

    TicTacToeMainWindow(QWidget *parent = nullptr);
    ~TicTacToeMainWindow();
    void DisplayStatusMessage(QString Message);
    void DisplayRemotePCMessage(QString Message);
    void SendMovesToRemotePC(QString Message);
    void displayError(QAbstractSocket::SocketError socketError);


protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent* ev);
    void Initialize();
    QString CreateMessage();
 bool checkForWin(int row, int col);
private slots:

    void on_pushButtonStart_clicked();

    void on_pushButtonConnect_clicked();

    void on_actionSource_Code_triggered();

    void on_actionmore_detials_triggered();

private:
    Ui::TicTacToeMainWindow *ui;
    void clearBoard();
     PlayerType m_PlayerType;
    Cell m_CellArray1[DEFAULT_NO_ROWS][DEFAULT_NO_COLS];
    static int currentPlayer;
    QString CreateMessage(int currentPlayer);



};
#endif // TICTACTOEMAINWINDOW_H
