[Setup]
OutputBaseFilename=Cabbage64Setup
AppName=Cabbage64
AppVersion=v0.5
DefaultDirName={pf}\Cabbage64
DefaultGroupName=Cabbage64
UninstallDisplayIcon={app}\Cabbage.exe
;OutputDir=userdocs:Inno Setup Examples Output
ChangesEnvironment=yes

[Types]
Name: "standard"; Description: "Standard installation; perfect for most uses"
Name: "minimal"; Description: "Minimal installation: for minimal installations"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "program"; Description: "Program Files, standard binaries and essential Csound dlls"; Types: standard minimal custom; Flags: fixed
;Name: "python"; Description: "Python opcodes, requires Python 2.7 to be installed"; Types: custom

[Files]
;these are the minimal files needed to run Cabbage
Source: "build\Cabbage.exe";              DestDir: "{app}"; Components: program
Source: "build\CabbagePluginSynth.dat";   DestDir: "{app}"; Components: program 
Source: "build\CabbagePluginEffect.dat";  DestDir: "{app}"; Components: program
Source: "build\opcodes.txt";              DestDir: "{app}"; Components: program
Source: "..\..\Examples\*";               DestDir: "{app}\Examples"; Components: program; Flags: recursesubdirs
Source: "..\..\Docs\*";                   DestDir: "{app}\Docs"; Components: program; Flags: recursesubdirs

;Source: "..\..\..\MingwLibs\libwinpthread-1.dll"; DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\libgomp-1.dll";       DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\libgcc_s_dw2-1.dll";  DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\libstdc++-6.dll";     DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\msvcr110.dll";        DestDir: "{app}"; Components: program
Source: "..\..\..\csound\build\csound64.dll";     DestDir: "{app}"; Components: program
Source: "C:\Program Files\Mega-Nerd\libsndfile\bin\libsndfile-1.dll";   DestDir: "{app}"; Components: program

;Regular Csound dlls
Source: "..\..\..\csound\build\libsndfile-1.dll";   DestDir: "{app}"; Components: program

;Special opcodes
;Source: "..\..\..\csound\build\osc.dll";            DestDir: "{app}"; Components: osc
;Source: "..\..\..\csound\build\liblo-7.dll";        DestDir: "{app}"; Components: osc
;Source: "..\..\..\csound\build\py.dll";             DestDir: "{app}"; Components: python  

[Icons]
Name: "{group}\Cabbage64"; Filename: "{app}\Cabbage.exe"

; NOTE: Most apps do not need registry entries to be pre-created. If you
; don't know what the registry is or if you need to use it, then chances are
; you don't need a [Registry] section.

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "CABBAGE_OPCODE_PATH"; ValueData: "{app}"; Check: NeedsAddVariable('{app}')
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{app};{olddata}"; Check: NeedsAddPath('{app}')

[Code]

function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  // look for the path with leading and trailing semicolon
  // Pos() returns 0 if not found
  Result := Pos(';' + Param + ';', ';' + OrigPath + ';') = 0;
end;

function NeedsAddVariable(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'CABBAGE_OPCODE_PATH', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  // look for the path with leading and trailing semicolon
  // Pos() returns 0 if not found
  Result := Pos(';' + Param + ';', ';' + OrigPath + ';') = 0;
end;
