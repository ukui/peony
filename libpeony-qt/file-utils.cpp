/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "file-utils.h"
#include "file-info.h"
#include "volume-manager.h"
#include <QUrl>
#include <QFileInfo>
#include <QFileInfoList>
#include <QTextCodec>
#include <QByteArray>
#include <QStandardPaths>
#include <QDir>
#include <QIcon>

using namespace Peony;

FileUtils::FileUtils()
{

}

QString FileUtils::getQStringFromCString(char *c_string, bool free)
{
    QString value = c_string;
    if (free)
        g_free(c_string);
    return value;
}

QString FileUtils::getFileUri(const GFileWrapperPtr &file)
{
    if (!G_IS_FILE (file.get()->get())) {
        return nullptr;
    }
    char *uri = g_file_get_uri(file.get()->get());
    QString urlString = QString(uri);
    QUrl url = urlString;
    g_free(uri);
    char *path = g_file_get_path(file.get()->get());
    if (path && !url.isLocalFile()) {
        QString urlString = QString("file://%1").arg(path);
        g_free(path);
        return urlString;
    }

    return urlString;
}

QString FileUtils::getFileBaseName(const GFileWrapperPtr &file)
{
    char *basename = g_file_get_basename(file.get()->get());
    return getQStringFromCString(basename);
}

QString FileUtils::getUriBaseName(const QString &uri)
{
    QUrl url = uri;
    return url.fileName();
}

GFileWrapperPtr FileUtils::getFileParent(const GFileWrapperPtr &file)
{
    return wrapGFile(g_file_get_parent(file.get()->get()));
}

QString FileUtils::getRelativePath(const GFileWrapperPtr &dir, const GFileWrapperPtr &file)
{
    char *relative_path = g_file_get_relative_path(dir.get()->get(), file.get()->get());
    return getQStringFromCString(relative_path);
}

GFileWrapperPtr FileUtils::resolveRelativePath(const GFileWrapperPtr &dir, const QString &relativePath)
{
    return wrapGFile(g_file_resolve_relative_path(dir.get()->get(),
                     relativePath.toUtf8().constData()));
}

bool FileUtils::getFileHasChildren(const GFileWrapperPtr &file)
{
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_DIRECTORY || type == G_FILE_TYPE_MOUNTABLE;
}

bool FileUtils::getFileIsFolder(const GFileWrapperPtr &file)
{
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_DIRECTORY;
}

bool FileUtils::getFileIsFolder(const QString &uri)
{
    auto info = FileInfo::fromUri(uri);
    if (!info.get()->isEmptyInfo()) {
        return info.get()->isDir();
    }

    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_DIRECTORY;
}

bool FileUtils::getFileIsSymbolicLink(const QString &uri)
{
    auto info = FileInfo::fromUri(uri);
    if (!info.get()->isEmptyInfo()) {
        return info.get()->isSymbolLink();
    }

    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_SYMBOLIC_LINK;
}

QStringList FileUtils::getChildrenUris(const QString &directoryUri)
{
    QStringList uris;

    GError *err = nullptr;
    GFile *top = g_file_new_for_uri(directoryUri.toUtf8().constData());
    GFileEnumerator *e = g_file_enumerate_children(top, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                   nullptr, &err);

    if (err) {
        g_error_free(err);
    }
    g_object_unref(top);
    if (!e)
        return uris;

    auto child_info = g_file_enumerator_next_file(e, nullptr, nullptr);
    while (child_info) {
        auto child = g_file_enumerator_get_child(e, child_info);

        auto uri = g_file_get_uri(child);
        auto path = g_file_get_path(child);
        QString urlString = uri;
        QUrl url = urlString;
        if (path && !url.isLocalFile()) {
            urlString = QString("file://%1").arg(path);
            g_free(path);
        } else {
            urlString = uri;
        }

        uris<<urlString;
        g_free(uri);
        g_object_unref(child);
        g_object_unref(child_info);
        child_info = g_file_enumerator_next_file(e, nullptr, nullptr);
    }

    g_file_enumerator_close(e, nullptr, nullptr);
    g_object_unref(e);

    return uris;
}

QString FileUtils::getNonSuffixedBaseNameFromUri(const QString &uri)
{
    QUrl url = uri;
    if (url.isLocalFile()) {
        QFileInfo info(url.path());
        return info.baseName();
    } else {
        QString suffixedBaseName = url.fileName();
        int index = suffixedBaseName.lastIndexOf(".");
        if (index != -1) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            QString suffix = suffixedBaseName.chopped(suffixedBaseName.size() - index);
            if (suffix == ".gz" || suffix == ".xz" || suffix == ".bz"
                    || suffix == ".bz2" || suffix == ".Z" ||
                    suffix == ".sit") {
                int secondIndex = suffixedBaseName.lastIndexOf('.');
                suffixedBaseName.chop(suffixedBaseName.size() - secondIndex);
            }
#else
            suffixedBaseName.chop(suffixedBaseName.size() - index);
#endif
        }
        return suffixedBaseName;
    }
}

QString FileUtils::getFileDisplayName(const QString &uri)
{
    auto fileInfo = FileInfo::fromUri(uri);
    if (!fileInfo.get()->isEmptyInfo()) {
        return fileInfo.get()->displayName();
    }

    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    if (!info.get()->get())
        return nullptr;
    return g_file_info_get_display_name(info.get()->get());
}

QString FileUtils::getFileIconName(const QString &uri, bool checkValid)
{
    auto fileInfo = FileInfo::fromUri(uri);
    if (!fileInfo.get()->isEmptyInfo()) {
        return fileInfo.get()->iconName();
    }

    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_ICON,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    if (!G_IS_FILE_INFO (info.get()->get()))
        return nullptr;
    GIcon *g_icon = g_file_info_get_icon (info.get()->get());
    QString icon_name;
    //do not unref the GIcon from info.
    if (G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        if (icon_names) {
            auto p = icon_names;
            if (*p)
                icon_name = QString (*p);
            if (checkValid) {
                while (*p) {
                    QIcon icon = QIcon::fromTheme(*p);
                    if (!icon.isNull()) {
                        icon_name = QString (*p);
                        break;
                    } else {
                        p++;
                    }
                }
            }
        }else {
            //if it's a bootable-media,maybe we can get the icon from the mount directory.
            char *bootableIcon = g_icon_to_string(g_icon);
            if(bootableIcon){
                icon_name = QString(bootableIcon);
                g_free(bootableIcon);
            }
        }
    }
    return icon_name;
}

GErrorWrapperPtr FileUtils::getEnumerateError(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    GError *err = nullptr;
    auto enumerator = wrapGFileEnumerator(g_file_enumerate_children(file.get()->get(),
                                          G_FILE_ATTRIBUTE_STANDARD_NAME,
                                          G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                          nullptr,
                                          &err));
    if (err) {
        return GErrorWrapper::wrapFrom(err);
    }
    return nullptr;
}

QString FileUtils::getTargetUri(const QString &uri)
{
    auto fileInfo = FileInfo::fromUri(uri);
    if (!fileInfo.get()->isEmptyInfo()) {
        return fileInfo.get()->targetUri();
    }

    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_TARGET_URI,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    return g_file_info_get_attribute_string(info.get()->get(),
                                            G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
}


QString FileUtils::getEncodedUri(const QString &uri)
{
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    QString encodedUri = g_file_get_uri(file);
    g_object_unref(file);

    return encodedUri;
}

QString FileUtils::getSymbolicTarget(const QString &uri)
{
    auto fileInfo = FileInfo::fromUri(uri);
    if (!fileInfo.get()->isEmptyInfo()) {
        return fileInfo.get()->symlinkTarget();
    }

    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file,
                                        G_FILE_ATTRIBUTE_STANDARD_SYMLINK_TARGET,
                                        G_FILE_QUERY_INFO_NONE,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);
    if (info) {
        return g_file_info_get_symlink_target(info);
        g_object_unref(info);
    }

    return uri;
}

bool FileUtils::isMountPoint(const QString &uri)
{
    bool flag = false;                      // The uri is a mount point

    GFile* file = g_file_new_for_uri(uri.toUtf8().constData());

    GList* mounts = nullptr;
    GVolumeMonitor* vm = g_volume_monitor_get();
    if (nullptr != vm) {
        mounts = g_volume_monitor_get_mounts(vm);
        if (nullptr != mounts) {
            for (GList* l = mounts; nullptr != l; l = l->next) {
                GMount* m = (GMount*)l->data;
                GFile* f = g_mount_get_root(m);
                if (g_file_equal(file, f)) {
                    flag = true;
                    g_object_unref(f);
                    break;
                }
                g_object_unref(f);
            }
        }
    }

    if (nullptr != vm) {
        g_object_unref(vm);
    }

    if (nullptr != mounts) {
        g_list_free(mounts);
    }

    if (nullptr != file) {
        g_object_unref(file);
    }

    return flag;
}

bool FileUtils::stringStartWithChinese(const QString &string)
{
    if (string.isEmpty())
        return false;

    auto firstStrUnicode = string.at(0).unicode();
    return (firstStrUnicode <=0x9FA5 && firstStrUnicode >= 0x4E00);
}

bool FileUtils::stringLesserThan(const QString &left, const QString &right)
{
    bool leftStartWithChinese = stringStartWithChinese(left);
    bool rightStartWithChinese = stringStartWithChinese(right);
    if (!(!leftStartWithChinese && !rightStartWithChinese)) {
        return leftStartWithChinese;
    }
    return left.toLower() < right.toLower();
}

const QString FileUtils::getParentUri(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto parent = getFileParent(file);
    auto parentUri = getFileUri(parent);
    return parentUri == uri? nullptr: parentUri;
}

const QString FileUtils::getOriginalUri(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto originalUri = getFileUri(file);
    return originalUri;
}

bool FileUtils::isFileExsit(const QString &uri)
{
    bool exist = false;
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    exist = g_file_query_exists(file, nullptr);
    g_object_unref(file);
    return exist;
}

const QStringList FileUtils::toDisplayUris(const QStringList &args)
{
    QStringList uris;
    for (QString path : args) {
        QUrl url = path;
        if (url.scheme().isEmpty()) {
            auto current_dir = g_get_current_dir();
            QDir currentDir = QDir(current_dir);
            g_free(current_dir);
            currentDir.cd(path);
            auto absPath = currentDir.absoluteFilePath(path);
            path = absPath;
            url = QUrl::fromLocalFile(absPath);
        } else if (path.startsWith ("computer:///") && path.endsWith(".link")) {
            GFile* file = g_file_new_for_uri(url.toString().toUtf8().constData());
            if (nullptr != file) {
                GFileInfo* info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, NULL);
                if (nullptr != info) {
                    url = g_file_info_get_attribute_string(info, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
                    g_object_unref(info);
                }
                g_object_unref(file);
            }
        }

        if (FileUtils::isFileExsit(url.toString())) {
            uris << url.toDisplayString();
        }
    }
    return uris;
}

bool FileUtils::isMountRoot(const QString &uri)
{
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file,
                                        "unix::is-mountpoint",
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);
    if (info) {
        bool isMount = g_file_info_get_attribute_boolean(info, "unix::is-mountpoint");
        g_object_unref(info);
        return isMount;
    }
    return false;
}

bool FileUtils::queryVolumeInfo(const QString &volumeUri, QString &volumeName, QString &unixDeviceName, const QString &volumeDisplayName)
{
    char *unix_dev_file = nullptr;

    if (!volumeUri.startsWith("computer:///"))
        return false;

    GFile *file = g_file_new_for_uri(volumeUri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file,
                                        "*",
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);

    if (!info) {
        return false;
    }

    auto displayName = volumeDisplayName;
    if (displayName.isNull()) {
        displayName = getFileDisplayName(volumeUri);
    }

    unix_dev_file = g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE_FILE);//for computer:///xxx.drive
    if(!unix_dev_file){//for computer:///xxx.mount
        char *targetUri = g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
        if(targetUri){
            char *realMountPoint = g_filename_from_uri(targetUri,NULL,NULL);
            const char *unix_dev = Peony::VolumeManager::getUnixDeviceFileFromMountPoint(realMountPoint);
            unixDeviceName = unix_dev;
            g_free(targetUri);
            g_free(realMountPoint);
        }
    }else{
        unixDeviceName = unix_dev_file;
        g_free(unix_dev_file);
    }

    auto list = displayName.split(":");
    if (list.count() > 1) {
        auto last = list.last();
        if (last.startsWith(" "))
            last.remove(0, 1);
        volumeName = last;
    } else {
        volumeName = displayName;
    }

    handleVolumeLabelForFat32(volumeName,unixDeviceName);
    return true;
}

bool FileUtils::isReadonly(const QString& uri)
{
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file, "access::*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
    g_object_unref(file);
    if (info) {
        bool read = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ);
        bool write = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
        bool execute = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE);

        if (read && !write && !execute) {
            return true;
        }
    }

    return false;
}

bool FileUtils::isFileDirectory(const QString &uri)
{
    bool isFolder = false;
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    isFolder = g_file_query_file_type(file,
                                      G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                      nullptr) == G_FILE_TYPE_DIRECTORY;
    g_object_unref(file);
    return isFolder;
}

bool FileUtils::isFileUnmountable(const QString &uri)
{
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_UNMOUNT, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
    g_object_unref(file);
    if (info) {
        bool unmountable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_UNMOUNT);
        g_object_unref(info);
        return unmountable;
    }
    return false;
}

/* @func:        convert a ascii string to unicode string. 将一个ascii字符串转换为unicode字符串
 * @gbkName      a string that needs to be converted from ascii to  Unicode. eg:"\\xb8\\xfc\\xd0\\xc2CODE"
 */
QString transcodeForGbkCode(QByteArray gbkName, QString &volumeName)
{
    int i;
    QByteArray dest,tmp;
    QString name;
    int len = gbkName.size();

    for(i = 0x0; i < len; ++i){
        if(92 == gbkName.at(i)){
            if(4 == tmp.size())
                dest.append(QByteArray::fromHex(tmp));
            else{
                if(tmp.size() > 4){
                    dest.append(QByteArray::fromHex(tmp.left(4)));
                    dest.append(tmp.mid(4));
                }else
                    dest.append(tmp);
            }
            tmp.clear();
            tmp.append(gbkName.at(i));
            continue;
        }else if(tmp.size() > 0){
            tmp.append(gbkName.at(i));
            continue;
        }else
            dest.append(gbkName.at(i));
    }

    if(4 == tmp.size())
        dest.append(QByteArray::fromHex(tmp));
    else{
        if(tmp.size() > 4){
            dest.append(QByteArray::fromHex(tmp.left(4)));
            dest.append(tmp.mid(4));
        }else
            dest.append(tmp);
    }

    /*
    * gio的api获取的卷名和/dev/disk/by-label先的名字不一致，有可能是卷名
    * 中含有特殊字符，导致/dev/disk/label下的卷名含有转义字符，导致二者的名字不一致
    * 而不是编码格式的不一致导致的，比如卷名：“数据光盘(2020-08-22)”，在/dev/disk/by-label
    * 写的名字:"数据光盘\x282020-08-22\x29",经过上述处理之后可以去除转义字符，在判断一次
    * 是否相等。比较完美的解决方案是找到能够判断字符串的编码格式，目前还没有找到实现方式，需要进一步完善
    */
    name = QString(dest);
    if (name == volumeName){
        return name;
    }

    name = QTextCodec::codecForName("GBK")->toUnicode(dest);
    return name;
}

/* @func:           determines whether the @volumeName needs to be transcoded. 判断字符串是否需要转码.
 * @volumeName      a string that needs to be converted from ascii to  Unicode. eg:"\\xb8\\xfc\\xd0\\xc2CODE"
 * @unixDeviceName  a device name. eg: /dev/sdb
 */
void FileUtils::handleVolumeLabelForFat32(QString &volumeName,const QString &unixDeviceName){
    QFileInfoList diskList;
    QFileInfo diskLabel;
    QDir diskDir;
    QString partitionName,linkTarget;
    QString tmpName,finalName;
    int i;
    QRegExp rx("[^\u4e00-\u9fa5]");		//non-chinese characters

    diskDir.setPath("/dev/disk/by-label");
    if(!diskDir.exists())               //this means: volume has no name.
        return;                         //            or there no mobile devices.

    diskList = diskDir.entryInfoList(); //all file from dir.
    /* eg: unixDeviceName == "/dev/sdb4"
     *     partitionName == "sdb4"
     */
    partitionName = unixDeviceName.mid(unixDeviceName.lastIndexOf('/')+1);

    for(i = 0; i < diskList.size(); ++i){
        diskLabel = diskList.at(i);
        linkTarget = diskLabel.symLinkTarget();
        linkTarget = linkTarget.mid(linkTarget.lastIndexOf('/')+1);
        if(linkTarget == partitionName)
            break;
        linkTarget.clear();
    }

    if(!linkTarget.isEmpty())
        tmpName = diskLabel.fileName();//可能带有乱码的名字

    if(!tmpName.isEmpty()){
        if(tmpName == volumeName || !tmpName.contains(rx)){      //ntfs、exfat格式或者非纯中文名的fat32设备,这个设备的名字不需要转码
            volumeName = tmpName;
            return;
        }else{
            finalName = transcodeForGbkCode(tmpName.toLocal8Bit(), volumeName);
            if(!finalName.isEmpty())
                volumeName = finalName;
        }
    }
}
