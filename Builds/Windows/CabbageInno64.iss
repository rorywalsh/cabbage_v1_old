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
Source: "build\Cabbage64.exe";              DestDir: "{app}"; Components: program
Source: "build\CabbagePluginSynth.dat";   DestDir: "{app}"; Components: program 
Source: "build\CabbagePluginEffect.dat";  DestDir: "{app}"; Components: program
Source: "build\opcodes.txt";              DestDir: "{app}"; Components: program
Source: "..\..\Examples\*";               DestDir: "{app}\Examples"; Components: program; Flags: recursesubdirs
Source: "..\..\Docs\*";                   DestDir: "{app}\Docs"; Components: program; Flags: recursesubdirs
Source: "..\..\..\csoundDocs\*";          DestDir: "{app}\csoundDocs"; Components: program; Flags: recursesubdirs

Source: "C:\mingw32\bin\libgcc_s_dw2-1.dll";  DestDir: "{app}"; Components: program
Source: "C:\mingw64\bin\libstdc++-6.dll";     DestDir: "{app}"; Components: program

;Source: "..\..\..\MingwLibs\libwinpthread-1.dll"; DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\libgomp-1.dll";       DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\libgcc_s_dw2-1.dll";  DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\libstdc++-6.dll";     DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\msvcr110.dll";        DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\csound64.dll";     DestDir: "{app}"; Components: program
Source: "C:\Program Files\Mega-Nerd\libsndfile64\bin\libsndfile-1.dll";   DestDir: "{app}"; Components: program

;Regular Csound dlls
;Source: "..\..\..\csound64\build\py.dll";             DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\signalflowgraph.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\serial.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\scansyn.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\doppler.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\fractalnoise.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\fareygen.dll"; DestDir: "{app}"; Components: program
;Source: "..\..\..\csound64\build\osc.dll";    DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\chua.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\mixer.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\ampmidid.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\cs_date.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\cellular.dll"; DestDir: "{app}"; Components: program
Source: "..\..\..\csound64\build\platerev.dll"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\Cabbage64"; Filename: "{app}\Cabbage64.exe"

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
