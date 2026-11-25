#!/bin/bash
set -e

DIR_APP="$HOME/biblia-1911"
ZIP_URL="[https://github.com/RomuloCSR/biblia-digital-almeida-1911-macos/releases/latest/download/biblia-digital-almeida-1911-macos.zip](https://github.com/RomuloCSR/biblia-digital-almeida-1911-macos/releases/latest/download/biblia-digital-almeida-1911-macos.zip)"

echo "========================================================"
echo "Instalando/Atualizando a Bíblia Almeida 1911 (macOS)..."
echo "========================================================"

rm -rf "$DIR_APP"
mkdir -p "$HOME/biblia_tmp"

echo "-> Baixando binário Universal e livros..."
curl -sL "$ZIP_URL" -o "$HOME/biblia_tmp/pacote.zip"
unzip -q "$HOME/biblia_tmp/pacote.zip" -d "$HOME/biblia_tmp"

mv "$HOME/biblia_tmp/biblia-1911" "$DIR_APP"
rm -rf "$HOME/biblia_tmp"

chmod +x "$DIR_APP/motor.biblia"

for RC in "$HOME/.zshrc" "$HOME/.bashrc"; do
    touch "$RC"
    if ! grep -q "alias biblia=" "$RC"; then
        echo "alias biblia='$DIR_APP/motor.biblia'" >> "$RC"
    fi
done

echo "========================================================"
echo "✅ Pronto! Para abrir a Bíblia no Terminal do Mac, digite:"
echo "   biblia"
echo "========================================================"
"$DIR_APP/motor.biblia"
