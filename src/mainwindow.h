#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMultimedia>
#include <QtMultimediaWidgets>
#include <QNetworkAccessManager>
#include <QMediaPlaylist>
#include <QMediaPlayer>

class urls{
    public:
        QUrl moj_api = QUrl("https://moj-apis.sharechat.com/webFeed");
        QUrl chingari_api = QUrl("https://api.chingari.io/post/trending-video-current?skip=0&limit=10&language=english'");
        QUrl josh_api_url = QUrl("https://share.myjosh.in/webview/apiwbody");
        QString josh_next_url = "https://feed-internal.coolfie.io/feed/latest?lang_code=en&page=1&rows=10";
};
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionRefresh_triggered();

    void on_actionPlay_triggered();

    void on_actionPause_triggered();

    void on_actionNext_triggered();

    void on_actionPrevious_triggered();

    void fetch_moj();

    void fetch_josh();

    void fetch_chingari();

    void get_icons(QUrl url);

    void onfinish_moj(QNetworkReply *rep);

    void onfinish_josh(QNetworkReply *rep);

    void onfinish_chingari(QNetworkReply *rep);

    void onfinish_get_icon(QNetworkReply *rep);

    void media_changed(QMediaPlayer::MediaStatus);

    void func_handler();

    QString KiloFormat(double num);

    void on_actionMute_triggered();

    void on_actionUnmute_triggered();

private:
    Ui::MainWindow* ui;
    QMediaPlayer* player;
    QVideoWidget* vw;
    QComboBox* dropdown;
    QMediaPlaylist* playlist;
    QNetworkAccessManager* mgr;
    QLabel* song_container, * subtitles_container;
    QVBoxLayout *layout;
};

#endif // MAINWINDOW_H
