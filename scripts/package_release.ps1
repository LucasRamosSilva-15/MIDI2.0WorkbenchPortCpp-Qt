param (
    [string]$Version = "v1.0.0"
)

$ErrorActionPreference = "Stop"

$distName = "MidiUmpAnalyzer-$Version-windows-x64"
$distPath = "dist\$distName"
$zipName = "dist\$distName.zip"

Write-Host "--- Parando aplicativo caso esteja aberto ---"
Stop-Process -Name MidiUmpAnalyzer -ErrorAction SilentlyContinue

Write-Host "`n--- Garantindo compilacao em modo Release ---"
cmake --build build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "Falha na compilacao CMake." -ForegroundColor Red
    exit 1
}

Write-Host "`n--- Limpando pasta de distribuicao ---"
if (Test-Path $distPath) { Remove-Item -Recurse -Force $distPath }
if (Test-Path $zipName) { Remove-Item -Force $zipName }
New-Item -ItemType Directory -Force -Path $distPath | Out-Null

Write-Host "`n--- Copiando Executavel Principal ---"
Copy-Item "build\Release\MidiUmpAnalyzer.exe" -Destination $distPath

Write-Host "`n--- Copiando Dependencias Qt (windeployqt ja foi rodado pelo CMake) ---"
# Copia todas as DLLs geradas
Get-ChildItem "build\Release\*.dll" | Copy-Item -Destination $distPath

# Copia subpastas de plugins criadas pelo windeployqt
$qtFolders = @("platforms", "styles", "iconengines", "imageformats", "networkinformation", "tls")
foreach ($folder in $qtFolders) {
    if (Test-Path "build\Release\$folder") {
        Copy-Item -Recurse "build\Release\$folder" -Destination $distPath
    }
}

Write-Host "`n--- Copiando Documentacao e Lancador ---"
Copy-Item "README.md" -Destination $distPath
Copy-Item "TESTS.md" -Destination $distPath
Copy-Item "CHANGELOG.md" -Destination $distPath
Copy-Item "QUICK_START.txt" -Destination $distPath
Copy-Item "Start-MidiUmpAnalyzer.bat" -Destination $distPath

Write-Host "`n--- Zipando Pacote Final ---"
Compress-Archive -Path "$distPath\*" -DestinationPath $zipName -Force

Write-Host "`n[SUCESSO] Empacotamento finalizado!" -ForegroundColor Green
Write-Host "Arquivo gerado: $zipName" -ForegroundColor Cyan
