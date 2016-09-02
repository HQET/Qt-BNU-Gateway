#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkReply>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_loginButton_clicked();

    void on_logoutButton_clicked();

    void on_forceButton_clicked();

    void replyFinished(QNetworkReply *reply);

private:
    void login(QString username, QString password);
    void logout();
    void force(QString username, QString password);
    void info();
    QSettings settings;
    QNetworkAccessManager *manager;
    QString ip;
    enum State {LOGIN, LOGOUT, FORCE, INFO} state;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
