# 项目指南

## 项目之外的相关资源（重要）

- [贡献准则](https://github.com/Yue-Lan/peony-qt_development_document/blob/master/contribution-criteria.md)
> 如果你想要参与peony-qt的开发，你需要牢记贡献准则中的要求，我希望peony-qt保持它的健壮性以及可维护性，并且足够“简单”，永远不要为了仅解决一个或几个问题而忽视了重要的原则问题。

- [gitbook文档](https://github.com/Yue-Lan/peony-qt_development_document)

> 这个文档是我之前站在开发者的角度写的一些随笔性质的记录
- [测试用例源代码](https://github.com/Yue-Lan/libpeony-qt-development-examples)
> 这些图形界面的测试用例是用来展示目前的工作进展的，目前我的框架中后端部分的代码占了大约95%，和GUI相关的部分我会在整个后端完善稳定之后再按照UKUI3.0文件管理器的设计稿实现，你可以从example的子项目中看到各个已实现功能的测试用例

## 如何了解本项目？

关于peony-qt，我要做的其实上只是一个名为UKUI3.0文件管理器的桌面应用而已，既不深奥也不高大上，也不是什么开辟式的创新工作，至于这一块怎么吹交给这一方面的人才去弄吧。虽然实现它的过程对于用户和旁观者来说真的无所谓，但是这对于一个开发人员是无比重要的一环，所有的一切都是为了自己的方便而决定的，我在总结了peony的一些坑之后，考量了各方文件管理器的设计，最终才选择了自己构建libpeony-qt的框架作为peony-qt的实现手段。

所有程序员应该都有一个梦想——一次编码，终身受用，当然我也有，我认为一个程序员最需要做到的就是节约时间，同样一周5天的工作日，如果能在一天之内干完这一周的工作，那么就意味着至多有4天的工作时间可以自由支配，不管是继续工作提升kpi还是钻研技术，再或者摸摸鱼，这些选择都是极其诱人的。

其实我目前对kpi根本没有什么想法，这可能也跟我现在我投身的是社区工作有关，那么对于技术的钻研还是摸鱼这两项呢？毋庸置疑是渴望的，而这一切都需要解放我自己的生产力。我在现在的peony上完全看不到希望，在peony-qt的开发过程中可能还能看到一丝曙光；实话说，用qt重写什么应用不是我的目标，只是结合现在我的工作背景来说，它是实现我野望的必要一步。

稍微有点跑题了，但是了解一个开发人员的心路历程应该也是了解它项目的一个切入点，不是吗？回归正题——要了解这个项目，光是我嘴上说说肯定是不够的，虽然我也写了不少markdown帮助大家去理解这个项目，但也未必能够让大家完全掌握的住它。

不管怎么样，我认为直接从本项目的源代码开始看起是不合理的。虽然我的README写的不太好，又有不少是Chinese English的，但是我还是建议首先去读一读它们。除了上面的资源之外，在本项目里面，许多子文件夹下也都写了关于该目录下相关内容的README文件。另外，如果实在不想看我写的文档的话，也可以直接从测试用例源码看起，对于有一定开发经验的人来说，这些例子都很简单。

要想深入了解libpeony-qt的框架的话，我建议结合代码、测试用例、所有markdown文档以及我提供的doxygen模板去分析。

## 如何去衡量工作的进展

目前的peony-qt项目正处于UI层重构的阶段，大家可以在build之后，执行./src/peony-qt查看，或者使用qt-creator构建项目然后选择src运行（默认运行的二进制文件为file-operation-test）

## 构建&测试

### 条件
1、系统要求：Ubuntu/UbuntuKylin 19.10
2、编译依赖： libglib2.0-dev， qt-default(>=5.12), libqt5x11extras5-dev
3、推荐： qt5-gtk2-platformtheme（针对gtk桌面环境，提供系统主题支持）

### 步骤(以优麒麟为例)
> sudo apt install libglib2.0-dev libqt5x11extras5-dev qt5-default

> git clone https://github.com/explorer-cs/peony-qt.git

> cd peony-qt && mkdir build && cd build

> qmake ..

> make

> ./src/peony-qt
