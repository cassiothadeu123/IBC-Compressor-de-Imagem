#ifndef StageDir
  #error StageDir must be provided with /DStageDir=...
#endif
#ifndef OutputDir
  #error OutputDir must be provided with /DOutputDir=...
#endif
#ifndef AppVersion
  #define AppVersion "0.1.0"
#endif

[Setup]
AppId={{A8668E30-8053-48FA-A55B-E7344345A296}
AppName=IBC Compressor de Imagem
AppVersion={#AppVersion}
AppPublisher=Instituto Biofísica da Consciência
DefaultDirName={autopf}\IBC Compressor de Imagem
DefaultGroupName=IBC Compressor de Imagem
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename=IBC-Compressor-de-Imagem-Windows-x64-Setup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
UninstallDisplayIcon={app}\IBCImageCompressor.exe

[Files]
Source: "{#StageDir}\bin\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\IBC Compressor de Imagem"; Filename: "{app}\IBCImageCompressor.exe"
Name: "{autodesktop}\IBC Compressor de Imagem"; Filename: "{app}\IBCImageCompressor.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Criar atalho na área de trabalho"; GroupDescription: "Atalhos adicionais:"

[Run]
Filename: "{app}\IBCImageCompressor.exe"; Description: "Abrir IBC Compressor de Imagem"; Flags: nowait postinstall skipifsilent
