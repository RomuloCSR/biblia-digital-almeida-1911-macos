#!/bin/bash
# Instalador da Bíblia Digital Almeida ARC-1911 para macOS.
# Uso: curl -fsSL https://raw.githubusercontent.com/RomuloCSR/biblia-digital-almeida-1911-macos/main/instalar.sh | bash
# Compatível com o bash 3.2 que vem no macOS (sem recursos do bash 4).
# Atualizado em 28/11/2026.

# O que: para o script no primeiro erro.
# Por que: evita continuar a instalação pela metade. Qualquer erro inesperado
# cai na rotina de saída (ao_sair), que desfaz a troca se ela já começou.
set -e

# O que: pastas, endereço do pacote e situação da instalação.
# Por que: a pasta final é ~/biblia-1911. A pasta temporária é criada com
# mktemp (nome único) e é a ÚNICA pasta que o instalador apaga; nunca toca
# em ~/biblia_tmp nem em outra pasta do usuário. O backup da versão antiga
# fica FORA da pasta temporária, para não ser apagado junto com ela
# (revisão de 28/11/2026).
DIR_APP="$HOME/biblia-1911"
ZIP_URL="https://github.com/RomuloCSR/biblia-digital-almeida-1911-macos/releases/latest/download/biblia-digital-almeida-1911-macos.zip"
DIR_TMP=""
BACKUP="$HOME/biblia-1911.anterior.$$"
N=0
while [ -e "$BACKUP" ]; do
    N=$((N + 1))
    BACKUP="$HOME/biblia-1911.anterior.$$.$N"
done
TINHA_ANTIGA=0   # 1 = havia uma instalação antiga e ela foi para o backup
TROCOU=0         # 1 = a troca começou (a partir daqui, erro = desfazer)
SUCESSO=0        # 1 = tudo deu certo
MOSTROU_ERRO=0   # 1 = a mensagem de erro específica já foi mostrada

# O que: desfaz a troca, devolvendo a versão antiga para ~/biblia-1911.
# Por que: se algo falhar depois da troca (cópia, atalho etc.), o usuário
# não pode ficar sem a Bíblia. Só diz que restaurou depois de conferir.
# Se não conseguir restaurar, NÃO apaga o backup e mostra onde ele está.
restaurar() {
    if [ "$TINHA_ANTIGA" = 1 ]; then
        rm -rf "$DIR_APP"
        if [ ! -e "$DIR_APP" ] && mv "$BACKUP" "$DIR_APP" \
            && [ -e "$DIR_APP" ] && [ ! -e "$BACKUP" ]; then
            echo "Sua instalação anterior foi restaurada em: $DIR_APP"
        else
            echo "ATENÇÃO: não foi possível restaurar a versão anterior sozinho."
            echo "Ela está guardada, sem perdas, em:"
            echo "   $BACKUP"
            echo "Para voltar a usá-la, renomeie essa pasta para: $DIR_APP"
        fi
    else
        # Não havia instalação antiga: remove a nova pela metade.
        rm -rf "$DIR_APP"
        echo "Nada foi instalado. Seu Mac ficou como estava."
    fi
}

# O que: rotina de saída (sucesso, erro ou Ctrl+C).
# Por que: em caso de erro, desfaz a troca se ela começou e informa o
# estado REAL da instalação antiga. Em qualquer caso, apaga só a pasta
# temporária criada pelo mktemp (28/11/2026).
ao_sair() {
    CODIGO=$?
    set +e
    if [ "$SUCESSO" != 1 ]; then
        if [ "$MOSTROU_ERRO" != 1 ]; then
            echo "========================================================"
            echo "ERRO: a instalação foi interrompida."
        fi
        if [ "$TROCOU" = 1 ]; then
            restaurar
        elif [ -d "$DIR_APP" ]; then
            echo "Sua instalação anterior continua em $DIR_APP, sem alterações."
        fi
        echo "========================================================"
        [ "$CODIGO" = 0 ] && CODIGO=1
    fi
    if [ -n "$DIR_TMP" ] && [ -d "$DIR_TMP" ]; then
        rm -rf "$DIR_TMP"
    fi
    exit "$CODIGO"
}
trap ao_sair EXIT
trap 'exit 130' HUP INT TERM

# O que: mostra o erro específico e o que fazer, e encerra com código 1.
# Por que: cada falha tem uma solução diferente; o usuário leigo precisa
# da orientação certa (internet, pacote corrompido, permissão). O estado
# da instalação antiga é informado depois, pela rotina ao_sair.
falhar() {
    echo "========================================================"
    echo "ERRO: $1"
    echo "$2"
    MOSTROU_ERRO=1
    exit 1
}

DICA_PERMISSAO="Verifique se há espaço livre no disco e permissão de escrita na sua pasta pessoal ($HOME)."

echo "========================================================"
echo "Instalando/Atualizando a Bíblia Almeida 1911 (macOS)..."
echo "========================================================"

# O que: cria uma pasta temporária nova, com nome único.
# Por que: o nome fixo ~/biblia_tmp podia apagar arquivos do usuário e
# restos antigos faziam o unzip perguntar "replace?" (28/11/2026).
DIR_TMP=$(mktemp -d "$HOME/.biblia-1911-instalar.XXXXXX") \
    || falhar "Não foi possível criar uma pasta temporária." "$DICA_PERMISSAO"

# O que: baixa o pacote. O -f faz o curl falhar em erro do servidor (ex.: 404).
# Por que: antes, uma página de erro era salva como se fosse o pacote.
# O < /dev/null impede que o curl leia o próprio script no modo "curl | bash".
echo "-> Baixando binário Universal e livros..."
curl -fsSL "$ZIP_URL" -o "$DIR_TMP/pacote.zip" < /dev/null \
    || falhar "Não foi possível baixar o pacote da Bíblia." \
              "Verifique sua conexão com a internet e tente de novo."

# O que: extrai o pacote sem fazer perguntas (-o) e em silêncio (-q).
# Por que: no modo "curl | bash" qualquer pergunta consumiria o resto do script.
DICA_PACOTE="Tente de novo em alguns minutos. Se o erro continuar, avise na aba Issues do projeto no GitHub."
unzip -oq "$DIR_TMP/pacote.zip" -d "$DIR_TMP" < /dev/null \
    || falhar "O pacote baixado está corrompido." "$DICA_PACOTE"

# O que: confere se o pacote trouxe o programa.
# Por que: só trocar a instalação antiga se a nova estiver completa.
if [ ! -f "$DIR_TMP/biblia-1911/motor.biblia" ]; then
    falhar "O pacote baixado está incompleto." "$DICA_PACOTE"
fi

# O que: deixa o programa executável ANTES da troca.
# Por que: se a permissão falhar, a instalação antiga nem é tocada (28/11/2026).
chmod +x "$DIR_TMP/biblia-1911/motor.biblia" \
    || falhar "Não foi possível preparar o programa (permissão)." "$DICA_PERMISSAO"

# O que: troca a instalação antiga pela nova. A antiga vai para o backup,
# fora da pasta temporária.
# Por que: a partir de TROCOU=1, qualquer falha restaura a versão antiga
# pela rotina ao_sair (28/11/2026).
if [ -e "$DIR_APP" ]; then
    mv "$DIR_APP" "$BACKUP" \
        || falhar "Não foi possível guardar a versão anterior." "$DICA_PERMISSAO"
    TINHA_ANTIGA=1
fi
TROCOU=1
mv "$DIR_TMP/biblia-1911" "$DIR_APP" \
    || falhar "Não foi possível copiar os arquivos para $DIR_APP." "$DICA_PERMISSAO"

# O que: cria o atalho "biblia" no zsh (padrão do macOS) e no bash
# (~/.bashrc e ~/.bash_profile, que é o arquivo lido pelo Terminal quando
# o usuário escolheu o bash).
# Por que: o usuário só precisa digitar "biblia". O grep evita repetir o
# atalho quando o instalador roda mais de uma vez. A linha em branco antes
# evita grudar o atalho na última linha do arquivo (28/11/2026).
for RC in "$HOME/.zshrc" "$HOME/.bashrc" "$HOME/.bash_profile"; do
    if [ -f "$RC" ] && grep -q "alias biblia=" "$RC" 2>/dev/null; then
        continue
    fi
    if [ -e "$RC" ] && [ ! -f "$RC" ]; then
        falhar "Não foi possível criar o atalho \"biblia\" em $RC (não é um arquivo comum)." \
               "Verifique esse arquivo na sua pasta pessoal e tente de novo."
    fi
    printf '\n%s\n' "alias biblia='$DIR_APP/motor.biblia'" >> "$RC" \
        || falhar "Não foi possível criar o atalho \"biblia\" em $RC." \
                  "Verifique a permissão desse arquivo e tente de novo."
done

# O que: confirma o sucesso e só então apaga o backup e a pasta temporária.
# Por que: a versão antiga só pode sumir quando a nova está completa (28/11/2026).
SUCESSO=1
if [ "$TINHA_ANTIGA" = 1 ]; then
    rm -rf "$BACKUP" \
        || echo "Aviso: não foi possível apagar a cópia antiga em $BACKUP (pode apagá-la à mão)."
fi
rm -rf "$DIR_TMP"
DIR_TMP=""

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
