#!/usr/bin/env bash
set -euo pipefail

VERSION="v1.0.0"

while [[ $# -gt 0 ]]; do
  case "$1" in
    -v|--version)
      VERSION="$2"
      shift 2
      ;;
    *)
      echo "Argumento desconhecido: $1"
      echo "Uso: $0 [-v|--version <versao>]"
      exit 1
      ;;
  esac
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${ROOT_DIR}"

BUILD_DIR="build"
DIST_NAME="MidiUmpAnalyzer-${VERSION}-linux-x64"
DIST_DIR="dist/${DIST_NAME}"
TAR_NAME="dist/${DIST_NAME}.tar.gz"

echo "=== Empacotamento de Release para Linux (${VERSION}) ==="

# Parar processo anterior se estiver rodando
pkill -f MidiUmpAnalyzer 2>/dev/null || true

# Configurar e compilar em modo Release
echo "--> Compilando projeto em modo Release com Ninja..."
cmake -B "${BUILD_DIR}" -S . -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}" --config Release

# Preparar diretório de distribuição
echo "--> Preparando pasta de distribuição..."
rm -rf "${DIST_DIR}" "${TAR_NAME}"
mkdir -p "${DIST_DIR}"
mkdir -p "dist"

# Copiar executável principal
echo "--> Copiando executável principal..."
cp -f "${BUILD_DIR}/MidiUmpAnalyzer" "${DIST_DIR}/"
chmod +x "${DIST_DIR}/MidiUmpAnalyzer"

# Copiar bibliotecas dependentes construídas localmente (libremidi, rtmidi)
echo "--> Copiando bibliotecas compartilhadas locais..."
find "${BUILD_DIR}/_deps" -maxdepth 4 -name "*.so*" -type f -exec cp -d {} "${DIST_DIR}/" \; 2>/dev/null || true
find "${BUILD_DIR}/_deps" -maxdepth 4 -name "*.so*" -type l -exec cp -d {} "${DIST_DIR}/" \; 2>/dev/null || true

# Copiar samples, docs e metadados
echo "--> Copiando documentação e assets..."
[ -d "samples" ] && cp -r "samples" "${DIST_DIR}/"
[ -d "docs" ] && cp -r "docs" "${DIST_DIR}/"
[ -f "README.md" ] && cp "README.md" "${DIST_DIR}/"
[ -f "CHANGELOG.md" ] && cp "CHANGELOG.md" "${DIST_DIR}/"
[ -f "TESTS.md" ] && cp "TESTS.md" "${DIST_DIR}/"
[ -f "QUICK_START.txt" ] && cp "QUICK_START.txt" "${DIST_DIR}/"

# Criar script de inicialização run.sh
cat << 'EOF' > "${DIST_DIR}/run.sh"
#!/usr/bin/env bash
set -e
APP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${APP_DIR}:${LD_LIBRARY_PATH:-}"
exec "${APP_DIR}/MidiUmpAnalyzer" "$@"
EOF
chmod +x "${DIST_DIR}/run.sh"

# Criar tarball comprimido
echo "--> Compactando pacote final em ${TAR_NAME}..."
tar -czf "${TAR_NAME}" -C "dist" "${DIST_NAME}"

echo ""
echo "[SUCESSO] Pacote Linux gerado com sucesso!"
echo "Arquivo gerado: ${TAR_NAME}"
