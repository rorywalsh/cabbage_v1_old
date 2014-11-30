; -- Example3.iss --
; Same as Example1.iss, but creates some registry entries too.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
OutputBaseFilename=Cabbage32Setup
AppName=Cabbage32
AppVersion=v0.5.13
DefaultDirName={pf}\Cabbage32
DefaultGroupName=Cabbage32
UninstallDisplayIcon={app}\Cabbage.exe
;OutputDir=userdocs:Inno Setup Examples Output
ChangesEnvironment=true

[Types]
Name: "full"; Description: "Full install"
Name: "minimal"; Description: "Minimal installation, includes only the Csound base dlls. "
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "program"; Description: "Program Files, standard binaries and essential Csound dlls"; Types: full minimal custom; Flags: fixed
Name: "csound"; Description: "Csound program files and audio/MIDI interfaces, for standalone Csound use"; Types: custom  full
Name: "csoundOpcodes"; Description: "Csound plugin opcodes"; Types: custom full
Name: "python"; Description: "Python opcodes, requires Python 2.7 to be installed"; Types: custom  full
Name: "csound4Max"; Description: "csound~ object for Max/MSP/M4L"; Types: custom  full
;C:\Users\rory\Documents\sourcecode\cabbageaudio\csound\build\frontends

[Files]
;these are the minimal files needed to run Cabbage
Source: "build\Cabbage.exe";                                  DestDir: "{app}"; Components: program
Source: "build\CabbagePluginSynth.dat";                       DestDir: "{app}"; Components: program 
Source: "build\CabbagePluginEffect.dat";                      DestDir: "{app}"; Components: program
Source: "build\opcodes.txt";                                  DestDir: "{app}"; Components: program
Source: "..\..\..\csound\build\frontends\max_csound_tilde\*";   DestDir: "{app}\csound4Max"; Components: csound4Max
Source: "..\..\..\csound\frontends\max_csound_tilde\examples\*";   DestDir: "{app}\csound4Max"; Components: csound4Max


Source: "..\..\Examples\*";               DestDir: "{app}\Examples"; Components: program; Flags: recursesubdirs
Source: "..\..\Docs\*";                   DestDir: "{app}\Docs"; Components: program; Flags: recursesubdirs
Source: "..\..\..\csoundDocs\*";                   DestDir: "{app}\csoundDocs"; Components: program; Flags: recursesubdirs

;Source: "..\..\..\MingwLibs\libwinpthread-1.dll"; DestDir: "{app}"; Components: program
;Source: "..\..\..\MingwLibs\libgomp-1.dll";       DestDir: "{app}"; Components: program
Source: "C:\mingw32\bin\libgcc_s_dw2-1.dll";  DestDir: "{app}\csound"; Components: program
Source: "C:\mingw32\bin\libstdc++-6.dll";     DestDir: "{app}\csound"; Components: program
;Source: "..\..\..\MingwLibs\msvcr110.dll";        DestDir: "{app}"; Components: program
Source: "..\..\..\csound\build\csound64.dll";     DestDir: "{app}"; Components: program
Source: "C:\Program Files (x86)\Mega-Nerd\libsndfile32\bin\libsndfile-1.dll";   DestDir: "{app}"; Components: program
Source: "..\..\..\csound\build\signalflowgraph.dll";   DestDir: "{app}"; Components: program

;Csound binaries
Source: "..\..\..\csound\build\csound.exe";             DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\libcsound64.dll.a";             DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\libcsnd6.dll.a";             DestDir: "{app}\csound"; Components: csound
Source: "C:\Program Files (x86)\Mega-Nerd\libsndfile32\bin\libsndfile-1.dll";   DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\csnd6.jar";             DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\test.csd";             DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\csnd6.dll";             DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\libportaudio-2.dll";     DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\pmidi.dll";              DestDir: "{app}\csound"; Components: csound
Source: "C:/Users/rory/Documents/sourcecode/portmidi/build/libportmidi.dll";              DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\ipmidi.dll";             DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\rtpa.dll";               DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\rtwinmm.dll";            DestDir: "{app}\csound"; Components: csound
;Source: "..\..\..\csound\build\virtual.dll";            DestDir: "{app}"; Components: csound
Source: "C:\Users\rory\Documents\sourcecode\liblo-0.28\src\.libs\liblo-7.dll";        DestDir: "{app}\csound"; Components: csoundOpcodes
Source: "..\..\..\csound\build\_jcsound6.dll";            DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\py.dll";             DestDir: "{app}\csound"; Components: python  
Source: "..\..\..\csound\build\signalflowgraph.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\serial.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\scansyn.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\doppler.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\fractalnoise.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\fareygen.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\osc.dll";     DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\chua.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\mixer.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\ampmidid.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\cs_date.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\cellular.dll"; DestDir: "{app}\csound"; Components: csound
Source: "..\..\..\csound\build\platerev.dll"; DestDir: "{app}\csound"; Components: csound

;Source: "..\..\..\csound\build\widgets.dll";            DestDir: "{app}"; Components: csound

;Csound opcodes
Source: "C:\Users\rory\Documents\sourcecode\liblo-0.28\src\.libs\liblo-7.dll";        DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\py.dll";             DestDir: "{app}"; Components: python  
Source: "..\..\..\csound\build\signalflowgraph.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\serial.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\scansyn.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\doppler.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\fractalnoise.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\fareygen.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\chua.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\mixer.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\ampmidid.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\cs_date.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\cellular.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\platerev.dll"; DestDir: "{app}"; Components: csoundOpcodes
Source: "..\..\..\csound\build\osc.dll";     DestDir: "{app}"; Components: csoundOpcodes

[Icons]
Name: "{group}\Cabbage32"; Filename: "{app}\Cabbage.exe"
; NOTE: Most apps do not need registry entries to be pre-created. If you
; don't know what the registry is or if you need to use it, then chances are
; you don't need a [Registry] section.

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "CABBAGE_OPCODE_PATH"; ValueData: "{app}"; Check: NeedsAddVariable('{app}')
;Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{app};{olddata}"; Check: NeedsAddPath('{app}')


[Tasks]
Name: modifypath; Description: Add Cabbage path to system path (recommended);  GroupDescription: "Group";
Name: modifypath; Description: "Add OPCODE6DIR64 to system (recommended if no other version of Csound is installed. If a version of Csound is already installed this might break that install)";  GroupDescription: "Group"; 


[Code]
#define SystemEnvRegKey "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"

function SetEnvSystem(const Name, Value: string; Refresh: Boolean): Boolean;
	begin
	  Result := RegWriteExpandStringValue(HKLM, '{#SystemEnvRegKey}', Name, Value);
	end;

function NextButtonClick(CurPageID: Integer): Boolean;
	begin
	  Result := True;
	  if CurPageID = wpSelectTasks then
	  begin
    	if WizardForm.TasksList.Checked[2] then
      	SetEnvSystem('OPCODE6DIR64', ExpandConstant('{app}'), True)
	  end;
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

const
    ModPathName = 'modifypath';
    ModPathType = 'system';

function ModPathDir(): TArrayOfString;
	begin
	    setArrayLength(Result, 1)
	    Result[0] := ExpandConstant('{app}');
	end;
#include "modpath.iss"