; 该脚本使用 HM VNISEdit 脚本编辑器向导产生

; 安装程序初始定义常量
!define PRODUCT_NAME "ED"
!define PRODUCT_VERSION "33.0"
!define PRODUCT_PUBLISHER "AndyLZA"
!define PRODUCT_WEB_SITE "https://andylza2008.github.io/ED/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\ED.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI.nsh"

; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_ICON "icon.ico"
!define MUI_UNICON "uicon.ico"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME
; 许可协议页面
!insertmacro MUI_PAGE_LICENSE "licence.txt"
; 安装目录选择页面
!insertmacro MUI_PAGE_DIRECTORY
; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
; 安装完成页面
!define MUI_FINISHPAGE_RUN "$INSTDIR\ED.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\ReadMe.txt"
!insertmacro MUI_PAGE_FINISH

; 安装卸载过程页面
!insertmacro MUI_UNPAGE_INSTFILES

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "SimpChinese"

; 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI 现代界面定义结束 ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "ED Setup.exe"
InstallDir "$PROGRAMFILES\ED"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show
BrandingText " "

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "ReadMe.txt"
  CreateDirectory "$SMPROGRAMS\ED"
  CreateShortCut "$SMPROGRAMS\ED\ED.lnk" "$INSTDIR\ED.exe"
  CreateShortCut "$DESKTOP\ED.lnk" "$INSTDIR\ED.exe"
  CreateShortCut "$SMPROGRAMS\ED\Debugger.lnk" "$INSTDIR\Debugger.exe"
  CreateShortCut "$DESKTOP\Debugger.lnk" "$INSTDIR\Debugger.exe"
  File "licence.txt"
  File "icon.ico"
  File "ED.exe"
  File "Debugger.exe"
  File "bk.jpeg"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\ED\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url" "" "$INSTDIR\icon.ico"
  CreateShortCut "$SMPROGRAMS\ED\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\ED.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\ED.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

/******************************
 *  以下是安装程序的卸载部分  *
 ******************************/

Section Uninstall
  ReadEnvStr $R0 UserName
  
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\bk.jpeg"
  Delete "$INSTDIR\Debugger.exe"
  Delete "$INSTDIR\ED.exe"
  Delete "$INSTDIR\icon.ico"
  Delete "$INSTDIR\licence.txt"
  Delete "$INSTDIR\ReadMe.txt"

  Delete "$SMPROGRAMS\ED\Uninstall.lnk"
  Delete "$SMPROGRAMS\ED\Website.lnk"
  Delete "$DESKTOP\Debugger.lnk"
  Delete "$SMPROGRAMS\ED\Debugger.lnk"
  Delete "$DESKTOP\ED.lnk"
  Delete "$SMPROGRAMS\ED\ED.lnk"
  Delete "C:\Users\$R0\.ed\Personalized_History"
  
  RMDir "C:\Users\$R0\.ed"
  
  RMDir "$SMPROGRAMS\ED"

  RMDir "$INSTDIR"


  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "您确实要完全移除 $(^Name) ，及其所有的组件？" IDYES +2
  Abort
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从您的计算机移除。"
FunctionEnd
