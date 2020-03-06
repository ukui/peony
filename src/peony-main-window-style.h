#ifndef PEONYMAINWINDOWSTYLE_H
#define PEONYMAINWINDOWSTYLE_H

#include <QProxyStyle>

class PeonyMainWindowStyle : public QProxyStyle
{
    Q_OBJECT
public:
    static PeonyMainWindowStyle *getStyle();

private:
    explicit PeonyMainWindowStyle(QObject *parent = nullptr);

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

};

#endif // PEONYMAINWINDOWSTYLE_H
