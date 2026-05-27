param (
    [switch]$SkipBuild
)

# Script de Execucao de Testes (v0.8.0)
$ErrorActionPreference = "Stop"

if (-Not $SkipBuild -and -Not $env:CI) {
    Write-Host "--- Parando app se estiver aberto ---"
    Stop-Process -Name MidiUmpAnalyzer -ErrorAction SilentlyContinue

    Write-Host "`n--- Compilando Projeto em Release ---"
    cmake --build build --config Release
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Falha no Build!" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "--- Modo CI / SkipBuild detectado: Pulando recompilacao local ---"
}

Write-Host "`n--- Rodando Testes Automatizados ---"
$testExe = "build\Release\UmpParserTests.exe"

if (-Not (Test-Path $testExe)) {
    Write-Host "Executavel de teste nao encontrado em: $testExe" -ForegroundColor Red
    exit 1
}

& $testExe
$testExit = $LASTEXITCODE

if ($testExit -eq 0) {
    Write-Host "`n[SUCESSO] Todos os testes passaram! (Exit Code 0)" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`n[FALHA] Algum teste falhou! (Exit Code 1)" -ForegroundColor Red
    exit 1
}
