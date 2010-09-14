; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; Customize the Qt dir installation and version
[Code]
#define QT_DIR 	 "C:\Qt\2010.04\qt\bin"
#define VERSION  "2.0.0"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{66E297AA-D2C9-4060-851F-3B8B9383ACF2}
AppName=TarotClub {#VERSION}
AppVerName=TarotClub-{#VERSION}
AppPublisher=TarotClub Corp.
AppPublisherURL=http://www.tarotclub.fr
AppSupportURL=http://www.tarotclub.fr
AppUpdatesURL=http://www.tarotclub.fr
DefaultDirName={pf}\TarotClub
DefaultGroupName=TarotClub
LicenseFile=..\COPYING-FR
OutputDir=.
OutputBaseFilename=TarotClub-{#VERSION}-Setup
Compression=lzma
SolidCompression=true
WizardImageFile=compiler:wizmodernimage-IS.bmp
WizardSmallImageFile=compiler:wizmodernsmallimage-IS.bmp
SetupIconFile=install.ico

[Languages]
Name: french; MessagesFile: compiler:Languages\French.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: ..\TarotClub-build-desktop\bin\TarotClub.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\src\data\cards\default\*; DestDir: {app}\default; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\BUILD; DestDir: {app}; Flags: ignoreversion
Source: ..\COPYING; DestDir: {app}; Flags: ignoreversion
Source: ..\COPYING-FR; DestDir: {app}; Flags: ignoreversion
Source: ..\HISTORY; DestDir: {app}; Flags: ignoreversion
Source: ..\INSTALL; DestDir: {app}; Flags: ignoreversion
Source: ..\README; DestDir: {app}; Flags: ignoreversion
Source: ..\src\data\fonts\kanzlei.ttf; DestDir: {app}; Flags: ignoreversion
Source: ..\src\data\fonts\kanzlei.ttf; DestDir: {fonts}; FontInstall: Kanzlei; Flags: onlyifdoesntexist uninsneveruninstall
Source: {#QT_DIR}\QtCore4.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\QtSvg4.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\mingwm10.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\QtGui4.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\QtXml4.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\QtNetwork4.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\libgcc_s_dw2-1.dll; DestDir: {app}; Flags: ignoreversion

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: {group}\TarotClub; Filename: {app}\TarotClub.exe; WorkingDir: {app}
Name: {group}\{cm:ProgramOnTheWeb,TarotClub}; Filename: http://www.tarotclub.fr
Name: {group}\{cm:UninstallProgram,TarotClub}; Filename: {uninstallexe}
Name: {commondesktop}\TarotClub; Filename: {app}\TarotClub.exe; Tasks: desktopicon; WorkingDir: {app}
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\TarotClub; Filename: {app}\TarotClub.exe; Tasks: quicklaunchicon; WorkingDir: {app}

[Run]
Filename: {app}\TarotClub.exe; Description: {cm:LaunchProgram,TarotClub}; Flags: nowait postinstall skipifsilent
