#include "tictactoemainwindow.h"
#include "ui_tictactoemainwindow.h"

#include <QtGui>
#include <QPalette>
#include<QMessageBox>

const int DEFAULT_X_OFFSET= 100;
const int DEFAULT_Y_OFFSET= 100;
const int DEFAULT_WIDTH  =  100;
const int DEFAULT_HEIGHT =  100;

TicTacToeMainWindow::TicTacToeMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TicTacToeMainWindow)
{
    ui->setupUi(this);
    Initialize();
    m_pClientSocket=new QTcpSocket(this);
     DisplayStatusMessage("Ready to play!");
       connect(ui->actionSource_Code, &QAction::triggered, this, &TicTacToeMainWindow::on_actionSource_Code_triggered);

}

TicTacToeMainWindow::~TicTacToeMainWindow()
{
    delete ui;
}


void TicTacToeMainWindow::paintEvent(QPaintEvent *pEvent)
{
    QWidget::paintEvent(pEvent);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::red);

    painter.setPen(Qt::blue);

    for (int iRowIndex = 0; iRowIndex < m_NoOfRows; iRowIndex++)
    {
        for (int iColIndex = 0; iColIndex < m_NoOfCols; iColIndex++)
        {
            painter.drawRect(m_CellArray[iRowIndex][iColIndex].m_CellRect);

            if (m_CellArray[iRowIndex][iColIndex].m_strText != "i")
            {
                // Increase the font size for "X" and "O"
                QFont font = painter.font();
                font.setPointSize(24); // Adjust the size as needed
                painter.setFont(font);

                painter.drawText(m_CellArray[iRowIndex][iColIndex].m_CellRect.left() - m_Height / 2,
                                 m_CellArray[iRowIndex][iColIndex].m_CellRect.top() - m_width / 2,
                                 m_CellArray[iRowIndex][iColIndex].m_strText);
            }
        }
    }
}

void TicTacToeMainWindow ::Initialize()
{
     m_NoOfRows=DEFAULT_NO_ROWS;
     m_NoOfCols=DEFAULT_NO_COLS;
     m_width=DEFAULT_WIDTH;
     m_Height=DEFAULT_HEIGHT;
     m_XOffset=DEFAULT_X_OFFSET;
     m_YOffset=DEFAULT_Y_OFFSET;
     //Initialize the Rectange

    for (int iRowIndex = 0; iRowIndex<m_NoOfRows; iRowIndex++)
    {
        for (int iColIndex = 0; iColIndex<m_NoOfCols; iColIndex++)
        {
             m_CellArray[iRowIndex][iColIndex].m_CellRect.setSize(QSize(m_width, m_Height));
             m_CellArray[iRowIndex][iColIndex].m_CellRect.setTopLeft(QPoint(m_XOffset + (m_width*(iColIndex+1)), m_YOffset + (m_Height*(iRowIndex+1))));

        }
    }
}

QString TicTacToeMainWindow ::CreateMessage()
{
    m_Message="";
    for(int i=0;i<DEFAULT_NO_ROWS;i++)
    {
        for(int j=0;j<DEFAULT_NO_COLS;j++)
        {
            m_Message+=m_CellArray[i][j].m_strText;
        }
    }
    return m_Message;
}

void TicTacToeMainWindow::on_pushButtonStart_clicked()
{
    if(ui->pushButtonStart->text()=="Start")
    {
         m_pBoxServer= new HelloWorldServer(this);
        bool success = m_pBoxServer->listen(QHostAddress::Any, quint16(ui->plainTextEditPort->toPlainText().toInt()));
        if(!success)
        {
            DisplayStatusMessage("Server failed...");
        }
        else
        {
            DisplayStatusMessage("Server Started...");
            this->setWindowTitle(this->windowTitle()+"|"+ui->plainTextEditPort->toPlainText());
            ui->pushButtonStart->setText("Stop");
        }

    }
    else
    {
        m_pBoxServer->close();
        DisplayStatusMessage("Server Stopped...");
        ui->pushButtonStart->setText("Start");
    }

}

void TicTacToeMainWindow::on_pushButtonConnect_clicked()
{
    try
    {
        if(ui->pushButtonConnect->text()=="Connect")
        {
            m_pClientSocket->connectToHost(ui->plainTextEditRemoteIPAddr->toPlainText(),quint16(ui->plainTextEditRemotePort->toPlainText().toInt()) );
                connect(m_pClientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
            ui->pushButtonConnect->setText("Disconnect");
            DisplayStatusMessage("Connected to Host =>" + ui->plainTextEditRemoteIPAddr->toPlainText() + "Port =>" + ui->plainTextEditRemotePort->toPlainText());
        }
        else
        {
            m_pClientSocket->disconnectFromHost();
            m_pClientSocket->close();
            ui->pushButtonConnect->setText("Connect");
            DisplayStatusMessage("Disconnected from Host =>" + ui->plainTextEditRemoteIPAddr->toPlainText());
        }
    }
    catch(QException* ex)
    {
        DisplayStatusMessage(QString("Error in Connection=>") + QString(ex->what()));
    }
}

bool TicTacToeMainWindow::checkForWin(int row, int col)
{
    QString symbolToCheck = m_CellArray[row][col].m_strText;

    // Check horizontally
    int count = 0;
    for (int i = 0; i < DEFAULT_NO_COLS; i++)
    {
        if (m_CellArray[row][i].m_strText == symbolToCheck)
        {
            count++;
            if (count == 3)
            {
                return true;
            }
        }
        else
        {
            count = 0;
        }
    }

    // Check vertically
    count = 0;
    for (int i = 0; i < DEFAULT_NO_ROWS; i++)
    {
        if (m_CellArray[i][col].m_strText == symbolToCheck)
        {
            count++;
            if (count == 3)
            {
                return true;
            }
        }
        else
        {
            count = 0;
        }
    }

    // Check diagonally (positive slope)
    count = 0;
    int startRow = row - qMin(row, col);
    int startCol = col - qMin(row, col);
    for (int i = startRow, j = startCol; i < DEFAULT_NO_ROWS && j < DEFAULT_NO_COLS; i++, j++)
    {
        if (m_CellArray[i][j].m_strText == symbolToCheck)
        {
            count++;
            if (count == 3)
            {
                return true;
            }
        }
        else
        {
            count = 0;
        }
    }

    // Check diagonally (negative slope)
    count = 0;
    startRow = row + qMin(DEFAULT_NO_ROWS - row - 1, col);
    startCol = col - qMin(DEFAULT_NO_ROWS - row - 1, col);
    for (int i = startRow, j = startCol; i >= 0 && j < DEFAULT_NO_COLS; i--, j++)
    {
        if (m_CellArray[i][j].m_strText == symbolToCheck)
        {
            count++;
            if (count == 3)
            {
                return true;
            }
        }
        else
        {
            count = 0;
        }
    }

    return false;
}

void TicTacToeMainWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    int row = -1, col = -1;  // Variables to store the clicked cell coordinates

    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {

        for (int i = 0; i < DEFAULT_NO_ROWS; i++)
        {
            for (int j = 0; j < DEFAULT_NO_COLS; j++)
            {
                QRect widgetRect = m_CellArray[i][j].m_CellRect;
                widgetRect.moveTopLeft(this->parentWidget()->mapToGlobal(widgetRect.topLeft()));
                QPoint currentPoint = event->pos();

                if (widgetRect.contains(currentPoint))
                {
                    row = i;
                    col = j;
                    break;  // Break out of the loop after finding the clicked cell
                }
            }
            if (row != -1 && col != -1)
            {
                break;  // Break out of the outer loop as well
            }
        }
    }

    // Handle the common logic after the clicks
    if (row != -1 && col != -1)
    {
        m_CellArray[row][col].m_strText = (event->button() == Qt::LeftButton) ? "X" : "O";
        QString message = CreateMessage();
        DisplayStatusMessage(message);
        this->repaint();
        // Now send moves to the Remote Connect PC
        SendMovesToRemotePC(message);

        // Check for a win after each move
        if (checkForWin(row, col))
        {
            // Opponent wins
            QMessageBox msgBox;
            msgBox.setWindowTitle("Game Over");
            msgBox.setText("Opponent wins!");

            QPushButton *okButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);

            // Connect the button's signal to a slot
            QObject::connect(okButton, &QPushButton::clicked, [&]() {
                // Handle the button click (clear the board and update remote PC)
                clearBoard();  // Add a function to clear the board
                msgBox.close();
            });

            msgBox.exec();
        }
    }

}

void TicTacToeMainWindow::clearBoard()
{
    for (int i = 0; i < DEFAULT_NO_ROWS; i++)
    {
        for (int j = 0; j < DEFAULT_NO_COLS; j++)
        {
            m_CellArray[i][j].m_strText = "";  // Clear the cell text for the first board
        }
    }

    // Reset any other necessary game state variables
    // For example, switch back to the starting player, if applicable
    m_PlayerType = PlayerType::PlayerX;

    // Repaint the board
    repaint();

    // Update the remote PC with the cleared board state
    SendMovesToRemotePC(CreateMessage());
}

QString TicTacToeMainWindow::CreateMessage(int currentPlayer)
{
    // Implement the logic to create a message based on the current player
    // For example:
    QString message = (currentPlayer == 1) ? "Player X's turn" : "Player O's turn";
    return message;
}

void TicTacToeMainWindow::DisplayStatusMessage(QString Message)
{

    QString statusbarMessage = "Welcome to Tic Tac Toe "  ;

    ui->statusbar->showMessage(statusbarMessage);
    ui->labelResult->setText(Message);
}
void TicTacToeMainWindow::DisplayRemotePCMessage(QString Message)
{
    //Check the message length first ..
    //This is a simple protocol message ie Just storing only cell moves
    //User might need to expand the protocol struct
    //to accomodate other settings paramaters eg Message="GridSize"+"#"+"X or Y"+"#"+Message;
    //based on the requirements
    if(Message.length()>=DEFAULT_NO_ROWS*DEFAULT_NO_COLS)
    {
        DisplayStatusMessage(Message);
    }
    else
    {
        DisplayStatusMessage("Message Length Small");
        return;
    }

    int index=0;
    for(int i=0;i<DEFAULT_NO_ROWS;i++)
    {
        for(int j=0;j<DEFAULT_NO_COLS;j++)
        {
            m_CellArray[i][j].m_strText=Message[index];
            index=index+1;
        }
    }
   this->repaint();
}

void TicTacToeMainWindow::SendMovesToRemotePC(QString Message)
{
    //This is a simple protocol message ie Just storing only cell moves
    //User might need to expand the protocol struct
    //to accomodate other settings paramaters eg Message="GridSize"+"#"+"X or Y"+"#"+Message;
    //based on the requirements
    m_pClientSocket->write(QString(Message + "\n").toUtf8());
}
void TicTacToeMainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
            case QAbstractSocket::RemoteHostClosedError:
                break;
            case QAbstractSocket::HostNotFoundError:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The host was not found. Please check the "
                                            "host name and port settings."));
                break;
            case QAbstractSocket::ConnectionRefusedError:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The connection was refused by the peer. "
                                            "Make sure the fortune server is running, "
                                            "and check that the host name and port "
                                            "settings are correct."));
                break;
            default:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The following error occurred: %1.")
                                         .arg(m_pClientSocket->errorString()));
            }

}

void TicTacToeMainWindow::on_actionSource_Code_triggered()
{
    QString welcomeMessage = "Welcome to Tic Tac Toe!\n\n";
    QString description = "Tic Tac Toe is a classic two-player game where each player takes turns marking a square in a 3x3 grid. The player who succeeds in placing three of their marks in a horizontal, vertical, or diagonal row wins the game.\n\n"
                          "Enjoy playing Tic Tac Toe!";

    QMessageBox aboutDialog(this);
    aboutDialog.setWindowTitle("About Tic Tac Toe");
    aboutDialog.setText(welcomeMessage + description);
    aboutDialog.setIcon(QMessageBox::Information);
    aboutDialog.exec();
}


void TicTacToeMainWindow::on_actionmore_detials_triggered()
{

        QString contactInfo = "Contact Information:\n"
                              "Name: [Your Name]\n"
                              "Email: [Your Email Address]\n";

        QString sourceCodeLink = "Source Code:\n"
                                 "GitHub Repository: [Your GitHub Repository Link]\n";

        QString details = contactInfo + "\n" + sourceCodeLink;

        QMessageBox::information(this, "More Details", details);


}

