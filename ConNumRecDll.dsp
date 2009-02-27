# Microsoft Developer Studio Project File - Name="ConNumRecDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ConNumRecDll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ConNumRecDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ConNumRecDll.mak" CFG="ConNumRecDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ConNumRecDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ConNumRecDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ConNumRecDll - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=Copy Release\ConNumRecDll.dll ..\DemoExe\ConNumRecDll.dll	Copy Release\ConNumRecDll.lib ..\DemoExe\ConNumRecDll.lib	Copy NumRec.h ..\DemoExe\NumRec.h	Copy NumRec_C.h ..\DemoExe\NumRec_C.h	Copy Release\ConNumRecDll.dll .\ConNumRecDll.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ConNumRecDll - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/ConNumRecDllD.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=Copy Debug\ConNumRecDllD.dll ..\DemoExe\ConNumRecDllD.dll	Copy Debug\ConNumRecDllD.lib ..\DemoExe\ConNumRecDllD.lib	Copy NumRec.h ..\DemoExe\NumRec.h	Copy NumRec_C.h ..\DemoExe\NumRec_C.h	Copy Debug\ConNumRecDllD.dll .\ConNumRecDllD.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ConNumRecDll - Win32 Release"
# Name "ConNumRecDll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BfPropa.cpp
# End Source File
# Begin Source File

SOURCE=.\CharCutFuc.cpp
# End Source File
# Begin Source File

SOURCE=.\ConNumChecker.cpp
# End Source File
# Begin Source File

SOURCE=.\ConNumProc.cpp
# End Source File
# Begin Source File

SOURCE=.\ConNumRecDll.cpp
# End Source File
# Begin Source File

SOURCE=.\ConNumRecDll.def
# End Source File
# Begin Source File

SOURCE=.\ConNumRecDll.rc
# End Source File
# Begin Source File

SOURCE=.\DongleCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\Featurefusion.cpp
# End Source File
# Begin Source File

SOURCE=.\IMGFUC.cpp
# End Source File
# Begin Source File

SOURCE=.\NumRec.cpp
# End Source File
# Begin Source File

SOURCE=.\PlateRecFuc.cpp
# End Source File
# Begin Source File

SOURCE=.\Profile1.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BfPropa.h
# End Source File
# Begin Source File

SOURCE=.\CharCutFuc.h
# End Source File
# Begin Source File

SOURCE=.\ConNumChecker.h
# End Source File
# Begin Source File

SOURCE=.\ConNumProc.h
# End Source File
# Begin Source File

SOURCE=.\DongleCheck.h
# End Source File
# Begin Source File

SOURCE=.\Featurefusion.h
# End Source File
# Begin Source File

SOURCE=.\IMGFUC.h
# End Source File
# Begin Source File

SOURCE=.\NumRec.h
# End Source File
# Begin Source File

SOURCE=.\NumRec_C.h
# End Source File
# Begin Source File

SOURCE=.\PlateRecFuc.h
# End Source File
# Begin Source File

SOURCE=.\Profile1.h
# End Source File
# Begin Source File

SOURCE=.\Projection.h
# End Source File
# Begin Source File

SOURCE=.\Rbcommon.h
# End Source File
# Begin Source File

SOURCE=.\Rbioeng.h
# End Source File
# Begin Source File

SOURCE=.\Rbstatus.h
# End Source File
# Begin Source File

SOURCE=.\Rbtypes.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ConNumRecDll.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Rbioew32.lib
# End Source File
# Begin Source File

SOURCE=.\Spromeps.lib
# End Source File
# Begin Source File

SOURCE=.\Sx32w.lib
# End Source File
# End Target
# End Project
