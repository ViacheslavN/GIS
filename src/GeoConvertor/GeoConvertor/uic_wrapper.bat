@echo off
SetLocal EnableDelayedExpansion
(set PATH=D:\ThirdParty\QT\QT-5.8.0.build.2015.x64.static\qtbase\bin;!PATH!)
if defined QT_PLUGIN_PATH (
    set QT_PLUGIN_PATH=D:\TEST\QT\QTBASE\plugins;!QT_PLUGIN_PATH!
) else (
    set QT_PLUGIN_PATH=D:\TEST\QT\QTBASE\plugins
)
D:\ThirdParty\QT\QT-5.8.0.build.2015.x64.static\qtbase\bin\uic.exe %*
EndLocal
