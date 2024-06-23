[Setup]
MinVersion=0,5.01.2600
AppCopyright=Thomas Moor
AppName=FlexFact
AppVersion=0.8x
AppVerName=FlexFact 0.8x
PrivilegesRequired=none
ShowLanguageDialog=no
LanguageDetectionMethod=uilanguage
AppID={{21DE0516-4938-4621-9995-FC451907FC14}}
UsePreviousAppDir=false
DefaultDirName={sd}\FAUDES\FlexFact
DefaultGroupName=FAUDES
OutputDir=.\
SetupIconFile= icons\icon_win_inst.ico


[Dirs]
Name: {app}\Examples


[Files]
Source: build\FlexFact.exe; DestDir: {app}
Source: build\simfaudes.exe; DestDir: {app}
Source: build\iomonitor.exe; DestDir: {app}
Source: build\*.dll; DestDir: {app}
Source: build\libfaudes.rti; DestDir: {app}
Source: examples\distsave\*; DestDir: {app}\Examples;  Flags: recursesubdirs createallsubdirs;
Source: LICENSE.txt; DestDir: {app}
Source: README.txt; DestDir: {app}


[Icons]
Name: "{group}\FlexFact"; Filename: "{app}\FlexFact.exe"
Name: "{group}\Uninstall FlexFact"; Filename: "{uninstallexe}"
Name: "{commondesktop}\FlexFact"; Filename: "{app}\FlexFact.exe" 
