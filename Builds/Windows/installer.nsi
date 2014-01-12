# Cabbage installer and uninstaller script
# Rory Walsh Copright, 2013

!define APPNAME "Cabbage"
!define COMPANYNAME "Cabbage Audio"
!define DESCRIPTION "Audio plugin and standalone development toolkit"
RequestExecutionLevel admin ;Require admin rights on NT6+ (When UAC is turned on)
InstallDir "$PROGRAMFILES\${COMPANYNAME}\${APPNAME}"

# This will be in the installer/uninstaller's title bar
Name "${COMPANYNAME} - ${APPNAME}"
Icon "logo.ico"
outFile "CabbageInstaller.exe"
 
!include LogicLib.nsh
; Include LogicLibrary
!include "LogicLib.nsh"
!include "EnvVarUpdate.nsh"

; Modern interface settings
!include "MUI2.nsh"
!define MUI_LANGUAGE
!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE "Welcome to the Cabbage installer"
!define MUI_WELCOMEPAGE_TEXT "This Wizard will install the Cabbage audio instrument development environment to your system. Note that this is alpha softare and is for testing purposes only. This software and can only used in production at the users own risk.$\r$\n$\r$\n$\r$\nCabbage Audio accepts no reponsibility for any weirdness.."


# Just three pages - license agreement, install location, and installation
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\GNUGeneralPublicLicense.txt"
!insertmacro MUI_PAGE_DIRECTORY 
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Language
 
  !insertmacro MUI_LANGUAGE "English"
 
!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights required!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend
 
function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd
 
section "install"
	# Files for the install directory - to build the installer, these should be in the same directory as the install script (this file)
	setOutPath $INSTDIR
	# Files added here should be removed by the uninstaller (see section "uninstall")
	File "build\Cabbage.exe"
	File "logo.ico"
	File /r "..\..\Examples"
	File /r "..\..\Docs"
	File /r "..\..\..\CsoundLibs\CsoundPlugins"
	File "build\CabbagePluginSynth.dat"
	File "build\CabbagePluginEffect.dat"
	File "build\opcodes.txt"
	File "..\..\..\CsoundLibs\csound64.dll"
	File "..\..\..\CsoundLibs\libsndfile-1.dll"
	File "..\..\..\MingwLibs\libwinpthread-1.dll"
	File "..\..\..\MingwLibs\libgomp-1.dll"
	File "..\..\..\MingwLibs\libgcc_s_dw2-1.dll"
	File "..\..\..\MingwLibs\libstdc++-6.dll"
 
	# Uninstaller - See function un.onInit and section "uninstall" for configuration
	writeUninstaller "$INSTDIR\uninstall.exe"
 
	# Start Menu
	createDirectory "$SMPROGRAMS\${COMPANYNAME}"
	createShortCut "$SMPROGRAMS\${COMPANYNAME}\uninstall.lnk" "$INSTDIR\uninstall.exe"
	createShortCut "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk" "$INSTDIR\Cabbage.exe" "" "$INSTDIR\logo.ico"


	${EnvVarUpdate} $0 "PATH" "P" "HKLM" "$INSTDIR"  ;Prepend path so we don't confuse a previously installed Csound 
 
sectionEnd
 
# Uninstaller
 
function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanantly remove ${APPNAME}?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
functionEnd
 
section "uninstall"
 
	# Remove Start Menu launcher
	delete "$SMPROGRAMS\${COMPANYNAME}\Cabbage.lnk"
	delete "$SMPROGRAMS\${COMPANYNAME}\uninstall.lnk"
	# Try to remove the Start Menu folder - this will only happen if it is empty
	rmDir "$SMPROGRAMS\${COMPANYNAME}"
 
	# Remove files
	delete $INSTDIR\Cabbage.exe
	delete $INSTDIR\logo.ico
	delete $INSTDIR\csound64.dll
	delete $INSTDIR\libsndfile-1.dll
	delete $INSTDIR\CabbagePluginEffect.dat
	delete $INSTDIR\CabbagePluginSynth.dat
	delete $INSTDIR\opcodes.txt
	delete $INSTDIR\libwinpthread-1.dll
	delete $INSTDIR\libgomp-1.dll
	delete $INSTDIR\libgcc_s_dw2-1.dll
	delete $INSTDIR\libstdc++-6.dll

	# Always delete uninstaller as the last action
	delete $INSTDIR\uninstall.exe
 
 	rmDir /r $INSTDIR\Examples
 	rmDir /r $INSTDIR\Docs
 	rmDir /r $INSTDIR\CsoundPlugins

 	${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"  ;remove

	# Try to remove the install directory - this will only happen if it is empty
	rmDir $INSTDIR

sectionEnd