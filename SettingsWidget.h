#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QSettings>
#include <QWidget>

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();
    void loadSettings();
    QSize mainWindowSize();
    QPoint mainWindowPosition();
    QStringList movieDirectories();
    QStringList tvShowDirectories();
    bool firstTime();

    void setMainWindowSize(QSize mainWindowSize);
    void setMainWindowPosition(QPoint mainWindowPosition);

public slots:
    void saveSettings();

private slots:
    void addMovieDir();
    void removeMovieDir();
    void movieListRowChanged(int currentRow);
    void addTvShowDir();
    void removeTvShowDir();
    void tvShowListRowChanged(int currentRow);

private:
    Ui::SettingsWidget *ui;
    QSettings m_settings;
    QStringList m_movieDirectories;
    QStringList m_tvShowDirectories;
    QSize m_mainWindowSize;
    QPoint m_mainWindowPosition;
    bool m_firstTime;
};

#endif // SETTINGSWIDGET_H
