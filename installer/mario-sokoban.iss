; Inno Setup script for Mario Sokoban (Inno Setup 6 or 7).
;
; It packages the folder produced by the CMake build (build\package by
; default). Build the game in Release first, then from the repository root:
;   iscc installer\mario-sokoban.iss
; Optional defines: /DAppVersion=1.6 /DSourceDir=<package folder> /DOutputDir=<folder>
; The GitHub workflow passes the version taken from the release tag.

#ifndef AppVersion
  #define AppVersion "1.6"
#endif
#ifndef SourceDir
  #define SourceDir "..\build\package"
#endif
#ifndef OutputDir
  #define OutputDir "..\build\installer"
#endif
#define AppName "Mario Sokoban"
#define AppExe "mario-sokoban.exe"
#define SetupName "setup_mario_sokoban"

[Setup]
; AppId identifies the application for upgrades and uninstall: keep it unchanged.
AppId={{A66AE347-E198-419A-9B99-D93A970EA031}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher=Rémi Fache
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
AllowNoIcons=yes
InfoAfterFile={#SourceDir}\règles.txt
OutputDir={#OutputDir}
OutputBaseFilename={#SetupName}-{#AppVersion}
SetupIconFile=..\caisse.ico
UninstallDisplayIcon={app}\{#AppExe}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Dirs]
; The game writes its levels, best times and medals next to the executable:
; let every user modify these folders, even under Program Files.
Name: "{app}\data"; Permissions: users-modify
Name: "{app}\niveaux perso"; Permissions: users-modify

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Excludes: "\niveaux perso\*,\data\*"; Flags: ignoreversion recursesubdirs createallsubdirs
; Personal levels are user data: never overwrite or remove them on upgrade/uninstall.
Source: "{#SourceDir}\niveaux perso\*"; DestDir: "{app}\niveaux perso"; Flags: onlyifdoesntexist uninsneveruninstall; Permissions: users-modify

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppExe}"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\{#AppExe}"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExe}"; Description: "{cm:LaunchProgram,{#AppName}}"; WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent
