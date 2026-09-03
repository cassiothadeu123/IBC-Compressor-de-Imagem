# IBC Compressor de Imagem

Aplicativo desktop 100% offline, construído em C++20, Qt 6 e Qt Quick/QML. O motor procura automaticamente a maior qualidade que respeite um tamanho máximo, com redimensionamento adaptativo opcional.

## O que já está implementado

- seleção múltipla e arrastar/soltar;
- JPEG, WebP, AVIF e PNG, conforme os plugins do Qt instalados;
- alvo exato em KB por arquivo;
- busca binária da melhor qualidade possível;
- redimensionamento progressivo apenas quando necessário;
- correção automática da orientação EXIF;
- remoção opcional de metadados;
- processamento paralelo em lote;
- cancelamento, progresso e relatório por arquivo;
- interface holográfica animada por shader;
- nenhum upload, login ou dependência de internet.

## Limite físico da compressão

Reduzir 200 MB para 900 KB equivale a cerca de 99,55%. Fotografias grandes podem alcançar esse alvo com excelente aparência usando AVIF/WebP e ajuste de resolução. Não é possível garantir compressão matematicamente sem perdas nessa proporção para qualquer conteúdo. Por isso o aplicativo procura preservar a qualidade visual e informa quando o alvo não pode ser atingido com as regras escolhidas.

## Compilar

Requisitos: Qt 6.6 ou superior (com WebP/AVIF quando desejado), CMake 3.24+, Ninja e compilador C++20.

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix dist
```

No macOS Universal, acrescente:

```bash
-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
```

## Próximas etapas de produção

- pré-visualização lado a lado com zoom sincronizado;
- estimativa de qualidade antes de salvar;
- perfis prontos para WhatsApp, sites, e-commerce e impressão;
- assinatura e notarização do pacote macOS;
- instalador Windows assinado;
- testes automatizados com imagens fotográficas, transparência e arquivos muito grandes.

## Instaladores automatizados

O fluxo em `.github/workflows/build-installers.yml` compila em máquinas nativas,
executa testes do motor, abre o aplicativo em modo gráfico sem tela e só então
gera o `EXE` para Windows x64 e o `DMG` Universal para macOS Intel/Apple Silicon.
