#include "create-template-operation.h"

#include "file-operation-manager.h"
#include "file-operation-utils.h"
#include <gio/gio.h>

#include <QMessageBox>

using namespace Peony;

#define TEMPLATE_DIR "file://" + QString(g_get_user_special_dir(G_USER_DIRECTORY_TEMPLATES)) + "/"

void CreateTemplateOperation::handleDuplicate(const QString &uri) {
    setHasError(true);
    QString name = uri.split("/").last();
    QRegExp regExp("\\(\\d+\\)");
    if (name.contains(regExp)) {
        int pos = 0;
        int num = 0;
        QString tmp;
        while ((pos = regExp.indexIn(name, pos)) != -1) {
            tmp = regExp.cap(0).toUtf8();
            pos += regExp.matchedLength();
            qDebug()<<"pos"<<pos;
        }
        tmp.remove(0,1);
        tmp.chop(1);
        num = tmp.toInt();

        num++;
        name = name.replace(regExp, QString("(%1)").arg(num));
        m_target_uri = m_dest_dir_uri + "/" + name;
    } else {
        if (name.contains(".")) {
            auto list = name.split(".");
            if (list.count() <= 1) {
                m_target_uri = m_dest_dir_uri + "/" + name + "(1)";
            } else {
                list.insert(1, "(1)");
                name = list.join(".");
                if (name.endsWith("."))
                    name.chop(1);
                m_target_uri = m_dest_dir_uri + "/" + name;
            }
        } else {
            name = name + "(1)";
            m_target_uri = m_dest_dir_uri + "/" + name;
        }
    }
}

CreateTemplateOperation::CreateTemplateOperation(const QString &destDirUri, Type type, const QString &templateName, QObject *parent) : FileOperation(parent)
{
    m_target_uri = destDirUri + "/" + templateName;
    QStringList srcUris;
    m_src_uri = TEMPLATE_DIR +templateName;
    srcUris<<m_src_uri;
    m_dest_dir_uri = destDirUri;
    m_type = type;
    m_info = std::make_shared<FileOperationInfo>(srcUris, destDirUri, FileOperationInfo::Type::Copy);
}

void CreateTemplateOperation::run()
{
    Q_EMIT operationStarted();
    Q_EMIT operationPrepared();
    switch (m_type) {
    case EmptyFile: {
        m_target_uri = m_dest_dir_uri + "/" + tr("NewFile");
retry_create_empty_file:
        GError *err = nullptr;
        g_file_create(wrapGFile(g_file_new_for_uri(m_target_uri.toUtf8())).get()->get(),
                      G_FILE_CREATE_NONE,
                      nullptr,
                      &err);
        if (err) {
            if (err->code == G_IO_ERROR_EXISTS) {
                g_error_free(err);
                handleDuplicate(m_target_uri);
                goto retry_create_empty_file;
            } else {
                Q_EMIT errored(m_src_uri, m_dest_dir_uri, GErrorWrapper::wrapFrom(err));
            }
        }
        break;
    }
    case EmptyFolder: {
        m_target_uri = m_dest_dir_uri + "/" + tr("NewFolder");
retry_create_empty_folder:
        GError *err = nullptr;
        g_file_make_directory(wrapGFile(g_file_new_for_uri(m_target_uri.toUtf8())).get()->get(),
                              nullptr,
                              &err);
        if (err) {
            if (err->code == G_IO_ERROR_EXISTS) {
                g_error_free(err);
                handleDuplicate(m_target_uri);
                goto retry_create_empty_folder;
            } else {
                Q_EMIT errored(m_src_uri, m_dest_dir_uri, GErrorWrapper::wrapFrom(err));
            }
        }
        break;
    }
    case Template: {
retry_create_template:
        qDebug()<<"create tmp";
        GError *err = nullptr;
        g_file_copy(wrapGFile(g_file_new_for_uri(m_src_uri.toUtf8())).get()->get(),
                    wrapGFile(g_file_new_for_uri(m_target_uri.toUtf8())).get()->get(),
                    GFileCopyFlags(G_FILE_COPY_NOFOLLOW_SYMLINKS),
                    nullptr,
                    nullptr,
                    nullptr,
                    &err);
        if (err) {
            if (err->code == G_IO_ERROR_EXISTS) {
                g_error_free(err);
                handleDuplicate(m_target_uri);
                goto retry_create_template;
            } else {
                Q_EMIT errored(m_src_uri, m_dest_dir_uri, GErrorWrapper::wrapFrom(err));
            }
        }
        break;
    }
    }
    Q_EMIT operationFinished();
}
