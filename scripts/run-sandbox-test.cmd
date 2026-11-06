@echo off
rem Fast sandbox test: copy the mapped game folder to a local path first.
rem D3DX texture loading gets "Access is denied" when reading assets straight
rem from a mapped folder (sandbox filesystem filter quirk), so the game must
rem run from a real local directory. Crash logs are copied back to the mapped
rem folder (read-write) so they show up on the host automatically.

xcopy /e /i /y "C:\Users\WDAGUtilityAccount\Desktop\DX9GF-Demo" "C:\Users\WDAGUtilityAccount\Desktop\game" >nul
cd /d "C:\Users\WDAGUtilityAccount\Desktop\game"
demo.exe
if exist demo.exe.crash.log copy /y demo.exe.crash.log "C:\Users\WDAGUtilityAccount\Desktop\DX9GF-Demo\"
pause
