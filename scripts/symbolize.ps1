# Resolves RVA offsets from demo.exe.crash.log to symbol names using the PDB.
# Usage: .\symbolize.ps1 -Image x64\Release\demo.exe -Offsets "1B0BE1 1B1DE1 ..."
param(
    [Parameter(Mandatory=$true)][string]$Image,
    [Parameter(Mandatory=$true)][string]$Offsets
)

Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public static class DbgHelp {
    [DllImport("dbghelp.dll", SetLastError=true, CharSet=CharSet.Ansi)]
    public static extern bool SymInitialize(IntPtr hProcess, string UserSearchPath, bool fInvadeProcess);
    [DllImport("dbghelp.dll", SetLastError=true, CharSet=CharSet.Ansi)]
    public static extern ulong SymLoadModule64(IntPtr hProcess, IntPtr hFile, string ImageName, string ModuleName, ulong BaseOfDll, uint DllSize);
    [DllImport("dbghelp.dll", SetLastError=true)]
    public static extern bool SymFromAddr(IntPtr hProcess, ulong Address, out ulong Displacement, ref SYMBOL_INFO Symbol);
    [DllImport("dbghelp.dll")]
    public static extern uint SymSetOptions(uint SymOptions);
    [DllImport("kernel32.dll", SetLastError=true)]
    public static extern IntPtr CreateFile(string lpFileName, uint dwDesiredAccess, uint dwShareMode, IntPtr lpSecurityAttributes, uint dwCreationDisposition, uint dwFlagsAndAttributes, IntPtr hTemplateFile);
    [DllImport("kernel32.dll")]
    public static extern IntPtr GetCurrentProcess();
    [DllImport("kernel32.dll")]
    public static extern bool CloseHandle(IntPtr handle);

    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]
    public struct SYMBOL_INFO {
        public uint SizeOfStruct;
        public uint TypeIndex;
        public ulong Reserved0;
        public ulong Reserved1;
        public uint Index;
        public uint Size;
        public ulong ModBase;
        public uint Flags;
        public ulong Value;
        public ulong Address;
        public uint Register;
        public uint Scope;
        public uint Tag;
        public uint NameLen;
        public uint MaxNameLen;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst=1024)]
        public string Name;
    }
}
"@

$SYMOPT_UNDNAME = 0x2; $SYMOPT_DEFERRED_LOADS = 0x4; $SYMOPT_LOAD_LINES = 0x10
[DbgHelp]::SymSetOptions($SYMOPT_UNDNAME -bor $SYMOPT_DEFERRED_LOADS -bor $SYMOPT_LOAD_LINES) | Out-Null
$hproc = [DbgHelp]::GetCurrentProcess()
if (-not [DbgHelp]::SymInitialize($hproc, $null, $false)) {
    throw "SymInitialize failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
}

# Read the PE image base so we can turn RVA -> absolute address
$base = 0
$lines = dumpbin /headers $Image | Out-String
$m = [regex]::Match($lines, "image base \((?<hex>0x[0-9A-Fa-f]+)")
if ($m.Success) { $base = [Convert]::ToUInt64($m.Groups["hex"].Value, 16) }
Write-Host "Image base: 0x$base"

# Load the PDB with a real file handle (IntPtr.Zero does not work)
# Use unsigned long access mask to avoid signed overflow of 0x80000000
$hFile = [DbgHelp]::CreateFile($Image, [System.UInt32]2147483648, 0x7, [IntPtr]::Zero, 3, 0, [IntPtr]::Zero)
if ($hFile -eq [IntPtr]::Zero) { throw "CreateFile failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())" }
$modBase = [DbgHelp]::SymLoadModule64($hproc, $hFile, $Image, $null, $base, 0)
[DbgHelp]::CloseHandle($hFile) | Out-Null
Write-Host "Module base: 0x$modBase"
if ($modBase -eq 0) { throw "SymLoadModule64 failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())" }

# SymLoadModule64 returns the actual base even when asked to pick one itself;
# always resolve against the returned base, not the dumpbin-parsed one
$base = $modBase

$si = New-Object DbgHelp+SYMBOL_INFO
# SizeOfStruct must be the NATIVE struct size (x64: 88 bytes, with Name[1]).
# Passing the managed marshaled size (which includes the 1024-byte name
# buffer) makes dbghelp reject the call with ERROR_INVALID_PARAMETER (87).
$si.SizeOfStruct = [System.Runtime.InteropServices.Marshal]::SizeOf([type][DbgHelp+SYMBOL_INFO]) - 1024
$si.MaxNameLen = 1023

foreach ($off in ($Offsets -split '[\s,]+' | Where-Object { $_ })) {
    $rva = [Convert]::ToUInt64($off, 16)
    $addr = $base + $rva
    $disp = [System.UInt64]0
    $symOk = [DbgHelp]::SymFromAddr($hproc, $addr, [ref]$disp, [ref]$si)
    if (-not $symOk) {
        $err = [Runtime.InteropServices.Marshal]::GetLastWin32Error()
        Write-Host ("0x{0,-12} <no symbol> (err {1})" -f $off, $err)
    } else {
        Write-Host ("0x{0,-12} {1} + 0x{2:X}" -f $off, $si.Name, $disp)
    }
}