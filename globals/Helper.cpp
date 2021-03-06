#include "Helper.h"

#include <QBuffer>
#include <QDir>
#include <QRegExp>
#include "globals/Globals.h"
#include "settings/Settings.h"

/**
 * @brief Encodes a string to latin1 percent encoding needed for some scrapers
 * @param str String to encode
 * @return Encoded string
 */
QString Helper::toLatin1PercentEncoding(QString str)
{
    str = str.toUtf8();
    str = str.replace("%", "%25");
    str = str.replace("ä", "%E4");
    str = str.replace("ö", "%F6");
    str = str.replace("ü", "%FC");
    str = str.replace("Ä", "%C4");
    str = str.replace("Ö", "%D6");
    str = str.replace("Ü", "%DC");
    str = str.replace("?", "%3F");
    str = str.replace(" ", "%20");
    str = str.replace("/", "%2F");
    str = str.replace(";", "%3B");
    str = str.replace("=", "%3D");
    str = str.replace("\"", "%22");
    str = str.replace("(", "%28");
    str = str.replace(")", "%29");
    str = str.replace("'", "%2C");
    str = str.replace(".", "%2E");
    str = str.replace(":", "%3A");
    str = str.replace("<", "%3C");
    str = str.replace(">", "%3E");
    str = str.replace("@", "%40");
    str = str.replace("[", "%5B");
    str = str.replace("]", "%5D");
    str = str.replace("\\", "%5C");
    str = str.replace("^", "%5E");
    str = str.replace("{", "%7B");
    str = str.replace("}", "%7D");
    str = str.replace("|", "%7C");
    str = str.replace("&", "%26");
    str = str.replace("#", "%23");
    return str;
}

QString Helper::urlFromEncoded(QString str)
{
    str = str.replace("&amp;", "&");
    return str;
}

/**
 * @brief Changes the format of a trailer url
 * @param url Trailer Url
 * @return Reformatted trailer url
 */
QString Helper::formatTrailerUrl(QString url)
{
    if (!Settings::instance()->useYoutubePluginUrls())
        return url;

    QString videoId;
    QRegExp rx("youtube.com/watch\\?v=([^&]*)");
    if (rx.indexIn(url, 0) != -1)
        videoId = rx.cap(1);

    if (videoId.isEmpty())
        return url;

    return QString("plugin://plugin.video.youtube/?action=play_video&videoid=%1").arg(videoId);
}


/**
 * @brief Returns true if path is a DVD directory
 * @param path
 * @return
 */
bool Helper::isDvd(QString path)
{
    if (path.endsWith("VIDEO_TS.IFO")) {
        QFileInfo fi(path);
        return fi.absolutePath().endsWith("VIDEO_TS");
    }
    QDir dir(path);
    QStringList filters;
    filters << "VIDEO_TS" << "VIDEO TS";
    if (dir.entryList(filters, QDir::Dirs | QDir::NoDotAndDotDot).count() > 0) {
        foreach (const QString filter, filters) {
            dir.setPath(path + QDir::separator() + filter);
            if (dir.entryList(QStringList() << "VIDEO_TS.IFO").count() == 1)
                return true;
        }
    }

    return false;
}

/**
 * @brief Returns true if path is a bluray directory
 * @param path
 * @return
 */
bool Helper::isBluRay(QString path)
{
    if (path.endsWith("index.bdmv")) {
        QFileInfo fi(path);
        return fi.absolutePath().endsWith("BDMV");
    }

    QDir dir(path);
    QStringList filters;
    filters << "BDMV";
    if (dir.entryList(filters, QDir::Dirs | QDir::NoDotAndDotDot).count() == 1) {
        dir.setPath(path + QDir::separator() + "BDMV");
        filters.clear();
        filters << "index.bdmv";
        if (dir.entryList(filters).count() == 1)
            return true;
    }

    return false;
}

QImage &Helper::resizeBackdrop(QImage &image, bool &resized)
{
    resized = false;
    if ((image.width() != 1920 || image.height() != 1080) &&
        image.width() > 1915 && image.width() < 1925 && image.height() > 1075 && image.height() < 1085) {
        image = image.scaled(1920, 1080, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        resized = true;
    }

    if ((image.width() != 1280 || image.height() != 720) &&
        image.width() > 1275 && image.width() < 1285 && image.height() > 715 && image.height() < 725) {
        image = image.scaled(1280, 720, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        resized = true;
    }

    return image;
}

QByteArray &Helper::resizeBackdrop(QByteArray &image)
{
    bool resized;
    QImage img = QImage::fromData(image);
    Helper::resizeBackdrop(img, resized);
    if (!resized)
        return image;
    QBuffer buffer(&image);
    img.save(&buffer, "jpg", 100);
    return image;
}

QString &Helper::sanitizeFileName(QString &fileName)
{
    fileName.replace("/", " ");
    fileName.replace("\\", " ");
    fileName.replace("$", "");
    fileName.replace("<", "");
    fileName.replace(">", "");
    fileName.replace(":", "");
    fileName.replace("\"", "");
    fileName.replace("?", "");
    fileName.replace("*", "");
    fileName = fileName.trimmed();
    return fileName;
}

QString Helper::stackedBaseName(const QString &fileName)
{
    QString baseName = fileName;
    QRegExp rx1a("(.*)([ _\\.-]*(?:cd|dvd|p(?:ar)?t|dis[ck])[ _\\.-]*[0-9]+)(.*)(\\.[^.]+)$", Qt::CaseInsensitive);
    QRegExp rx1b("(.*)([ _\\.-]+)$");
    QRegExp rx2a("(.*)([ _\\.-]*(?:cd|dvd|p(?:ar)?t|dis[ck])[ _.-]*[a-d])(.*)(\\.[^.]+)$", Qt::CaseInsensitive);
    QRegExp rx2b("(.*)([ _\\.-]+)$");

    QList<QList<QRegExp> > regex;
    regex << (QList<QRegExp>() << rx1a << rx1b);
    regex << (QList<QRegExp>() << rx2a << rx2b);

    foreach (QList<QRegExp> rx, regex) {
        if (rx.at(0).indexIn(fileName) != -1) {
            QString title = rx.at(0).cap(1);
            QString volume = rx.at(0).cap(2);
            /*QString ignore = rx.at(0).cap(3);
            QString extension = rx.at(0).cap(4);*/
            while (rx.at(1).indexIn(title) != -1) {
                title = rx.at(1).capturedTexts().at(1);
                volume.prepend(rx.at(1).capturedTexts().at(2));
            }
            baseName = title;
        }
    }

    return baseName;
}

QString Helper::appendArticle(const QString &text)
{
    if (!Settings::instance()->ignoreArticlesWhenSorting())
        return text;

    QString name = text;
    foreach (const QString &article, Settings::instance()->advanced()->sortTokens()) {
        if (text.startsWith(article + " ", Qt::CaseInsensitive) && text.length() > article.length()) {
            name = text.mid(article.length()+1) + ", " + text.mid(0, article.length());
            break;
        }
    }
    return name;
}

QString Helper::mapGenre(const QString &text)
{
    if (Settings::instance()->advanced()->genreMappings().isEmpty())
        return text;

    if (Settings::instance()->advanced()->genreMappings().contains(text))
        return Settings::instance()->advanced()->genreMappings().value(text);
    return text;
}

QStringList Helper::mapGenre(const QStringList &genres)
{
    if (Settings::instance()->advanced()->genreMappings().isEmpty())
        return genres;

    QStringList mappedGenres;
    foreach (const QString &genre, genres)
        mappedGenres << Helper::mapGenre(genre);
    return mappedGenres;
}

QString Helper::mapCertification(const QString &text)
{
    if (Settings::instance()->advanced()->certificationMappings().isEmpty())
        return text;

    if (Settings::instance()->advanced()->certificationMappings().contains(text))
        return Settings::instance()->advanced()->certificationMappings().value(text);
    return text;
}

QString Helper::mapStudio(const QString &text)
{
    if (Settings::instance()->advanced()->studioMappings().isEmpty())
        return text;

    if (Settings::instance()->advanced()->studioMappings().contains(text))
        return Settings::instance()->advanced()->studioMappings().value(text);
    return text;
}

QString Helper::mapCountry(const QString &text)
{
    if (Settings::instance()->advanced()->countryMappings().isEmpty())
        return text;

    if (Settings::instance()->advanced()->countryMappings().contains(text))
        return Settings::instance()->advanced()->countryMappings().value(text);
    return text;
}
