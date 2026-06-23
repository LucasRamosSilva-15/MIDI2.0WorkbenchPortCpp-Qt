param (
    [string]$Version = "v1.0.0",
    [switch]$EnableRtMidi,
    [switch]$EnableWms
)

$ErrorActionPreference = "Stop"

if ($EnableRtMidi -and $EnableWms) {
    Write-Host "Erro: As flags -EnableRtMidi e -EnableWms sao mutuamente exclusivas para manter a pureza das builds. Escolha apenas uma." -ForegroundColor Red
    exit 1
}

if ($EnableRtMidi) {
    $buildDir = "build-rtmidi"
    $distName = "MidiUmpAnalyzer-$Version-windows-x64-rtmidi"
    
    # Se a pasta não existe, configura
    if (-not (Test-Path $buildDir)) {
        Write-Host "`n--- Garantindo configuracao com RtMidi ---"
        $cmakeArgs = @("-G", "Visual Studio 17 2022", "-A", "x64", "-B", $buildDir, "-DENABLE_RTMIDI=ON")
        if ($env:Qt6_DIR) {
            $cmakeArgs += "-DCMAKE_PREFIX_PATH=$env:Qt6_DIR"
        }
        & cmake $cmakeArgs
    }
} elseif ($EnableWms) {
    $buildDir = "build-wms"
    $distName = "MidiUmpAnalyzer-$Version-windows-x64-wms"
    
    # Se a pasta não existe, configura
    if (-not (Test-Path $buildDir)) {
        Write-Host "`n--- Garantindo configuracao com Windows MIDI Services ---"
        $cmakeArgs = @(
            "-G", "Visual Studio 17 2022",
            "-A", "x64",
            "-B", $buildDir,
            "-DENABLE_WINDOWS_MIDI_SERVICES=ON",
            "-DENABLE_WINDOWS_MIDI_SERVICES_UI_INTEGRATION_RESEARCH=ON",
            "-DENABLE_WINDOWS_MIDI_SERVICES_BACKEND_INTEGRATION_PREP=ON",
            "-DENABLE_WINDOWS_MIDI_SERVICES_BACKEND_EXPERIMENTAL_CAPTURE=ON",
            "-DENABLE_WINDOWS_MIDI_SERVICES_REAL_WINRT_ACTIVATION_ATTEMPT=ON",
            "-DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake",
            "-DWINDOWS_MIDI_SERVICES_SDK_ROOT=C:\vcpkg\installed\x64-windows"
        )
        if ($env:Qt6_DIR) {
            $cmakeArgs += "-DCMAKE_PREFIX_PATH=$env:Qt6_DIR"
        }
        & cmake $cmakeArgs
    }
} else {
    $buildDir = "build"
    $distName = "MidiUmpAnalyzer-$Version-windows-x64"
}

$distPath = "dist\$distName"
$zipName = "dist\$distName.zip"

Write-Host "--- Parando aplicativo caso esteja aberto ---"
Stop-Process -Name MidiUmpAnalyzer -ErrorAction SilentlyContinue

Write-Host "`n--- Garantindo compilacao em modo Release ---"
cmake --build $buildDir --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "Falha na compilacao CMake." -ForegroundColor Red
    exit 1
}

Write-Host "`n--- Limpando pasta de distribuicao ---"
if (Test-Path $distPath) { Remove-Item -Recurse -Force $distPath }
if (Test-Path $zipName) { Remove-Item -Force $zipName }
New-Item -ItemType Directory -Force -Path $distPath | Out-Null

Write-Host "`n--- Copiando Executavel Principal ---"
Copy-Item "$buildDir\Release\MidiUmpAnalyzer.exe" -Destination $distPath

Write-Host "--- Procurando dependencias nativas do Windows MIDI Services ---"
$midiDll = Get-ChildItem -Path . -Filter "Microsoft.Windows.Devices.Midi2.dll" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1

if (-not $midiDll -and (Test-Path "C:\vcpkg")) {
    Write-Host "Nao encontrada no projeto. Procurando no diretorio do vcpkg (C:\vcpkg)..."
    $midiDll = Get-ChildItem -Path "C:\vcpkg\installed\x64-windows" -Filter "Microsoft.Windows.Devices.Midi2.dll" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
}

if ($midiDll) {
    Write-Host "DLL nativa encontrada em: $($midiDll.FullName)"
    Copy-Item -Path $midiDll.FullName -Destination $distPath -Force
    Write-Host "Microsoft.Windows.Devices.Midi2.dll copiada com sucesso para $distPath !"
} else {
    Write-Host "CRÍTICO: Microsoft.Windows.Devices.Midi2.dll NAO ENCONTRADA em nenhum lugar do projeto ou no vcpkg!" -ForegroundColor Red
}

if ($EnableRtMidi) {
    Write-Host "`n--- Copiando DLL do RtMidi (se existir) ---"
    if (Test-Path "$buildDir\_deps\rtmidi-build\Release\rtmidi.dll") {
        Copy-Item "$buildDir\_deps\rtmidi-build\Release\rtmidi.dll" -Destination $distPath
    }
}

Write-Host "`n--- Copiando Dependencias Qt (windeployqt ja foi rodado pelo CMake) ---"
# Copia todas as DLLs geradas
Get-ChildItem "$buildDir\Release\*.dll" | Copy-Item -Destination $distPath

# Copia subpastas de plugins criadas pelo windeployqt
$qtFolders = @("platforms", "styles", "iconengines", "imageformats", "networkinformation", "tls")
foreach ($folder in $qtFolders) {
    if (Test-Path "$buildDir\Release\$folder") {
        Copy-Item -Recurse "$buildDir\Release\$folder" -Destination $distPath
    }
}

Write-Host "`n--- Copiando Documentacao e Lancador ---"
Copy-Item "README.md" -Destination $distPath
Copy-Item "TESTS.md" -Destination $distPath
Copy-Item "CHANGELOG.md" -Destination $distPath
Copy-Item "QUICK_START.txt" -Destination $distPath
Copy-Item "Start-MidiUmpAnalyzer.bat" -Destination $distPath
Copy-Item -Recurse "samples" -Destination $distPath

Write-Host "`n--- Zipando Pacote Final ---"
Compress-Archive -Path "$distPath\*" -DestinationPath $zipName -Force

Write-Host "`n[SUCESSO] Empacotamento finalizado!" -ForegroundColor Green
Write-Host "Arquivo gerado: $zipName" -ForegroundColor Cyan
