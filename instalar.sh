#!/bin/bash
# Instalador da Bíblia Digital Almeida ARC-1911 para macOS.
# Uso: curl -fsSL https://raw.githubusercontent.com/RomuloCSR/biblia-digital-almeida-1911-macos/main/instalar.sh | bash
# Compatível com o bash 3.2 que vem no macOS (sem recursos do bash 4).
# Atualizado em 28/11/2026.

# O que: para o script no primeiro erro.
# Por que: evita continuar a instalação pela metade.
set -e

# O que: pastas e endereço do pacote.
# Por que: a pasta final é ~/biblia-1911; a pasta temporária guarda o download
# até ele estar completo, para nunca apagar a instalação antiga à toa.
DIR_APP="$HOME/biblia-1911"
DIR_TMP="$HOME/biblia_tmp"
ZIP_URL="https://github.com/RomuloCSR/biblia-digital-almeida-1911-macos/releases/latest/download/biblia-digital-almeida-1911-macos.zip"

# O que: apaga a pasta temporária ao sair, com sucesso ou com erro.
# Por que: não deixar restos que atrapalhem a próxima instalação (28/11/2026).
limpar() {
    rm -rf "$DIR_TMP"
}
trap limpar EXIT

# O que: mostra uma mensagem de erro clara e encerra com código 1.
# Por que: o usuário leigo precisa saber que deu errado e que nada foi perdido.
falhar() {
    echo "========================================================"
    echo "ERRO: $1"
    echo "Sua instalação anterior (se existir) foi mantida."
    echo "Verifique sua conexão com a internet e tente de novo."
    echo "========================================================"
    exit 1
}

echo "========================================================"
echo "Instalando/Atualizando a Bíblia Almeida 1911 (macOS)..."
echo "========================================================"

# O que: começa com uma pasta temporária limpa.
# Por que: restos de uma instalação interrompida faziam o unzip parar
# e perguntar "replace?", quebrando a instalação em silêncio (28/11/2026).
rm -rf "$DIR_TMP"
mkdir -p "$DIR_TMP"

# O que: baixa o pacote. O -f faz o curl falhar em erro do servidor (ex.: 404).
# Por que: antes, uma página de erro era salva como se fosse o pacote.
# O < /dev/null impede que o curl leia o próprio script no modo "curl | bash".
echo "-> Baixando binário Universal e livros..."
curl -fsSL "$ZIP_URL" -o "$DIR_TMP/pacote.zip" < /dev/null \
    || falhar "Não foi possível baixar o pacote da Bíblia."

# O que: extrai o pacote sem fazer perguntas (-o) e em silêncio (-q).
# Por que: no modo "curl | bash" qualquer pergunta consumiria o resto do script.
unzip -oq "$DIR_TMP/pacote.zip" -d "$DIR_TMP" < /dev/null \
    || falhar "O pacote baixado está corrompido."

# O que: confere se o pacote trouxe o programa.
# Por que: só trocar a instalação antiga se a nova estiver completa.
if [ ! -f "$DIR_TMP/biblia-1911/motor.biblia" ]; then
    falhar "O pacote baixado está incompleto."
fi

# O que: troca a instalação antiga pela nova só agora, com o download completo.
# Por que: se algo falhar na troca, a versão antiga volta para o lugar (28/11/2026).
if [ -d "$DIR_APP" ]; then
    mv "$DIR_APP" "$DIR_TMP/versao_antiga"
fi
if ! mv "$DIR_TMP/biblia-1911" "$DIR_APP"; then
    if [ -d "$DIR_TMP/versao_antiga" ]; then
        mv "$DIR_TMP/versao_antiga" "$DIR_APP"
    fi
    falhar "Não foi possível copiar os arquivos para $DIR_APP."
fi

chmod +x "$DIR_APP/motor.biblia"

# O que: cria o atalho "biblia" no zsh (padrão do macOS) e no bash.
# Por que: o usuário só precisa digitar "biblia". O grep evita repetir o atalho
# quando o instalador roda mais de uma vez.
for RC in "$HOME/.zshrc" "$HOME/.bashrc"; do
    touch "$RC"
    if ! grep -q "alias biblia=" "$RC"; then
        echo "alias biblia='$DIR_APP/motor.biblia'" >> "$RC"
    fi
done

# O que: explica como abrir a Bíblia depois.
# Por que: o atalho "biblia" só funciona em uma janela NOVA do Terminal.
echo "========================================================"
echo "✅ Pronto! A Bíblia foi instalada."
echo "Para ler depois: abra uma NOVA janela do Terminal"
echo "(Cmd + N) e digite:"
echo "   biblia"
echo "========================================================"

# O que: abre a Bíblia agora, lendo o teclado pelo /dev/tty.
# Por que: no modo "curl | bash" a entrada do script é o próprio download,
# e o programa fecharia na hora. Só abre se houver um terminal disponível.
if [ -r /dev/tty ] && ( : < /dev/tty ) 2>/dev/null; then
    "$DIR_APP/motor.biblia" < /dev/tty || true
fi
