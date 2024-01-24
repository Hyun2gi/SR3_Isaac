rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"

rmdir /s /q .\Client\Include\Debug
rmdir /s /q .\Client\Include\ipch
rmdir /s /q .\Client\Include\Release
rmdir /s /q .\Client\Include\x64

rmdir /s /q .\Tools\Include\Debug
rmdir /s /q .\Tools\Include\ipch
rmdir /s /q .\Tools\Include\Release
rmdir /s /q .\Tools\Include\x64

rmdir /s /q .\Engine\System\Include\Debug
rmdir /s /q .\Engine\Utility\Include\Debug

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb

del /f /q .\Tools\Bin\Tools.exe
del /f /q .\Tools\Bin\Tools.ilk
del /f /q .\Tools\Bin\Tools.pdb
del /f /q Frame142.VC.db
