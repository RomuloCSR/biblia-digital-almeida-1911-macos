# Bíblia Digital Almeida 1911 em PT-BR (macOS)

Texto bíblico baseado na edição **Almeida 1911 (Domínio Público)**, com atualização ortográfica e gramatical para o Português do Brasil realizada em **11/2026**.

> **Nota editorial:** O conteúdo está em processo contínuo de atualização e pode eventualmente apresentar pequenas falhas ortográficas ou gramaticais. Colabore nos enviando seu feedback pela aba **Issues** deste repositório. Obrigado!

- **Objetivo:** Motor leve e rápido de navegação bíblica para macOS (Intel e Apple Silicon M1/M2/M3/M4), sem anúncios e 100% offline.
- **Autor:** Romulo CSR ([github.com/RomuloCSR](https://github.com/RomuloCSR))
- **Apoio espontâneo via PIX:** `21980934118`

---

## Como baixar e usar (Passo a passo rápido)

### Opção 1: Instalação automática em 1 comando (Recomendado)
1. Abra o aplicativo **Terminal** no seu Mac (`Cmd + Espaço`, digite `Terminal` e aperte **Enter**).
2. Cole o comando abaixo e aperte **Enter**:

```bash
curl -sL [https://raw.githubusercontent.com/RomuloCSR/biblia-digital-almeida-1911-macos/main/instalar.sh](https://raw.githubusercontent.com/RomuloCSR/biblia-digital-almeida-1911-macos/main/instalar.sh) | bash
```

3. Depois de instalado, sempre que quiser ler a Bíblia, basta abrir o **Terminal** e digitar:

```bash
biblia
```

### Opção 2: Baixar manualmente pela aba Releases
1. Na lateral direita desta página, clique em **Releases** e baixe o arquivo **`biblia-digital-almeida-1911-macos.zip`**.
2. Extraia o arquivo `.zip` em qualquer pasta do seu Mac (por exemplo, em `Documentos`).
3. Abra o **Terminal** dentro da pasta extraída e execute `./motor.biblia` *(ou recompile localmente com `clang -O2 motor.biblia.c -o motor.biblia`, caso possua as ferramentas de linha de comando instaladas)*.

---

## Este aplicativo é seguro? (Sobre alertas de segurança do macOS)

**Sim, 100% seguro.** Este é um projeto independente, gratuito e sem fins lucrativos, publicado em código aberto no **GitHub** (plataforma de referência mundial utilizada por desenvolvedores).

- **Por que o macOS exibe o aviso *"não pode ser aberto porque é de um desenvolvedor não identificado"*?** Sistemas operacionais modernos (macOS, Windows, Android e algumas distribuições Linux) emitem avisos preventivos do **Gatekeeper** para qualquer programa que não possua um certificado comercial pago da Apple nem esteja distribuído pela App Store.
- **Como liberar na primeira abertura (se baixado manualmente pelo Finder):**
  1. Clique com o **botão direito** (ou `Control + clique`) sobre o arquivo **`motor.biblia`** e escolha **Abrir**.
  2. Na janela de confirmação que aparecer, clique novamente em **Abrir** (isso precisa ser feito apenas uma vez).
  3. *(Alternativa pelo Terminal, dentro da pasta extraída: `xattr -d com.apple.quarantine motor.biblia`)*
- **Transparência total (Código Aberto):** Todo o código-fonte deste aplicativo (`motor.biblia.c`), o script de instalação (`instalar.sh`) e os textos dos 66 livros estão abertos e visíveis nesta página. Qualquer técnico ou programador pode inspecionar o código e confirmar que não há coleta de dados, rastreamento ou arquivos maliciosos.
