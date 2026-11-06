@echo off
rem End-to-end artifact test: extract the distribution zip inside the sandbox
rem (like a player would), run the game from a local path, and copy any crash
rem log back to the mapped dist folder so it shows up on the host.

mkdir "C:\Users\WDAGUtilityAccount\Desktop\game" 2>nul
tar -xf "C:\Users\WDAGUtilityAccount\Desktop\dist\DX9GF-Demo.zip" -C "C:\Users\WDAGUtilityAccount\Desktop\game"
cd /d "C:\Users\WDAGUtilityAccount\Desktop\game"
demo.exe
if exist demo.exe.crash.log copy /y demo.exe.crash.log "C:\Users\WDAGUtilityAccount\Desktop\dist\"
pause
