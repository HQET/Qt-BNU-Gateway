#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("Hongxu Xu", "xGateway")
{
    ui->setupUi(this);
    if (settings.value("remember", false).toBool())
    {
        ui->usernameEdit->setText(settings.value("username").toString());
        ui->passwordEdit->setText(settings.value("password").toString());
        ui->rememberCheckBox->setChecked(true);
    }
    
    ui->infoLabel->setText("未登录");

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    if (ui->rememberCheckBox->isChecked())
    {
        settings.setValue("username", username);
        settings.setValue("password", password);
        settings.setValue("remember", true);
    }
    else
    {
        settings.clear();
    }
    login(username, password);
}

void MainWindow::on_logoutButton_clicked()
{
    logout();
}

void MainWindow::on_forceButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    if (ui->rememberCheckBox->isChecked())
    {
        settings.setValue("username", username);
        settings.setValue("password", password);
        settings.setValue("remember", true);
    }
    else
    {
        settings.clear();
    }
    force(username, password);
}

void MainWindow::replyFinished(QNetworkReply *reply)
{
    QString res = QString::fromUtf8(reply->readAll().data());
    switch (state)
    {
    case LOGIN:
        if (res.contains("成功"))
        {
            ui->statusBar->showMessage("登录成功！");
            info();
        }
        else
        {
            int s = res.indexOf("<p style=\"color:#FF0;\">");
            if (s == -1)
            {
                ui->statusBar->showMessage("未知错误");
            }
            else
            {
                s = res.indexOf("(", s) + 1;
                int t = res.indexOf(")", s);
                ui->statusBar->showMessage(res.mid(s, t - s));
            }
        }
        break;
    case INFO:
    {
        QStringList info = res.split(",");
        QString usedData = info.at(0);
        usedData = QString::number(usedData.toDouble() / 1024 / 1024);
        QString usedTime = info.at(1);
        int s = usedTime.toInt();
        int m = s / 60;
        s %= 60;
        int h = m / 60;
        m %= 60;
        ip = info.at(5);
        ui->infoLabel->setText(
                    "已用流量：" + usedData + "MB\n" +
                    "已用时长：" + QString::number(h) + "时" + QString::number(m) + "分" + QString::number(s) + "秒\n" +
                    "账户余额：" + info.at(2) + "元\n" +
                    "IP地址：" + info.at(5)
                    );
    }
        break;
    case LOGOUT:
    {
        if (res.contains("成功"))
        {
            ui->statusBar->showMessage("注销成功！");
        }
        else
        {
            ui->statusBar->showMessage("注销失败！");
            qDebug() << res;
        }
    }
        break;
    case FORCE:
        ui->statusBar->showMessage(res);
        break;
    }
}

void MainWindow::login(QString username, QString password)
{
    state = LOGIN;
    QNetworkRequest request(QUrl("http://gw.bnu.edu.cn:803/srun_portal_pc.php?ac_id=1&"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    
    QByteArray data(QString("action=login&ac_id=1&user_ip=&nas_ip="
                            "&user_mac=&url=&save_me=1&username="
                            + username + "&password=" + password).toUtf8());
    
    manager->post(request, data);
}

void MainWindow::logout()
{
    state = LOGOUT;

    ui->infoLabel->setText("未登录");

    QNetworkRequest request(QUrl("http://gw.bnu.edu.cn:803/srun_portal_pc.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    ip = "";

    QByteArray data(QString("action=auto_logout&info=&user_ip=" + ip).toUtf8());

    manager->post(request, data);
}

void MainWindow::force(QString username, QString password)
{
    state = FORCE;

    ui->infoLabel->setText("未登录");

    QNetworkRequest request(QUrl("http://gw.bnu.edu.cn:801/include/auth_action.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));

    QByteArray data(QString("action=logout&ajax=1&username="
                            + username + "&password=" + password).toUtf8());

    manager->post(request, data);
}

void MainWindow::info()
{
    state = INFO;
    
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    QString k = QString::number(qrand() % 100001);

    QNetworkRequest request(QUrl("http://gw.bnu.edu.cn:803/include/auth_action.php?k=" + k));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    
    QByteArray data(QString("action=get_online_info&key=" + k).toUtf8());
    manager->post(request, data);
}
