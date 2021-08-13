#include "pluginwidget.h"
#include <QDebug>
pluginwidget::pluginwidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setFocusPolicy(Qt::NoFocus);
    QVBoxLayout *mainvbox=new QVBoxLayout(this);
    this->setLayout(mainvbox);
    stackWidget=new QStackedWidget(this);

    stackWidget->setFixedSize(400,550);
    mainvbox->addWidget(stackWidget);
    mainvbox->addStretch(10);
    toolbutton_widget=new QWidget(this);
    toolbutton_widget->setFixedSize(400,80);
    //toolbutton_widget->setStyleSheet("Background-color:#FFFFFF;border-radius:40px");
    if(QGSettings::isSchemaInstalled("org.ukui.style")){
        themeSetting=new QGSettings("org.ukui.style");
        themeName=themeSetting->get("style-name").toString();
    }
    connect(themeSetting,&QGSettings::changed,this,[=](){
         changeSearchBoxBackground();
    });
    changeSearchBoxBackground();
    mainvbox->addWidget(toolbutton_widget);
    mainvbox->setContentsMargins(0,0,0,0);
    QHBoxLayout *Hbox=new QHBoxLayout(toolbutton_widget);
    toolbutton_widget->setLayout(Hbox);
    QPushButton *but1=new QPushButton(toolbutton_widget);
    but1->setFocusPolicy(Qt::NoFocus);

    but1->setFixedSize(120,64);
    QPushButton *but2=new QPushButton(toolbutton_widget);
    but2->setFocusPolicy(Qt::NoFocus);
    but2->setFixedSize(120,64);
    //QPushButton *but3=new QPushButton(toolbutton_widget);
    //QPushButton *but4=new QPushButton(toolbutton_widget);



   // but1->setStyleSheet("Background-color:red");

    label = new QLabel();
    label2 = new QLabel();
    //label2->setStyleSheet(QString("border:1px solid red;"));
    //label->setStyleSheet(QString("border:1px solid red;"));
    label2->setPixmap(QPixmap(":/img/focus-select.png")/*,0,Qt::AlignHCenter*/);
    label2->setFixedSize(32,32);
    QHBoxLayout* myLayout = new QHBoxLayout();
    myLayout->addStretch();
    myLayout->addWidget(label2);
    myLayout->addWidget(label);
    myLayout->addStretch();
    but1->setLayout(myLayout);
    label->setText(tr("专注"));

    label->setStyleSheet("color:#469BFF;border:0px;background:transparent;");

     label3 = new QLabel();
     label4 = new QLabel();
    //label4->setStyleSheet(QString("border:1px solid red;"));
    //label3->setStyleSheet(QString("border:1px solid red;"));
    label4->setPixmap(QPixmap(":/img/memo.png"));
    label4->setFixedSize(32,32);
    label3->setText(tr("便签"));
    QHBoxLayout* myLayout2 = new QHBoxLayout();
    myLayout2->addStretch();
    myLayout2->addWidget(label4);
    myLayout2->addWidget(label3);
    myLayout2->addStretch();


    but2->setLayout(myLayout2);


    Hbox->addStretch();
    Hbox->addWidget(but1);
    //Hbox->addSpacing(60);
    //Hbox->addStretch(20);
    Hbox->addWidget(but2);
    Hbox->addStretch();
    //Hbox->addSpacing(63);


    connect(but1,&QPushButton::clicked,this,&pluginwidget::button_1_click);
    connect(but2,&QPushButton::clicked,this,&pluginwidget::button_2_click);
    //connect(but3,&QPushButton::clicked,this,&pluginwidget::button_3_click);
    //connect(but4,&QPushButton::clicked,this,&pluginwidget::button_4_click);
    init();
}
void pluginwidget::init()
{
    //插件专注模式
    QPluginLoader loader("/opt/FocusPluginWidget/bin/libFocusPluginWidget.so");
    plugin=loader.instance();
    if(plugin)
    {
        app=qobject_cast<AbstractInterface *>(plugin);
        focuswidget = app->createPluginWidget(this,true);
    }


    //插件便签
    QPluginLoader loader1("/usr/lib/x86_64-linux-gnu/ukui-notebook/libukui-notebook.so");
    pluginnote=loader1.instance();

    if(pluginnote)
    {
        app1=qobject_cast<kyMenuPluginInterface *>(pluginnote);
        notewidget = app1->createWidget(this);
    }
//    notewidget=new QWidget(this);
    //notewidget->setFixedSize(400,550);
    //notewidget->setStyleSheet("Background-color:gray;border-radius:32px");

    //插件提醒

    secondwidget=new QWidget(this);
    secondwidget->setFixedSize(400,550);
    secondwidget->setStyleSheet("Background-color:gray;border-radius:32px");

    //插件计算器
    threeWidget=new QWidget(this);
    threeWidget->setFixedSize(400,550);
    threeWidget->setStyleSheet("Background-color:gray;border-radius:32px");




    stackWidget->addWidget(focuswidget);
    stackWidget->addWidget(notewidget);
    stackWidget->addWidget(secondwidget);
    stackWidget->addWidget(threeWidget);

    stackWidget->setCurrentWidget(focuswidget);
}
void pluginwidget::mouseReleaseEvent(QMouseEvent *e)
{
//重写防止点击空白退出，和滑动翻页
    return;
}
void pluginwidget::mousePressEvent(QMouseEvent *event)
{
//重写防止点击空白退出，和滑动翻页
    return ;
}
void pluginwidget::button_1_click()
{
    //qDebug()<<"专注界面";
    stackWidget->setCurrentWidget(focuswidget);
    label4->setPixmap(QPixmap(":/img/memo.png"));
    label2->setPixmap(QPixmap(":/img/focus-select.png")/*,0,Qt::AlignHCenter*/);
    label->setStyleSheet("color:#469BFF;border:0px;background:transparent;");
    label3->setStyleSheet("border:0px;background:transparent;");
//    QPalette pe ;
//    pe.setColor(QPalette::WindowText,QColor(70,159,255));
//    label->setPalette(pe);
}
void pluginwidget::button_2_click()
{
    //暂时空缺
    stackWidget->setCurrentWidget(notewidget);
    label4->setPixmap(QPixmap(":/img/memo-select.png"));
    label2->setPixmap(QPixmap(":/img/focus.png")/*,0,Qt::AlignHCenter*/);
    label3->setStyleSheet("color:#469BFF;border:0px;background:transparent;");
    label->setStyleSheet("border:0px;background:transparent;");
}

void pluginwidget::button_3_click()
{

    //暂时空缺
    //stackWidget->setCurrentWidget(threeWidget);
}
void pluginwidget::button_4_click()
{

    //暂时空缺
    //stackWidget->setCurrentWidget(focuswidget);
}
//搜索框适配主题
void pluginwidget::changeSearchBoxBackground()
{
    themeName=themeSetting->get("style-name").toString();
    if(themeName=="ukui-dark")
    {
        toolbutton_widget->setStyleSheet("border-radius:40px;background:#FF2C3239;color:white;");
    }
    else
    {
        toolbutton_widget->setStyleSheet("border-radius:40px;background:white;color:rgba(58,67,78,0.25);");
    }
}

pluginwidget::~pluginwidget()
{
    if(stackWidget)
        delete stackWidget;
    if(toolbutton_widget)
        delete toolbutton_widget;
    if(themeSetting)
        delete themeSetting;
    if(label)
        delete label;
    if(label2)
        delete label2;
    if(label3)
        delete label3;
    if(label4)
        delete label4;
    if(plugin)
        delete plugin;
    if(app)
        delete app;
    if(focuswidget)
        delete focuswidget;
    if(pluginnote)
        delete pluginnote;
    if(app1)
        delete app1;
    if(notewidget)
        delete notewidget;
    if(secondwidget)
        delete secondwidget;
    if(threeWidget)
        delete threeWidget;
    stackWidget=nullptr;
    toolbutton_widget=nullptr;
    themeSetting=nullptr;
    label=nullptr;
    label2=nullptr;
    label3=nullptr;
    label4=nullptr;
    plugin=nullptr;
    app=nullptr;
    focuswidget=nullptr;
    pluginnote=nullptr;
    app1=nullptr;
    notewidget=nullptr;
    secondwidget=nullptr;
    threeWidget=nullptr;

}
