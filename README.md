# Bíblia Digital Almeida 1911 em PT-BR (macOS)

Texto bíblico baseado na edição **Almeida 1911 (Domínio Público)**, com atualização ortográfica e gramatical para o Português do Brasil realizada em **11/2026**.

> **Nota editorial:** O conteúdo está em processo contínuo de atualização e pode eventualmente apresentar pequenas falhas ortográficas ou gramaticais. Colabore nos enviando seu feedback pela aba **Issues** deste repositório. Obrigado!

- **Objetivo:** Motor leve e rápido de navegação bíblica para macOS (Intel e Apple Silicon M1/M2/M3/M4), sem anúncios e 100% offline.
- **Requisito:** macOS 11 Big Sur ou mais novo.
- **Autor:** Romulo CSR ([github.com/RomuloCSR](https://github.com/RomuloCSR))
- **Apoio espontâneo via PIX:** `21980934118`

---

## Como baixar e usar (Passo a passo rápido)

### Opção 1: Instalação automática em 1 comando (Recomendado)
1. Abra o aplicativo **Terminal** no seu Mac (`Cmd + Espaço`, digite `Terminal` e aperte **Enter**).
2. Cole o comando abaixo e aperte **Enter**:

```bash
curl -fsSL https://raw.githubusercontent.com/RomuloCSR/biblia-digital-almeida-1911-macos/main/instalar.sh | bash
```

3. Depois de instalado, sempre que quiser ler a Bíblia, basta abrir uma **nova janela** do **Terminal** e digitar:

```bash
biblia
```

### Opção 2: Baixar manualmente pela aba Releases
1. Na lateral direita desta página, clique em **Releases** e baixe o arquivo **`biblia-digital-almeida-1911-macos.zip`**.
2. Extraia o arquivo `.zip` em qualquer pasta do seu Mac (por exemplo, em `Documentos`).
3. Abra o **Terminal** dentro da pasta extraída e execute `./motor.biblia`.
4. *(Opcional, para quem programa: o arquivo `.zip` não traz o código-fonte. O código-fonte está neste repositório, em `terminal.c` e na pasta `kernel`. Para compilar: `clang -O2 -mmacosx-version-min=11.0 terminal.c kernel/biblia_kernel.c -o motor.biblia`, com as ferramentas de linha de comando da Apple instaladas.)*

---

## Este aplicativo é seguro? (Sobre alertas de segurança do macOS)

Este é um projeto independente, gratuito e sem fins lucrativos, publicado em código aberto no **GitHub**.

- **O que você precisa saber:**
  - O código é aberto: o código-fonte (`terminal.c` e a pasta `kernel`), o instalador (`instalar.sh`) e os textos dos 66 livros estão nesta página, e qualquer pessoa pode conferir.
  - Funciona sem internet (só o download precisa de internet).
  - Não coleta nenhum dado seu.
  - O programa **não é assinado nem verificado (notarizado) pela Apple**. É por isso que o macOS mostra um aviso na primeira vez.
- **Por que o macOS exibe o aviso *"não pode ser aberto porque é de um desenvolvedor não identificado"*?** O **Gatekeeper** do macOS avisa sobre todo programa que não tem a assinatura paga da Apple nem vem da App Store.
- **Como liberar na primeira abertura (se baixado manualmente pelo Finder):**
  1. Tente abrir o **`motor.biblia`** uma vez. O macOS vai mostrar o aviso e bloquear.
  2. Abra as configurações do Mac:
     - **macOS 11 e 12:** menu Apple > **Preferências do Sistema** > **Segurança e Privacidade** > aba **Geral**.
     - **macOS 13 ou mais novo:** menu Apple > **Ajustes do Sistema** > **Privacidade e Segurança** (role a tela até o fim).
  3. Ao lado do aviso sobre o `motor.biblia`, clique em **Abrir Mesmo Assim**. Confirme com sua senha, se for pedida (isso precisa ser feito apenas uma vez).
  4. *(No macOS 15 Sequoia ou mais novo, o atalho antigo de `Control + clique` > **Abrir** não libera mais o programa. Use os passos acima.)*
  5. *(Alternativa pelo Terminal, dentro da pasta extraída: `xattr -d com.apple.quarantine motor.biblia`)*
